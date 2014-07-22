/**
 * @file
 * @brief Contains code used when running tests.
 */

#pragma once
#include "sprite/backend.hpp"

void sprite_dumpvalue(llvm::Value *);
void sprite_dumptype(llvm::Value *);
void sprite_dumptype(llvm::Type *);

namespace sprite { size_t init_debug(); }

namespace sprite { namespace backend
{
  /// Indicates a test failed.
  SPRITE_DECLARE_ERROR_TYPE(testing_error)
}}

namespace sprite { namespace backend { namespace testing
{
  // Represents a header file for declaring certain C library functions.
  struct clib_h
  {
    type char_ = types::char_();
    type FILE_p = *types::struct_("FILE");
    type int_ = get_type<int>();
    type size_t_= get_type<size_t>();
    type void_ = types::void_();
  
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
    // void *malloc(size_t size);
    function const malloc = extern_((*char_)(size_t_), "malloc");
    // void free(void *ptr);
    function const free = extern_(void_(*char_), "free");
  };
  
  /**
   * @brief Builds and tests a module.
   *
   * Builds the specified function, then compiles and runs it.  For the test to
   * pass, the function must produce the expected output and return 0.
   *
   * Parameters:
   *     body:
   *         Specifies the function body.  Some utility functions are provided
   *         through the argument.  The function accepts an argument of type
   *         clib.FILE_p and returns an int.
   *     expected_output:
   *         The expected output of the function.  The function should write
   *         the output to the filehandle it receives.
   *     print_module:
   *         If true, then the module will be printed during compilation.
   *
   * Exceptions:
   *     Throws testing_error if the test fails.
   *
   * Returns:
   *     Nothing.
   */
  void test_function(
      std::function<void(clib_h const &)> const & body
    , std::string const & expected_output
    , bool print_module = false
    , bool view_cfg = false
    );
}}}
