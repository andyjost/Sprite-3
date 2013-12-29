/**
 * @file
 * @brief Provides utilities.
 */

#pragma once
#include "llvm/IR/DerivedTypes.h"
#include "sprite/backend/config.hpp"
#include "sprite/backend/core/wrappers_fwd.hpp"
#include <type_traits>

namespace sprite { namespace backend { namespace aux
{
  /// Enables a template only when T can be converted to llvm::Type.
  template<typename T> using enable_if_type = 
      std::enable_if<std::is_convertible<T*, llvm::Type*>::value, void *>
    ;

  /**
   * @brief Enables a template only when T can be converted to llvm::Constant
   * but is not an llvm::GlobalValue.
   *
   * Does not permit llvm::GlobalValue, which is a Constant, but has a more
   * specific test.
   */
  template<typename T> using enable_if_constant_not_global =
      std::enable_if<
          std::is_convertible<T*, llvm::Constant*>::value
              && !std::is_convertible<T*, GlobalValue*>::value
        , void *
        >
    ;

  /**
   * @brief Enables a template only when T can be converted to
   * llvm::Instruction.
   */
  template<typename T> using enable_if_instruction =
      std::enable_if<
          std::is_convertible<T*, llvm::Instruction*>::value
        , void *
        >
    ;
  /// Enables a template only when T can be converted to llvm::GlobalValue.
  template<typename T> using enable_if_global_value = 
      std::enable_if<std::is_convertible<T*, GlobalValue*>::value, void *>
    ;

  /// Enables a template only when T can be converted to string_ref.
  template<typename T> using enable_if_convertible_to_string =
      std::enable_if<
          std::is_convertible<T, string_ref>::value
        , void *
        >
    ;

  /// Disables a template only when T can be converted to string_ref.
  template<typename T> using DisableIfConvertibleToString =
      std::enable_if<
          !std::is_convertible<T, string_ref>::value
        , void *
        >
    ;

  /// Enables a template only when T can be converted to uint64_t;
  template<typename T> using enable_if_convertible_to_int64 =
      std::enable_if<std::is_convertible<T, uint64_t>::value, void *>
    ;

  /// Disables a template only when T can be converted to uint64_t;
  template<typename T> using disable_if_convertible_to_int64 =
      std::enable_if<!std::is_convertible<T, uint64_t>::value, void *>
    ;

  /// Enables a template only when T can be converted to an array of Constant *.
  template<typename T> using enable_if_convertible_to_constant_array =
      std::enable_if<
          std::is_convertible<T, llvm::ArrayRef<llvm::Constant*>>::value
        , void *
        >
    ;

  /// Enables a template only when T can be converted to an array of Value *.
  template<typename T> using enable_if_convertible_to_value_array =
      std::enable_if<
          std::is_convertible<T, llvm::ArrayRef<Function*>>::value
        , void *
        >
    ;

  // Extract an LLVM api pointer from any suitable object.
  template<typename T> T * ptr(object<T> const & tp) { return tp.ptr(); }
  template<typename T>
    auto ptr(aux::arg_with_flags<T> const & x) -> decltype(ptr(x.arg()))
    { return ptr(x.arg()); }
  template<typename T> T * ptr(T * t) { return t; }
  // This version allows is_constarg to work for arguments it will reject.  It
  // must never be called, though.
  void * ptr(...);

  /// True if a value of type T can be accepted as an LLVM constant value.
  template<typename T>
  constexpr bool is_constarg()
  {
    return std::is_constructible<
        Constant*, decltype(ptr(*static_cast<T*>(0)))
      >::value;
  }

  /// True if a value of type T can be accepted as an LLVM type value.
  template<typename T>
  constexpr bool is_typearg()
  {
    return std::is_constructible<
        Type*, decltype(ptr(*static_cast<T*>(0)))
      >::value;
  }
}}}

namespace sprite { namespace backend
{
  using aux::ptr;
}}
