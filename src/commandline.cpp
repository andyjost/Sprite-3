#include "sprite/backend.hpp"
#include "sprite/compiler.hpp"
#include "sprite/config.hpp"
#include "sprite/curryinput.hpp"
#include "sprite/icurry_parser.hpp"
#include "sprite/commandline.hpp"
#include "llvm/Bitcode/ReaderWriter.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/system_error.h"
#include "llvm/Support/raw_ostream.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fstream>
#include <cstdio>

namespace sprite
{
  void compile_file(
      std::string const & curryfile
    , curry::Library & lib
    , compiler::LibrarySTab & stab
    , llvm::LLVMContext & context
    , bool save_bitcode
    )
  {
    std::string const modulename = sprite::get_modulename(curryfile);
    std::string const readablefile = sprite::get_readablefile(curryfile);
    std::string const bitcodefile = sprite::get_bitcodefile(curryfile);
  
    // Generate the readable Curry file.
    sprite::make_readable_file(curryfile);
    std::ifstream input(readablefile);
    if(!input)
      throw backend::compile_error("Could not open \"" + readablefile + "\"");
  
    // Parse the input program.
    input >> lib;
    if(modulename != lib.modules.back().name)
    {
      throw backend::compile_error(
          "File \"" + readablefile + "\" defines the wrong module ("
        + lib.modules.back().name + ")."
        );
    }

    // Read the program IR from the .bc file, if possible.  Update the symbol
    // table with the IR.  The subsequent call to compile will fill in the rest
    // of the symbol table.
    bool const bitcode_up_to_date = is_up_to_date(bitcodefile, readablefile);
    if(bitcode_up_to_date)
    {
      std::string errmsg;
      llvm::Module * M = load_compiled_module(bitcodefile, context, errmsg);
      if(!M)
      {
        throw backend::compile_error(
            "Error reading bitcode from \"" + bitcodefile + "\": " + errmsg
          );
      }
      stab.modules.emplace(
          modulename
        , compiler::ModuleSTab{stab, lib.modules.back(), backend::module(M)}
        );
    }

    compiler::compile(lib, stab, context);

    // If asked to, write out the .bc file.
    if(!bitcode_up_to_date && save_bitcode)
    {
      std::string errmsg;
      llvm::raw_fd_ostream fout(
          bitcodefile.c_str(), errmsg, llvm::raw_fd_ostream::F_Binary
        );

      auto module = stab.modules.at(modulename).module_ir.ptr();
      llvm::WriteBitcodeToFile(module, fout);

      if(!errmsg.empty())
      {
        try { std::remove(bitcodefile.c_str()); } catch(...) {}
        throw sprite::backend::compile_error(
            "Error while writing bitcode file \"" + bitcodefile + "\": "
          + errmsg
          );
      }
    }
  }

  llvm::Module * load_compiled_module(
      std::string const & filename, llvm::LLVMContext & context
    , std::string & errmsg
    )
  {
    llvm::OwningPtr<llvm::MemoryBuffer> buffer;
    llvm::error_code ec;
    ec = llvm::MemoryBuffer::getFile(filename, buffer);
    if(ec)
    {
      ec = llvm::MemoryBuffer::getFile(
          join_path(SPRITE_LIBINSTALL "/", filename), buffer
        );
    }
    if(ec)
    {
      errmsg = ec.message();
      return 0;
    }
    return llvm::ParseBitcodeFile(buffer.get(), context, &errmsg);
  }

  void _create_main_function(
      compiler::ModuleCompiler const & compiler
    , curry::Qname const & start
    )
  {
    backend::extern_(
        backend::types::int_(32)(), "main", {}
      , [&]{
          // Construct the root expression (just the "main" symbol).
          backend::value root_p = compiler.node_alloc();
          root_p = construct(compiler, root_p, {start, {}});
  
          // Evaluate and then print the root expression.
          compiler.rt.normalize(root_p);
          compiler.rt.printexpr(root_p, "\n");
          
          backend::return_(0);
        }
      );
  }

  void insert_main_function(
      compiler::LibrarySTab const & stab, curry::Qname const & start
    )
  {
    auto & module_stab = stab.modules.at(start.module);
    auto & compiler = *module_stab.compiler;
    backend::scope _ = module_stab.module_ir;
    _create_main_function(compiler, start);
    
  }

  bool is_up_to_date(
      std::string const & file, std::string const & relative_to
    )
  {
    struct stat file_stat, relative_stat;
    int err = ::stat(file.c_str(), &file_stat);
    if(err) return false;
    err = ::stat(relative_to.c_str(), &relative_stat);
    if(err) return false;
    return relative_stat.st_mtime <= file_stat.st_mtime;
  }

  std::string get_curryfile(std::string const & inputfile)
  {
    std::string const extension = get_extension(inputfile);
    if(!extension.empty() && extension != ".curry")
    {
      throw backend::compile_error(
          "Invalid extension (" + extension + ") for Curry file."
        );
    }
    std::string const basedir = dirname(inputfile);
    std::string const modulename = get_modulename(inputfile);
    return join_path(basedir, modulename + ".curry");
  }

  std::string const & get_curry2read()
  {
    static std::string curry2read =
        std::string(SPRITE_LIBINSTALL) + "/cmc/translator/bin/curry2read";
    return curry2read;
  }

  void make_readable_file(std::string const & inputfile)
  {
    std::string const curryfile = sprite::get_curryfile(inputfile);
    std::string const readablefile = sprite::get_readablefile(curryfile);
    if(!is_up_to_date(readablefile, curryfile))
    {
      std::string const & curry2read = sprite::get_curry2read();
      int ok = std::system((curry2read + " -q " + curryfile).c_str());
      if(ok != 0)
        throw backend::compile_error("curry2read failed");
    }
  }
}
