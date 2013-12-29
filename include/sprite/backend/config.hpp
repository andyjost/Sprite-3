/**
 * @file
 * @brief Contains common bootstrapping code for the Sprite LLVM module.
 */

#pragma once
#include <array>
#include "llvm/ADT/ArrayRef.h"
#include "sprite/backend/support/typenames.hpp"

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
  class ConstantExpr;
  class ConstantFP;
  class ConstantInt;
  class ConstantPointerNull;
  class ConstantStruct;

  // Values.
  class Value;
  class Function;
  class GlobalValue;
  class GlobalVariable;
  class Instruction;

  // ADTs.
  class StringRef;
  class Twine;
}

namespace sprite { namespace backend
{
  // Types.
  using llvm::Type;
  using llvm::ArrayType;
  using llvm::FunctionType;
  using llvm::IntegerType;
  using llvm::PointerType;
  using llvm::StructType;

  SPRITE_DECLARE_TYPENAME(Type)
  SPRITE_DECLARE_TYPENAME(ArrayType)
  SPRITE_DECLARE_TYPENAME(FunctionType)
  SPRITE_DECLARE_TYPENAME(IntegerType)
  SPRITE_DECLARE_TYPENAME(PointerType)
  SPRITE_DECLARE_TYPENAME(StructType)

  // Values.
  using llvm::Value;
  using llvm::Function;
  using llvm::GlobalValue;
  using llvm::GlobalVariable;
  using llvm::Instruction;

  SPRITE_DECLARE_TYPENAME(Value)
  SPRITE_DECLARE_TYPENAME(Function)
  SPRITE_DECLARE_TYPENAME(GlobalValue)
  SPRITE_DECLARE_TYPENAME(GlobalVariable)
  SPRITE_DECLARE_TYPENAME(Instruction)

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
  struct FPType : llvm::Type
  {
    static bool classof(Type const * tp)
      { return tp->isFloatingPointTy(); }
  };

  SPRITE_DECLARE_TYPENAME(FPType)

  // Constants.
  using llvm::Constant;
  using llvm::ConstantAggregateZero;
  using llvm::ConstantArray;
  using llvm::ConstantExpr;
  using llvm::ConstantFP;
  using llvm::ConstantInt;
  using llvm::ConstantPointerNull;
  using llvm::ConstantStruct;

  SPRITE_DECLARE_TYPENAME(ConstantAggregateZero);
  SPRITE_DECLARE_TYPENAME(ConstantArray);
  SPRITE_DECLARE_TYPENAME(ConstantExpr);
  SPRITE_DECLARE_TYPENAME(ConstantFP);
  SPRITE_DECLARE_TYPENAME(ConstantInt);
  SPRITE_DECLARE_TYPENAME(ConstantPointerNull);
  SPRITE_DECLARE_TYPENAME(ConstantStruct);


  // ADTs.
  using string_ref = llvm::StringRef;
  using twine = llvm::Twine;

  SPRITE_DECLARE_TYPENAME(string_ref);
  SPRITE_DECLARE_TYPENAME(twine);

  // Arbitrary-precision intrinsic types.
  using llvm::APInt;
  using llvm::APFloat;

  SPRITE_DECLARE_TYPENAME(APInt);
  SPRITE_DECLARE_TYPENAME(APFloat);

  /**
   * @brief Extends llvm::ArrayRef<T>.
   *
   * The LLVM @p ArrayRef cannot be constructed from a @p
   * std::initializer_list.  This class behaves just like that class but adds
   * an additional constructor for better initialization.
   */
  template<typename T>
  struct array_ref : llvm::ArrayRef<T>
  {
    using llvm::ArrayRef<T>::ArrayRef;

    /// Initializes using a brace-enclosed initializer list.
    array_ref(std::initializer_list<T> const & args)
      : llvm::ArrayRef<T>(args.begin(), args.size())
    {}

    array_ref(std::initializer_list<T> && args)
      : llvm::ArrayRef<T>(args.begin(), args.size())
    {}

    /// Initializes from std::array.
    template<size_t N>
    array_ref(std::array<T,N> const & args)
      : llvm::ArrayRef<T>(args.begin(), N)
    {}

    template<size_t N>
    array_ref(std::array<T,N> && args)
      : llvm::ArrayRef<T>(args.begin(), N)
    {}
  };
}}
