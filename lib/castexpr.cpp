#include "sprite/backend/core/castexpr.hpp"
#include "sprite/backend/core/constant.hpp"
#include "sprite/backend/core/detail/current_builder.hpp"
#include "sprite/backend/core/detail/flag_checks.hpp"
#include "sprite/backend/core/get_constant.hpp"
#include "sprite/backend/core/type.hpp"
#include "sprite/backend/support/exceptions.hpp"
#include "sprite/backend/support/miscellaneous.hpp"
#include "llvm/IR/InstrTypes.h"

namespace
{
  using namespace sprite::backend;

  //@{
  /// Apply a cast operation to a constant or value.
  template<llvm::Instruction::CastOps Op>
  value apply_cast(Value * src, Type * ty)
  {
    return value(SPRITE_APICALL(current_builder().CreateCast(Op, src, ty)));
  }
  template<llvm::Instruction::CastOps> constant apply_cast(Constant *, Type *);
  //@}

  // Workaround for inconsistent LLVM names.
  struct FixupConstantExpr : llvm::ConstantExpr
  {
    // Instruction.def says FPExt, but ConstantExpr says FPExtend.
    static Constant *getFPExt(Constant *C, Type *Ty)
      { return llvm::ConstantExpr::getFPExtend(C, Ty); }
  };

