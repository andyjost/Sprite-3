#include <iostream>
#include "sprite/curryinput.hpp"
#include "sprite/compiler.hpp"
#include "sprite/runtime.hpp"

using namespace sprite;
using namespace sprite::compiler::member_labels; // for ND_* and VT_* enums.
namespace tgt = sprite::backend;

namespace
{
  using namespace sprite::compiler;

  // Looks up a node symbol table from the library.
  inline compiler::NodeSTab const &
  lookup(compiler::LibrarySTab const & table, curry::Qname const & qname)
    { return table.modules.at(qname.module).nodes.at(qname.name); }

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
        compiler::ModuleCompiler const & compiler_
      , tgt::value const & root_p_
      , curry::Function const * fundef_ = nullptr
      )
      : compiler(compiler_)
      , root_p(bitcast(root_p_, *compiler_.ir.node_t))
      , target_p(root_p)
      , fundef(fundef_)
      , resolved_path_alloca(tgt::local(*compiler_.ir.node_t))
    {}

    compiler::ModuleCompiler const & compiler;

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

    // Looks up a node using the function paths and path reference.  The result
    // is a node_t* in the target.
    tgt::value resolve_path(size_t pathid) const
    {
      this->resolved_path_alloca = this->root_p;
      _resolve_path(this->fundef->paths.at(pathid));
      return this->resolved_path_alloca;
    }

    // Same as @p resolve_path but returns a char* in the target.
    tgt::value resolve_path_char_p(size_t pathid) const
      { return bitcast(resolve_path(pathid), *tgt::types::char_()); }

    // Helper for @p resolve_path.  Updates @p resolved_path_alloca.
    void _resolve_path(curry::Function::PathElem const & pathelem) const
    {
      // Walk the base path.
      if(pathelem.base != curry::nobase)
        _resolve_path(this->fundef->paths.at(pathelem.base));

      // Add this index.
      tgt::type node_pt = *this->compiler.ir.node_t;
      switch(pathelem.idx)
      {
        case 0:
          this->resolved_path_alloca = bitcast(
              this->resolved_path_alloca.arrow(ND_SLOT0), node_pt
            );
          break;
        case 1:
          this->resolved_path_alloca = bitcast(
              this->resolved_path_alloca.arrow(ND_SLOT1), node_pt
            );
          break;
        default: assert(0 && "Can't do arity>2 yet"); // FIXME
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
                  this->resolved_path_alloca.arrow(ND_SLOT0), node_pt
                );
            }
        );
    }

    result_type operator()(curry::Rule const & rule)
      { rule.visit(*this); }

    // Subcases of Rule.
    result_type operator()(int rule)
      {} // Not implemented.

    result_type operator()(double rule)
      {} // Not implemented.

    // Rewrites the root as a FWD node.
    result_type operator()(curry::Ref rule)
    {
      tgt::value target = this->resolve_path_char_p(rule.pathid);
      this->target_p.arrow(ND_VPTR) = compiler.rt.fwd_vt;
      this->target_p.arrow(ND_TAG) = compiler::FWD;
      this->target_p.arrow(ND_SLOT0) = target;
    }

    result_type operator()(curry::Expr const & expr)
    {
      // The target node pointer is clobbered so that recursion can be used.
      // Save it for below.
      tgt::value orig_target_p = this->target_p;

      // Each child needs to be allocated and initialized.  This children are
      // stored as i8* pointers.
      std::vector<tgt::value> children;
      children.reserve(expr.args.size());
      for(auto const & subexpr: expr.args)
      {
        // Avoid creating FWD nodes for subexpressions.
        if(curry::Ref const * varref = subexpr.getvar())
        {
          // Retrieve the pointer to an existing node.
          tgt::value child = this->resolve_path_char_p(varref->pathid);
          children.push_back(child);
        }
        // Allocate a new node and place its contents with a recursive call.
        else
        {
          tgt::value child = this->compiler.node_alloc();
          children.push_back(child);
          // Clobber the root so the recursive call works.
          this->target_p = bitcast(child, *this->compiler.ir.node_t);
          (*this)(subexpr);
        }
      }

      // Restore the original target.
      this->target_p = orig_target_p;

      // Set the vtable and tag.
      this->compiler.node_init(
          this->target_p, lookup(compiler.lib_stab, expr.qname)
        );

      // Set the child pointers.
      assert(children.size() <= 2 && "n>2 Not implemented");
      for(size_t i=0; i<children.size(); ++i)
        this->target_p.arrow(ND_SLOT0+i) = children[i];
    }
  };

  struct FunctionCompiler : Rewriter
  {
    using result_type = void;
    using Rewriter::Rewriter;

    FunctionCompiler(
        compiler::ModuleCompiler const & compiler_
      , tgt::value const & root_p_
      , curry::Function const * fundef_ = nullptr
      )
      : Rewriter(compiler_, root_p_, fundef_)
      , inductive_alloca(tgt::local(*this->compiler.ir.node_t))
    {}

  private:

    // A static allocation holding a pointer in the target to the current
    // inductive position.  The allocation is needed when a FWD node is
    // encountered, so that it can communicate the new target to other sections
    // of code.
    tgt::ref inductive_alloca;

  public:

    result_type operator()(curry::Branch const & branch)
    {
      // Look up the inductive node.
      tgt::value const inductive = this->resolve_path(branch.pathid);

      // Declare the jump table in the target program.
      tgt::type char_p = *tgt::types::char_();
      size_t const table_size = TAGOFFSET + branch.cases.size();
      tgt::global jumptable_ =
          tgt::static_(char_p[table_size], tgt::flexible(".jtable"));
      // FIXME: The backend API should be improved to simplify these casts.
      tgt::globalvar jumptable =
          globalvaraddr(llvm::dyn_cast<llvm::GlobalVariable>(jumptable_.ptr()));

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
        this->compiler.clib.printf("FAIL case hit.\n");
        tgt::return_().set_metadata("case...FAIL");
      }

      // FWD case
      {
        tgt::scope _ = labels[TAGOFFSET + FWD];
        // Advance to the target of the FWD node and repeat the jump.
        tgt::value const inductive = bitcast(
            this->inductive_alloca.arrow(ND_SLOT0), *this->compiler.ir.node_t
          );
        this->inductive_alloca = inductive;
        tgt::value const index = inductive.arrow(ND_TAG) + TAGOFFSET;
        tgt::goto_(jumptable[index], labels);
      }

      // CHOICE case
      {
        tgt::scope _ = labels[TAGOFFSET + CHOICE];
        this->compiler.clib.printf("CHOICE case hit.\n");
        tgt::return_().set_metadata("case...CHOICE");
      }

      // OPER case
      {
        tgt::scope _ = labels[TAGOFFSET + OPER];
        // Head-normalize the inductive node.
        vinvoke(this->inductive_alloca, VT_H, tailcall);
        tgt::return_();
      }

      // Add the label addresses to the jump table.
      jumptable.set_initializer(addresses);
      // tgt::ref jump = &jumptable[TAGOFFSET];

      // Store the address of the inductive node in allocated space, then use
      // its tag to make the first.
      this->inductive_alloca = inductive;
      tgt::value const index = inductive.arrow(ND_TAG) + TAGOFFSET;
      tgt::goto_(jumptable[index], labels);
    }

    result_type operator()(curry::Rule const & rule)
    {
      // Trace.
      // compiler.clib.printf("S> --- ");
      // compiler.rt.printexpr(target_p, "\n");
      // compiler.clib.fflush(nullptr);

      // Step.
      static_cast<Rewriter*>(this)->operator()(rule);

      // Trace.
      // compiler.clib.printf("S> +++ ");
      // compiler.rt.printexpr(target_p, "\n");
      // compiler.clib.fflush(nullptr);
      tgt::return_();
    }
  };

  // Helper to simplify the use of FunctionCompiler.
  void compile_function(
      compiler::ModuleCompiler const & compiler
    , tgt::value const & root_p
    , curry::Function const & fun
    )
  {
    ::FunctionCompiler c(compiler, root_p, &fun);
    return fun.def.visit(c);
  }

  // Makes a function that returns the given static C-string.  Returns a
  // pointer to that function.
  tgt::constant make_name_func(
      compiler::ModuleCompiler const & compiler, std::string const & name
    )
  {
    tgt::function f = tgt::static_<tgt::function>(
        compiler.ir.labelfun_t, tgt::flexible(".name"), {}
      , [&] { tgt::return_(name); }
      );
    return &f;
  }

  // Makes a function that returns the given integer.  Returns a
  // pointer to that function.
  tgt::constant make_arity_func(
      compiler::ModuleCompiler const & compiler, int64_t arity
    )
  {
    tgt::function f = tgt::static_<tgt::function>(
        compiler.ir.arityfun_t, tgt::flexible(".arity"), {}
      , [&] { tgt::return_(arity); }
      );
    return &f;
  }
  // Makes a function that returns the successor range of a node.  Returns a
  // pointer to that function.
  tgt::constant make_succ_func(
      compiler::ModuleCompiler const & compiler, int64_t arity
    )
  {
    tgt::function f = tgt::static_<tgt::function>(
        compiler.ir.rangefun_t, tgt::flexible(".succ")
      , {"node_p", "begin_out_pp", "end_out_pp"}
      );

    if(arity < 3)
    {
      tgt::scope _ = f;
      // Compute the size of pointers and get an integer type with the same
      // bitwidth.
      size_t sizeof_ptr = sizeof_(*compiler.ir.node_t);
      tgt::type size_t_ = tgt::types::int_(sizeof_ptr * 8);

      // Get the function paramters.
      tgt::value node_p = tgt::arg("node_p");
      tgt::ref begin_out_pp = tgt::arg("begin_out_pp");
      tgt::ref end_out_pp = tgt::arg("end_out_pp");

      // Compute the begining address.
      tgt::value begin_cp = &node_p.arrow(ND_SLOT0);
      tgt::value begin = begin_cp;
      tgt::value end = typecast(begin_cp, size_t_) + arity * sizeof_ptr;

      // Assign the outputs.
      begin_out_pp = begin;
      end_out_pp = end;
      tgt::return_();
    }
    else
      assert(0 && "arity > 2 not supported");

    return &f;
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
      LibrarySTab & lib_stab_, curry::Module const & src
    )
    : source(&src), module_ir(src.name), nodes()
  {
    // Load the compiler data (e.g., headers) with the module in scope.
    scope _ = module_ir;
    this->compiler.reset(new ModuleCompiler(lib_stab_));
  }

  // Constructs an expression at the given node address.
  value construct(
      compiler::ModuleCompiler const & compiler
    , tgt::value const & root_p
    , curry::Rule const & expr
    )
  {
    ::Rewriter c(compiler, root_p);
    expr.visit(c);
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
      curry::Library const & lib
    , compiler::LibrarySTab & stab
    )
  {
    for(auto const & cymodule: lib.modules)
    {
      // Create a new LLVM module and symbol table entry.
      auto rv = stab.modules.emplace(
          cymodule.name, ModuleSTab{stab, cymodule}
        );
      compiler::ModuleSTab & module_stab = rv.first->second;

      // Helpful aliases.
      tgt::module & module_ir = module_stab.module_ir;
      compiler::ModuleCompiler const & compiler = *module_stab.compiler;
      
      // Set the module as the current scope.  Subsequent statements will add
      // functions, type definitions, and data to this module.
      tgt::scope _ = module_ir;
  
      // Create the null step function.
      function nullstep(module_ir->getFunction(".nullstep"));
      if(!nullstep.ptr())
        nullstep = inline_(compiler.ir.stepfun_t, ".nullstep", {}, []{});

      // Add the static vtable for each constructor to the module.
      for(auto const & dtype: cymodule.datatypes)
      {
        size_t tag = compiler::CTOR;
        for(auto const & ctor: dtype.constructors)
        {
          // Lookup the N function and define it as needed.
          std::string const n_name = ".N." + std::to_string(ctor.arity);
          function N(module_ir->getFunction(n_name.c_str()));
          if(!N.ptr())
          {
            N = inline_(
                compiler.ir.stepfun_t, n_name, {"root_p"}
              , [&]{
                  tgt::value root_p = arg("root_p");
                  tgt::value child;
                  // FIXME: need to implement "dot" and "arrow" with names.
                  switch(ctor.arity)
                  {
                    case 2:
                      child = bitcast(
                          root_p.arrow(ND_SLOT1), *compiler.ir.node_t
                        );
                      child.arrow(ND_VPTR).arrow(VT_N)(child);
                    case 1:
                      child = bitcast(
                          root_p.arrow(ND_SLOT0), *compiler.ir.node_t
                        );
                      child.arrow(ND_VPTR).arrow(VT_N)(child)
                          .set_attribute(tailcall);
                    case 0:
                      break;
                    default:
                    {
                      ref chldrn = bitcast(
                          root_p.arrow(ND_SLOT0), *compiler.ir.node_t
                        );
                      tgt::value last_call;
                      for(size_t i=0; i<ctor.arity; ++i)
                      {
                        child = chldrn[i];
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
  
          // Create the vtable.
          tgt::globalvar vt = static_(
              compiler.ir.vtable_t, ".vt.CTOR." + ctor.name
            )
              .set_initializer(_t(
                  make_name_func(compiler, ctor.name)
                , make_arity_func(compiler, ctor.arity) // FIXME
                , make_succ_func(compiler, ctor.arity)
                , &N, &nullstep
                ));
  
          // Update the symbol tables.
          module_stab.nodes.emplace(
              ctor.name, compiler::NodeSTab(ctor, vt, tag++)
            );
        }
      }
  
      // Update the symbol tables with the functions.
      for(auto const & fun: cymodule.functions)
      {
        // Forward declaration.
        function step = inline_(
            compiler.ir.stepfun_t, ".step." + fun.name, {"root_p"}
          );
        function N = inline_(
            compiler.ir.stepfun_t, ".N." + fun.name, {"root_p"}
          , [&]{
              tgt::value root_p = arg("root_p");
              step(root_p);
              vinvoke(root_p, VT_N, tailcall);
              return_();
            }
          );
        function H = inline_(
            compiler.ir.stepfun_t, ".H." + fun.name, {"root_p"}
          , [&]{
              tgt::value root_p = arg("root_p");
              step(root_p);
              vinvoke(root_p, VT_H, tailcall);
            }
          );
  
        tgt::globalvar vt =
            static_(compiler.ir.vtable_t, ".vtable.for." + fun.name)
            .set_initializer(_t(
                make_name_func(compiler, fun.name)
              , make_arity_func(compiler, fun.arity) // FIXME
              , make_succ_func(compiler, fun.arity)
              , &N, &H
              ));

        module_stab.nodes.emplace(
            fun.name
          , compiler::NodeSTab(fun, vt, compiler::OPER)
          );
      }
  
      // Compile the functions.
      for(auto const & fun: cymodule.functions)
      {
        auto step = module_ir.getglobal(".step." + fun.name);
        tgt::scope _ = dyn_cast<function>(step);
        ::compile_function(compiler, arg("root_p"), fun);
      }
    }
  }
}}

