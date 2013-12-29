/**
 * @file
 * @brief Utilities for pretty-printing type names.
 */

#pragma once
#include <cstdlib>
#include <cxxabi.h>
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Type.h"
#include <memory>
#include <string>
#include <typeinfo>

namespace sprite { namespace llvm
{
  namespace aux
  {
    /// Demangles a symbol name (e.g., one returned by typeinfo).
    inline std::string demangle(std::string const & mangled)
    {
      int status;
      std::unique_ptr<char[], void (*)(void*)> result(
          abi::__cxa_demangle(mangled.c_str(), 0, 0, &status), std::free);
      return result.get() ? std::string(result.get()) : mangled;
    }
  }

  /// Returns a human-readable version of a type name.
  template<typename T>
  inline std::string typename_(T const & arg)
    { return aux::demangle(typeid(arg).name()); }

  /// Returns a human-readable name for certain types.
  template<typename T> struct typename_impl
    { static std::string name() { return aux::demangle(typeid(T).name()); } };

  /// Declares a specialization of @p typename_impl.
  #define SPRITE_DECLARE_TYPENAME(name_)                  \
      template<> struct typename_impl<name_>                   \
        { static std::string name() { return #name_; } }; \
    /**/

  /// Returns the name of an LLVM type object.
  inline std::string llvm_typename(::llvm::Type * type)
  {
    assert(type);
    std::string str;
    ::llvm::raw_string_ostream ss(str);
    type->print(ss);
    return str;
  }
}}

