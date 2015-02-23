/**
 * @file
 * @brief Contains common bootstrapping code for the Sprite LLVM module.
 */

#pragma once
#include <array>
#include "sprite/backend/support/typenames.hpp"

/**
 * @brief Calls to the LLVM C++-API are wrapped in this macro.
 *
 * If the program is compiled with @p -MSPRITE_SHOW_LLVM_API_CALLS, then the
 * calls are printed to the output.
 */
// #define SPRITE_SHOW_LLVM_API_CALLS
#ifdef SPRITE_SHOW_LLVM_API_CALLS
#include <iostream>
#include <string>
namespace sprite { namespace backend { namespace aux {
  inline std::string _location(std::string const & file, size_t line)
    { return " at " + file + ":" + std::to_string(line) + "\n"; }
}}}
#define SPRITE_APICALL(...)                                        \
    (                                                              \
        (std::cout << "LLVM-API CALL: " << #__VA_ARGS__            \
          << ::sprite::backend::aux::_location(__FILE__, __LINE__) \
          )                                                        \
      , __VA_ARGS__                                                \
      )                                                            \
  /**/
#else
#define SPRITE_APICALL(...) __VA_ARGS__
#endif

/// Defines the metadata used to tag implicit continuation targets added by sprite.
#define SPRITE_IMPLIED_METADATA "sprite.implied"

namespace sprite { namespace backend
{
  /// Metadata used to indicate how an implicit branch is used.
  enum MdBranchType {
      MD_CONT /// The associated implied branch instruction is a continuation.
    , MD_LOOP /// The associated implied branch instruction is a loopback.
  };
}}

/// Defines the metadata used to tag loop back edges.
#define SPRITE_LOOP_METADATA "sprite.loop"

// Forward-declare some LLVM types that will be used.
namespace llvm
{
  class Module;

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
  class BlockAddress;

  // Values.
  class Value;
  class BasicBlock;
  class Function;
  class GlobalValue;
  class GlobalVariable;
  class MDNode;

  // Instructions.
  class Instruction;
  class SwitchInst;

  // ADTs.
  class StringRef;
  class Twine;
}

namespace sprite { namespace backend
{
  // A unique type for use with enable_if.
  struct En_ {};

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

  // Values.
  using llvm::Value;
  using llvm::BasicBlock;
  using llvm::Function;
  using llvm::GlobalValue;
  using llvm::GlobalVariable;
  using llvm::Instruction;
  using llvm::SwitchInst;
  using llvm::MDNode;

  SPRITE_DECLARE_TYPENAME(Value)
  SPRITE_DECLARE_TYPENAME(BasicBlock)
  SPRITE_DECLARE_TYPENAME(Function)
  SPRITE_DECLARE_TYPENAME(GlobalValue)
  SPRITE_DECLARE_TYPENAME(GlobalVariable)
  SPRITE_DECLARE_TYPENAME(Instruction)
  SPRITE_DECLARE_TYPENAME(SwitchInst)
  SPRITE_DECLARE_TYPENAME(MDNode)

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
}}
