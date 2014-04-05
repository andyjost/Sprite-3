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

int main()
{
  using namespace sprite::backend;

  module const fib_module("fib");
  scope _ = fib_module;
  auto const char_ = types::char_();
  auto const i32 = types::int_(32);
  auto const i64 = types::int_(64);

  // Declare external functions.
  auto const printf = extern_(i32(*char_, dots), "printf");
  // auto const gets = extern_((*char_)(*char_), "gets");
  // auto const atoi = extern_<Function>(i32(*char_), "atoi");

  auto const fib = extern_(i64(i64), "fib", {"n"});
  {
    scope _ = fib;
    value n = arg("n");
    if_(n <(signed_) (2), []{return_(1);});
    return_(fib(n-2) + fib(n-1));
  }

  auto const main = extern_(i32(), "main");
  {
    scope _ = main;
    value const x = fib(5);
    printf("fib(5)=%d\n", x);
    return_(0);
  }

  {
    using namespace llvm;
    verifyModule(*fib_module.ptr(), PrintMessageAction);
    PassManager pm;
    pm.add(createPrintModulePass(&outs()));
    pm.run(*fib_module.ptr());
  }

  return 0;
}
