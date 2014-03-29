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

/*
    define i64 @fib(i64 %n) {
    entry:
      %test = icmp ult i64 %n, 2
      br i1 %test, label %terminate, label %recurse
    terminate:
      ret i64 1
    recurse:
      %a = sub i64 %n, 1
      %b = sub i64 %n, 2
      %c = call i64 @fib(i64 %a)
      %d = call i64 @fib(i64 %b)
      %e = add i64 %c, %d
      ret i64 %e
    }
    
    declare i32 @printf(i8 *, ...);
    
    @.str = private unnamed_addr constant [4 x i8] c"%d\0A\00"
    
    define i32 @main() {
    entry:
      %x = call i64 @fib(i64 6)
      %fmt = getelementptr [4 x i8]* @.str, i64 0, i64 0
      call i32 (i8*, ...)* @printf(i8* %fmt, i64 %x)
      ret i32 0
    }
*/

int main()
{
  using namespace sprite::backend;

  module const fib_module("fib");
  scope _ = fib_module;
  auto const char_ = types::char_();
  auto const i32 = types::int_(32);
  auto const i64 = types::int_(64);

  // Declare external functions.
  auto const printf = extern_<Function>(i32(*char_, dots), "printf");
  // auto const gets = extern_<Function>((*char_)(*char_), "gets");
  // auto const atoi = extern_<Function>(i32(*char_), "atoi");

  // C Syntax:
  //
  //     size_t fib(size_t n)
  //     {
  //       if(n<2)
  //         return 1;
  //       else
  //         return fib(n-2) + fib(n-1)
  //     }
  //
  // Goal Syntax:
  //
  //     auto fib = extern_(i64(i64), "fib", {"n"});
  //     {
  //       scope _ = fib;
  //       auto const n = arg("n");
  //       label terminate, recurse;
  //       if_(n < 2, []{return_(1)}) // could this work??
  //       if_(n < 2, terminate, recurse);
  //       {
  //         scope _ = terminate;
  //         return_(1);
  //       }
  //       {
  //         scope _ = recurse;
  //         return_(fib(n-2) + fib(n-1))
  //       }
  //     }

  auto const fib = extern_(i64(i64), "fib", {"n"});
  {
    scope _ = fib;
    value n = arg("n");
    if_(n < 2, []{return_(1);});
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

