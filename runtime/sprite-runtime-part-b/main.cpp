#include "sprite/compiler.hpp"
#include "sprite/runtime.hpp"
#include "sprite/backend/support/testing.hpp"
#include "llvm/Bitcode/ReaderWriter.h"
#include "llvm/Support/raw_ostream.h"

using namespace sprite::backend;

void build_vt_for_Char(sprite::compiler::ir_h const & ir);
void build_vt_for_choice(sprite::compiler::ir_h const & ir);
void build_vt_for_Float(sprite::compiler::ir_h const & ir);
void build_vt_for_fwd(sprite::compiler::ir_h const & ir);
void build_vt_for_Int64(sprite::compiler::ir_h const & ir);

namespace sprite { namespace compiler
{
  function make_succ_function(function fun, ir_h const & ir, int64_t arity);
}}

// A trivial node is one with label and arity, but no meaningful action for H
// or N.
void build_vt_for_trivial_node(
    sprite::compiler::ir_h const & ir, std::string const & name, size_t arity
  )
{
  extern_(ir.vtable_t, sprite::compiler::get_vt_name(name))
      .set_initializer(_t(
          &get_label_function(ir, name)
        , &get_arity_function(ir, arity)
        , &get_succ_function(ir, arity)
        , &get_null_step_function(ir)
        , &get_null_step_function(ir)
        ))
	  ;
}

// Note: choice is trivial because H and N are never called.  The pull-tab
// steps are applied by a parent when it observes its child is a choice.
void build_vt_for_choice(sprite::compiler::ir_h const & ir)
  { build_vt_for_trivial_node(ir, "choice", 2); }
void build_vt_for_failed(sprite::compiler::ir_h const & ir)
  { build_vt_for_trivial_node(ir, "failed", 0); }
void build_vt_for_freevar(sprite::compiler::ir_h const & ir)
  { build_vt_for_trivial_node(ir, "freevar", 0); }

