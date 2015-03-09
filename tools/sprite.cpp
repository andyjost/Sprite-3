#include <iostream>
#include "llvm/Bitcode/ReaderWriter.h"
#include "llvm/ExecutionEngine/JIT.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/Linker.h"
#include "llvm/PassManager.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/system_error.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Transforms/IPO.h"
#include "llvm/Transforms/Scalar.h"
#include "sprite/compiler.hpp"
#include "sprite/config.hpp"
#include "sprite/curryinput.hpp"
#include "sprite/icurry_parser.hpp"
#include "sprite/commandline.hpp"

namespace
{
   llvm::LLVMContext & context = llvm::getGlobalContext();
  int main_(int argc, char const *argv[])
  {
    if(argc != 2)
    {
      std::cerr << "Usage: " << argv[0] << " <file.curry>" << std::endl;
      return 1;
    }

    sprite::export_sprite_lib_to_path();
    
    // Compile the input file.
    sprite::curry::Library lib;
    sprite::compiler::LibrarySTab stab;
    sprite::compile_file(argv[1], lib, stab, context, false, false);
    std::string topmodule = lib.modules.front().name;
  
    // Declare the main function.
    sprite::insert_main_function(
        stab, sprite::curry::Qname{topmodule, "main"}, false /*enable_tracing*/
      );
  
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
        buffer.get(), stab.context(), &errmsg
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
  
    std::cout << "Linking done..." << std::endl;
    rtlib->dump();
  
    // Run optimization passes.
    // std::vector<const char *> exportList;
    // llvm::PassManager Passes;
    // Passes.add(new llvm::DataLayout(rtlib));
    // Passes.add(llvm::createDemoteRegisterToMemoryPass());
    // Passes.add(llvm::createInternalizePass(exportList));
    // Passes.add(llvm::createScalarReplAggregatesPass());
    // Passes.add(llvm::createInstructionCombiningPass());
    // Passes.add(llvm::createGlobalOptimizerPass());
    // Passes.add(llvm::createFunctionInliningPass());
    // Passes.run(*rtlib);
  
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
    std::cout << "Begin Execution..." << std::endl;
    // rtlib->dump();
    void * main_fp = jit->getPointerToFunction(rtlib->getFunction("main"));
    int32_t (*target_program)() = (int32_t(*)())(intptr_t)(main_fp);
    std::cout << "Ready..." << std::endl;
    return target_program();
    #endif
  
    // Write a bitcode file and interpret it.
    {
      std::string err;
      auto & module_stab = stab.modules.at(topmodule);
      llvm::raw_fd_ostream fout("sprite-out.bc", err, llvm::raw_fd_ostream::F_Binary);
      llvm::WriteBitcodeToFile(module_stab.module_ir.ptr(), fout);
    }
    std::system("llvm-link-3.3 sprite-out.bc " SPRITE_LIBINSTALL "/sprite-rt.bc > tmp.bc");
    std::system("mv tmp.bc sprite-out.bc");
    int const status = std::system("lli-3.3 sprite-out.bc");
    return WEXITSTATUS(status);
  }
}

int main(int argc, char const *argv[])
{
  try
    { return main_(argc, argv); }
  catch(std::exception const & e)
  {
    std::cerr << e.what() << std::endl; 
    return EXIT_FAILURE;
  }
}

