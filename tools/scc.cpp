#include "sprite/compiler.hpp"
#include "sprite/config.hpp"
#include "sprite/commandline.hpp"
#include "sprite/backend/support/exceptions.hpp"
#include <iostream>
#include "llvm/Linker.h"
#include "llvm/Bitcode/ReaderWriter.h"
#include "llvm/Support/raw_ostream.h"
#include <getopt.h>
#include <set>
#include <vector>

namespace
{
  llvm::LLVMContext & context = llvm::getGlobalContext();
  bool compile_only = false;
  std::string mainmodule;
  std::string outputfile = "a.out";
  std::vector<std::string> files;
  std::vector<llvm::Module*> modules;

  template<typename Vector>
  void remove_duplicates(Vector & v)
  {
    std::set<typename Vector::value_type> seen;
    Vector u;
    u.reserve(v.size());
    for(auto && item: v)
    {
      if(seen.count(item) == 0)
      {
        seen.insert(item);
        u.emplace_back(std::move(item));
      }
    }
    u.swap(v);
  }
  
  void say_usage(std::ostream & out)
  {
    out
      << "Usage: scc [options] [curryfiles...]\n"
      << "Options:\n"
      << "   -c, --compile\n"
      << "       Compile only (do not link).\n"
      << "   -h, --help\n"
      << "       Display this help message.\n"
      << "   -m FILE, --main=FILE\n"
      << "       Start the program using the 'main' function in FILE.\n"
      << "   -o FILE, --output=FILE\n"
      << "       Write the final program to FILE.\n"
      ;
  }

  void parse_args(int argc, char *argv[])
  {
    // Process any positional arguments before the first option.  getopts_long
    // will skip them.
    while(argc > 1)
    {
      if(*argv[1] != '-')
      {
        files.push_back(argv[1]);
        argv++;
        argc--;
      }
      else break;
    }

    // Process options and remaining positional arguments.
    while(true)
    {
      static option long_options[] =
      {
        {"compile", no_argument,  0, 'c'},
        {"help",    no_argument,  0, 'h'},
        {"main",    no_argument,  0, 'm'},
        {"output",  no_argument,  0, 'o'},
        {0, 0, 0, 0}
      };

      if(optind == argc)
        break;

      int const i = getopt_long(argc, argv, "chm:o:", long_options, 0);

      switch(i)
      {
        case -1:
          files.push_back(argv[optind]);
          argv++;
          argc--;
          break;
        case 'c':
          compile_only = true;
          break;
        case 'h':
          say_usage(std::cout);
          exit(EXIT_SUCCESS);
        case 'm':
          mainmodule = optarg;
          files.push_back(mainmodule);
          break;
        case 'o':
          outputfile = optarg;
          break;
        default:
          std::exit(EXIT_FAILURE);
      }
    }

    remove_duplicates(files);

    if(files.empty())
    {
      say_usage(std::cerr);
      std::exit(EXIT_FAILURE);
    }
  }

  void insert_main_function(
      sprite::compiler::LibrarySTab const & stab
    , std::string const & module_name
    , sprite::curry::Function const & fun
    )
  {
    if(fun.arity != 0)
    {
      std::string s = fun.arity == 1 ? std::string() : std::string("s");
      throw sprite::backend::compile_error(
          "'main' symbol in module '" + module_name + "' takes "
        + std::to_string(fun.arity) + " argument" + s
        + ", but should take zero."
        );
    }
    sprite::curry::Qname const start{module_name, fun.name};
    sprite::insert_main_function(stab, start);
  }

  // A wrapper for the library function.  Exits on failure.
  llvm::Module * load_compiled_module(std::string const & filename)
  {
    std::string errmsg;
    llvm::Module * M = sprite::load_compiled_module(filename, context, errmsg);
    if(!M)
    {
      std::cerr
        << "scc: an error occurred while loading a compiled module from "
        << "'" << filename << "': " << errmsg << std::endl
        ;
      std::exit(EXIT_FAILURE);
    }
    return M;
  }

  int main_(int argc, char *argv[])
  {
    parse_args(argc, argv);

    // Compile each Curry file.
    sprite::curry::Library lib;
    sprite::compiler::LibrarySTab stab;
    for(auto const & file: files)
      sprite::compile_file(file, lib, stab, context, compile_only);

    // Link the program, if requested.  By default, add a main symbol to any
    // module that has a Curry function named main.  The linker will take care
    // of any problems (multiple or zero definitions).  But if the main module
    // was specified, just create a single main function.  That allows them to
    // have ignored main functions (say, for module-level unit tests).
    // Detecting the main function only works if the input was Curry rather
    // than bitcode.
    if(!compile_only)
    {
      for(sprite::curry::Module const & mod: lib.modules)
      {
        if(!mainmodule.empty() && mainmodule != mod.name)
          continue;

        for(sprite::curry::Function const & fun: mod.functions)
        {
          if(fun.name == "main")
            insert_main_function(stab, mod.name, fun);
        }
      }

      // Load the runtime library.
      llvm::Module * pgm = load_compiled_module("sprite-rt.bc");
  
      // Link the compiled modules into the runtime module.
      for(auto const & item: stab.modules)
      {
        auto const & module_ir = item.second.module_ir;
        std::string errmsg;
        bool failed = llvm::Linker::LinkModules(
            pgm, module_ir.ptr(), llvm::Linker::PreserveSource, &errmsg
          );
        if(failed)
        {
          std::cerr << errmsg << std::endl;
          return EXIT_FAILURE;
        }
      }

      // Write out the final program.
      std::string errstr;
      llvm::raw_fd_ostream fout(
          outputfile.c_str(), errstr, llvm::raw_fd_ostream::F_Binary
        );
      llvm::WriteBitcodeToFile(pgm, fout);
      if(!errstr.empty())
        throw sprite::backend::compile_error(errstr);
    }

    return 0;
  }
}

int main(int argc, char *argv[])
{
  try
    { return main_(argc, argv); }
  catch(std::exception const & e)
  {
    std::cerr << e.what() << std::endl; 
    return EXIT_FAILURE;
  }
}
