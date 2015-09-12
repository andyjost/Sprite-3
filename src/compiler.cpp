#include <cassert>
#include <iostream>
#include "sprite/curryinput.hpp"
#include "sprite/compiler.hpp"
#include "sprite/runtime.hpp"
#include <algorithm>
#include <iterator>
#include <boost/scope_exit.hpp>
#include <tuple>
#include "sprite/tree_utils.hpp"

// DIAGNOSTIC - this may warrant a command-line option setting.
#include "llvm/Analysis/Verifier.h"

using namespace sprite;
using namespace sprite::compiler::member_labels; // for ND_* and VT_* enums.
namespace tgt = sprite::backend;

namespace
{
  using namespace sprite::compiler;
  namespace curry = sprite::curry;
  using sprite::curry::Qname;

  curry::CaseLhs const & getlhs(curry::CaseLhs const & arg)
    { return arg; }
  curry::CaseLhs const & getlhs(std::shared_ptr<curry::Case> const & arg)
    { return arg->lhs; }

  struct ValueSaver
  {
    ValueSaver(value & ref_, value x=value())
      : ref(ref_), saved(ref_)
    { ref_ = x; }
    ~ValueSaver()  { ref = saved; }
  private:
    value & ref;
    value saved;
  };

  struct GetLhsCaseType
  {
    using result_type = type;

    template<typename T>
    result_type operator()(T) const
      { return get_type<T>(); }

    result_type operator()(Qname const &) const
      { throw compile_error("Expected a non-enumerable type."); }
  };

  type get_case_type(curry::CaseLhs const & lhs)
  {
    GetLhsCaseType visitor;
    return lhs.visit(visitor);
  }

  type get_case_type(std::vector<std::shared_ptr<curry::Case>> const & cases)
  {
    type const ty = get_case_type(cases[0]->lhs);
    for(auto const & case_: cases)
    {
      type const ty_ = get_case_type(case_->lhs);
      if(ty.ptr() != ty_.ptr())
        throw compile_error("Malformed switch.");
    }
    return ty;
  }

  struct GetLhsCaseValue
  {
    using result_type = constant_int;

    result_type operator()(char c) const
      { return cast<constant_int>(get_constant(c)); }

    result_type operator()(int64_t i) const
      { return cast<constant_int>(get_constant(i)); }

    result_type operator()(double) const
      { throw compile_error("Can't form case with floating-point value."); }

    result_type operator()(Qname const &) const
      { throw compile_error("Expected a non-enumerable type."); }
  };

  constant_int get_case_value(curry::CaseLhs const & lhs)
  {
    GetLhsCaseValue visitor;
    return lhs.visit(visitor);
  }

  struct GetLhsArity
  {
    using result_type = size_t;

    GetLhsArity(compiler::ModuleSTab const & module_stab_)
      : module_stab(module_stab_)
    {}

    compiler::ModuleSTab const & module_stab;

    template<typename T>
    result_type operator()(T) const { return 0; }

    result_type operator()(Qname const & qname) const
      { return module_stab.lookup(qname).source->arity; }
  };

  size_t get_lhs_arity(
      curry::CaseLhs const & lhs, compiler::ModuleSTab const & module_stab
    )
  {
    GetLhsArity visitor(module_stab);
    return lhs.visit(visitor);
  }


  struct GetLhsCaseTag
  {
    using result_type = compiler::tag_t;

    GetLhsCaseTag(compiler::ModuleSTab const & module_stab_)
      : module_stab(module_stab_)
    {}

    compiler::ModuleSTab const & module_stab;

    template<typename T>
    result_type operator()(T const &) const
      { return compiler::CTOR; }

    result_type operator()(Qname const & qname) const
    { return module_stab.lookup(qname).tag; }
  };

  compiler::tag_t get_case_tag(
      curry::CaseLhs const & lhs, compiler::ModuleSTab const & module_stab
    )
  {
    GetLhsCaseTag visitor(module_stab);
    return lhs.visit(visitor);
  }

  struct GetGeneratorFromCase
  {
    using result_type = function;

    GetGeneratorFromCase(compiler::ModuleSTab const & module_stab_)
      : module_stab(module_stab_)
    {}

    compiler::ModuleSTab const & module_stab;

    result_type operator()(char) const
      { return module_stab.rt().Cy_NoGenerator("Char"); }

    result_type operator()(int64_t) const
      { return module_stab.rt().Cy_NoGenerator("Int"); }

    result_type operator()(double) const
      { return module_stab.rt().Cy_NoGenerator("Float"); }

    result_type operator()(Qname const & qname) const
      { return module_stab.lookup(qname).generator; }
  };

  function get_generator_from_case(
      curry::CaseLhs const & lhs, compiler::ModuleSTab const & module_stab
    )
  {
    GetGeneratorFromCase visitor(module_stab);
    return lhs.visit(visitor);
  }


  std::string get_base_function_name(curry::Function const & fun)
  {
    std::string const & str = fun.name;
    if(fun.is_aux)
    {
      size_t const found = str.rfind("#branch");
      if(found != std::string::npos)
        return str.substr(0, found);
      throw compile_error("badly named aux function");
    }
    return str;
  }

  /**
   * Move the contents of a node from src to tgt.  Leaves src in an invalid
   * state if it has an extended successor array.
   */
  void move(value const & src, value const & tgt, size_t arity)
  {
    tgt.arrow(ND_VPTR) = src.arrow(ND_VPTR);
    tgt.arrow(ND_TAG) = src.arrow(ND_TAG);
    tgt.arrow(ND_AUX) = src.arrow(ND_AUX);
    if(arity > 0)
      tgt.arrow(ND_SLOT0) = src.arrow(ND_SLOT0);
    if(arity > 1)
      tgt.arrow(ND_SLOT1) = src.arrow(ND_SLOT1);
  }

  /**
   * @brief Composes an expression by rewriting the given root.
   *
   * If the expression contains variable references, then they are
   * resolved relative to the root, in which case the function
   * definition must be supplied.
   */
  struct Rewriter
  {
    using result_type = void;

    Rewriter(
        compiler::ModuleSTab const & module_stab_
      , tgt::value const & root_p_
      , curry::Function const * fundef_ = nullptr
      )
      : module_stab(module_stab_)
      , root_p(bitcast(root_p_, node_pointer_type))
      , target_p(root_p)
      , fundef(fundef_)
      , resolved_path_alloca(tgt::local(node_pointer_type))
    {}

    compiler::ModuleSTab const & module_stab;

    rt_h const & rt = module_stab.rt();

    // Any needed types (for convenience).
    tgt::type node_pointer_type = *module_stab.rt().node_t;

    // A pointer in the target to the root node undergoing pattern matching
    // or rewriting.
    tgt::value root_p;

    // The current node that will be rewritten.  When constructing nested
    // expressions, root_p will never change, but the target will change to
    // point to the subexpression currently under construction.
    tgt::value target_p;

    // The definition of the function being compiled.
    curry::Function const * fundef;

    // A node_t* containing the most-recently-resolved path.  Requires alloca
    // to skip over FWD nodes.
    mutable tgt::ref resolved_path_alloca;

    // A mapping that contains free variables and other local allocations.
    // These are not reachable from the root node.
    mutable std::map<size_t, tgt::ref> freevar_alloca;

    // The offset used to tag path ids for local expressions.
    static size_t const LOCAL_ID_START = 1L << 16;

    // The next available local ID.
    size_t next_local_id = LOCAL_ID_START;

    // The label containing code to handle the out-of-memory condition.  It
    // changes while compiling a function.  In general, is is the most recent
    // safe point crossed just before allocating one or more nodes.  If the
    // garbage collector run, then all of the nodes after this point were
    // collected, since have not yet been attached to a root.
    mutable label out_of_memory_handler = nullptr;

    // Sets a "safe point".  If the garbage collector runs in subsequent code,
    // then after the collector runs, execution resumes at the point where this
    // was most recently called.  (Actually, it resumes at the entry to the
    // current basic block, which is good enough.)
    void set_out_of_memory_handler_returning_here()
      { this->out_of_memory_handler = rt.make_restart_point(); }

