#include <fstream>
#include <iostream>
#include "llvm/Bitcode/ReaderWriter.h"
#include "llvm/ExecutionEngine/JIT.h"
#include "llvm/Linker.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/system_error.h"
#include "llvm/Support/TargetSelect.h"
#include "sprite/compiler.hpp"
#include "sprite/config.hpp"
#include "sprite/curryinput.hpp"
#include "sprite/icurry_parser.hpp"

namespace
{
  std::string dirname(std::string const & path)
  {
    size_t const pos = path.find_last_of("/");
    return path.substr(0, pos == std::string::npos ? 0 : pos);
  }

  std::string basename(std::string const & path)
  {
    size_t const pos = path.find_last_of("/");
    return path.substr(pos == std::string::npos ? 0 : pos + 1);
  }

  std::string remove_extension(std::string const & path)
  {
    size_t const pos = path.find_last_of(".");
    return pos == std::string::npos ? path : path.substr(0, pos);
  }

  std::string joinpath(std::string const & dirname, std::string const & path)
  {
    if(!path.empty() && path.front() == '/')
      return path;
    if(dirname.empty())
      return path;
    return dirname.back() == '/' ? dirname + path : dirname + "/" + path;
  }
}

int main(int argc, char const *argv[])
{
  if(argc != 2)
  {
    std::cerr << "Usage: " << argv[0] << " <file.curry>" << std::endl;
    return 1;
  }

  std::string const curry2read =
      std::string(SPRITE_LIBINSTALL) + "/cmc/translator/bin/curry2read";
  std::string const curryfile(argv[1]);
  std::string const readablefile = joinpath(
      dirname(curryfile)
    , ".curry/" + remove_extension(basename(curryfile)) + ".read"
    );

  // Generate the readable Curry file.
  int ok = std::system((curry2read + " -q " + curryfile).c_str());
  if(ok != 0) return 1;
  std::ifstream input(readablefile);
  if(!input)
  {
    std::cerr << "Could not open \"" << readablefile << "\"" << std::endl;
    return 1;
  }

  // Parse the input program.
  sprite::curry::Library lib;
  input >> lib;
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

        // Evaluate and then print the root expression.
        compiler.rt.normalize(root_p);
        compiler.rt.printexpr(root_p, "\n");
        
        tgt::return_(0);
      }
    );

  // module_stab.module_ir->dump();

  // NOT READY YET
  #if 0
  // Load the runtime library.
  llvm::OwningPtr<llvm::MemoryBuffer> buffer;
  llvm::error_code err = llvm::MemoryBuffer::getFile(
      SPRITE_LIBINSTALL "/sprite-rt.bc", buffer
    );
  if(err)
  {
    std::cerr << err.message() << std::endl;
    return EXIT_FAILURE;
  }

  // Make the runtime library into a module.
  std::string errmsg;
  llvm::Module *rtlib = llvm::ParseBitcodeFile(
      buffer.get(), module_stab.module_ir.context(), &errmsg
    );
  if(!rtlib)
  {
    std::cerr << errmsg << std::endl;
    return EXIT_FAILURE;
  }

  // Link the compiled program code into the runtime module.
  bool failed = llvm::Linker::LinkModules(
      rtlib, module_stab.module_ir.ptr(), llvm::Linker::PreserveSource, &errmsg
    );
  if(failed)
  {
    std::cerr << errmsg << std::endl;
    return EXIT_FAILURE;
  }

  // Create the JIT
  llvm::InitializeNativeTarget();
  llvm::ExecutionEngine * jit = llvm::EngineBuilder(rtlib)
      .setErrorStr(&errmsg)
      .setEngineKind(llvm::EngineKind::JIT)
      .create();
  if(!jit)
  {
    std::cerr << "Failed to create JIT compiler: " << errmsg << std::endl;
    return EXIT_FAILURE;
  }

  // Execute the program.
  void * main_fp = jit->getPointerToFunction(rtlib->getFunction("main"));
  int32_t (*target_program)() = (int32_t(*)())(intptr_t)(main_fp);
  return target_program();
  #endif

  // Write a bitcode file and interpret it.
  {
    std::string err;
    llvm::raw_fd_ostream fout("sprite-out.bc", err, llvm::raw_fd_ostream::F_Binary);
    llvm::WriteBitcodeToFile(module_stab.module_ir.ptr(), fout);
  }
  std::system("llvm-link-3.3 sprite-out.bc " SPRITE_LIBINSTALL "/sprite-rt.bc > tmp.bc");
  std::system("mv tmp.bc sprite-out.bc");
  int const status = std::system("lli-3.3 sprite-out.bc");
  return WEXITSTATUS(status);
}

