#include "sprite/compiler.hpp"
#include "llvm/Bitcode/ReaderWriter.h"
#include "llvm/Support/raw_ostream.h"

void build_fwd_vt(sprite::compiler::ir_h const & ir);

int main()
{
	sprite::backend::module module_b("sprit_runtime_part_b");
	sprite::backend::scope _ = module_b;
	sprite::compiler::ir_h ir;

  build_fwd_vt(ir);

  llvm::WriteBitcodeToFile(module_b.ptr(), llvm::outs());
}