    // Node allocator for use in the rewriter.  Automatically uses this
    // object's out-of-memory handler.
    value node_alloc(type const & ty) const
      { return rt.node_alloc(ty, this->out_of_memory_handler); }

    // Places an expression at an uninitialized location.
    template<typename RuleOrCaseLhs>
    tgt::value new_(tgt::value const & data, RuleOrCaseLhs const & def)
    {
      ValueSaver saver(target_p, data);
      (*this)(def);
      return data;
    }

    // Allocates a new node and places an expression there.
    template<typename RuleOrCaseLhs>
    tgt::value new_(RuleOrCaseLhs const & def)
    {
      tgt::value data = this->node_alloc(*rt.node_t);
      return this->new_(data, def);
    }

    // Deallocates the extended child array stored in the root node, if present.
    void destroy_target()
    {
      if(root_p.ptr() == target_p.ptr() && fundef && fundef->arity > 2)
        vinvoke(this->target_p, VT_DESTROY);
    }

    // Looks up a node using the function paths and path reference.  The result
    // is a node_t* in the target.  If the variable referenced is a free
    // variable, then space for it will be allocated if necessary.
    tgt::value resolve_path(size_t pathid) const
    {
      static curry::Function::PathElem const local_path{curry::local, 0, {}};
      auto const & pathelem = pathid >= LOCAL_ID_START
          ? local_path
          : this->fundef->paths.at(pathid)
        ;

      switch(pathelem.base)
      {
        // Handle special variables.
        case curry::freevar:
        case curry::bind:
        case curry::local:
        {
          auto it = freevar_alloca.find(pathid);
          if(it == freevar_alloca.end())
          {
            // Create a node* on the stack.
            auto pair = freevar_alloca.emplace(
                pathid, tgt::local(node_pointer_type)
              );
            // Allocate a new node and store its address on the stack.
            auto & p = pair.first->second;
            p = this->node_alloc(*rt.node_t);
            p.arrow(ND_VPTR) = rt.freevar_vt;
            p.arrow(ND_TAG) = compiler::FREE;
            p.arrow(ND_AUX) = rt.Cy_NextChoiceId++;
            p.arrow(ND_SLOT0) = nullptr;
            this->resolved_path_alloca = p;
          }
          else
            this->resolved_path_alloca = it->second;
          break;
        }
        // Handle regular variables.
        default:
          this->resolved_path_alloca = this->root_p;
          _resolve_path(pathelem);
          break;
      }
      return this->resolved_path_alloca;
    }

    // Same as @p resolve_path but returns a char* in the target.
    tgt::value resolve_path_char_p(size_t pathid) const
      { return bitcast(resolve_path(pathid), *tgt::types::char_()); }

    // Helper for @p resolve_path.  Updates @p resolved_path_alloca.
    void _resolve_path(curry::Function::PathElem const & pathelem) const
    {
      assert(pathelem.base != curry::freevar);
      // Walk the base path.
      if(pathelem.base != curry::nobase)
        _resolve_path(this->fundef->paths.at(pathelem.base));

      // Add this index to the path.
      size_t const term_arity = this->module_stab.lookup(pathelem.typename_)
          .source->arity;
      assert(pathelem.idx < term_arity);
      switch(term_arity)
      {
        case 0: assert(0 && "indexing a term with no successors");
        case 1:
        case 2:
          this->resolved_path_alloca = bitcast(
              this->resolved_path_alloca.arrow(ND_SLOT0 + pathelem.idx)
            , node_pointer_type
            );
          break;
        default:
        {
          value children = bitcast(
              this->resolved_path_alloca.arrow(ND_SLOT0), **node_pointer_type
            );
          this->resolved_path_alloca = children[pathelem.idx];
        }
      }

      // Skip FWD nodes.
      // TODO: this would be a good place to collapse chains of FWD nodes.
      tgt::while_(
          // FIXME: this should work without the cast to int.
          // FIXME: no signed/unsigned flag is really required, here, is it?
          [&]{
              this->resolved_path_alloca.arrow(ND_TAG)
                  ==(tgt::signed_) (static_cast<tag_t>(FWD));
            }
        , [&]{
              // rt.printf("Fwd\n"); // DEBUG
              this->resolved_path_alloca = bitcast(
                  this->resolved_path_alloca.arrow(ND_SLOT0), node_pointer_type
                );
            }
        );
    }

    result_type operator()(curry::Rule const & rule)
      { rule.visit(*this); }

    result_type operator()(curry::CaseLhs const & rule)
      { rule.visit(*this); }

    result_type operator()(curry::Free const &)
    {
      this->destroy_target();
      this->target_p.arrow(ND_VPTR) = rt.freevar_vt;
      this->target_p.arrow(ND_TAG) = compiler::FREE;
      this->target_p.arrow(ND_AUX) = rt.Cy_NextChoiceId++;
      this->target_p.arrow(ND_SLOT0) = nullptr;

    }

    result_type operator()(Qname const & qname)
    {
      auto const & node_stab = this->module_stab.lookup(qname);
      size_t const arity = node_stab.source->arity;
      std::vector<curry::Rule> args(arity, curry::Free());
      curry::Term const term(qname, std::move(args));
      (*this)(term);
    }

  private:

    /// Rewrites the target to a simple constructor with built-in data.
    template<typename Data>
    void rewrite_fundamental_data(Data data, tgt::value const & vt)
    {
      this->destroy_target();
      this->target_p.arrow(ND_VPTR) = vt;
      this->target_p.arrow(ND_TAG) = compiler::CTOR;
      auto slot0 = this->target_p.arrow(ND_SLOT0);
      ref slot0_typed(bitcast(&slot0, *get_type<Data>()));
      slot0_typed = data;
    }

  public:
  
    // Subcases of Rule.
    result_type operator()(char data)
      { return rewrite_fundamental_data(data, rt.Char_vt); }

    result_type operator()(int64_t data)
      { return rewrite_fundamental_data(data, rt.Int64_vt); }

    result_type operator()(double data)
      { return rewrite_fundamental_data(data, rt.Float_vt); }

    // Rewrites the root as a FAIL node.
    result_type operator()(curry::Fail const &)
    {
      this->destroy_target();
      this->target_p.arrow(ND_VPTR) = rt.failed_vt;
      this->target_p.arrow(ND_TAG) = compiler::FAIL;
    }

    // Rewrites the root as a FWD node.
    result_type operator()(curry::Ref rule)
    {
      tgt::value target = this->resolve_path_char_p(rule.pathid);
      this->destroy_target();
      this->target_p.arrow(ND_VPTR) = rt.fwd_vt;
      this->target_p.arrow(ND_TAG) = compiler::FWD;
      this->target_p.arrow(ND_SLOT0) = target;
    }

