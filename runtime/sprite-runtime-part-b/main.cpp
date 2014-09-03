#include "sprite/compiler.hpp"
#include "llvm/Bitcode/ReaderWriter.h"
#include "llvm/Support/raw_ostream.h"

#define SPRITE_HANDLE_BUILTIN(name)                              \
    void build_vt_for_##name(sprite::compiler::ir_h const & ir); \
  /**/
#include "sprite/builtins.def"

int main()
{
	sprite::backend::module module_b("sprit_runtime_part_b");
	sprite::backend::scope _ = module_b;
	sprite::compiler::ir_h ir;

  #define SPRITE_HANDLE_BUILTIN(name) build_vt_for_##name(ir);
  #include "sprite/builtins.def"

  llvm::WriteBitcodeToFile(module_b.ptr(), llvm::outs());
}
