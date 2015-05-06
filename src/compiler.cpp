#include <iostream>
#include "sprite/curryinput.hpp"
#include "sprite/compiler.hpp"
#include "sprite/runtime.hpp"
#include <algorithm>

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

  // Performs a pull tab at node * src, having the specified arity.  itgt is
  // the index of the successor that is a choice.
  void exec_pulltab(
      rt_h const & rt, value const & src, value const & tgt, size_t arity
    , size_t itgt, label const & out_of_memory_handler
    )
  {
    value lhs = rt.node_alloc(*rt.node_t, out_of_memory_handler);
    value rhs = rt.node_alloc(*rt.node_t, out_of_memory_handler);
    lhs.arrow(ND_VPTR) = src.arrow(ND_VPTR);
    lhs.arrow(ND_TAG) = src.arrow(ND_TAG);
    rhs.arrow(ND_VPTR) = src.arrow(ND_VPTR);
    rhs.arrow(ND_TAG) = src.arrow(ND_TAG);
    // OK to ignore aux.  The root cannot be a choice because a choice
    // has no step function.

    if(arity < 3)
    {
      for(size_t i=0; i<arity; ++i)
      {
        if(i == itgt)
        {
          lhs.arrow(ND_SLOT0+i) = tgt.arrow(ND_SLOT0);
          rhs.arrow(ND_SLOT0+i) = tgt.arrow(ND_SLOT1);
        }
        else
        {
          value tmp = src.arrow(ND_SLOT0+i);
          lhs.arrow(ND_SLOT0+i) = tmp;
          rhs.arrow(ND_SLOT0+i) = tmp;
        }
      }
    }
    else
    {
      // The LHS argument array is stolen from the root node.
      value lhs_children = set_extended_child_array(
          lhs, src.arrow(ND_SLOT0)
        );
      // The RHS argument array is allocated.
      value rhs_children = set_extended_child_array(
          rhs, rt.Cy_ArrayAllocTyped(static_cast<aux_t>(arity))
        );

      for(size_t i=0; i<arity; ++i)
      {
        if(i == itgt)
        {
          lhs_children[i] = tgt.arrow(ND_SLOT0);
          rhs_children[i] = tgt.arrow(ND_SLOT1);
        }
        else
        {
          // Nothing to do for LHS.
          rhs_children[i] = lhs_children[i];
        }
      }
    }
    // Do not call this->destroy_target.  The successor list was stolen.
    src.arrow(ND_VPTR) = rt.choice_vt;
    src.arrow(ND_TAG) = compiler::CHOICE;
    src.arrow(ND_AUX) = tgt.arrow(ND_AUX);
    src.arrow(ND_SLOT0) = bitcast(lhs, *types::char_());
    src.arrow(ND_SLOT1) = bitcast(rhs, *types::char_());
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
    // is a referent to node_t* in the target.  If the variable referenced is a
    // free variable, then space for it will be allocated if necessary.
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

    // For variable instantiation.
    result_type operator()(curry::Free const &)
    {
      this->destroy_target();
      this->target_p.arrow(ND_VPTR) = rt.freevar_vt;
      this->target_p.arrow(ND_TAG) = compiler::FREE;
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
    // The final "apply" in a partial evaluation is shown below.  The terms
    // beginning with "P." are the partial nodes created by this function.
    //
    // Expression: (f a0 ... ak) an
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
      , bool enable_tracing_ = false
      )
      : Rewriter(module_stab_, root_p_, fundef_)
      , inductive_alloca(tgt::local(node_pointer_type))
      , enable_tracing(enable_tracing_)
    {
      if(enable_tracing) trace_step_start(rt, root_p);
    }

  private:

    // A static allocation holding a pointer in the target to the current
    // inductive position.  The allocation is needed when a FWD node is
    // encountered, so that it can communicate the new target to other sections
    // of code.
    tgt::ref inductive_alloca;
    bool enable_tracing;

    // Emits code to clean up any function-specific allocations and then
    // returns.
    void clean_up_and_return()
    {
      for(size_t i=LOCAL_ID_START; i<next_local_id; ++i)
        rt.CyMem_PopRoot(enable_tracing);
      if(enable_tracing) trace_step_end(rt, root_p);
      return_();
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
        rt.CyMem_PushRoot(p, enable_tracing);
        return next_local_id++;
      }
      else
        throw compile_error("Invalid branch condition");
    }

    template<typename Cases>
    compiler::tag_t instantiate_variable(value var, Cases const & cases_)
    {
      assert(cases.size());
      Cases cases = cases_;
      std::reverse(cases.begin(), cases.end());

      compiler::tag_t tag = compiler::CHOICE;
      if(cases.size() == 1)
      {
        ValueSaver saver(target_p, var);
        auto const & lhs = cases.front()->lhs;
        (this->Rewriter::operator())(lhs);
        tag = get_case_tag(lhs, module_stab);
      }
      else
      {
        set_out_of_memory_handler_returning_here();
        ValueSaver saver(target_p);

        size_t i=0;
        size_t const n = cases.size();
        auto choose_storage = [&] {
          if(i+1 == n)
            // No need to destroy var.
            return var;
          else
            return this->node_alloc(*rt.node_t);
        };

        value rhs_node = choose_storage();
        target_p = rhs_node;
        (this->Rewriter::operator())(cases.front()->lhs);

        for(++i; i<n; ++i)
        {
          value lhs_node = new_(cases.at(i)->lhs);

          value choice_node = choose_storage();
          choice_node.arrow(ND_VPTR) = rt.choice_vt;
          choice_node.arrow(ND_TAG) = compiler::CHOICE;
          choice_node.arrow(ND_SLOT0) = lhs_node;
          choice_node.arrow(ND_SLOT1) = rhs_node;

          rhs_node = choice_node;
        }
      }
      return tag;
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
        if(!branch.isflex)
        {
          rt.Cy_Suspend();
          clean_up_and_return();
        }
        else
        {
          tgt::value inductive = this->inductive_alloca;
          compiler::tag_t tag = instantiate_variable(inductive, branch.cases);
          tgt::goto_(jumptable[tag+TAGOFFSET], labels);
        }
      }

      // FWD case
      {
        tgt::scope _ = labels[TAGOFFSET + FWD];
        // Advance to the target of the FWD node and repeat the jump.
        tgt::value const inductive = bitcast(
            this->inductive_alloca.arrow(ND_SLOT0), node_pointer_type
          );
        this->inductive_alloca = inductive;
        tgt::value const index = inductive.arrow(ND_TAG) + TAGOFFSET;
        tgt::goto_(jumptable[index], labels);
      }

      // CHOICE case
      {
        tgt::scope _ = labels[TAGOFFSET + CHOICE];

        // FIXME: would an allocated branch condition would be reclaimed?
        // There is a local stack of detached roots.  It would be great to add
        // the local computation to it only just before the collector runs.
        set_out_of_memory_handler_returning_here();
        tgt::value inductive = this->inductive_alloca;

        if(pathid >= LOCAL_ID_START)
        {
          // The choice arose from a non-trivial branch discriminator.
          std::string const name = get_base_function_name(*this->fundef);
          curry::Qname const qname{module_stab.source->name, name};
          auto & node_stab = this->module_stab.lookup(qname);

          value copy_of_root = this->node_alloc(*rt.node_t);
          move(root_p, copy_of_root, this->fundef->arity);
          root_p.arrow(ND_VPTR) = &*node_stab.auxvt.at(&branch);
          root_p.arrow(ND_SLOT0) = bitcast(copy_of_root, *rt.char_t);
          root_p.arrow(ND_SLOT1) = bitcast(inductive, *rt.char_t);
          exec_pulltab(
              this->rt, this->root_p, inductive, 2, 1
            , this->out_of_memory_handler
            );
        }
        else
        {
          size_t const itgt = this->fundef->paths.at(pathid).idx;
          // FIXME: why not zip the choice all the way to the root in one step?
          exec_pulltab(
              this->rt, this->target_p, inductive, this->fundef->arity, itgt
            , this->out_of_memory_handler
            );
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
    , bool enable_tracing
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
        ::FunctionCompiler c(module_stab, arg("root_p"), &fun, enable_tracing);
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
      rt_h const & rt, size_t arity, bool enable_tracing
    )
  {
    value root_p = arg("root_p");
    ref child = local(*rt.node_t);
    label current = scope::current_label();

    auto handle_child = [&](size_t ichild){
      value call;
      size_t constexpr table_size = 6; // FAIL, FREE, FWD, CHOICE, OPER, CTOR
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
                , {labels[0], current, labels[0], labels[2], labels[0], current}
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
        // Ignore variables; do the second jump.
        scope _ = labels[TAGOFFSET + FREE];
        make_jump(2);
        // rt.printf("[not implemented] Failing due to free variable in ctor term");
        // if(arity > 2) vinvoke(root_p, VT_DESTROY);
        // root_p.arrow(ND_VPTR) = rt.failed_vt;
        // root_p.arrow(ND_TAG) = compiler::FAIL;
        // return_();
      }

      // FWD case.
      {
        scope _ = labels[TAGOFFSET + FWD];
        child = bitcast(child.arrow(ND_SLOT0), *rt.node_t);
        make_jump(1);
      }

      // CHOICE case.
      {
        scope _ = labels[TAGOFFSET + CHOICE];
        label out_of_memory_handler = rt.make_restart_point();
        if(enable_tracing) trace_step_start(rt, root_p);
        exec_pulltab(rt, root_p, child, arity, ichild, out_of_memory_handler);
        if(enable_tracing) trace_step_end(rt, root_p);
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
          {&labels[0], &labels[1], &labels[2], &labels[3], &labels[4], &labels[5]};
      jumptable1.set_initializer(addresses1);

      // The FWD and OPER rules are unreachable, so just put FAIL there.  Treat
      // FREE variables like constructors, since they can appear in a value.
      block_address addresses2[table_size] =
      {
          &labels[TAGOFFSET+FAIL]   // FAIL
        , &current                  // FREE
        , &labels[TAGOFFSET+FAIL]   // FWD
        , &labels[TAGOFFSET+CHOICE] // CHOICE
        , &labels[TAGOFFSET+FAIL]   // OPER
        , &current                  // CTOR
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

  void compile_ctor_vtable(
      tgt::global & vt
    , curry::DataType const & dtype
    , size_t ictor
    , compiler::ModuleSTab & module_stab
    , bool enable_tracing
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
        , [&]{ compile_ctor_N_function_body(rt, ctor.arity, enable_tracing); }
        );
    }
    vt.set_initializer(_t(
        &rt.Cy_NoAction
      , &N
      , &rt.Cy_Label(ctor.name)
      , &rt.Cy_Sentinel()
      , &rt.Cy_Arity(ctor.arity)
      , &rt.Cy_Succ(ctor.arity)
      , &rt.Cy_Succ(ctor.arity)
      , &rt.Cy_Destroy(ctor.arity)
      , &rt.CyVt_Equality(module_stab.source->name, dtype.name)
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
      , &rt.Cy_Arity(fun.arity)
      , &rt.Cy_Succ(fun.arity)
      , &rt.Cy_Succ(fun.arity)
      , &rt.Cy_Destroy(fun.arity)
      , &rt.CyVt_Equality("oper")
      , &rt.CyVt_Compare("oper")
      , &rt.CyVt_Show("oper")
      ));
  }

  /**
   * @brief Builds the definition of an aux function for the given function at
   * the specified branch.
   *
   * An aux function is used when a non-trivial branch discriminator evaluates to
   * a choice.  For example, in the sign function:
   *
   *   sign a = if a<0 then -1 else if a>0 then 1 else 0
   *
   * either expression a<0 or a>0 could produce a choice.  (Actualy, with some
   * analysis we could prove that the second expression cannot produce a choice
   * if the first did not, but the compiler does not attempt that analysis.)
   * The problem in that case is that the result after pull-tabbing cannot be
   * written in terms of the original function.  To execute a pull-tab step,
   * the remainder of the function, starting with the branch where the choice
   * was encountered, must be compiled.  The result can be written in terms of
   * that function.  For sign, there are two aux functions.
   *
   *     sign#branch0 (sign a) c = if c then -1 else if a>0 then 1 else 0
   *     sign#branch1 (sign a) c = if c then 1 else 0
   *
   * An aux function always has two successors: (1) the original root node
   * (which must be an application of the main function), and (2) the bound
   * expression.  Note that, due to (1), this is a somewhat irregular case in
   * that normally a function would never pattern-match against a function.
   *
   * To define an aux function, we do the following:
   *
   *   1. Adjust the name by appending #branchN, where N is a unique numeric ID
   *      provided for the branch in question.
   *   2. Rebase every variable reference, adding an initial index of 0.  This
   *      is because the original root is moved to the first argument position.
   *   3. Replace the branch discriminator with a reference to index 1.
   *   4. Copy the function from the given branch to the end.
   */
  curry::Function build_aux_function(
      curry::Function const & fun, curry::Branch b, size_t id
    )
  {
    curry::Function aux;
    aux.is_aux = true;
    aux.name = fun.name + "#branch" + std::to_string(id);
    aux.arity = 2;
    aux.paths = fun.paths;

    // There will be two new variables, for the new base and the discriminator.
    size_t const newbase = aux.paths.size();
    curry::Ref const cond{newbase + 1};

    for(auto & pathelem: aux.paths)
    {
      if(pathelem.base == curry::nobase)
        pathelem.base = newbase;
    }
    aux.paths.emplace_back(
        curry::Function::PathElem{curry::nobase, 0, Qname{"Prelude","(,)"}}
      );
    aux.paths.emplace_back(
        curry::Function::PathElem{curry::nobase, 1, Qname{"Prelude","(,)"}}
      );

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
        compile_function(module_stab, aux, enable_tracing);
      }
    }

    void operator()(curry::Rule const &) {}

    ModuleSTab & module_stab;
    NodeSTab & node_stab;
    bool enable_tracing;
    size_t counter;
  };

  void compile_aux_functions(
      compiler::ModuleSTab & module_stab
    , curry::Function const & fun
    , bool enable_tracing
    )
  {
    curry::Qname const qname{module_stab.source->name, fun.name};
    compiler::NodeSTab & node_stab = module_stab.lookup(qname);
    CompileAuxVisitor mkaux{module_stab, node_stab, enable_tracing, 0};
    fun.def.visit(mkaux);
  }
}