  // Specialize the constant version of apply_cast each cast operation.
  #define HANDLE_CAST_INST(N, OPC, CLASS)                                               \
      template<> constant apply_cast<llvm::Instruction::OPC>(Constant * src, Type * ty) \
        { return constant(SPRITE_APICALL(FixupConstantExpr::get##OPC(src, ty))); }      \
    /**/
  #include "llvm/IR/Instruction.def"
  #undef HANDLE_CAST_INST

  /// Implements the typecase function for either constants or values.
  template<typename ConstantOrValue>
  ConstantOrValue typecast_impl(ConstantOrValue const & src, aux::arg_with_flags<type> const & tgt)
  {
    using LlvmConstantOrValue = typename ConstantOrValue::element_type;
    Type * const tgt_type = ptr(tgt);
    assert(tgt_type);
    LlvmConstantOrValue * const val = ptr(src);
    assert(val);
    Type * const src_type = val->getType();
    assert(src_type);

    // Quick exit.
    if(src_type == tgt_type) return src;

    if(src_type->isIntegerTy())
    {
      if(tgt_type->isIntegerTy())
      {
        unsigned const src_sz = src_type->getScalarSizeInBits();
        unsigned const tgt_sz = tgt_type->getScalarSizeInBits();
        if(src_sz < tgt_sz)
        {
          SPRITE_ALLOW_FLAGS(
              tgt.flags(), "integer extension"
            , operator_flags::SIGNED | operator_flags::UNSIGNED
            )
          check_for_exactly_one_signed_flag(tgt.flags(), "integer extension");

          if(tgt.flags().signed_())
            return apply_cast<llvm::Instruction::SExt>(val, tgt_type);
          else
            return apply_cast<llvm::Instruction::ZExt>(val, tgt_type);
        }
        else if(tgt_sz < src_sz)
        {
          // These flags are ignored, but may be set.
          SPRITE_ALLOW_FLAGS(tgt.flags(), "integer truncation"
            , operator_flags::SIGNED | operator_flags::UNSIGNED
            );
          return apply_cast<llvm::Instruction::Trunc>(val, tgt_type);
        }
        return src; // no-op
      }
      else if(tgt_type->isFloatingPointTy())
      {
        SPRITE_ALLOW_FLAGS(
            tgt.flags(), "integer-to-floating-point conversion"
          , operator_flags::SIGNED | operator_flags::UNSIGNED
          )
        check_for_exactly_one_signed_flag(
            tgt.flags(), "integer-to-floating-point conversion"
          );

        if(tgt.flags().signed_())
          return apply_cast<llvm::Instruction::SIToFP>(val, tgt_type);
        else
          return apply_cast<llvm::Instruction::UIToFP>(val, tgt_type);
      }
      else if(tgt_type->isPointerTy())
      {
        SPRITE_ALLOW_FLAGS(tgt.flags(), "integer-to-pointer conversion", 0)
        return apply_cast<llvm::Instruction::IntToPtr>(val, tgt_type);
      }
      throw type_error(
          "Expected an integer, floating-point or pointer type as the target "
          "of a typecast from an integer type"
        );
    }
    else if(src_type->isFloatingPointTy())
    {
      if(tgt_type->isIntegerTy())
      {
        SPRITE_ALLOW_FLAGS(
            tgt.flags(), "floating-point-to-integer conversion"
          , operator_flags::SIGNED | operator_flags::UNSIGNED
          )
        check_for_exactly_one_signed_flag(
            tgt.flags(), "floating-point-to-integer conversion"
          );

        if(tgt.flags().signed_())
          return apply_cast<llvm::Instruction::FPToSI>(val, tgt_type);
        else
          return apply_cast<llvm::Instruction::FPToUI>(val, tgt_type);
      }
      else if(tgt_type->isFloatingPointTy())
      {
        unsigned const src_sz = getFPBitWidth(src_type);
        unsigned const tgt_sz = getFPBitWidth(tgt_type);
        if(src_sz < tgt_sz)
        {
          SPRITE_ALLOW_FLAGS(tgt.flags(), "floating-point extension", operator_flags::SIGNED)
          return apply_cast<llvm::Instruction::FPExt>(val, tgt_type);
        }
        else if(tgt_sz < src_sz)
        {
          SPRITE_ALLOW_FLAGS(tgt.flags(), "floating-point truncation", operator_flags::SIGNED)
          return apply_cast<llvm::Instruction::FPTrunc>(val, tgt_type);
        }
        return src; // no-op
      }
      throw type_error(
          "Expected an integer or floating-point type as the target of a "
          "typecast from a floating-point type"
        );
    }
    else if(src_type->isPointerTy())
    {
      src_type->dump();
      tgt_type->dump();
      if(tgt_type->isIntegerTy())
      {
        SPRITE_ALLOW_FLAGS(tgt.flags(), "pointer-to-integer conversion", 0)
        return apply_cast<llvm::Instruction::PtrToInt>(val, tgt_type);
      }
      throw type_error(
          "Expected an integer type as the target of a typecast from a pointer type"
        );
    }
    throw type_error(
        "Expected an integer, floating-point, or pointer type as the source "
        "of a typecast"
      );
  }
}

namespace sprite { namespace backend
{
  constant typecast(constant const & src, aux::arg_with_flags<type> const & tgt)
    { return typecast_impl<constant>(src, tgt); }

  value typecast(value const & src, aux::arg_with_flags<type> const & tgt)
    { return typecast_impl<value>(src, tgt); }

  constant bitcast(constant const & src, type const & tgt)
    { return apply_cast<llvm::Instruction::BitCast>(src.ptr(), tgt.ptr()); }

  value bitcast(value const & src, type tgt)
    { return apply_cast<llvm::Instruction::BitCast>(src.ptr(), tgt.ptr()); }

  type coerce(type const & lhs, type const & rhs)
  {
    if(lhs->isIntegerTy())
    {
      if(rhs->isIntegerTy())
        return lhs->getScalarSizeInBits() < rhs->getScalarSizeInBits()
          ? rhs : lhs;
      else if(rhs->isFloatingPointTy())
        return rhs;
    }
    else if(lhs->isFloatingPointTy())
    {
      if(rhs->isIntegerTy())
        return lhs;
      else if(rhs->isFloatingPointTy())
        return getFPBitWidth(lhs.ptr()) < getFPBitWidth(rhs.ptr()) ? rhs : lhs;
    }
    throw type_error("Expected an integer or floating-point type in coerce");
  }
}}

