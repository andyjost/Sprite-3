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
#include <cstdlib>
#include <sstream>
#include <cstring>
#include <vector>

#include <iostream> // DEBUG

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
    curry::Module const & cymodule = lib.modules.back();
    if(modulename != cymodule.name)
    {
      throw backend::compile_error(
          "File \"" + readablefile + "\" defines the wrong module ("
        + cymodule.name + ")."
        );
    }

    // Process the imported modules first.
    for(std::string const & import: cymodule.imports)
    {
      if(stab.modules.count(import) == 0)
        compile_file(get_module_file(import), lib, stab, context, false);
    }

    // Read the program IR from the .bc file, if possible.  Update the symbol
    // table with the IR.  The subsequent call to compile will fill in the rest
    // of the symbol table.
    bool const bitcode_up_to_date = 
         is_up_to_date(bitcodefile, readablefile)
      && is_up_to_date(bitcodefile, "/proc/self/exe");

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
        , compiler::ModuleSTab{cymodule, backend::module(M)}
        );
    }

    compiler::compile(cymodule, stab, context);

    // If asked to, write out the .bc file.
    if(save_bitcode && !bitcode_up_to_date)
    {
      std::string errmsg;
      llvm::raw_fd_ostream fout(
          bitcodefile.c_str(), errmsg, llvm::raw_fd_ostream::F_Binary
        );

      auto module = stab.modules.at(modulename).module_ir.ptr();
      llvm::WriteBitcodeToFile(module, fout);

      if(!errmsg.empty())
      {
        std::remove(bitcodefile.c_str());
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
      compiler::ModuleSTab const & module_stab
    , curry::Qname const & start
    )
  {
    backend::extern_(
        backend::types::int_(32)(), "main", {}
      , [&]{
          // Construct the root expression (just the "main" symbol).
          backend::value root_p = node_alloc(module_stab);
          root_p = construct(module_stab, root_p, {start, {}});

          // Evaluate and then print the root expression.
          module_stab.rt().normalize(root_p);
          module_stab.rt().printexpr(root_p, "\n");

          backend::return_(0);
        }
      );
  }

  void insert_main_function(
      compiler::LibrarySTab const & stab, curry::Qname const & start
    )
  {
    auto & module_stab = stab.modules.at(start.module);
    backend::scope _ = module_stab.module_ir;
    _create_main_function(module_stab, start);

  }

  bool is_up_to_date(
      std::string const & file, std::string const & relative_to
    )
  {
    struct stat file_stat, relative_stat;
    int err = ::stat(file.c_str(), &file_stat);
    if(err) return false;
    if(&file == &relative_to) return true; // used by get_module_file.
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

  std::vector<std::string> const & get_separated_currylib_paths()
  {
    static std::vector<std::string> paths;
    if(paths.empty())
    {
      std::string const & pathstr = get_currylib_path();
      size_t a = 0, b = 0;
      do
      {
        b = pathstr.find_first_of(':', a);
        std::string const elem = pathstr.substr(a, b-a);
        if(!elem.empty())
          paths.push_back(elem);
        a = b + 1;
      }
      while(b != std::string::npos);
    }
    return paths;
  }

  std::string const & get_currylib_path()
  {
    static std::string lib_path;
    if(lib_path.empty())
    {
      // Start with the CURRYPATH environment variable.
      char * user_path = std::getenv("CURRYPATH");
      if(user_path)
      {
        lib_path += user_path;
        lib_path += ":";
      }
      // Add the Sprite system library path.
      lib_path += join_path(SPRITE_LIBINSTALL "/", "currylib/");
    }
    return lib_path;
  }

  std::string get_module_file(std::string const & module)
  {
    std::string const file = module + ".curry";
    for(std::string const & path: get_separated_currylib_paths())
    {
      std::string full_path = join_path(path, file);
      if(is_up_to_date(full_path, full_path))
        return full_path;
    }
    throw backend::compile_error(
        "No Curry file for module \"" + module + "\" was found in path "
          + get_currylib_path()
      );
  }

  std::string const & get_curry2read()
  {
    static std::string curry2read = join_path(
        SPRITE_LIBINSTALL "/", "cmc/translator/bin/curry2read"
      );
    return curry2read;
  }

  std::string const & get_llc()
  {
    static std::string llc = join_path(SPRITE_LIBINSTALL, "llc");
    return llc;
  }

  std::string const & get_cc()
  {
    static std::string cc = join_path(SPRITE_LIBINSTALL, "cc");
    return cc;
  }

  void make_readable_file(std::string const & inputfile)
  {
    std::string const curryfile = sprite::get_curryfile(inputfile);
    std::string const readablefile = sprite::get_readablefile(curryfile);
    if(!is_up_to_date(readablefile, curryfile))
    {
      std::stringstream cmd;
      cmd << sprite::get_curry2read() << " -q " << curryfile;
      int ok = std::system(cmd.str().c_str());
      if(ok != 0)
        throw backend::compile_error("curry2read failed");
    }
  }

  void make_assembly_file(
      std::string const & bitcodefile, std::string const & assemblyfile
    , bool remove_source
    )
  {
    std::stringstream cmd;
    cmd << sprite::get_llc() << " " << bitcodefile << " -o " << assemblyfile;
    int ok = std::system(cmd.str().c_str());
    if(remove_source && std::remove(bitcodefile.c_str()) != 0 && ok != 0)
    {
      cmd.str("");
      cmd << "Error removing \"" << bitcodefile << "\": " << strerror(errno);
      throw backend::compile_error(cmd.str());
    }
    if(ok != 0)
      throw backend::compile_error("llc failed");
  }

  /// Uses the platform-specific compiler to convert assembly to an executable.
  void make_executable_file(
      std::string const & assemblyfile, std::string const & executablefile
    , bool remove_source
    )
  {
    std::stringstream cmd;
    cmd << sprite::get_cc() << " " << assemblyfile << " -o " << executablefile;
    int ok = std::system(cmd.str().c_str());
    if(remove_source && std::remove(assemblyfile.c_str()) != 0 && ok != 0)
    {
      cmd.str("");
      cmd << "Error removing \"" << assemblyfile << "\": " << strerror(errno);
      throw backend::compile_error(cmd.str());
    }
    if(ok != 0)
      throw backend::compile_error("cc failed");
  }
}
