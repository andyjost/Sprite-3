#include "sprite/compiler.hpp"
#include "sprite/runtime.hpp"
#include "llvm/Bitcode/ReaderWriter.h"
#include "llvm/Support/raw_ostream.h"

using namespace sprite::backend;

namespace sprite { namespace compiler
{
  function make_succ_function(function fun, ir_h const & ir, int64_t arity);
}}

// Builds a vtable for a constructor.
void build_vt_for_constructor(
    sprite::compiler::ir_h const & ir, std::string const & name, size_t arity
  )
{
  // Note: this would need to be improved if built-in constructors with arity>0
  // are added.  This code assumes the N function never needs to do anything.
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

  // Build the vtables for built-in constructors.
  #define SPRITE_HANDLE_BUILTIN(name, arity)      \
      build_vt_for_constructor(ir, #name, arity); \
    /**/
  #include "sprite/builtins.def"

  llvm::WriteBitcodeToFile(module_b.ptr(), llvm::outs());
}
