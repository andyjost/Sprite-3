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
#include <cstdio>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

namespace
{
  llvm::LLVMContext & context = llvm::getGlobalContext();
  bool compile_only = false;
  bool preprocess_only = false;
  int save_temps = 0;
  char optlvl = '3'; // 0, 1, 2, 3, s, or z
  std::string mainmodule;
  std::string outputfile = "a.out";
  std::vector<std::string> files;
  std::vector<llvm::Module*> modules;

  enum OutputType { OUTPUT_BITCODE=0, OUTPUT_ASSEMBLY=1, OUTPUT_EXECUTABLE=2 };
  OutputType output_type = OUTPUT_EXECUTABLE;

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
      << "   -b, --output-bitcode\n"
      << "       Write out the final program as LLVM bitcode.\n"
      << "   -c, --compile\n"
      << "       Compile only (do not link).\n"
      << "   -E, --preprocess\n"
      << "       Proprocess the source into ICurry only.\n"
      << "   -h, --help\n"
      << "       Display this help message.\n"
      << "   -m MODULE, --main=MODULE\n"
      << "       Start the program using the 'main' function in MODULE.\n"
      << "   -o FILE, --output=FILE\n"
      << "       Write the final program to FILE.\n"
      << "   --save-temps\n"
      << "       Save temporary files.\n"
      << "   -S, --output-assembly\n"
      << "       Write out the final program as assembly.\n"
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
        {"output-bitcode",  no_argument,  0, 'b'},
        {"compile",         no_argument,  0, 'c'},
        {"preprocess",      no_argument,  0, 'E'},
        {"help",            no_argument,  0, 'h'},
        {"main",            no_argument,  0, 'm'},
        {"optlvl",          no_argument,  0, 'O'},
        {"output",          no_argument,  0, 'o'},
        {"output-assembly", no_argument,  0, 'S'},
        {"save-temps",      no_argument, &save_temps, 1},
        {0, 0, 0, 0}
      };

      if(optind == argc)
        break;

      int const i = getopt_long(argc, argv, "bcEhm:o:S", long_options, 0);

      switch(i)
      {
        case 0: break;
        case -1:
          files.push_back(argv[optind]);
          argv++;
          argc--;
          break;
        case 'b':
          output_type = OUTPUT_BITCODE;
          break;
        case 'c':
          compile_only = true;
          break;
        case 'E':
          preprocess_only = true;
          break;
        case 'h':
          say_usage(std::cout);
          exit(EXIT_SUCCESS);
        case 'm':
          mainmodule = optarg;
          files.push_back(mainmodule);
          break;
        case 'O':
          optlvl = optarg[0];
          if(optlvl == '\0' || optarg[1] != '\0' ||
              (optlvl != '0' && optlvl != '1' && optlvl != '2'
                && optlvl != '3' && optlvl != 's' && optlvl != 'z'
                )
            )
          {
            std::cerr << "invalid optimization level: -O" << optarg << std::endl;
            exit(EXIT_FAILURE);
          }
          break;
        case 'o':
          outputfile = optarg;
          break;
        case 'S':
          output_type = OUTPUT_ASSEMBLY;
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

  void write_bitcode_to_file(llvm::Module * M, std::string const & filename)
  {
    std::string errstr;
    llvm::raw_fd_ostream fout(
        filename.c_str(), errstr, llvm::raw_fd_ostream::F_Binary
      );
    llvm::WriteBitcodeToFile(M, fout);
    if(!errstr.empty())
      throw sprite::backend::compile_error(errstr);
  }

  int main_(int argc, char *argv[])
  {
    sprite::export_sprite_lib_to_path();
    parse_args(argc, argv);

    // Compile each Curry file.
    sprite::curry::Library lib;
    sprite::compiler::LibrarySTab stab;
    for(auto const & file: files)
    {
      sprite::make_readable_file(file);
      if(!preprocess_only)
        sprite::compile_file(file, lib, stab, context, compile_only);
    }

    // Link the program, if requested.  By default, add a main symbol to any
    // module that has a Curry function named main.  The linker will take care
    // of any problems (multiple or zero definitions).  But if the main module
    // was specified, just create a single main function.  That allows them to
    // have ignored main functions (say, for module-level unit tests).
    // Detecting the main function only works if the input was Curry rather
    // than bitcode.
    if(!preprocess_only && !compile_only)
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

      // Determine the names used for the final output and any temporary files.
      std::string final_bitcode;
      std::string final_assembly;
      std::string final_executable;
      std::string const final_base = sprite::remove_extension(outputfile);

      switch(output_type)
      {
        case OUTPUT_BITCODE:
          final_bitcode = outputfile;
          break;
        case OUTPUT_ASSEMBLY:
          final_bitcode = final_base + ".bc";
          final_assembly = outputfile;
          break;
        case OUTPUT_EXECUTABLE:
          final_bitcode = final_base + ".bc";
          final_assembly = final_base + ".s";
          final_executable = outputfile;
          break;
      }

      // Write out the program file(s).
      if(optlvl == '0')
        write_bitcode_to_file(pgm, final_bitcode);
      else
      {
        std::string const unopt_bitcode = final_base + "-unopt.bc";
        write_bitcode_to_file(pgm, unopt_bitcode);
        sprite::make_optimized_bitcode(unopt_bitcode, final_bitcode, optlvl, !save_temps);
      }

      if(output_type > OUTPUT_BITCODE)
        sprite::make_assembly_file(final_bitcode, final_assembly, !save_temps);
      
      if(output_type > OUTPUT_ASSEMBLY)
        sprite::make_executable_file(final_assembly, final_executable, !save_temps);
    }

    return EXIT_SUCCESS;
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
