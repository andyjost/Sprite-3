/**
 * @file
 * @brief Contains common bootstrapping code for the Sprite LLVM module.
 */

#pragma once
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Function.h"
#include "sprite/llvm/support/typenames.hpp"

// Forward-declare some LLVM types that will be used.
namespace llvm
{
  // Derived types.
  class Type;
  class ArrayType;
  class FunctionType;
  class IntegerType;
  class PointerType;
  class StructType;

  // Constants.
  class Constant;
  class ConstantAggregateZero;
  class ConstantArray;
  class ConstantFP;
  class ConstantInt;
  class ConstantPointerNull;
  class ConstantStruct;

  // Values.
  class Value;
  class Function;
  class GlobalValue;
  class Instruction;
}

namespace sprite { namespace llvm
{
  // The actual LLVM API is prefixed as llvm_.
  namespace llvm_ = ::llvm;

  // Types.
  using llvm_::Type;
  using llvm_::ArrayType;
  using llvm_::FunctionType;
  using llvm_::IntegerType;
  using llvm_::PointerType;
  using llvm_::StructType;

  SPRITE_DECLARE_TYPENAME(ArrayType)
  SPRITE_DECLARE_TYPENAME(FunctionType)
  SPRITE_DECLARE_TYPENAME(IntegerType)
  SPRITE_DECLARE_TYPENAME(PointerType)
  SPRITE_DECLARE_TYPENAME(StructType)

  // Values.
  using llvm_::Value;
  using llvm_::Function;
  using llvm_::GlobalValue;
  using llvm_::Instruction;

  /// Returns a human-readable version of an LLVM type name.
  inline std::string typename_(Type & tp)
  {
    std::string buf;
    llvm_::raw_string_ostream sbuf(buf);
    sbuf << tp;
    return sbuf.str();
  }

  /// Returns a human-readable version of an LLVM type name.
  inline std::string typename_(Type * tp)
    { return typename_(*tp); }

  /**
   * @brief Represents a floating-point type.
   *
   * LLVM does not define a type class for floating-point types as it does for
   * other types such as @p IntegerType.  Instead, floating-point types are
   * handled using the generic class @p Type.  That poses a real problem for @p
   * typeobj, which uses the type of the wrapped object to activate the
   * relevant operators.
   *
   * To work around that problem, this type is invented.  When a floating-point
   * type is produced, this library will cast its <tt>Type *</tt> to a
   * <tt>FPType *</tt> before applying the wrapping.  In that way, it is
   * possible to carry the type information.  When the type is ultimately used
   * (by an LLVM API function), it will be converted back to <tt>Type *</tt>,
   * since, obviously, no function defined by LLVM accepts this type.
   */
  struct FPType : llvm_::Type
  {
    static bool classof(Type const * tp)
      { return tp->isFloatingPointTy(); }
  };

  SPRITE_DECLARE_TYPENAME(FPType)

  // Constants.
  using llvm_::Constant;
  using llvm_::ConstantAggregateZero;
  using llvm_::ConstantArray;
  using llvm_::ConstantFP;
  using llvm_::ConstantInt;
  using llvm_::ConstantPointerNull;
  using llvm_::ConstantStruct;

  SPRITE_DECLARE_TYPENAME(ConstantAggregateZero);
  SPRITE_DECLARE_TYPENAME(ConstantArray);
  SPRITE_DECLARE_TYPENAME(ConstantFP);
  SPRITE_DECLARE_TYPENAME(ConstantInt);
  SPRITE_DECLARE_TYPENAME(ConstantPointerNull);
  SPRITE_DECLARE_TYPENAME(ConstantStruct);

  // using llvm_::APInt;
  // using llvm_::APFloat;

  // ADTs.
  using llvm_::StringRef;

  /**
   * @brief Extends llvm::ArrayRef<T>.
   *
   * The LLVM @p ArrayRef cannot be constructed from a @p
   * std::initializer_list.  This class behaves just like that class but adds
   * an additional constructor for better initialization.
   */
  template<typename T>
  struct ArrayRef : llvm_::ArrayRef<T>
  {
    using llvm_::ArrayRef<T>::ArrayRef;

    /// Initializes using a brace-enclosed initializer list.
    ArrayRef(std::initializer_list<T> const & args)
      : llvm_::ArrayRef<T>(args.begin(), args.size())
    {}

    ArrayRef(std::initializer_list<T> && args)
      : llvm_::ArrayRef<T>(args.begin(), args.size())
    {}
  };
}}
