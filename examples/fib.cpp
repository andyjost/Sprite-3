/**
 * @file
 * @brief A program that produces the hello world LLVM module.
 */

// FIXME: This does not belong in the examples directory, or the README needs
// to be updated.

#include "sprite/llvm.hpp"
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
  using namespace sprite::llvm;

  llvm::Module * fib_module
    = new llvm::Module("fib", llvm::getGlobalContext());
  TypeFactory const types(fib_module);
  auto const char_ = types.char_();
  auto const i32 = types.int_(32);
  auto const i64 = types.int_(64);

  // Declare printf.
  auto const printf = dyn_cast<llvm::Function>(
      extern_(i32(*char_, dots), "printf")
    );
  printf->setCallingConv(llvm::CallingConv::C);

  // Declare gets.
  auto const gets = dyn_cast<llvm::Function>(
      extern_((*char_)(*char_), "gets")
    );
  gets->setCallingConv(llvm::CallingConv::C);

  // Declare atoi.
  auto const atoi = dyn_cast<llvm::Function>(
      extern_(i32(*char_), "atoi")
    );
  atoi->setCallingConv(llvm::CallingConv::C);

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
  //     auto fib = extern_(i64(i64 | "n"), "fib");
  //     {
  //       scope _ = fib;
  //       auto const n = arg("n");
  //       label terminate, recurse;
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

  auto const fib = dyn_cast<llvm::Function>(extern_(i64(i64), "fib"));
  {
    using namespace llvm;
    BasicBlock* entryb = BasicBlock::Create(getGlobalContext(), "entry", fib.ptr());
    BasicBlock* terminateb = BasicBlock::Create(getGlobalContext(), "terminate", fib.ptr());
    BasicBlock* recurseb = BasicBlock::Create(getGlobalContext(), "recurse", fib.ptr());

    // Label: entry
    IRBuilder<> entry(entryb);
    auto const n = fib->arg_begin();
    auto const test = entry.CreateICmpULT(n, (i64 % 2).ptr());
    entry.CreateCondBr(test, terminateb, recurseb);

    // Label: terminate
    IRBuilder<> terminate(terminateb);
    terminate.CreateRet((i64 % 1).ptr());

    // Label: recurse
    IRBuilder<> recurse(recurseb);
    auto const a = recurse.CreateSub(n, (i64 % 1).ptr());
    auto const b = recurse.CreateSub(n, (i64 % 2).ptr());
    auto const c = recurse.CreateCall(fib.ptr(), a);
    auto const d = recurse.CreateCall(fib.ptr(), b);
    auto const e = recurse.CreateAdd(c, d);
    recurse.CreateRet(e);

  }

  auto const main = extern_(i32(), "main");
  auto const func_main = dyn_cast<llvm::Function>(main);

  {
    using namespace llvm;
    // TODO
    BasicBlock* entryb = BasicBlock::Create(getGlobalContext(), "entry", func_main.ptr());
    IRBuilder<> entry(entryb);
    auto const x = entry.CreateCall(fib.ptr(), (i64 % 6).ptr());
    entry.CreateCall2(printf.ptr(), (*char_ % "%d\n").ptr(), x);
    entry.CreateRet((i32 % 0).ptr());
  }

  {
    using namespace llvm;
    verifyModule(*fib_module, PrintMessageAction);
    PassManager pm;
    pm.add(createPrintModulePass(&outs()));
    pm.run(*fib_module);
  }

  return 0;
}

