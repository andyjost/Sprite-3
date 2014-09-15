/**
 * @file Provides common facilities for the Sprite command-line tools.
 */
#include <string>
#include <utility>

namespace sprite { namespace compiler
{
  struct LibrarySTab;
}}

namespace sprite { namespace curry
{
  struct Library;
}}

namespace sprite
{
  /**
   * @brief Executes curry2read to generate the .read file.
   */
  void make_readable_file(std::string const & curryfile);

  /// Uses llc to compile bitcode to assembly.
  void make_assembly_file(
      std::string const & bitcodefile, std::string const & assemblyfile
    , bool remove_source
    );

  /// Uses the platform-specific compiler to convert assembly to an executable.
  void make_executable_file(
      std::string const & assemblyfile, std::string const & executablefile
    , bool remove_source
    );

  /**
   * @brief Compiles a Curry file to Sprite IR.
   *
   * The library and library symbol table are updated with the new definitions.
   * If the @p curryfile has no .curry extension, it will be added.  If a
   * corresponding bitcode (.bc) file already exists and is not newer than the
   * source code file, it will be simply be read.
   */
  void compile_file(
      std::string const & curryfile
    , curry::Library &
    , compiler::LibrarySTab &
    , llvm::LLVMContext & context
    , bool save_bitcode
    );

  /**
   * @brief Loads a compiled Curry file.
   *
   * If the file does not exist, then this function looks for it in the
   * directory specified by SPRITE_LIBINSTALL.  On error, a NULL pointer is
   * returned and the error string is set.
   */
  llvm::Module * load_compiled_module(
      std::string const & filename, llvm::LLVMContext & context
    , std::string & errmsg
    );

  /**
   * @brief Inserts the "main" symbol into a module.
   *
   * @p start is the name of the start symbol.  The "main" symbol is added to
   * the module specified by @p start.
   */
  void insert_main_function(
      compiler::LibrarySTab const & stab, curry::Qname const & start
    );

  inline std::string dirname(std::string const & path)
  {
    size_t const pos = path.find_last_of("/");
    return path.substr(0, pos == std::string::npos ? 0 : pos);
  }

  inline std::string basename(std::string const & path)
  {
    size_t const pos = path.find_last_of("/");
    return path.substr(pos == std::string::npos ? 0 : pos + 1);
  }

  inline std::string remove_extension(std::string const & path)
  {
    size_t const pos = path.find_last_of(".");
    return pos == std::string::npos ? path : path.substr(0, pos);
  }

  inline std::string get_extension(std::string const & path)
  {
    size_t const pos = path.find_last_of(".");
    return pos == std::string::npos ? std::string("") : path.substr(pos);
  }

  inline std::string join_path(
      std::string const & dirname, std::string const & path
    )
  {
    if(!path.empty() && path.front() == '/')
      return path;
    if(dirname.empty())
      return path;
    return dirname.back() == '/' ? dirname + path : dirname + "/" + path;
  }

  /**
   * @brief True only if both files exist and a @p relative_to is not newer
   * than @p file.
   */
  bool is_up_to_date(
      std::string const & file, std::string const & relative_to
    );

  /// Gets the Curry module name from an input file name.
  inline std::string get_modulename(std::string const & curryfile)
    { return remove_extension(basename(curryfile)); }

  /// Gets the corresponding Curry file name from an input file name.
  std::string get_curryfile(std::string const & inputfile);

  /// Gets the corresponding readable file name from an input file name.
  inline std::string get_readablefile(std::string const & curryfile)
  {
    std::string const basedir = dirname(curryfile);
    std::string const modulename = get_modulename(curryfile);
    return join_path(basedir, ".curry/" + modulename + ".read");
  }

  /// Gets the corresponding bitcode file name from an input file name.
  inline std::string get_bitcodefile(std::string const & curryfile)
  {
    std::string const basedir = dirname(curryfile);
    std::string const modulename = get_modulename(curryfile);
    return join_path(basedir, ".curry/" + modulename + ".bc");
  }

  /// Gets the path to the curry2read program.
  std::string const & get_curry2read();

  /// Gets the path to the llc program.
  std::string const & get_llc();

  /// Gets the path to a C compiler.
  std::string const & get_cc();
}
