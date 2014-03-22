/**
 * @file
 * @brief A program that produces the hello world LLVM module.
 */

// FIXME: This does not belong in the examples directory, or the README needs
// to be updated.

// FIXME: I'd like to embed a verification command in this file.  After running
// the executable produced by this file, that command should be run to define
// the passing criteria.  LLVM does something similar, but I need to revisit
// the details.

#include "sprite/backend.hpp"
#include <cmath>
#include <limits>

#include "llvm/Analysis/Verifier.h"
#include <llvm/Assembly/PrintModulePass.h>
#include <llvm/Pass.h>
#include <llvm/PassManager.h>
#include <llvm/IR/IRBuilder.h>

// DEBUG
#include <iostream>

/*
Goal:
    ; Declare the string constant as a global constant.
    @.str = private unnamed_addr constant [13 x i8] c"hello world\0A\00"
    
    ; External declaration of the puts function
    declare i32 @puts(i8* nocapture) nounwind
    
    ; Definition of main function
    define i32 @main() {   ; i32()*
      ; Convert [13 x i8]* to i8  *...
      %cast210 = getelementptr [13 x i8]* @.str, i64 0, i64 0
    
      ; Call puts function to write out the string to stdout.
      call i32 @puts(i8* %cast210)
      ret i32 0
    }

Problems:
  1. Need global variable attributes private and unnamed_addr.
  2. Need function attributes nocapture and nounwind.
  3. Need to populate the function body.
*/
int main()
{
  /// [Hello world]
  using namespace sprite::backend;

  module const hello_module("hello");
  auto const char_ = hello_module.char_();
  auto const i32 = hello_module.int_(32);
  auto const puts = extern_<Function>(i32(*char_), "puts");

  /// [Using context]
  auto const main = extern_<Function>(i32(), "main");
  {
    context _ = main.entry();
    puts(*char_ % "hello world\n");
    return_(0);
  }
  /// [Using context]
  /// [Hello world]

  {
    using namespace llvm;
    verifyModule(*hello_module.ptr(), PrintMessageAction);
    PassManager pm;
    pm.add(createPrintModulePass(&outs()));
    pm.run(*hello_module.ptr());
  }

  return 0;
}