    // Rewrites the root as a partial node.
    // 
    // The final "apply" in a partial evaluation is shown below.
    // 
    // Expression: apply (f a0 ... ak) an
    /*
                           apply
                         /       \
                        /         \
              PartialSpine.1.n     an
               |       \
               .        ak
               .
               .
              PartialSpine.{n-1}.n
               |                  \
               |                   a
               |
              PartialTerminus.n.n(&f)
    */
    // The two numbers following "P." indicate the remaining number of
    // arguments to be bound, and the final arity, respectively.  They are
    // stored in the "aux" and "tag" locations in the node.  The terminal node,
    // whose type is PartialTerminus, contains the v-table for f as data.
    //
    result_type operator()(curry::Partial const & term)
    {
      // Use only the least significant n-1 bits to avoid any possibility of
      // interpreting the tag as a negative value.
      auto const & node_stab = this->module_stab.lookup(term.qname);
      size_t const N = node_stab.source->arity;
      aux_t constexpr signbit = 1 << (sizeof(aux_t) * 8 - 1);
      if(N & signbit)
        compile_error("Too many successors");
      aux_t const n = static_cast<aux_t>(N & ~signbit);
      size_t const niter = term.args.size() + 1;
      size_t i = 0;

      // The final iteration writes to the target node.  Prior iterations
      // allocate a new node.
      auto choose_storage = [&] {
          if(i+1 == niter)
          {
            this->destroy_target();
            return this->target_p;
          }
          else
            return this->node_alloc(*rt.node_t);
        };

      // Create the terminal node, which has the function's vtable as its data.
      tgt::value prev_node = choose_storage();
      prev_node.arrow(ND_VPTR) = rt.PartialTerminus_vt;
      prev_node.arrow(ND_TAG) = n;
      prev_node.arrow(ND_AUX) = n;
      prev_node.arrow(ND_SLOT0) = &node_stab.vtable;
      prev_node.arrow(ND_SLOT1) = nullptr;

      for(++i; i<niter; ++i)
      {
        // Create the data successor.
        tgt::value data = new_(term.args.at(i-1));

        // Build the spine node.
        tgt::value this_node = choose_storage();
        this_node.arrow(ND_VPTR) = rt.PartialSpine_vt;
        this_node.arrow(ND_TAG) = n;
        this_node.arrow(ND_AUX) = (n-i);
        this_node.arrow(ND_SLOT0) = prev_node;
        this_node.arrow(ND_SLOT1) = data;
        prev_node = this_node;
      }
    }

    result_type operator()(curry::Term const & term)
    {
      std::vector<tgt::value> child_data;
      child_data.reserve(term.args.size());

      {
        // The target node pointer is clobbered so that recursion can be used.
        // Save it for below.
        ValueSaver saver(target_p);

        // Each child needs to be allocated and initialized.  The children are
        // stored as i8* pointers.
        for(auto const & subexpr: term.args)
        {
          // Avoid creating FWD nodes for subexpressions.
          if(curry::Ref const * varref = subexpr.getvar())
          {
            // Retrieve the pointer to an existing node.
            tgt::value child = this->resolve_path_char_p(varref->pathid);
            child_data.push_back(child);
          }
          // Allocate a new node and place its contents with a recursive call.
          else
          {
            tgt::value child = this->node_alloc(*rt.char_t);
            child_data.push_back(child);
            // Clobber the root so the recursive call works.
            this->target_p = bitcast(child, node_pointer_type);
            (*this)(subexpr);
          }
        }
      }
      // (The original target is restored.)

      // Set the vtable and tag.
      node_init(
          this->target_p
        , this->module_stab
        , module_stab.lookup(term.qname)
        );

      // Set the child pointers.
      if(child_data.size() < 3)
        for(size_t i=0; i<child_data.size(); ++i)
          this->target_p.arrow(ND_SLOT0+i) = child_data[i];
      else
      {
        // Note: pseudo-C-code shown in comments for clarity.
        // char *& aux = node->slot0;
        ref aux = this->target_p.arrow(ND_SLOT0);
        // aux = array_alloc(n);
        aux = rt.Cy_ArrayAllocTyped(static_cast<aux_t>(child_data.size()));
        // char ** children = (char **)aux;
        value children = bitcast(aux, **types::char_());
        for(size_t i=0; i<child_data.size(); ++i)
          children[i] = child_data[i];
      }
    }

    result_type operator()(curry::NLTerm const & term)
    {
      for(curry::NLTerm::Step const & step: term.steps)
        this->new_(this->resolve_path(step.varid), step.term);
      return (*this)(*term.result);
    }

    result_type operator()(curry::ExternalCall const & term)
    {
      // E.g., "div" translates to "CyPrelude_div"
      std::string const symbol = "Cy" + term.qname.module + "_" + term.qname.name;
      tgt::function fun = extern_<tgt::function>(rt.stepfun_t, symbol);
      fun(this->target_p);
    }
  };

  struct FunctionCompiler : Rewriter
  {
    using result_type = void;
    using Rewriter::Rewriter;

    FunctionCompiler(
        compiler::ModuleSTab const & module_stab_
      , tgt::value const & root_p_
      , curry::Function const * fundef_ = nullptr
      , compiler::CompilerOptions const & options_ = compiler::CompilerOptions()
      )
      : Rewriter(module_stab_, root_p_, fundef_)
      , inductive_alloca(tgt::local(node_pointer_type))
      , options(options_)
    {
      if(options.enable_tracing) trace_step_start(rt, root_p);
    }

  private:

    // A static allocation holding a pointer in the target to the current
    // inductive position.  The allocation is needed when a FWD node is
    // encountered, so that it can communicate the new target to other sections
    // of code.
    tgt::ref inductive_alloca;

    compiler::CompilerOptions const & options;

    // Assuming a pull tab is now required using the current inductive node
    // as target, get its parent, the index of the inductive node, and the arity
    // of the parent.
    std::tuple<tgt::value, size_t, size_t>
    get_parent_itgt_arity(size_t pathid) const
    {
      // Handle continuations.
      if(pathid >= LOCAL_ID_START)
        return std::make_tuple(this->root_p, 1, 2);
      else
      {
        auto const & path = this->fundef->paths.at(pathid);
        assert(path.base != curry::freevar);
        assert(path.base != curry::bind);
        assert(path.base != curry::local);
        if(path.base == curry::nobase)
          return std::make_tuple(this->root_p, path.idx, this->fundef->arity);
        else
          return std::make_tuple(
              this->resolve_path(path.base)
            , path.idx
            , this->module_stab.lookup(path.typename_).source->arity
            );
      }
    }

    // Emits code to clean up any function-specific allocations and then
    // returns.
    void clean_up_and_return()
    {
      for(size_t i=LOCAL_ID_START; i<next_local_id; ++i)
        rt.CyMem_PopRoot(options.enable_tracing);
      if(options.enable_tracing) trace_step_end(rt, root_p);
      return_();
    }

    template<typename...Ts>
    void error(std::string const & message, Ts const &...ts)
    {
      rt.fprintf(rt.stderr_, message.c_str(), std::string(ts).c_str()...);
      rt.Cy_Suspend();
    }

  public:

    // Evaluates the condition to get a path index.
    size_t build_condition(curry::Rule const & condition)
    {
      if(curry::Ref const * cond = condition.getvar())
        return cond->pathid;
      else if(condition.getterm())
      {
        this->set_out_of_memory_handler_returning_here();
        value p = this->new_(this->resolve_path(next_local_id), condition);

        // Add the local allocation, p, as a new root for gc.
        rt.CyMem_PushRoot(p, options.enable_tracing);
        return next_local_id++;
      }
      else
        throw compile_error("Invalid branch condition");
    }

    void narrow(value var, size_t pathid, curry::CaseLhs const & example)
    {
      assert(cases.size());
      // Get the stencil.  Add one to the free variable, if necessary.
      ref stencil = var.arrow(ND_SLOT0);
      if_(stencil == (*rt.char_t)(nullptr), [&]{
        function G = get_generator_from_case(example, module_stab);
        if(!G.ptr()) return;
        var.arrow(ND_SLOT0) = G(var, var.arrow(ND_AUX));
        stencil = var.arrow(ND_SLOT0);
      });

      // Copy the stencil to produce an instance.
      value instance = rt.CyFree_CopyStencil(stencil);
      rt.CyMem_PushRoot(instance, options.enable_tracing);
      BOOST_SCOPE_EXIT((&rt)(&options))
        { rt.CyMem_PopRoot(options.enable_tracing); }
      BOOST_SCOPE_EXIT_END;

      tgt::value parent; size_t itgt; size_t arity;
      std::tie(parent, itgt, arity) = get_parent_itgt_arity(pathid);
      exec_pulltab(rt, parent, instance, itgt, arity);
    }

