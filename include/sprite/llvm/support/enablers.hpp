/**
 * @file
 * @brief Provides utilities.
 */

#pragma once
#include "llvm/IR/DerivedTypes.h"
#include "sprite/llvm/config.hpp"
#include "sprite/llvm/core/wrappers_fwd.hpp"
#include <type_traits>

namespace sprite { namespace llvm { namespace aux
{
  /// Enables a template only when T can be converted to llvm::Type.
  template<typename T> using EnableIfLlvmType = 
      std::enable_if<std::is_convertible<T*, llvm_::Type*>::value, void *>
    ;

  /**
   * @brief Enables a template only when T can be converted to llvm::Constant
   * but is not an llvm::GlobalValue.
   *
   * Does not permit llvm::GlobalValue, which is a Constant, but has a more
   * specific test.
   */
  template<typename T> using EnableIfLlvmConstantNotGlobal =
      std::enable_if<
          std::is_convertible<T*, llvm_::Constant*>::value
              && !std::is_convertible<T*, llvm_::GlobalValue*>::value
        , void *
        >
    ;

  /**
   * @brief Enables a template only when T can be converted to
   * llvm::Instruction.
   */
  template<typename T> using EnableIfLlvmInstruction =
      std::enable_if<
          std::is_convertible<T*, llvm_::Instruction*>::value
        , void *
        >
    ;
  /// Enables a template only when T can be converted to llvm::GlobalValue.
  template<typename T> using EnableIfLlvmGlobalValue = 
      std::enable_if<std::is_convertible<T*, llvm_::GlobalValue*>::value, void *>
    ;

  /// Enables a template only when T can be converted to llvm::StringRef.
  template<typename T> using EnableIfConvertibleToString =
      std::enable_if<
          std::is_convertible<T, llvm_::StringRef>::value
        , void *
        >
    ;

  /// Disables a template only when T can be converted to llvm::StringRef.
  template<typename T> using DisableIfConvertibleToString =
      std::enable_if<
          !std::is_convertible<T, llvm_::StringRef>::value
        , void *
        >
    ;

  /// Enables a template only when T can be converted to uint64_t;
  template<typename T> using EnableIfConvertibleToInt64 =
      std::enable_if<std::is_convertible<T, uint64_t>::value, void *>
    ;

  /// Disables a template only when T can be converted to uint64_t;
  template<typename T> using DisableIfConvertibleToInt64 =
      std::enable_if<!std::is_convertible<T, uint64_t>::value, void *>
    ;

  /// Enables a template only when T can be converted to an array of Constant *.
  template<typename T> using EnableIfConvertibleToConstantArray =
      std::enable_if<
          std::is_convertible<T, llvm_::ArrayRef<llvm_::Constant*>>::value
        , void *
        >
    ;

  /// Enables a template only when T can be converted to an array of Value *.
  template<typename T> using EnableIfConvertibleToValueArray =
      std::enable_if<
          std::is_convertible<T, llvm_::ArrayRef<llvm_::Value*>>::value
        , void *
        >
    ;

  // Extract an LLVM api pointer from any suitable object.
  template<typename T> T * ptr(Wrapper<T> const & tp) { return tp.ptr(); }
  template<typename T>
    auto ptr(aux::ArgWithFlags<T> const & x) -> decltype(ptr(x.arg()))
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

namespace sprite { namespace llvm
{
  using aux::ptr;
}}
