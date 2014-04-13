// Tests branch instructions.
#include "sprite/backend.hpp"
#include "llvm/Analysis/Verifier.h"
#include <llvm/Assembly/PrintModulePass.h>
#include <llvm/Pass.h>
#include <llvm/PassManager.h>
#include <llvm/IR/IRBuilder.h>
#include "llvm/ExecutionEngine/JIT.h"
#include "llvm/Support/TargetSelect.h"
#include <cstdlib>

using namespace llvm;
using namespace sprite::backend;

static size_t const BUFSZ = 4096;

// The module must have a function called main that takes no arguments and 
void check_module(module const & m, std::string const & expected_output)
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

  // PassManager pm;
  // pm.add(createPrintModulePass(&outs()));
  // pm.run(*m.ptr());

  // Run the program.
  char buf[BUFSZ];
  FILE * strm = fmemopen(buf, BUFSZ, "w");
  assert(strm);

  void * fp = jit->getPointerToFunction(m->getFunction("main"));
  int (*target_program)(FILE*) = (int(*)(FILE*))(intptr_t)(fp);
  target_program(strm);

  int const ok = fclose(strm);
  assert(ok == 0);

  // Check its output.
  std::string program_output(buf);
  if(program_output != expected_output)
  {
    std::cerr
        << "Program output:\n\n" << program_output
        << "\n\n!=\n\nExpected output:\n\n" << expected_output
        << std::endl;
    assert(0);
  }
}

using funtab_t = std::map<std::string, function>;

// Represents a header file for declaring certain C library functions.
struct clib_h
{
  type char_ = types::char_();
  type int_ = get_type<int>();
  type size_t_= get_type<size_t>();
  type FILE_p = *types::struct_("FILE");

  // int snprintf(char *str, size_t size, const char *format, ...);
  function const snprintf = extern_(int_(*char_, size_t_, *char_, dots), "snprintf");
  // FILE * fmemopen (void *buf, size_t size, const char *opentype)
  function const fmemopen = extern_(FILE_p(*char_, size_t_, *char_), "fmemopen");
  // int fprintf(FILE *stream, const char *format, ...);
  function const fprintf = extern_(int_(FILE_p, *char_, dots), "fprintf");
  // int fprintf(const char *format, ...);
  function const printf = extern_(int_(*char_, dots), "printf");
  // int fflush(FILE *stream);
  function const fflush = extern_(int_(FILE_p), "fflush");
  // int fclose(FILE *fp);
  function const fclose = extern_(int_(FILE_p), "fclose");
};

// Builds a module, compiles it, and then runs the main function.  The body of
// main is provided by calling main_body in the scope of the main function.
// When the program is run, its output should match expected_output.
template<typename MainBody>
void run_test(MainBody const & main_body, std::string const & expected_output)
{
  module m;
  scope _ = m;
  clib_h clib;

  // type char_ = types::char_();
  type int_ = get_type<int>();

  auto main = extern_(int_(clib.FILE_p), "main", {"file"});
  {
    scope _ = main;
    main_body(clib);
  }

  check_module(m, expected_output);
  delete m.ptr(); // FIXME: this is pretty ugly.
}

int main()
{
  InitializeNativeTarget();

  // Test the "hello world!" program.
  run_test(
      [](clib_h const & clib)
      {
        clib.fprintf(arg("file"), "hello world!");
        return_(0);
      }
    , "hello world!"
    );

  // Test goto.
  run_test(
      [](clib_h const & clib)
      {
        value file = arg("file");
        clib.fprintf(file, "a");
        label l;
        goto_(l);
        { // l:
          scope _ = l;
          clib.fprintf(file, "b");
          return_(0);
        }
      }
    , "ab"
    );

  // Test if with two branches.
  // run_test(
  //     [](clib_h const & clib)
  //     {
  //       value file = arg("file");
  //       clib.fprintf(file, "a");
  //       if_(1
  //         , [&]{clib.fprintf(file, "b"); return_(0);}
  //         , [&]{clib.fprintf(file, "c"); return_(0);}
  //         );
  //       // TODO: need to remove the empty basic block, here.
  //     }
  //   , "ab"
  //   );
  // run_test(
  //     [](clib_h const & clib)
  //     {
  //       value file = arg("file");
  //       clib.fprintf(file, "a");
  //       if_(1, [&]{clib.fprintf(file, "b");}, [&]{clib.fprintf(file, "c");});
  //       clib.fprintf(file, "d");
  //       return_(0);
  //     }
  //   , "abd"
  //   );
  // run_test(
  //     [](clib_h const & clib)
  //     {
  //       value file = arg("file");
  //       clib.fprintf(file, "a");
  //       if_(0, [&]{clib.fprintf(file, "b");}, [&]{clib.fprintf(file, "c");});
  //       clib.fprintf(file, "d");
  //       return_(0);
  //     }
  //   , "acd"
  //   );
}