int main()
{
	module module_b("sprit_runtime_part_b");
	scope _ = module_b;
	sprite::compiler::ir_h ir;
  sprite::backend::testing::clib_h clib;

  // Declare the null step function.
  extern_(ir.stepfun_t, ".nullstep", {}, []{});

  // Declare the predefined arity and successor functions.
  for(size_t i=0; i<SPRITE_PREDEF_ARITY_LIMIT; ++i)
  {
    extern_<function>(
        ir.arityfun_t, ".arity." + std::to_string(i), {"node_p"}
      , [&] { return_(i); }
      );

    function f = extern_<function>(
        ir.rangefun_t, ".succ." + std::to_string(i)
      , {"node_p", "begin_out_pp", "end_out_pp"}
      );
    make_succ_function(f, ir, i);
  }

  // Build the vtables for built-in constructors.
  #define SPRITE_HANDLE_BUILTIN(name, arity) build_vt_for_##name(ir);
  #include "sprite/builtins.def"

  // Create stubs for unimplemented external functions.
  #define DECLARE_EXTERNAL_STUB(name)                                 \
      extern_<function>(                                              \
          ir.stepfun_t, #name, {}                                     \
        , [&] {                                                       \
            clib.printf(                                              \
              "External function \"" #name "\" is not implemented.\n" \
            );                                                        \
            clib.exit(EXIT_FAILURE);                                  \
          }                                                           \
        );                                                            \
    /**/
  // PAKCS Prelude externals
  // =======================
  #if 0
  DECLARE_EXTERNAL_STUB(seq)
  DECLARE_EXTERNAL_STUB(ensureNotFree)
  DECLARE_EXTERNAL_STUB(prim_error)
  DECLARE_EXTERNAL_STUB(failed)
  DECLARE_EXTERNAL_STUB(==)
  DECLARE_EXTERNAL_STUB(compare)
  DECLARE_EXTERNAL_STUB(prim_ord)
  DECLARE_EXTERNAL_STUB(prim_chr)
  // DECLARE_EXTERNAL_STUB(prim_Int_plus)
  DECLARE_EXTERNAL_STUB(prim_Int_minus)
  DECLARE_EXTERNAL_STUB(prim_Int_times)
  DECLARE_EXTERNAL_STUB(prim_Int_div)
  DECLARE_EXTERNAL_STUB(prim_Int_mod)
  DECLARE_EXTERNAL_STUB(prim_Int_quot)
  DECLARE_EXTERNAL_STUB(prim_Int_rem)
  DECLARE_EXTERNAL_STUB(prim_negateFloat)
  DECLARE_EXTERNAL_STUB(=:=)
  DECLARE_EXTERNAL_STUB(success)
  DECLARE_EXTERNAL_STUB(&)
  DECLARE_EXTERNAL_STUB(>>=)
  DECLARE_EXTERNAL_STUB(return)
  DECLARE_EXTERNAL_STUB(prim_putChar)
  DECLARE_EXTERNAL_STUB(getChar)
  DECLARE_EXTERNAL_STUB(prim_readFile)
  DECLARE_EXTERNAL_STUB(prim_readFileContents)
  DECLARE_EXTERNAL_STUB(prim_writeFile)
  DECLARE_EXTERNAL_STUB(prim_appendFile)
  DECLARE_EXTERNAL_STUB(catch)
  DECLARE_EXTERNAL_STUB(prim_show)
  DECLARE_EXTERNAL_STUB(apply)
  DECLARE_EXTERNAL_STUB(cond)
  DECLARE_EXTERNAL_STUB(letrec)
  DECLARE_EXTERNAL_STUB(=:<=)
  DECLARE_EXTERNAL_STUB(=:<<=)
  DECLARE_EXTERNAL_STUB(ifVar)
  DECLARE_EXTERNAL_STUB(failure)
  #endif

  // CMC Prelude externals
  // =======================
  DECLARE_EXTERNAL_STUB(ensureNotFree)
  DECLARE_EXTERNAL_STUB($!)
  DECLARE_EXTERNAL_STUB($!!)
  DECLARE_EXTERNAL_STUB($##)
  DECLARE_EXTERNAL_STUB(prim_error)
  DECLARE_EXTERNAL_STUB(failed)
  DECLARE_EXTERNAL_STUB(==)
  DECLARE_EXTERNAL_STUB(compare)
  DECLARE_EXTERNAL_STUB(ord)
  DECLARE_EXTERNAL_STUB(chr)
  // DECLARE_EXTERNAL_STUB(+)
  //DECLARE_EXTERNAL_STUB(-)
  // DECLARE_EXTERNAL_STUB(*)
  DECLARE_EXTERNAL_STUB(div)
  DECLARE_EXTERNAL_STUB(mod)
  DECLARE_EXTERNAL_STUB(quot)
  DECLARE_EXTERNAL_STUB(rem)
  DECLARE_EXTERNAL_STUB(prim_negateFloat)
  DECLARE_EXTERNAL_STUB(=:=)
  DECLARE_EXTERNAL_STUB(success)
  DECLARE_EXTERNAL_STUB(&)
  DECLARE_EXTERNAL_STUB(>>=)
  DECLARE_EXTERNAL_STUB(return)
  DECLARE_EXTERNAL_STUB(prim_putChar)
  DECLARE_EXTERNAL_STUB(getChar)
  DECLARE_EXTERNAL_STUB(prim_readFile)
  DECLARE_EXTERNAL_STUB(prim_readFileContents)
  DECLARE_EXTERNAL_STUB(prim_writeFile)
  DECLARE_EXTERNAL_STUB(prim_appendFile)
  DECLARE_EXTERNAL_STUB(catch)
  DECLARE_EXTERNAL_STUB(prim_show)
  DECLARE_EXTERNAL_STUB(?)
  DECLARE_EXTERNAL_STUB(apply)
  DECLARE_EXTERNAL_STUB(cond)
  DECLARE_EXTERNAL_STUB(letrec)
  DECLARE_EXTERNAL_STUB(=:<=)
  DECLARE_EXTERNAL_STUB(=:<<=)
  DECLARE_EXTERNAL_STUB(ifVar)
  DECLARE_EXTERNAL_STUB(failure)

  // (+)
  #define FORWARD_CALL(prim, extname)                                  \
      auto prim = extern_<function>(ir.stepfun_t, #prim);              \
      extern_<function>(                                               \
          ir.stepfun_t, extname, {"root"}, [&] { prim(arg("root")); }  \
        );                                                             \
    /**/
  FORWARD_CALL(prim_Int_plus, "+")
  FORWARD_CALL(prim_Int_minus, "-")
  FORWARD_CALL(prim_Int_times, "*")
  
  llvm::WriteBitcodeToFile(module_b.ptr(), llvm::outs());
}