    std::pair<value, size_t> construct_continuation(curry::Branch const & branch)
    {
      set_out_of_memory_handler_returning_here();
      tgt::value inductive = this->inductive_alloca;

      // The binding arose from a non-trivial branch discriminator.
      std::string const name = get_base_function_name(*this->fundef);
      curry::Qname const qname{module_stab.source->name, name};
      auto & node_stab = this->module_stab.lookup(qname);

      // Begin to construct the replacement.  Successors are set below.
      value copy_of_root = this->node_alloc(*rt.node_t);
      move(root_p, copy_of_root, this->fundef->arity);
      root_p.arrow(ND_VPTR) = &*node_stab.auxvt.at(&branch);

      // See if any additional arguments are required and add them, if necessary.
      auto pathid_args = find_pathids_that_are_aux_arguments(*this->fundef, branch);
      size_t arity = 2 + pathid_args.size();
      if(pathid_args.empty())
      {
        root_p.arrow(ND_SLOT0) = bitcast(copy_of_root, *rt.char_t);
        root_p.arrow(ND_SLOT1) = bitcast(inductive, *rt.char_t);
      }
      else
      {
        value successors = set_extended_child_array(
            root_p
          , rt.Cy_ArrayAllocTyped(static_cast<aux_t>(arity))
          );
        successors[0] = copy_of_root;
        successors[1] = inductive;
        for(size_t i=2; i<arity; ++i)
          successors[i] = this->resolve_path(pathid_args.at(i-2));
      }
      return std::make_pair(inductive, arity);
    }

    result_type operator()(curry::Branch const & branch)
    {
      // Save and resove the next_local_id variable and manage freevar_alloca.
      // ATables are nested.  When a table is traversed, it may create a local
      // allocation for the branch condition.  That node is detached from the
      // main computation, so the compiler must emit calls to CyMem_PushRoot
      // upon allocation and matching calls to CyMem_PopRoot just before any
      // return.  Keeping track of the next_local_id variable is used to emit
      // the proper number of CyMem_PopRoot calls.  Additionally, when a local
      // allocation expires in this way, it is erased from the freevar_alloca
      // list so that a fresh alloca occurs if the local ID is reused.
      struct FreevarAllocaManager
      {
        FreevarAllocaManager(Rewriter & r)
          : rewriter(r), prev(r.next_local_id)
        {}
        ~FreevarAllocaManager()
        {
          while(rewriter.next_local_id > prev)
          {
            rewriter.next_local_id--;
            rewriter.freevar_alloca.erase(rewriter.next_local_id);
          }
        }
      private:
        Rewriter & rewriter;
        size_t prev;
      } _freevar_alloca_manager(*this);

      size_t const pathid = build_condition(branch.condition);
      // Declare the jump table in the target program.
      size_t const table_size = TAGOFFSET + branch.num_tag_cases();
      tgt::globalvar jumptable =
          tgt::static_((*rt.char_t)[table_size], tgt::flexible(".jtable"))
              .as_globalvar();

      // Declare placeholders for the pre-defined special labels.  Definitions
      // are provided below.
      std::vector<tgt::label> labels(TAGOFFSET);

      // Add a label for each constructor at the branch position.
      if(branch.iscomplete)
      {
        // Generate a separate code block for each case.
        for(auto const & case_: branch.cases)
        {
          // Allocate a label, then recursively generate the next step in its
          // scope.
          tgt::label tmp;
          tgt::scope _ = tmp;
          case_->action.visit(*this);
          labels.push_back(tmp);
        }
      }
      else
      {
        // All cases share one code block (where tag==CTOR).
        tgt::label tmp;
        {
          tgt::scope _ = tmp;
          type const ty = get_case_type(branch.cases);
          tgt::value const cond = *bitcast(
              &inductive_alloca.arrow(ND_SLOT0), *ty
            );
          auto sw = switch_(cond, labels[TAGOFFSET + FAIL]);
          for(auto const & case_: branch.cases)
          {
            label tmp2;
            {
              scope _ = tmp2;
              case_->action.visit(*this);
            }
            sw->addCase(get_case_value(case_->lhs).ptr(), tmp2.ptr());
          }
        }
        labels.push_back(tmp);
      }

      std::vector<tgt::block_address> addresses;
      for(tgt::label const & l: labels)
        addresses.push_back(&l);

      // FAIL case
      {
        tgt::scope _ = labels[TAGOFFSET + FAIL];
        (this->Rewriter::operator())(curry::Fail());
        clean_up_and_return();
      }

      // FREE case
      {
        tgt::scope _ = labels[TAGOFFSET + FREE];
        value inductive;
        if(pathid >= LOCAL_ID_START)
        {
          size_t arity;
          std::tie(inductive, arity) = construct_continuation(branch);
        }
        else
          inductive = static_cast<value>(this->inductive_alloca);
        narrow(inductive, pathid, branch.cases.front()->lhs);
        clean_up_and_return();
      }

      // FWD case
      {
        tgt::scope _ = labels[TAGOFFSET + FWD];
        // rt.printf("Fwd\n"); // DEBUG
        // Advance to the target of the FWD node and repeat the jump.
        tgt::value const inductive = bitcast(
            this->inductive_alloca.arrow(ND_SLOT0), node_pointer_type
          );
        this->inductive_alloca = inductive;
        tgt::value const index = inductive.arrow(ND_TAG) + TAGOFFSET;
        tgt::goto_(jumptable[index], labels);
      }

      // BINDING case
      {
        tgt::scope _ = labels[TAGOFFSET + BINDING];

        if(pathid >= LOCAL_ID_START)
        {
          value inductive;
          size_t arity;
          std::tie(inductive, arity) = construct_continuation(branch);
          exec_pullbind(this->rt, this->root_p, inductive, 1, arity);
        }
        else
        {
          tgt::value inductive = this->inductive_alloca;
          tgt::value parent; size_t itgt; size_t arity;
          std::tie(parent, itgt, arity) = get_parent_itgt_arity(pathid);
          exec_pullbind(this->rt, parent, inductive, itgt, arity);
        }
        clean_up_and_return();
      }

      // CHOICE case
      {
        tgt::scope _ = labels[TAGOFFSET + CHOICE];

        if(pathid >= LOCAL_ID_START)
        {
          value inductive;
          size_t arity;
          std::tie(inductive, arity) = construct_continuation(branch);
          exec_pulltab(this->rt, this->root_p, inductive, 1, arity);
        }
        else
        {
          // FIXME: why not zip the choice all the way to the root in one step?
          tgt::value inductive = this->inductive_alloca;
          tgt::value parent; size_t itgt; size_t arity;
          std::tie(parent, itgt, arity) = get_parent_itgt_arity(pathid);
          exec_pulltab(this->rt, parent, inductive, itgt, arity);
        }
        clean_up_and_return();
      }

      // OPER case
      {
        tgt::scope _ = labels[TAGOFFSET + OPER];
        tgt::value inductive = this->inductive_alloca;
        // Head-normalize the inductive node.
        vinvoke(inductive, VT_H);
        // Repeat the previous jump.
        tgt::value const index = inductive.arrow(ND_TAG) + TAGOFFSET;
        tgt::goto_(jumptable[index], labels);
      }

      // Add the label addresses to the jump table.
      jumptable.set_initializer(addresses);

      // Look up the inductive node.
      tgt::value const inductive = this->resolve_path(pathid);

      // Store the address of the inductive node in allocated space, then use
      // its tag to make the first jump.
      this->inductive_alloca = inductive;
      tgt::value const index = inductive.arrow(ND_TAG) + TAGOFFSET;
      tgt::goto_(jumptable[index], labels);
    }

    result_type operator()(curry::Rule const & rule)
    {
      // The rewrite step is always a series of allocations and memory stores
      // that finishes by attaching all allocated nodes to the root.  If memory
      // allocation fails, causing gc to run, restart here.
      this->set_out_of_memory_handler_returning_here();

      // Step.
      (this->Rewriter::operator())(rule);

      clean_up_and_return();
    }
  };

