/**
 * @file
 * @brief Contains exception code.
 */

#pragma once
#include <string>
#include <exception>
#include "llvm/ADT/Twine.h"

namespace sprite { namespace backend
{
  /// Base class for all error types defined in this module.
  struct error : std::exception
  {
    /// Creates a error instance with optional message.
    error(std::string const & type, std::string const & msg = "")
      : _msg()
    {
      if(msg.empty())
        _msg = type;
      else
        _msg = type + ": " + msg;
      
      // if(_msg.back() != '.') _msg.push_back('.');
    }

    /// Inherited what method.
    virtual char const * what() const throw() override
      { return _msg.c_str(); }

  private:

    /// Holds the error message.
    std::string _msg;
  };

  /// Declares a new exception type.
  #define SPRITE_DECLARE_ERROR_TYPE(name)                              \
      struct name : error                                              \
        { name(twine const & msg = "") : error(#name, msg.str()) {} }; \
    /**/

  /// Indicates an incorrect object type was used.
  SPRITE_DECLARE_ERROR_TYPE(type_error)

  /// Indicates an incorrect value was encountered.
  SPRITE_DECLARE_ERROR_TYPE(value_error)

  /// Indicates an incorrect parameter was supplied.
  SPRITE_DECLARE_ERROR_TYPE(parameter_error)

  /// Indicates an error at compile time.
  SPRITE_DECLARE_ERROR_TYPE(compile_error)

  /// Indicates an error using class scope.
  SPRITE_DECLARE_ERROR_TYPE(scope_error)

  /// Indicates a naming error (such as a conflict with a previous definition).
  SPRITE_DECLARE_ERROR_TYPE(name_error)
}}
