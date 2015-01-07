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

  // Looks up a node symbol table from the library.
  inline compiler::NodeSTab const &
  lookup(compiler::ModuleSTab const & table, curry::Qname const & qname)
  {
    try
      { return table.nodes.at(qname); }
    catch(std::out_of_range const & e)
    {
      // DIAGNOSTIC
      // std::cerr
      //   << "Contents of symbol table for module " << table.source->name
      //   << "\n";
      // for(auto const & node: table.nodes)
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

    // Any needed types (for convenience).
    tgt::type node_pointer_type = *module_stab.ir().node_t;

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
      tgt::value data = node_alloc_typed(this->module_stab);
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
            pair.first->second = node_alloc_typed(this->module_stab);
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
      size_t const term_arity = lookup(
          this->module_stab, pathelem.typename_
        ).source->arity;
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
                  ==(tgt::signed_) ((int)FWD);
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
      this->target_p.arrow(ND_VPTR) = vt;
      this->target_p.arrow(ND_TAG) = compiler::CTOR;
      auto slot0 = this->target_p.arrow(ND_SLOT0);
      ref slot0_typed(bitcast(&slot0, *get_type<Data>()));
      slot0_typed = data;
    }

  public:
  
    // Subcases of Rule.
    result_type operator()(char data)
      { return rewrite_integer_data_ctor(data, module_stab.rt().Char_vt); }

    result_type operator()(int64_t data)
      { return rewrite_integer_data_ctor(data, module_stab.rt().Int64_vt); }

    result_type operator()(double data)
      { return rewrite_integer_data_ctor(data, module_stab.rt().Float_vt); }

    // Rewrites the root as a FAIL node.
    result_type operator()(curry::Fail const &)
    {
      this->target_p.arrow(ND_VPTR) = module_stab.rt().failed_vt;
      this->target_p.arrow(ND_TAG) = compiler::FAIL;
    }

    // Rewrites the root as a FWD node.
    result_type operator()(curry::Ref rule)
    {
      tgt::value target = this->resolve_path_char_p(rule.pathid);
      this->target_p.arrow(ND_VPTR) = module_stab.rt().fwd_vt;
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
      // Use only the first 31 bits to avoid any possibility of interpreting
      // the tag as a negative value.
      auto const & node_stab = lookup(this->module_stab, term.qname);
      size_t const N = node_stab.source->arity;
      if(N & ~0x7FFFFFFF)
        compile_error("Too many successors");
      int32_t const n = static_cast<int32_t>(N & 0x7FFFFFFF);
      size_t const niter = term.args.size() + 1;
      size_t i = 0;

      // The final iteration writes to the target node.  Prior iterations
      // allocate a new node.
      auto choose_storage = [&] {
          if(i+1 == niter)
            return this->target_p;
          else
            return node_alloc_typed(this->module_stab);
        };

      // Create the terminal node, which has the function's vtable as its data.
      tgt::value prev_node = choose_storage();
      prev_node.arrow(ND_VPTR) = module_stab.rt().PartialTerminus_vt;
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
        this_node.arrow(ND_VPTR) = module_stab.rt().PartialSpine_vt;
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

      // Each child needs to be allocated and initialized.  This children are
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
          tgt::value child = node_alloc(this->module_stab);
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
        , lookup(module_stab, term.qname)
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
        aux = array_alloc(this->module_stab, child_data.size());
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
      tgt::function fun = extern_<tgt::function>(
          this->module_stab.ir().stepfun_t, term.qname.name
        );
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
      )
      : Rewriter(module_stab_, root_p_, fundef_)
      , inductive_alloca(tgt::local(node_pointer_type))
    {}

  private:

    // A static allocation holding a pointer in the target to the current
    // inductive position.  The allocation is needed when a FWD node is
    // encountered, so that it can communicate the new target to other sections
    // of code.
    tgt::ref inductive_alloca;

  public:

    // Evaluates the condition to get a path index.
    size_t build_condition(curry::Rule const & condition)
    {
      if(curry::Ref const * cond = condition.getvar())
        return cond->pathid;
      else if(condition.getterm())
      {
        this->new_(this->resolve_path(next_local_id), condition);
        return next_local_id++;
      }
      else
        throw compile_error("Invalid branch condition");
    }

    result_type operator()(curry::Branch const & branch)
    {
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
        tgt::return_();
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
          module_stab.clib().printf("Choice in branch expression.");
          module_stab.clib().fflush(nullptr);
          module_stab.clib().exit(1);
        }
        else
        {
          module_stab.clib().fflush(nullptr);
          size_t const critical = this->fundef->paths.at(pathid).idx;
          size_t const n = this->fundef->arity;
          tgt::value lhs = node_alloc_typed(this->module_stab);
          tgt::value rhs = node_alloc_typed(this->module_stab);

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
                rhs, array_alloc(this->module_stab, n)
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
          this->target_p.arrow(ND_VPTR) = module_stab.rt().choice_vt;
          this->target_p.arrow(ND_TAG) = compiler::CHOICE;
          this->target_p.arrow(ND_AUX) = inductive.arrow(ND_AUX);
          this->target_p.arrow(ND_SLOT0) = bitcast(lhs, *types::char_());
          this->target_p.arrow(ND_SLOT1) = bitcast(rhs, *types::char_());
        }
        tgt::return_();
      }

      // OPER case
      {
        tgt::scope _ = labels[TAGOFFSET + OPER];
        // Head-normalize the inductive node.
        vinvoke(this->inductive_alloca, VT_H, tailcall);
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
      // // Trace.
      // module_stab.clib().printf("S> --- ");
      // module_stab.rt().printexpr(target_p, "\n");
      // module_stab.clib().fflush(nullptr);

      // Step.
      static_cast<Rewriter*>(this)->operator()(rule);

      // // Trace.
      // module_stab.clib().printf("S> +++ ");
      // module_stab.rt().printexpr(target_p, "\n");
      // module_stab.clib().fflush(nullptr);
      tgt::return_();
    }
  };

  // Helper to simplify the use of FunctionCompiler.
  void compile_function(
      compiler::ModuleSTab const & module_stab
    , tgt::value const & root_p
    , curry::Function const & fun
    )
  {
    try
    {
      ::FunctionCompiler c(module_stab, root_p, &fun);
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

  // Returns the "show" function for a constructor.
  function get_show_function_for(
      compiler::ModuleSTab & module_stab
    , curry::Constructor const & ctor
    )
  {
    auto const & ir = module_stab.ir();
    auto const & clib = module_stab.clib();
    if(module_stab.source->name == "Prelude")
    {
      // Special case for Prelude.[]
      if(ctor.name == "[]")
      {
        return extern_<function>(
            ir.showfun_t, ".show.[]", {"root", "stream"}
          , [&]{clib.fputs("[]", arg("stream"));}
          );
      }

      // Special case for Prelude.:
      else if(ctor.name == ":")
        return extern_<function>(ir.showfun_t, ".show.:");

      // Special case for tuples.  A type is a tuple if it is in the Prelude,
      // its name begins with '(', and its name ends with ')'.
      else if(
          ctor.name.size()
        && ctor.name.front() == '(' && ctor.name.back() == ')'
        )
      { return extern_<function>(ir.showfun_t, ".show.()"); }
    }
    return get_generic_show_function(ir);
  }

  void compile_ctor_vtable(
      tgt::global & vt
    , curry::DataType const & dtype
    , size_t ictor
    , compiler::ir_h const & ir
    , compiler::ModuleSTab & module_stab
    )
  {
    // Look up the N function and define it as needed.
    auto const & ctor = dtype.constructors[ictor];
    tgt::module & module_ir = module_stab.module_ir;
    std::string const n_name = ".N." + std::to_string(ctor.arity);
    function N(module_ir->getFunction(n_name.c_str()));
    if(!N.ptr())
    {
      N = static_<function>(
          ir.stepfun_t, flexible(n_name), {"root_p"}
        , [&]{
            tgt::value root_p = arg("root_p");
            tgt::value child;
            // FIXME: need to implement "dot" and "arrow" with names.
            switch(ctor.arity)
            {
              case 2:
                child = bitcast(
                    root_p.arrow(ND_SLOT1), *ir.node_t
                  );
                child.arrow(ND_VPTR).arrow(VT_N)(child);
              case 1:
                child = bitcast(
                    root_p.arrow(ND_SLOT0), *ir.node_t
                  );
                child.arrow(ND_VPTR).arrow(VT_N)(child)
                    .set_attribute(tailcall);
              case 0:
                break;
              default:
              {
                tgt::value children = bitcast(
                    root_p.arrow(ND_SLOT0), **ir.node_t
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
        &get_label_function(ir, ctor.name)
      , &get_arity_function(ir, ctor.arity)
      , &get_succ_function(ir, ctor.arity)
      , &get_vt_for_equality(ir, module_stab.source->name, dtype.name)
      , &get_show_function_for(module_stab, ctor)
      , &N, &get_null_step_function(ir)
      ));
  }

  void compile_function_vtable(
      tgt::global & vt
    , curry::Function const & fun
    , compiler::ir_h const & ir
    , compiler::ModuleSTab & module_stab
    )
  {
    // Forward declaration.
    tgt::module & module_ir = module_stab.module_ir;
    std::string const stepname = ".step." + fun.name;
    function step(module_ir->getFunction(stepname.c_str()));
    if(!step.ptr())
      step = static_<function>(ir.stepfun_t, stepname, {"root_p"});

    std::string const nname = ".N." + fun.name;
    function N(module_ir->getFunction(nname.c_str()));
    if(!N.ptr())
    {
      N = static_<function>(
          ir.stepfun_t, nname, {"root_p"}
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
          ir.stepfun_t, flexible(hname), {"root_p"}
        , [&]{
            tgt::value root_p = arg("root_p");
            step(root_p);
            vinvoke(root_p, VT_H, tailcall);
            return_();
          }
        );
    }

    vt.set_initializer(_t(
        &get_label_function(ir, fun.name)
      , &get_arity_function(ir, fun.arity)
      , &get_succ_function(ir, fun.arity)
      , &get_vt_for_primitive_equality(ir, "oper")
      , &get_generic_show_function(ir)
      , &N, &H
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
    compiler::ir_h const & ir = module_stab.ir();
    
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
          tgt::global vt = extern_(ir.vtable_t, vtname);
          if(is_primary && !vt.has_initializer())
            compile_ctor_vtable(vt, dtype, ictor, ir, module_stab);
  
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
        tgt::global vt = extern_(ir.vtable_t, vtname);
        if(is_primary && !vt.has_initializer())
          compile_function_vtable(vt, fun, ir, module_stab);

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
            tgt::scope _ = dyn_cast<function>(step);
            ::compile_function(module_stab, arg("root_p"), fun);
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