  // Helper to simplify the use of FunctionCompiler.
  void compile_function(
      compiler::ModuleSTab const & module_stab
    , curry::Function const & fun
    , compiler::CompilerOptions const & options
    )
  {
    try
    {
      auto step = module_stab.module_ir.getglobal(".step." + fun.name);
      function stepf = dyn_cast<function>(step);
      if(stepf->size() == 0) // function has no body
      {
        tgt::scope fscope = dyn_cast<function>(step);
        label entry_;
        goto_(entry_);
        tgt::scope _ = entry_;
        ::FunctionCompiler c(module_stab, arg("root_p"), &fun, options);
        return fun.def.visit(c);
      }
    }
    catch(...)
    {
      std::cerr
        << "[" << module_stab.source->name << "] In function " << fun.name
        << std::endl;
      throw;
    }
  }

  void compile_ctor_N_function_body(
      rt_h const & rt, size_t arity, compiler::CompilerOptions const & options
    )
  {
    value root_p = arg("root_p");
    ref child = local(*rt.node_t);
    label current = scope::current_label();

    auto handle_child = [&](size_t ichild){
      value call;
      size_t constexpr table_size = 7; // FAIL, FREE, FWD, BINDING, CHOICE, OPER, CTOR
      // The first jump normalizes children.
      label labels[table_size];
      globalvar jumptable1 =
          static_((*rt.char_t)[table_size], flexible(".jtable"))
              .as_globalvar();

      // The second jump looks for failures and choices pulled up in the first
      // step.  The new current label (after this function returns) is where
      // the second table lands on finding a constructor.
      current = label();
      globalvar jumptable2 =
          static_((*rt.char_t)[table_size], flexible(".jtable"))
              .as_globalvar();

      auto make_jump = [&](int i){
          ref index = local(rt.tag_t);
          index = child.arrow(ND_TAG) + TAGOFFSET;
          if_(
              index >(signed_) (rt.tag_t(table_size-1))
            , [&]{ index = rt.tag_t(table_size-1); }
            );
          switch(i)
          {
            case 1:
              goto_(jumptable1[index], labels);
              break;
            case 2:
              goto_(
                  jumptable2[index]
                , {   labels[TAGOFFSET+FAIL]
                    , current
                    , labels[TAGOFFSET+FAIL]
                    , labels[TAGOFFSET+BINDING]
                    , labels[TAGOFFSET+CHOICE]
                    , labels[TAGOFFSET+FAIL]
                    , current
                    }
                );
              break;
          }
        };

      // FAIL case.
      {
        scope _ = labels[TAGOFFSET + FAIL];
        if(arity > 2) vinvoke(root_p, VT_DESTROY);
        root_p.arrow(ND_VPTR) = rt.failed_vt;
        root_p.arrow(ND_TAG) = compiler::FAIL;
        return_();
      }

      // FREE case.
      {
        // If the variable has been narrowed in this computation, then clone
        // its stencil and perform a pull-tab.  Otherwise, ignore it, since an
        // unnarrowed variable can be considered part of a data value.
        scope _ = labels[TAGOFFSET + FREE];
        if_(rt.Cy_TestChoiceIsMade(child.arrow(ND_AUX))
          , [&]{
              value instance = rt.CyFree_CopyStencil(child.arrow(ND_SLOT0));
              exec_pulltab(rt, root_p, instance, ichild, arity);
              return_();
          });
        make_jump(2);
      }

      // FWD case.
      {
        scope _ = labels[TAGOFFSET + FWD];
        // rt.printf("Fwd\n"); // DEBUG
        child = bitcast(child.arrow(ND_SLOT0), *rt.node_t);
        make_jump(1);
      }

      // BINDING case.
      {
        scope _ = labels[TAGOFFSET + BINDING];
        if(options.enable_tracing) trace_step_start(rt, root_p);
        exec_pullbind(rt, root_p, child, ichild, arity);
        if(options.enable_tracing) trace_step_end(rt, root_p);
        return_();
      }

      // CHOICE case.
      {
        scope _ = labels[TAGOFFSET + CHOICE];
        if(options.enable_tracing) trace_step_start(rt, root_p);
        exec_pulltab(rt, root_p, child, ichild, arity);
        if(options.enable_tracing) trace_step_end(rt, root_p);
        return_();
      }

      // OPER case (recursive call to child.N, then loop back to the jump).
      {
        scope _ = labels[TAGOFFSET + OPER];
        child.arrow(ND_VPTR).arrow(VT_N)(child);
        make_jump(1);
      }

      // CTOR case #1 (recursive call to child.N then second jump).
      {
        scope _ = labels[TAGOFFSET + CTOR];
        call = child.arrow(ND_VPTR).arrow(VT_N)(child);
        make_jump(2);
      }

      // Initialize the jump tables.
      block_address addresses1[table_size] =
          {&labels[0], &labels[1], &labels[2], &labels[3], &labels[4], &labels[5], &labels[6]};
      jumptable1.set_initializer(addresses1);

      // The FWD and OPER rules are unreachable, so just put FAIL there.  Treat
      // FREE variables like constructors, since they can appear in a value.
      block_address addresses2[table_size] =
      {
          &labels[TAGOFFSET+FAIL]    // FAIL
        , &current                   // FREE
        , &labels[TAGOFFSET+FAIL]    // FWD
        , &labels[TAGOFFSET+BINDING] // BINDING
        , &labels[TAGOFFSET+CHOICE]  // CHOICE
        , &labels[TAGOFFSET+FAIL]    // OPER
        , &current                   // CTOR
      };
      jumptable2.set_initializer(addresses2);

      // Kick off the process.
      make_jump(1);
      return call;
    };

    switch(arity)
    {
      case 2:
      {
        scope _ = current;
        child = bitcast(root_p.arrow(ND_SLOT1), *rt.node_t);
        handle_child(1);
      }
      case 1:
      {
        scope _ = current;
        child = bitcast(root_p.arrow(ND_SLOT0), *rt.node_t);
        handle_child(0).set_attribute(tailcall);
      }
      case 0: break;
      default:
      {
        value children = bitcast(
            root_p.arrow(ND_SLOT0), **rt.node_t
          );
        value last_call;
        for(size_t ichild=0; ichild<arity; ++ichild)
        {
          scope _ = current;
          child = children[ichild];
          last_call = handle_child(ichild);
        }
        last_call.set_attribute(tailcall);
        break;
      }
    }

    // Put the return in the last scope.
    {
      scope _ = current;
      return_();
    }
  }

  // Compiles the generator function for the specified data type, which belongs
  // in the given module.
  function compile_generator(
      compiler::ModuleSTab & module_stab, curry::Module const & cymodule
    , curry::DataType const & dtype
    )
  {
    auto const & rt = module_stab.rt();
    auto & C = dtype.constructors;
    size_t const N = C.size();
    assert(N);

    std::string const name = dtype.name + ".gen";
    function G(module_stab.module_ir->getFunction(name.c_str()));
    if(!G.ptr())
    {
      G = static_<function>(rt.genfun_t, name, {"node_p", "id"});
      scope _ = G;
      value node_p = arg("node_p");
      value id = arg("id");
      Rewriter rewriter(module_stab, node_p);
      rewriter.set_out_of_memory_handler_returning_here();
      // If the (only) constructor has successors, then we must make a choice
      // between it and "failed" so that the constraint store can be made
      // aware of the first choice.
      if(N==1 && C[0].arity > 0)
      {
        auto choice = rewriter.node_alloc(*rt.node_t);
        value lhs = rewriter.new_(Qname{cymodule.name, C[0].name});
        value rhs = rewriter.new_(curry::Rule()); // failure
        choice.arrow(ND_VPTR) = rt.choice_vt;
        choice.arrow(ND_TAG) = compiler::CHOICE;
        choice.arrow(ND_AUX) = id;
        choice.arrow(ND_SLOT0) = lhs;
        choice.arrow(ND_SLOT1) = rhs;
        return_(choice);
      }
      else
      {
        using curry::Constructor;
        std::function<value(Constructor const *, Constructor const *, bool)> mktree =
            [&](Constructor const * begin, Constructor const * end, bool is_head)
            {
              size_t const n = end - begin;
              assert(n);
              if(n==1)
                return rewriter.new_(
                    Qname{cymodule.name, begin->name}
                  );
              else
              {
                auto choice = rewriter.node_alloc(*rt.node_t);
                Constructor const * middle = begin + (end-begin)/2;
                value lhs = mktree(begin, middle, false);
                value rhs = mktree(middle, end, false);
                choice.arrow(ND_VPTR) = rt.choice_vt;
                choice.arrow(ND_TAG) = compiler::CHOICE;
                // The head of the tree has the designated choice ID.
                choice.arrow(ND_AUX) = is_head ? id : rt.Cy_NextChoiceId++;
                choice.arrow(ND_SLOT0) = lhs;
                choice.arrow(ND_SLOT1) = rhs;
                return choice;
              }
            };
        return_(mktree(&*C.begin(), &*C.end(), true));
      }
    }
    return G;
  }
  
