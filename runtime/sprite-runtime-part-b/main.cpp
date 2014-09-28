#include "sprite/compiler.hpp"
#include "sprite/runtime.hpp"
#include "llvm/Bitcode/ReaderWriter.h"
#include "llvm/Support/raw_ostream.h"

using namespace sprite::backend;

#define SPRITE_HANDLE_BUILTIN(name)                              \
    void build_vt_for_##name(sprite::compiler::ir_h const & ir); \
  /**/
#include "sprite/builtins.def"

namespace sprite { namespace compiler
{
  function make_succ_function(function fun, ir_h const & ir, int64_t arity);
}}

// Create the vtable for FAIL nodes.
void build_vt_for_fail(sprite::compiler::ir_h const & ir)
{
  extern_(ir.vtable_t, sprite::compiler::get_vt_name("fail"))
      .set_initializer(_t(
          &get_label_function(ir, "failed")
        , &get_arity_function(ir, 0)
        , &get_succ_function(ir, 0)
        , &get_null_step_function(ir)
        , &get_null_step_function(ir)
        ))
	  ;
}

int main()
{
	module module_b("sprit_runtime_part_b");
	scope _ = module_b;
	sprite::compiler::ir_h ir;

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

  #define SPRITE_HANDLE_BUILTIN(name) build_vt_for_##name(ir);
  #include "sprite/builtins.def"

  llvm::WriteBitcodeToFile(module_b.ptr(), llvm::outs());
}
