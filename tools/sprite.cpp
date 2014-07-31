#include "sprite/icurry_parser.hpp"
#include "sprite/curryinput.hpp"
#include "sprite/compiler.hpp"
#include "llvm/Bitcode/ReaderWriter.h"
#include "llvm/Support/raw_ostream.h"
#include <iostream>

int main()
{
  // Parse the input program.
  sprite::curry::Library lib;
  std::cin >> lib;
  std::string topmodule = lib.modules.front().name;
  // sprite::compiler::prettyprint(lib);

  // Compile the program.
  sprite::compiler::LibrarySTab stab;
  sprite::compiler::compile(lib, stab);

  // Declare the main function.
  namespace tgt = sprite::backend;
  auto & module_stab = stab.modules.at(topmodule);
  auto & compiler = *module_stab.compiler;
  tgt::scope _ = module_stab.module_ir;
  tgt::extern_(
      tgt::types::int_(32)(), "main", {}
    , [&]{
        // Construct the root expression (just the "main" symbol).
        tgt::value root_p = compiler.node_alloc();
        sprite::curry::Qname const main_{topmodule, "main"};
        root_p = construct(compiler, root_p, {main_, {}});

        // Print, evaluate, and then print the root expression.
        compiler.rt.printexpr(root_p, "\n");
        compiler.rt.normalize(root_p);
        compiler.rt.printexpr(root_p, "\n");
        
        tgt::return_(0);
      }
    );

  // Write a bitcode file and interpret it.
  {
    std::string err;
    llvm::raw_fd_ostream fout("sprite-out.bc", err, llvm::raw_fd_ostream::F_Binary);
    llvm::WriteBitcodeToFile(module_stab.module_ir.ptr(), fout);
  }
  std::system("llvm-link sprite-out.bc ../runtime/sprite-rt.bc > tmp.bc");
  std::system("mv tmp.bc sprite-out.bc");
  int const status = std::system("lli sprite-out.bc");
  return WEXITSTATUS(status);
}