  void compile_ctor_vtable(
      tgt::global & vt
    , curry::DataType const & dtype
    , size_t ictor
    , compiler::ModuleSTab & module_stab
    , compiler::CompilerOptions const & options
    )
  {
    compiler::rt_h const & rt = module_stab.rt();

    // Look up the N function, or define it as needed.
    auto const & ctor = dtype.constructors[ictor];
    tgt::module & module_ir = module_stab.module_ir;
    std::string const n_name = ".N." + std::to_string(ctor.arity);
    function N(module_ir->getFunction(n_name.c_str()));
    if(!N.ptr())
    {
      N = static_<function>(
          rt.stepfun_t, n_name, {"root_p"}
        , [&]{ compile_ctor_N_function_body(rt, ctor.arity, options); }
        );
    }
    vt.set_initializer(_t(
        &rt.Cy_NoAction
      , &N
      , &rt.Cy_Label(ctor.name)
      , &rt.Cy_Sentinel()
      , ictor
      , &rt.Cy_Arity(ctor.arity)
      , &rt.Cy_Succ(ctor.arity)
      , &rt.Cy_Succ(ctor.arity)
      , &rt.Cy_Destroy(ctor.arity)
      , &rt.CyVt_Equal(module_stab.source->name, dtype.name)
      , &rt.CyVt_Equate(module_stab.source->name, dtype.name)
      , &rt.CyVt_NsEquate(module_stab.source->name, dtype.name)
      , &rt.CyVt_Compare(module_stab.source->name, dtype.name)
      , &rt.CyVt_Show(module_stab.source->name, dtype.name)
      ));
  }

  void compile_function_vtable(
      tgt::global & vt
    , curry::Function const & fun
    , compiler::ModuleSTab & module_stab
    )
  {
    auto const & rt = module_stab.rt();
    auto const & module_ir = module_stab.module_ir;

    std::string const stepname = ".step." + fun.name;
    function step(module_ir->getFunction(stepname.c_str()));
    if(!step.ptr())
      step = static_<function>(rt.stepfun_t, stepname, {"root_p"});

    std::string const nname = ".N." + fun.name;
    function N(module_ir->getFunction(nname.c_str()));
    if(!N.ptr())
    {
      N = static_<function>(
          rt.stepfun_t, nname, {"root_p"}
        , [&]{
            tgt::value root_p = arg("root_p");
            step(root_p);
            vinvoke(root_p, VT_N, tailcall);
            return_();
          }
        );
    }

    std::string const hname = ".H." + fun.name;
    function H(module_ir->getFunction(hname.c_str()));
    if(!H.ptr())
    {
      H = static_<function>(
          rt.stepfun_t, hname, {"root_p"}
        , [&]{
            tgt::value root_p = arg("root_p");
            step(root_p);
            vinvoke(root_p, VT_H, tailcall);
            return_();
          }
        );
    }

    vt.set_initializer(_t(
        &H
      , &N
      , &rt.Cy_Label(fun.name)
      , &rt.Cy_Sentinel()
      , sprite::compiler::OPER
      , &rt.Cy_Arity(fun.arity)
      , &rt.Cy_Succ(fun.arity)
      , &rt.Cy_Succ(fun.arity)
      , &rt.Cy_Destroy(fun.arity)
      , &extern_(rt.vtable_t, ".vt.OPER.Prelude.prim_error_cmp_fun")
      , &extern_(rt.vtable_t, ".vt.OPER.Prelude.prim_error_cmp_fun")
      , &extern_(rt.vtable_t, ".vt.OPER.Prelude.prim_error_cmp_fun")
      , &extern_(rt.vtable_t, ".vt.OPER.Prelude.prim_error_cmp_fun")
      , &extern_(rt.vtable_t, ".vt.OPER.Prelude.prim_label")
      ));
  }

  /**
   * @brief Builds the definition of an aux function for the given function at
   * the specified branch.
   *
   * An aux function is used when a non-trivial branch discriminator evaluates to
   * a choice, free variable, or binding..  For example, in the sign function:
   *
   *   sign a = if a<0 then -1 else if a>0 then 1 else 0
   *
   * either expression a<0 or a>0 could produce one of those values.  (Actualy,
   * with some analysis we could prove that the second expression cannot
   * produce a choice if the first did not, but the compiler does not attempt
   * that analysis.) The problem in that case is that the result after
   * pull-tabbing cannot be written in terms of the original function.  To
   * execute a pull-tab step, the remainder of the function, starting with the
   * branch where the choice was encountered, must be compiled.  The result can
   * be written in terms of that function.  For sign, there are two aux
   * functions.
   *
   *     sign#branch0 (sign a) c = if c then -1 else if a>0 then 1 else 0
   *     sign#branch1 (sign a) c = if c then 1 else 0
   *
   * An aux function always has at least two successors: (1) the original root
   * node (which must be an application of the main function), and (2) the
   * bound expression.  In addition, any free variables allocated within the
   * bound expression that are subsequently used are appended to the argument
   * list and changed to regular variables in the definition of the aux
   * function.  Note that due to (1), this is a somewhat irregular case in that
   * normally a function would never pattern-match against a function.
   *
   * To define an aux function, we do the following:
   *
   *   1. Adjust the name by appending #branchN, where N is a unique numeric ID
   *      provided for the branch in question.
   *   2. Rebase every variable reference, adding an initial index of 0.  This
   *      is because the original root is moved to the first argument position.
   *   3. Replace the branch discriminator with a reference to index 1.
   *   4. Identify and tranform free variables that must appear as arguments to
   *      the aux function.
   *   5. Copy the function from the given branch to the end.
   */
  curry::Function build_aux_function(
      curry::Function const & fun, curry::Branch b, size_t id
    )
  {
    auto pathid_args = find_pathids_that_are_aux_arguments(fun, b);

    curry::Function aux;
    aux.is_aux = true;
    aux.name = fun.name + "#branch" + std::to_string(id);
    aux.arity = 2 + pathid_args.size();
    aux.paths = fun.paths;
    aux.variable_expansions = fun.variable_expansions;

    // There will be two new variables, for the new base and the discriminator.
    size_t const newbase = aux.paths.size();
    curry::Ref const cond{newbase + 1};

    for(auto & pathelem: aux.paths)
    {
      if(pathelem.base == curry::nobase)
        pathelem.base = newbase;
    }
    // Use a representative tuple with the correct arity for baseless paths.
    Qname const tuple_typename =
      {"Prelude", "(," + std::string(pathid_args.size(), ',') + ")"};
    aux.paths.emplace_back(
        curry::Function::PathElem{curry::nobase, 0, tuple_typename}
      );
    aux.paths.emplace_back(
        curry::Function::PathElem{curry::nobase, 1, tuple_typename}
      );

    // Transform the free variables that require it into normal variables to be
    // passed as args.
    size_t next = 2;
    for(size_t i: pathid_args)
    {
      aux.paths.at(i) =
        curry::Function::PathElem{curry::nobase, next++, tuple_typename};
    }

    // Copy the branch, but share everything under it (because the cases are
    // stored in shared_ptrs).  Then modify the discriminator.  It is just a
    // reference to variable "cond," now.
    curry::Branch branch = b;
    branch.condition = curry::Rule(cond);
    aux.def = curry::Definition(branch);

    return aux;
  }

