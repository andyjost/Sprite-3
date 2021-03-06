#include "sprite/backend/support/testing.hpp"
#include "llvm/Analysis/Verifier.h"
#include <llvm/Assembly/PrintModulePass.h>
#include <llvm/Pass.h>
#include <llvm/PassManager.h>
#include <llvm/IR/IRBuilder.h>
#include "llvm/ExecutionEngine/JIT.h"
#include "llvm/Support/TargetSelect.h"
#include <cstdlib>
#include <iostream>
#include <sstream>

using namespace llvm;
using namespace sprite::backend;

void sprite_dumpvalue(llvm::Value * arg) { arg->dump(); }
void sprite_dumptype(llvm::Value * arg) { arg->getType()->dump(); }
void sprite_dumptype(llvm::Type * arg) { arg->dump(); }

namespace sprite
{
  size_t init_debug()
  {
    size_t x = 0;
    x ^= (size_t)(void (*)(llvm::Value *))(&sprite_dumpvalue);
    x ^= (size_t)(void (*)(llvm::Value *))(&sprite_dumptype);
    x ^= (size_t)(void (*)(llvm::Type *))(&sprite_dumptype);
    return x;
  }
}

namespace
{
  static size_t const BUFSZ = 4096;

  /**
   * @brief Executes code in a module and checks its output.
   *
   * The module must have a function called "main" with the correct signature.
   * see test_function).
   */
  void run_and_test_module(
      module const & m, std::string const & expected_output
    )
  {
    verifyModule(*m.ptr(), PrintMessageAction);

    std::string err;
    ExecutionEngine * jit = EngineBuilder(m.ptr())
        .setErrorStr(&err)
        .setEngineKind(EngineKind::JIT)
        .create();
    if(!jit)
    {
      std::cerr << "Failed to create JIT compiler: " << err << std::endl;
      std::exit(EXIT_FAILURE);
    }
  
    // Run the program.
    char buf[BUFSZ];
    FILE * strm = fmemopen(buf, BUFSZ, "w");
    assert(strm);
  
    void * fp = jit->getPointerToFunction(m->getFunction("main"));
    int (*target_program)(FILE*) = (int(*)(FILE*))(intptr_t)(fp);
    if(target_program(strm) != 0)
      throw testing_error("The program did not return 0.");
  
    int const ok = fclose(strm);
    assert(ok == 0);
    (void) ok;
  
    // Check the output.
    std::string program_output(buf);
    if(program_output != expected_output)
    {
      std::stringstream ss;
      ss << "The program output did not match the expected result.\n"
         << "=== Program output: ===\n\n" << program_output << "\n\n"
         << "=== Expected output: ===\n\n" << expected_output << "\n\n"
         << std::endl;
      throw testing_error(ss.str());
    }
  }
}

namespace sprite { namespace backend { namespace testing
{
  void test_function(
      std::function<void(clib_h const &)> const & body
    , std::string const & expected_output
    , bool print_module
    , bool view_cfg
    )
  {
    // Build the module.
    using namespace sprite::backend;
    InitializeNativeTarget();

    module m(".anon");
    scope _ = m;
    clib_h clib;
  
    type int_ = get_type<int>();
  
    auto main = extern_(int_(clib.FILE_p), "main", {"file"});
    {
      scope _ = main;
      body(clib);
    }

    // Print debug info, if requested.
    if(print_module)
    {
      PassManager pm;
      pm.add(createPrintModulePass(&outs()));
      pm.run(*m.ptr());
    }

    if(view_cfg)
      main->viewCFG();
  
    // JIT Compile and check the output.
    run_and_test_module(m, expected_output);

    delete m.ptr(); // FIXME: this is pretty ugly.
  }
}}}
