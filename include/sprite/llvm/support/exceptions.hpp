/**
 * @file
 * @brief Contains exception code.
 */

#pragma once
#include <string>
#include <exception>
#include "llvm/ADT/Twine.h"

namespace sprite { namespace llvm
{
  /// Base class for all error types defined in this module.
  struct Error : std::exception
  {
    /// Creates a Error instance with optional message.
    Error(std::string const & type, std::string const & msg = "")
      : _msg()
    {
      if(msg.empty())
        _msg = type;
      else
        _msg = type + ": " + msg;
      
      if(_msg.back() != '.') _msg.push_back('.');
    }

    /// Inherited what method.
    virtual char const * what() const throw() override
      { return _msg.c_str(); }

  private:

    /// Holds the error message.
    std::string _msg;
  };

  /// Declares a new exception type.
  #define SPRITE_DECLARE_ERROR_TYPE(name)                                      \
      struct name : Error                                                      \
        { name(::llvm::Twine const & msg = "") : Error(#name, msg.str()) {} }; \
    /**/

  /// Indicates an incorrect object type was used.
  SPRITE_DECLARE_ERROR_TYPE(TypeError)

  /// Indicates an incorrect value was encountered.
  SPRITE_DECLARE_ERROR_TYPE(ValueError)

  /// Indicates an incorrect parameter was supplied.
  SPRITE_DECLARE_ERROR_TYPE(ParameterError)

  /// Indicates an error at runtime.
  SPRITE_DECLARE_ERROR_TYPE(RuntimeError)
}}