  // Traverses a function definition and compiles an aux function for each
  // branch that has a non-trivial condition.
  struct CompileAuxVisitor
  {
    using result_type = void;

    void operator()(curry::Branch const & branch) 
    {
      // Recurse to find all branches.  The deeper branches must be processed
      // first because they are used when compiling shallower branches.
      for(auto const & case_: branch.cases)
        case_->action.visit(*this);

      if(!branch.condition.is_trivial())
      {
        rt_h const & rt = module_stab.rt();
        size_t const id = counter++;

        // Build the aux function definition.
        curry::Function aux = build_aux_function(
            node_stab.function(), branch, id
          );

        // Build the vtable and then update the symbol table of the main function.
        std::string const vtname =
            ".vt.OPER." + module_stab.source->name + "." + aux.name;
        tgt::global vt = extern_(rt.vtable_t, vtname);
        compile_function_vtable(vt, aux, module_stab);
        using sprite::backend::globalvar;
        std::shared_ptr<globalvar> tmp(new globalvar(vt.as_globalvar()));
        node_stab.auxvt[&branch] = tmp;

        // Compile the aux function.
        compile_function(module_stab, aux, options);
      }
    }

    void operator()(curry::Rule const &) {}

    ModuleSTab & module_stab;
    NodeSTab & node_stab;
    compiler::CompilerOptions const & options;
    size_t counter;
  };

  void compile_aux_functions(
      compiler::ModuleSTab & module_stab
    , curry::Function const & fun
    , compiler::CompilerOptions const & options
    )
  {
    curry::Qname const qname{module_stab.source->name, fun.name};
    compiler::NodeSTab & node_stab = module_stab.lookup(qname);
    CompileAuxVisitor mkaux{module_stab, node_stab, options, 0};
    fun.def.visit(mkaux);
  }
}

namespace sprite { namespace compiler
{
  // Performs a pull tab at node * src, having the specified arity.  itgt is
  // the index of the successor that is a choice.
  void exec_pulltab(
      rt_h const & rt, value const & src, value const & tgt, size_t itgt
    , size_t arity
    )
  {
    // Note: this function has been generalized for choice_arity != 2, though
    // that code is not in use (yet).
    size_t const choice_arity = 2;
    assert(choice_arity > 0);
    label out_of_memory_handler = rt.make_restart_point();
    std::vector<value> values;
    for(size_t cid=0; cid<choice_arity; ++cid)
    {
      value arg = rt.node_alloc(*rt.node_t, out_of_memory_handler);
      values.push_back(arg);
    }
    // Done allocating everything. Now it's okay to partially initialize the nodes.
    for(size_t cid=0; cid<choice_arity; ++cid)
    {
      auto & arg = values[cid];
      arg.arrow(ND_VPTR) = src.arrow(ND_VPTR);
      arg.arrow(ND_TAG) = src.arrow(ND_TAG);
      // arg.arrow(ND_SLOT0) = nullptr; // DEBUG
      // arg.arrow(ND_SLOT1) = nullptr; // DEBUG
      // OK to ignore aux.  The root cannot be a choice because a choice
      // has no step function.
    }

    if(arity < 3)
    {
      for(size_t i=0; i<arity; ++i)
      {
        if(i == itgt)
        {
          if(choice_arity < 3)
          {
            for(size_t cid=0; cid<choice_arity; ++cid)
              values[cid].arrow(ND_SLOT0+i) = tgt.arrow(ND_SLOT0+cid);
          }
          else
          {
            value choices = get_extended_child_array(rt, tgt);
            for(size_t cid=0; cid<choice_arity; ++cid)
              values[cid].arrow(ND_SLOT0+i) = choices[cid];
          }
        }
        else
        {
          value tmp = src.arrow(ND_SLOT0+i);
          for(auto & arg: values)
            arg.arrow(ND_SLOT0+i) = tmp;
        }
      }
    }
    else
    {
      std::vector<value> child_arrays;
      // The first argument array is stolen from the root node.
      child_arrays.push_back(
          set_extended_child_array(values[0], src.arrow(ND_SLOT0))
        );
      // The others are allocated.
      for(size_t j=1; j<choice_arity; ++j)
      {
        child_arrays.push_back(
            set_extended_child_array(
                values[j]
              , rt.Cy_ArrayAllocTyped(static_cast<aux_t>(arity))
              )
          );
      }

      // The LHS argument array is stolen from the root node.
      for(size_t i=0; i<arity; ++i)
      {
        if(i == itgt)
        {
          if(choice_arity < 3)
          {
            for(size_t cid=0; cid<choice_arity; ++cid)
              child_arrays[cid][i] = tgt.arrow(ND_SLOT0+cid);
          }
          else
          {
            value choices = get_extended_child_array(rt, tgt);
            for(size_t cid=0; cid<choice_arity; ++cid)
              child_arrays[cid][i] = choices[cid];
          }
        }
        else
        {
          value tmp = child_arrays[0][i];
          for(size_t j=1; j<choice_arity; ++j)
            child_arrays[j][i] = tmp;
        }
      }
    }

    // Do not call this->destroy_target.  The successor list was stolen.
    src.arrow(ND_VPTR) = rt.choice_vt;
    src.arrow(ND_TAG) = compiler::CHOICE;
    src.arrow(ND_AUX) = tgt.arrow(ND_AUX);

    switch(choice_arity)
    {
      case 2:
        src.arrow(ND_SLOT1) = bitcast(values[1], *types::char_());
      case 1:
        src.arrow(ND_SLOT0) = bitcast(values[0], *types::char_());
      case 0:
        break;
      default:
      {
        value choice_successors = set_extended_child_array(
            src
          , rt.Cy_ArrayAllocTyped(static_cast<aux_t>(choice_arity))
          );
        for(size_t j=0; j<choice_arity; ++j)
          choice_successors[j] = bitcast(values[j], *types::char_());
      }
    }
  }

  void exec_pullbind(
      rt_h const & rt, value const & src, value const & tgt, size_t itgt
    , size_t arity
    )
  {
    label out_of_memory_handler = rt.make_restart_point();
    value tmp = rt.node_alloc(*rt.node_t, out_of_memory_handler);
    tmp.arrow(ND_VPTR) = src.arrow(ND_VPTR);
    tmp.arrow(ND_TAG) = src.arrow(ND_TAG);

    if(arity < 3)
    {
      for(size_t i=0; i<arity; ++i)
      {
        if(i == itgt)
          tmp.arrow(ND_SLOT0+i) = tgt.arrow(ND_SLOT0);
        else
          tmp.arrow(ND_SLOT0+i) = src.arrow(ND_SLOT0+i);
      }
    }
    else
    {
      value children = set_extended_child_array(tmp, src.arrow(ND_SLOT0));
      children[itgt] = tgt.arrow(ND_SLOT0);
    }

    src.arrow(ND_VPTR) = tgt.arrow(ND_VPTR);
    src.arrow(ND_TAG) = BINDING;
    src.arrow(ND_SLOT0) = tmp;
    src.arrow(ND_SLOT1) = tgt.arrow(ND_SLOT1);
  }

  tgt::value vinvoke(tgt::value const & node_p, compiler::VtMember member)
    { return node_p.arrow(compiler::ND_VPTR).arrow(member)(node_p); }

  tgt::value vinvoke(
      tgt::value const & node_p, compiler::VtMember member, tgt::attribute attr
    )
  { return vinvoke(node_p, member).set_attribute(attr); }

  void trace_step_start(rt_h const & rt, value const & root_p)
  {
    rt.printf("S> --- ");
    rt.CyTrace_ShowIndent();
    rt.Cy_ReprFull(root_p, rt.stdout_());
    rt.fflush(nullptr);
  }

  void trace_step_end(rt_h const & rt, value const & root_p)
  {
    rt.printf("S> +++ ");
    rt.CyTrace_ShowIndent();
    rt.Cy_ReprFull(root_p, rt.stdout_());
    rt.fflush(nullptr);
  }

