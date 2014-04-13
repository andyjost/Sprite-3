#include "sprite/backend.hpp"
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Metadata.h>
#include "llvm/Analysis/Verifier.h"
#include <llvm/Assembly/PrintModulePass.h>

using namespace llvm;
using namespace sprite::backend;

int main()
{
  module m;
  scope _ = m;
  // type int_ = types::int_();
  type void_ = types::void_();

  // NamedMDNode *NMD = m->getOrInsertNamedMetadata("sprite");
  // NMD->addOperand(metadata(5).ptr());

  // ------
  // Define the module
  // ------

  // !0 = metadata !{ metadata !"an example type tree" }
  // !1 = metadata !{ metadata !"int", metadata !0 }
  // !2 = metadata !{ metadata !"float", metadata !0 }
  // !3 = metadata !{ metadata !"const float", metadata !2, i64 1 }
  auto _0 = metadata("an example type tree");
  auto _1 = metadata("int", _0);
  auto _2 = metadata("float", _0);
  auto _3 = metadata("const float", _2, 1);
  (void) _1;

  function f = static_(void_(), "f");
  {
    scope _ = f;
    f(); // FIXME: null functions fail to compile when called
    return_(0);
    return_(1).set_metadata("sprite.implied", _3);
  }

  // llvm::Instruction & ret = f->front().back();
  // ret.setMetadata("sprite", Node);
  // ret.setMetadata("sprite", m->getNamedMetadata("sprite")->getOperand(0));

  // ------
  // Clean up the module
  // ------

  auto it = f->front().rbegin();
  instruction a(&*it);
  // if(it->getMetadata("sprite.implied"))
  if(a.get_metadata("sprite.implied"))
  {
    std::cout << "Got sprite metadata" << std::endl;;
    // a->eraseFromParent();
  }

  m->dump();
  verifyModule(*m.ptr(), PrintMessageAction);
}
