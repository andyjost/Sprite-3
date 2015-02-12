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

    // Emits code to clean up any function-specific allocations and then return.
    void clean_up_and_return()
    {
      for(size_t i=LOCAL_ID_START; i<next_local_id; ++i)
        rt.CyMem_PopRoot();
      return_();
    }

    // Node allocator for use in the rewriter.  Automatically uses this
    // object's out-of-memory handler.
    value node_alloc(type const & ty) const
      { return rt.node_alloc(ty, this->out_of_memory_handler); }

    // Places an expression at an uninitialized location.
    tgt::value new_(tgt::value const & data, curry::Rule const & rule)
    {
      tgt::value orig_target_p = this->target_p;
      this->target_p = data;
      (*this)(rule);
      this->target_p = orig_target_p;
      return data;
    }

    // Allocates a new node and places an expression there.
    tgt::value new_(curry::Rule const & rule)
    {
      tgt::value data = this->node_alloc(*rt.node_t);
      return this->new_(data, rule);
    }

    // Sets the extended child array to the given value, and returns the same,
    // cast to char**.
    tgt::value set_extended_child_array(
        tgt::value const & node, tgt::value const & array
      )
    {
      ref slot0 = node.arrow(ND_SLOT0);
      slot0 = array;
      return bitcast(slot0, **types::char_());
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
            pair.first->second = this->node_alloc(*rt.node_t);
            this->resolved_path_alloca = pair.first->second;
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

  private:

    /// Rewrites the target to a simple constructor with built-in data.
    template<typename Data>
    void rewrite_integer_data_ctor(Data data, tgt::value const & vt)
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
      { return rewrite_integer_data_ctor(data, rt.Char_vt); }

    result_type operator()(int64_t data)
      { return rewrite_integer_data_ctor(data, rt.Int64_vt); }

    result_type operator()(double data)
      { return rewrite_integer_data_ctor(data, rt.Float_vt); }

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
      // The target node pointer is clobbered so that recursion can be used.
      // Save it for below.
      tgt::value orig_target_p = this->target_p;

      // Each child needs to be allocated and initialized.  The children are
      // stored as i8* pointers.
      std::vector<tgt::value> child_data;
      child_data.reserve(term.args.size());
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

      // Restore the original target.
      this->target_p = orig_target_p;

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
      return (*this)(term.result);
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
    {}

  private:

    // A static allocation holding a pointer in the target to the current
    // inductive position.  The allocation is needed when a FWD node is
    // encountered, so that it can communicate the new target to other sections
    // of code.
    tgt::ref inductive_alloca;

    bool enable_tracing;

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
        rt.CyMem_PushRoot(p);
        return next_local_id++;
      }
      else
        throw compile_error("Invalid branch condition");
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

      // Look up the inductive node.
      size_t const pathid = build_condition(branch.condition);
      tgt::value const inductive = this->resolve_path(pathid);

      // Declare the jump table in the target program.
      tgt::type char_p = *tgt::types::char_();
      size_t const table_size = TAGOFFSET + branch.cases.size();
      tgt::globalvar jumptable =
          tgt::static_(char_p[table_size], tgt::flexible(".jtable"))
              .as_globalvar();

      // Declare placeholders for the pre-defined special labels.  Definitions
      // are provided below.
      std::vector<tgt::label> labels(4);

      // Add a label for each constructor at the branch position.
      for(auto const & case_: branch.cases)
      {
        // Allocate a label, then recursively generate the next step in its
        // scope.
        tgt::label tmp;
        tgt::scope _ = tmp;
        case_.action.visit(*this);
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
        if(pathid >= LOCAL_ID_START)
        {
          rt.printf("Choice in branch expression.");
          rt.fflush(nullptr);
          rt.exit(1);
        }
        else
        {
          size_t const critical = this->fundef->paths.at(pathid).idx;
          size_t const n = this->fundef->arity;
          tgt::value lhs = this->node_alloc(*rt.node_t);
          tgt::value rhs = this->node_alloc(*rt.node_t);

          lhs.arrow(ND_VPTR) = this->target_p.arrow(ND_VPTR);
          lhs.arrow(ND_TAG) = this->target_p.arrow(ND_TAG);
          rhs.arrow(ND_VPTR) = this->target_p.arrow(ND_VPTR);
          rhs.arrow(ND_TAG) = this->target_p.arrow(ND_TAG);
          // OK to ignore aux.  The root cannot be a choice because a choice
          // has no step function.

          if(n < 3)
          {
            for(size_t i=0; i<n; ++i)
            {
              if(i == critical)
              {
                lhs.arrow(ND_SLOT0+i) = inductive.arrow(ND_SLOT0);
                rhs.arrow(ND_SLOT0+i) = inductive.arrow(ND_SLOT1);
              }
              else
              {
                lhs.arrow(ND_SLOT0+i) = this->target_p.arrow(ND_SLOT0+i);
                rhs.arrow(ND_SLOT0+i) = lhs.arrow(ND_SLOT0+i);
              }
            }
          }
          else
          {
            // The LHS argument array is stolen from the root node.
            value lhs_children = set_extended_child_array(
                lhs, this->target_p.arrow(ND_SLOT0)
              );
            // The RHS argument array is allocated.
            value rhs_children = set_extended_child_array(
                rhs, rt.Cy_ArrayAllocTyped(static_cast<aux_t>(n))
              );

            for(size_t i=0; i<n; ++i)
            {
              if(i == critical)
              {
                lhs_children[i] = inductive.arrow(ND_SLOT0);
                rhs_children[i] = inductive.arrow(ND_SLOT1);
              }
              else
              {
                // Nothing to do for LHS.
                rhs_children[i] = lhs_children[i];
              }
            }
          }
          // Do not call this->destroy_target.  The successor list was stolen.
          this->target_p.arrow(ND_VPTR) = rt.choice_vt;
          this->target_p.arrow(ND_TAG) = compiler::CHOICE;
          this->target_p.arrow(ND_AUX) = inductive.arrow(ND_AUX);
          this->target_p.arrow(ND_SLOT0) = bitcast(lhs, *types::char_());
          this->target_p.arrow(ND_SLOT1) = bitcast(rhs, *types::char_());
        }
        clean_up_and_return();
      }

      // OPER case
      {
        tgt::scope _ = labels[TAGOFFSET + OPER];
        // Head-normalize the inductive node.
        vinvoke(this->inductive_alloca, VT_H);
        // Repeat the previous jump.
        tgt::value const index = inductive.arrow(ND_TAG) + TAGOFFSET;
        tgt::goto_(jumptable[index], labels);
      }

      // Add the label addresses to the jump table.
      jumptable.set_initializer(addresses);
      // tgt::ref jump = &jumptable[TAGOFFSET];

      // Store the address of the inductive node in allocated space, then use
      // its tag to make the first jump.
      this->inductive_alloca = inductive;
      tgt::value const index = inductive.arrow(ND_TAG) + TAGOFFSET;
      tgt::goto_(jumptable[index], labels);
    }

    result_type operator()(curry::Rule const & rule)
    {
      if(enable_tracing)
      {
        rt.printf("S> --- ");
        rt.Cy_Repr(target_p, rt.stdout_(), true);
        rt.putchar('\n');
        rt.fflush(nullptr);
      }

      // The rewrite step is always a series of allocations and memory stores
      // that finishes by attaching all allocated nodes to the root.  If memory
      // allocation fails, causing gc to run, restart here.
      this->set_out_of_memory_handler_returning_here();

      // Step.
      static_cast<Rewriter*>(this)->operator()(rule);

      if(enable_tracing)
      {
        rt.printf("S> +++ ");
        rt.Cy_Repr(target_p, rt.stdout_(), true);
        rt.putchar('\n');
        rt.fflush(nullptr);
      }
      clean_up_and_return();
    }
  };

  // Helper to simplify the use of FunctionCompiler.
  void compile_function(
      compiler::ModuleSTab const & module_stab
    , tgt::value const & root_p
    , curry::Function const & fun
    , bool enable_tracing
    )
  {
    try
    {
      ::FunctionCompiler c(module_stab, root_p, &fun, enable_tracing);
      return fun.def.visit(c);
    }
    catch(...)
    {
      std::cerr
        << "[" << module_stab.source->name << "] In function " << fun.name
        << std::endl;
      throw;
    }
  }

  void compile_ctor_vtable(
      tgt::global & vt
    , curry::DataType const & dtype
    , size_t ictor
    , compiler::ModuleSTab & module_stab
    )
  {
    compiler::rt_h const & rt = module_stab.rt();

    // Look up the N function and define it as needed.
    auto const & ctor = dtype.constructors[ictor];
    tgt::module & module_ir = module_stab.module_ir;
    std::string const n_name = ".N." + std::to_string(ctor.arity);
    function N(module_ir->getFunction(n_name.c_str()));
    if(!N.ptr())
    {
      N = static_<function>(
          rt.stepfun_t, flexible(n_name), {"root_p"}
        , [&]{
            tgt::value root_p = arg("root_p");
            tgt::value child;
            // FIXME: need to implement "dot" and "arrow" with names.
            switch(ctor.arity)
            {
              case 2:
                child = bitcast(
                    root_p.arrow(ND_SLOT1), *rt.node_t
                  );
                child.arrow(ND_VPTR).arrow(VT_N)(child);
              case 1:
                child = bitcast(
                    root_p.arrow(ND_SLOT0), *rt.node_t
                  );
                child.arrow(ND_VPTR).arrow(VT_N)(child)
                    .set_attribute(tailcall);
              case 0:
                break;
              default:
              {
                tgt::value children = bitcast(
                    root_p.arrow(ND_SLOT0), **rt.node_t
                  );
                tgt::value last_call;
                for(size_t i=0; i<ctor.arity; ++i)
                {
                  child = children[i];
                  last_call = child.arrow(ND_VPTR).arrow(VT_N)(child);
                }
                last_call.set_attribute(tailcall);
                break;
              }
            }
            tgt::return_();
          }
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
          rt.stepfun_t, flexible(hname), {"root_p"}
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
}

namespace sprite { namespace compiler
{
  tgt::value vinvoke(tgt::value const & node_p, compiler::VtMember member)
    { return node_p.arrow(compiler::ND_VPTR).arrow(member)(node_p); }

  tgt::value vinvoke(
      tgt::value const & node_p, compiler::VtMember member, tgt::attribute attr
    )
  { return vinvoke(node_p, member).set_attribute(attr); }

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
            compile_ctor_vtable(vt, dtype, ictor, module_stab);
  
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
        // Create or find the vtable and maybe compile code to fill it.
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
          auto step = module_ir.getglobal(".step." + fun.name);
          function stepf = dyn_cast<function>(step);
          if(stepf->size() == 0) // function has no body
          {
            tgt::scope fscope = dyn_cast<function>(step);
            label entry_;
            goto_(entry_);
            tgt::scope _ = entry_;
            ::compile_function(module_stab, arg("root_p"), fun, enable_tracing);
          }
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