namespace sprite { namespace compiler
{
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
    rt.Cy_Repr(root_p, rt.stdout_(), true);
    rt.putchar('\n');
    rt.fflush(nullptr);
  }

  void trace_step_end(rt_h const & rt, value const & root_p)
  {
    rt.printf("S> +++ ");
    rt.CyTrace_ShowIndent();
    rt.Cy_Repr(root_p, rt.stdout_(), true);
    rt.putchar('\n');
    rt.fflush(nullptr);
  }

  void trace_step_tmp(rt_h const & rt, value const & root_p)
  {
    rt.printf("T> +++ ");
    rt.CyTrace_ShowIndent();
    rt.Cy_Repr(root_p, rt.stdout_(), true);
    rt.putchar('\n');
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
    , bool enable_tracing
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
            compile_ctor_vtable(vt, dtype, ictor, module_stab, enable_tracing);
  
          // Update the symbol tables.
          module_stab.nodes.emplace(
              curry::Qname{cymodule.name, ctor.name}
            , compiler::NodeSTab(ctor, vt.as_globalvar(), tag++)
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
          compile_aux_functions(module_stab, fun, enable_tracing);
          compile_function(module_stab, fun, enable_tracing);
        }
      }

      // Special cases for the Prelude.
      if(cymodule.name == "Prelude")
      {
        // Update symbol table entries for Prelude.? with the built-in
        // choice implementation.
        {
          auto choice = module_stab.nodes.find({"Prelude", "?"});
          if(choice == module_stab.nodes.end())
            throw compile_error("Prelude.? was not found");
          choice->second.vtable.reset(module_stab.rt().choice_vt.as_globalvar());
          choice->second.tag = CHOICE;
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

