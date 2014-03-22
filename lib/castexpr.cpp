#include "sprite/backend/core/castexpr.hpp"
#include "sprite/backend/core/constant.hpp"
#include "sprite/backend/core/get_constant.hpp"
#include "sprite/backend/core/type.hpp"
#include "sprite/backend/support/exceptions.hpp"
#include "sprite/backend/support/miscellaneous.hpp"

namespace sprite { namespace backend
{
  constant typecast(constant const & src, aux::arg_with_flags<type> const & tgt)
  {
    Type * const tgt_type = ptr(tgt);
    assert(tgt_type);
    Constant * const val = ptr(src);
    assert(val);
    Type * const src_type = val->getType();
    assert(src_type);

    // Quick exit.
    if(src_type == tgt_type) return src;

    if(src_type->isIntegerTy())
    {
      if(tgt_type->isIntegerTy())
      {
        unsigned const src_sz = src_type->getIntegerBitWidth();
        unsigned const tgt_sz = tgt_type->getIntegerBitWidth();
        if(src_sz < tgt_sz)
        {
          SPRITE_ALLOW_FLAGS(
              tgt, "integer extension"
            , operator_flags::SIGNED | operator_flags::UNSIGNED
            )
          check_for_exactly_one_signed_flag(tgt.flags(), "integer extension");

          if(tgt.flags().signed_())
            return constant(SPRITE_APICALL(
                ConstantExpr::getSExt(val, tgt_type)
              ));
          else
            return constant(SPRITE_APICALL(
                ConstantExpr::getZExt(val, tgt_type)
              ));
        }
        else if(tgt_sz < src_sz)
        {
          // These flags are ignored, but may be set.
          SPRITE_ALLOW_FLAGS(tgt, "integer truncation"
            , operator_flags::SIGNED | operator_flags::UNSIGNED
            );
          return constant(SPRITE_APICALL(ConstantExpr::getTrunc(val, tgt_type)));
        }
        return src; // no-op
      }
      else if(tgt_type->isFloatingPointTy())
      {
        SPRITE_ALLOW_FLAGS(
            tgt, "integer-to-floating-point conversion"
          , operator_flags::SIGNED | operator_flags::UNSIGNED
          )
        check_for_exactly_one_signed_flag(
            tgt.flags(), "integer-to-floating-point conversion"
          );

        if(tgt.flags().signed_())
          return constant(SPRITE_APICALL(
              ConstantExpr::getSIToFP(val, tgt_type)
            ));
        else
          return constant(SPRITE_APICALL(
              ConstantExpr::getUIToFP(val, tgt_type)
            ));
      }
      else if(tgt_type->isPointerTy())
      {
        SPRITE_ALLOW_FLAGS(tgt, "integer-to-pointer conversion", 0)
        return constant(SPRITE_APICALL( 
            ConstantExpr::getIntToPtr(val, tgt_type)
          ));
      }
      throw type_error(
          "Expected integer, floating-point or pointer type as the target of "
          "typecast from an integer type"
        );
    }
    else if(src_type->isFloatingPointTy())
    {
      if(tgt_type->isIntegerTy())
      {
        SPRITE_ALLOW_FLAGS(
            tgt, "floating-point-to-integer conversion"
          , operator_flags::SIGNED | operator_flags::UNSIGNED
          )
        check_for_exactly_one_signed_flag(
            tgt.flags(), "floating-point-to-integer conversion"
          );

        if(tgt.flags().signed_())
          return constant(SPRITE_APICALL(
              ConstantExpr::getFPToSI(val, tgt_type)
            ));
        else
          return constant(SPRITE_APICALL(
              ConstantExpr::getFPToUI(val, tgt_type)
            ));
      }
      else if(tgt_type->isFloatingPointTy())
      {
        unsigned const src_sz = getFPBitWidth(src_type);
        unsigned const tgt_sz = getFPBitWidth(tgt_type);
        if(src_sz < tgt_sz)
        {
          SPRITE_ALLOW_FLAGS(tgt, "floating-point extension", operator_flags::SIGNED)
          return constant(SPRITE_APICALL(
              ConstantExpr::getFPExtend(val, tgt_type)
            ));
        }
        else if(tgt_sz < src_sz)
        {
          SPRITE_ALLOW_FLAGS(tgt, "floating-point truncation", operator_flags::SIGNED)
          return constant(SPRITE_APICALL(
              ConstantExpr::getFPTrunc(val, tgt_type)
            ));
        }
        return src; // no-op
      }
      throw type_error(
          "Expected integer or floating-point type as the target of typecast "
          "from a floating-point type"
        );
    }
    else if(src_type->isPointerTy())
    {
      if(tgt_type->isIntegerTy())
      {
        SPRITE_ALLOW_FLAGS(tgt, "pointer-to-integer conversion", 0)
        return constant(SPRITE_APICALL(
            ConstantExpr::getPtrToInt(val, tgt_type)
          ));
      }
      throw type_error(
          "Expected integer type as the target of typecast from integer."
        );
    }
    throw type_error(
        "Expected integer, floating-point, or pointer type as the source "
        "of typecast."
      );
  }

  constant bitcast(constant const & src, type const & tgt)
  {
    return constant(SPRITE_APICALL(
        ConstantExpr::getBitCast(ptr(src), ptr(tgt))
      ));
  }
}}

