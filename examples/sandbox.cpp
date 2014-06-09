// FIXME: This does not belong in the examples directory, or the README needs
// to be updated.

#include "sprite/backend.hpp"
#include <cmath>
#include <limits>
#include <iostream>

#include "llvm/Analysis/Verifier.h"
#include <llvm/Assembly/PrintModulePass.h>
#include <llvm/Pass.h>
#include <llvm/PassManager.h>
#include <llvm/IR/IRBuilder.h>
#include "llvm/ExecutionEngine/JIT.h"
#include "llvm/Support/TargetSelect.h"

using namespace sprite::backend;

module build_module()
{

  module const sb_module("sandbox");
  scope _ = sb_module;
  auto const char_ = types::char_();
  auto const i32 = types::int_(32);
  auto const i64 = types::int_(64);

  // Declare external functions.
  auto const printf = extern_(i32(*char_, dots), "printf");
  auto const gets = extern_((*char_)(*char_), "gets");
  auto const fflush = extern_(i32(*char_), "fflush");

  auto const main = extern_(i32(), "main");
  {
    scope _ = main;

    value x = &local(i64);
    *x = 42;

    ref y = local(i64);
    y = 5;

    printf("hello world!, x=%d, y=%d\n", *x, y);
    printf("Enter some text: \n");
    fflush(nullptr);
    ref buf = local(char_, 256);
    value ok = gets(&buf);
    if_(/*!*/ok, [&printf] // FIXME: the bool test is not really working
    {
      printf("gets failed!");
      return_(1);
    });
    printf("You entered \"%s\"\n", &buf);
    return_(0);
  }

  return sb_module;
}

int main()
{
  module sb_module = build_module();

  using namespace llvm;
  verifyModule(*sb_module.ptr(), PrintMessageAction);

  InitializeNativeTarget();
  std::string err;
  ExecutionEngine * jit = EngineBuilder(sb_module.ptr())
      .setErrorStr(&err)
      .setEngineKind(EngineKind::JIT)
      .create();
  if(!jit)
  {
    std::cout << "Failed to create JIT: " << err << std::endl;
    return 1;
  }

  PassManager pm;
  pm.add(createPrintModulePass(&outs()));
  pm.run(*sb_module.ptr());

  void * fp = jit->getPointerToFunction(sb_module->getFunction("main"));
  int32_t (*go)() = (int32_t(*)())(intptr_t)(fp);
  std::cout << "Running the program" << std::endl;
  go();
  std::cout << "Done" << std::endl;
  return 0;
}
