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
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

static bool remove_file(std::ostream & err, std::string const & file)
{
  if(std::remove(file.c_str()) != 0)
  {
    err << "Error removing \"" << file << "\": " << strerror(errno);
    return false;
  }
  return true;
}

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
    bool const bitcode_is_up_to_date = 
         is_up_to_date(bitcodefile, readablefile)
      && is_up_to_date(bitcodefile, "/proc/self/exe");

    if(bitcode_is_up_to_date)
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
    if(save_bitcode && !bitcode_is_up_to_date)
    {
      std::string errmsg;
      llvm::raw_fd_ostream fout(
          bitcodefile.c_str(), errmsg, llvm::raw_fd_ostream::F_Binary
        );

      auto module = stab.modules.at(modulename).module_ir.ptr();
      llvm::WriteBitcodeToFile(module, fout);

      if(!errmsg.empty())
      {
        remove_file(std::cerr, bitcodefile);
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
    using namespace backend;
    using namespace sprite::compiler::member_labels;
    auto const & rt = module_stab.rt();

    function const print_action = extern_(
        rt.stepfun_t, "main.print_action", {"root"}
      , [&] {
          label redo = rt.make_restart_point();
          value show = rt.node_alloc(*rt.node_t, redo);
          curry::Qname const lshow {"Prelude", "show"};
          show.arrow(ND_VPTR) = &module_stab.lookup(lshow).vtable;
          show.arrow(ND_TAG) = sprite::compiler::OPER;
          show.arrow(ND_SLOT0) = bitcast(arg("root"), *rt.char_t);
          rt.Cy_Normalize(show);
          rt.Cy_CyStringToCString(show, rt.stdout_());
          rt.putchar('\n');
        }
      );

    extern_(
        types::int_(32)(), "main", {}
      , [&]{
          label redo = rt.make_restart_point();
          value root_p = rt.node_alloc(*rt.node_t, redo);
          root_p = construct(module_stab, root_p, {start, {}});
          rt.Cy_Eval(root_p, &print_action);
          return_(0);
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

  void export_sprite_lib_to_path()
  {
    char const * oldpath = std::getenv("PATH");
    std::string newpath =
        std::string(SPRITE_LIBINSTALL "/:")
      + std::string(oldpath ? oldpath : "");
    ::setenv("PATH", newpath.c_str(), true);
  }

  std::string const & get_curry2read()
  {
    // Note: the curry2read and curry2poly programs are identical, except that
    // the latter introduces some hidden functions, such as .equals.Cons, which
    // are needed by the compiler.  The name of this function could be
    // misleading, but its job is to generate the (augmented) readable file, so
    // I keep it in spite of the discrepency.
    static std::string curry2read = join_path(
        SPRITE_LIBINSTALL "/", "cmc/translator/bin/curry2poly"
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
    std::string const & curry2read = sprite::get_curry2read();
    bool const readablefile_is_up_to_date =
         is_up_to_date(readablefile, curryfile)
      && is_up_to_date(readablefile, curry2read);
    if(!readablefile_is_up_to_date)
    {
      std::stringstream cmd;
      cmd << curry2read << " -q " << curryfile;
      int ok = std::system(cmd.str().c_str());
      if(ok != 0)
      {
        // Remove any files curry2read might have created.
        std::string const base = remove_extension(readablefile);
        remove_file(std::cerr, base + ".fcy");
        remove_file(std::cerr, base + ".fint");
        remove_file(std::cerr, base + ".icur");
        remove_file(std::cerr, base + ".icurry");
        remove_file(std::cerr, base + ".poly");
        remove_file(std::cerr, base + ".read");
        throw backend::compile_error(curry2read + " failed");
      }
    }
  }

  void make_assembly_file(
      std::string const & bitcodefile, std::string const & assemblyfile
    , bool remove_source
    )
  {
    std::stringstream cmd;
    std::string const & llc = sprite::get_llc();
    cmd << llc << " " << bitcodefile << " -o " << assemblyfile;
    int ok = std::system(cmd.str().c_str());

    cmd.str("");
    if(remove_source && !remove_file(cmd, bitcodefile) && ok != 0)
      throw backend::compile_error(cmd.str());
    if(ok != 0)
      throw backend::compile_error(llc + " failed");
  }

  /// Uses the platform-specific compiler to convert assembly to an executable.
  void make_executable_file(
      std::string const & assemblyfile, std::string const & executablefile
    , bool remove_source
    )
  {
    std::stringstream cmd;
    std::string const & cc = sprite::get_cc();
    cmd << cc << " " << assemblyfile << " -o " << executablefile;
    int ok = std::system(cmd.str().c_str());
    cmd.str("");
    if(remove_source && !remove_file(cmd, assemblyfile) && ok != 0)
      throw backend::compile_error(cmd.str());
    if(ok != 0)
      throw backend::compile_error(cc + " failed");
  }
}