  void trace_step_tmp(rt_h const & rt, value const & root_p)
  {
    rt.printf("T> +++ ");
    rt.CyTrace_ShowIndent();
    rt.Cy_ReprFull(root_p, rt.stdout_());
    rt.fflush(nullptr);
  }

  ModuleSTab::ModuleSTab(
      curry::Module const & src, llvm::LLVMContext & context
    )
    : source(&src), module_ir(src.name, context), nodes()
  {
    // Load the compiler data (e.g., headers) with the module in scope.
    scope _ = module_ir;
    this->headers.reset(new Headers());
  }

  ModuleSTab::ModuleSTab(
      curry::Module const & src, sprite::backend::module const & M
    )
    : source(&src), module_ir(M), nodes()
  {
    // Load the compiler data (e.g., headers) with the module in scope.
    scope _ = module_ir;
    this->headers.reset(new Headers());
  }

  // Looks up a node symbol table from the library.
  compiler::NodeSTab const &
  ModuleSTab::lookup(curry::Qname const & qname) const
  {
    try
      { return this->nodes.at(qname); }
    catch(std::out_of_range const & e)
    {
      // DIAGNOSTIC
      // std::cerr
      //   << "Contents of symbol table for module " << this->source->name
      //   << "\n";
      // for(auto const & node: this->nodes)
      // {
      //   std::cerr << "  " << node.first.str();
      //   if(node.second.tag == CTOR)
      //     std::cerr << " (CONSTRUCTOR)\n";
      //   else
      //     std::cerr << " (FUNCTION)\n";
      // }
      // 
      // std::cerr << "(End contents of symbol table)\n" << std::endl;
      
      throw compile_error("symbol '" + qname.str() + "' not found.");
    }
  }

  compiler::NodeSTab & ModuleSTab::lookup(curry::Qname const & qname)
  {
    return const_cast<compiler::NodeSTab &>(
        static_cast<ModuleSTab const *>(this)->lookup(qname)
      );
  }

  // Constructs an expression at the given node address.
  value construct(
      compiler::ModuleSTab const & module_stab
    , tgt::value const & root_p
    , curry::Rule const & term
    )
  {
    ::Rewriter c(module_stab, root_p);
    term.visit(c);
    return c.target_p;
  }

  void prettyprint(curry::Library const & lib)
  {
    using std::cout;
    using std::endl;
    for(auto const & module: lib.modules)
    {
      cout << "Module: " << module.name << "\n";
      cout << "Imports:";
      for(auto const & import: module.imports)
        cout <<  " " << import;
      cout << "\n";
      cout << "DataTypes:";
      for(auto const & dtype: module.datatypes)
      {
        cout << "\n    " << dtype.name << " = ";
        bool first = true;
        for(auto const & ctor: dtype.constructors)
        {
          if(!first) { cout << " | "; } else { first = false; }
          cout << ctor.name << "(" << ctor.arity << ")";
        }
      }
      cout << "\nFunctions:";
      for(auto const & fun: module.functions)
      {
        cout << "\n    " << fun.name << "(" << fun.arity << ")";
      }
      cout << "\n\n";
    }
  }

  void compile(
      curry::Module const & cymodule
    , compiler::LibrarySTab & stab
    , llvm::LLVMContext & context
    , compiler::CompilerOptions const & options
    )
  {
    // Create a new LLVM module and symbol table entry.
    auto rv = stab.modules.emplace(
        cymodule.name, ModuleSTab{cymodule, context}
      );
    // OK if the module symbol table already exists.  The IR may have been
    // loaded from a file.
    compiler::ModuleSTab & module_stab = rv.first->second;

    tgt::module & module_ir = module_stab.module_ir;
    auto const & rt = module_stab.rt();
    
    // Set the module as the current scope.  Subsequent statements will add
    // functions, type definitions, and data to this module.
    tgt::scope _ = module_ir;
  
    // The loop body for procesing one module.  The primary module and imported
    // modules are handled separately.  For the primary module, compile code
    // and create the vtables.  For the non-primary (i.e., imported) modules,
    // create declarations only.
    auto process_module = [&](curry::Module const & cymodule, bool is_primary)
    {
      // Add the vtable for each constructor to the module.
      for(auto const & dtype: cymodule.datatypes)
      {
        size_t tag = compiler::CTOR;
        size_t const N = dtype.constructors.size();
        for(size_t ictor=0; ictor<N; ++ictor)
        {
          auto const & ctor = dtype.constructors[ictor];
          // Create or find the vtable and maybe compile code to fill it.
          std::string const vtname = ".vt.CTOR." + cymodule.name + "." + ctor.name;
          tgt::global vt = extern_(rt.vtable_t, vtname);
          if(is_primary && !vt.has_initializer())
            compile_ctor_vtable(vt, dtype, ictor, module_stab, options);
  
          // Update the symbol tables.
          module_stab.nodes.emplace(
              curry::Qname{cymodule.name, ctor.name}
            , compiler::NodeSTab(ctor, vt.as_globalvar(), nullptr, tag++)
            );
        }

        // Circle back and fill in the generator.  It can be compiled only
        // after all constructors for the type have been added to the symbol
        // table.
        function G = compile_generator(module_stab, cymodule, dtype);
        for(auto const & ctor: dtype.constructors)
          module_stab.lookup({cymodule.name, ctor.name}).generator = G;
      }

      // Special cases for the Prelude data types.
      if(cymodule.name == "Prelude")
      {
        auto & rt = module_stab.rt();

        // FIXME: These definitions should probably be added to the Prelude so
        // that const_cast is not needed.  When this was added, PAKCS was
        // temporarily unavailable and so the Prelude could not be updated.
        auto & dts = const_cast<curry::Module &>(cymodule).datatypes;
        char const * builtins[] = {"Char", "Int", "Float", "IO", "Success"};
        for(std::string builtin: builtins)
        {
          curry::Constructor node; node.name = builtin; node.arity = 0;
          dts.emplace_back(curry::DataType{builtin, {node}});
          auto const & ctor = dts.back().constructors.front();
          module_stab.nodes.emplace(
              curry::Qname{"Prelude", builtin}
            , compiler::NodeSTab(
                  ctor, rt.CyVt_Builtin(builtin).as_globalvar()
                , rt.Cy_NoGenerator(builtin), ctor.arity
                )
            );
        }
      }

      // Update the symbol tables with the forward declarations of functions.
      for(auto const & fun: cymodule.functions)
      {
        // Create or find the vtable.
        std::string const vtname = ".vt.OPER." + cymodule.name + "." + fun.name;
        tgt::global vt = extern_(rt.vtable_t, vtname);
        if(is_primary && !vt.has_initializer())
          compile_function_vtable(vt, fun, module_stab);

        // Update the symbol tables.
        module_stab.nodes.emplace(
            curry::Qname{cymodule.name, fun.name}
          , compiler::NodeSTab(fun, vt.as_globalvar(), compiler::OPER)
          );
      }
  
      // Compile the functions.
      if(is_primary)
      {
        for(auto const & fun: cymodule.functions)
        {
          compile_aux_functions(module_stab, fun, options);
          compile_function(module_stab, fun, options);
        }
      }

      // Special cases for the Prelude functions.
      if(cymodule.name == "Prelude")
      {
        auto & rt = module_stab.rt();

        // Update symbol table entries for Prelude.? with the built-in
        // choice implementation.
        {
          auto & choice_stab = module_stab.lookup({"Prelude", "?"});
          choice_stab.vtable.reset(rt.choice_vt.as_globalvar());
          choice_stab.tag = CHOICE;
        }
      }

      // DIAGNOSTIC
      // module_ir.ptr()->dump();
      llvm::verifyModule(*module_ir.ptr(), llvm::PrintMessageAction);
    };

    // Process the imports.
    for(auto const & import: cymodule.imports)
    {
      auto p = stab.modules.find(import);
      if(p == stab.modules.end())
        throw compile_error("Imported module \"" + import + "\" was not found");
      process_module(*p->second.source, false);
    }

    // Process the primary module.
    process_module(cymodule, true);
  }
}}

