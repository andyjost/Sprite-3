/**
 * @file
 * @brief Implements constant expression operators.
 */

#pragma once
#include "sprite/backend/core/castexpr.hpp"
#include "sprite/backend/core/constant.hpp"
#include "sprite/backend/core/detail/current_builder.hpp"
#include "sprite/backend/core/get_value.hpp"
#include "sprite/backend/core/global.hpp"
#include "sprite/backend/core/label.hpp"
#include "sprite/backend/core/operator_flags.hpp"
#include "sprite/backend/core/type.hpp"
#include "sprite/backend/core/value.hpp"
#include "sprite/backend/support/exceptions.hpp"
#include "sprite/backend/support/type_traits.hpp"
#include "llvm/IR/Instructions.h"

namespace sprite { namespace backend
{
  /**
   * @brief Computes the alignment of a type.
   *
   * @snippet constexprs.cpp alignof_
   */
  inline constant alignof_(type const & ty)
    { return constant(SPRITE_APICALL(ConstantExpr::getAlignOf(ty.ptr()))); }

  // No wrapper for constant expression getSizeOf.  Use i64(sizeof_(ty))
  // instead.

  /**
   * @brief Computes the offset of a field in a struct.
   *
   * @snippet constexprs.cpp offsetof_
   */
  inline constant offsetof_(type const & ty, unsigned FieldNo)
  {
    if(auto const p = dyn_cast<StructType>(ty))
    {
      return constant(
          SPRITE_APICALL(ConstantExpr::getOffsetOf(p.ptr(), FieldNo))
        );
    }
    throw type_error("Expected StructType for offsetof_.");
  }

  /**
   * @brief Computes the offset of a field in a struct.
   *
   * @snippet constexprs.cpp offsetof_
   */
  inline constant offsetof_(type const & ty, constant FieldNo)
  {
    return constant(
        SPRITE_APICALL(ConstantExpr::getOffsetOf(ty.ptr(), FieldNo.ptr()))
      );
  }

  /**
   * @brief Performs a select operation.
   *
   * @snippet constexprs.cpp select
   */
  template<typename If, typename True, typename False>
  inline typename std::enable_if<
      is_constarg<If>::value && is_constarg<True>::value
        && is_constarg<False>::value
    , constant
    >::type
  select(If const & cond, True const & true_, False const & false_)
  {
    return constant(SPRITE_APICALL(
        ConstantExpr::getSelect(ptr(cond), ptr(true_), ptr(false_))
      ));
  }

  /**
   * @brief Integer or floating-point negation.
   *
   * @snippet constexprs.cpp neg
   */
  #define SPRITE_UNOP -
  #define SPRITE_OP_NAME "negation"
  #define SPRITE_OP_INT_FLAG_CHECK SPRITE_ALLOW_NSW_NUW_FLAGS
  #define SPRITE_OP_CONST_INT_IMPL(arg,flags)             \
      ConstantExpr::getNeg(arg, flags.nuw(), flags.nsw()) \
    /**/
  #define SPRITE_OP_VALUE_INT_IMPL(arg,flags)                        \
      current_builder().CreateNeg(arg, "", flags.nuw(), flags.nsw()) \
    /**/
  #define SPRITE_OP_FP_FLAG_CHECK SPRITE_ALLOW_SIGNED_FLAG
  #define SPRITE_OP_CONST_FP_IMPL(arg,flags) ConstantExpr::getFNeg(arg)
  #define SPRITE_OP_VALUE_FP_IMPL(arg,flags) \
      current_builder().CreateFNeg(arg, "")  \
    /**/
  #include "sprite/backend/core/detail/unary_operator.def"

  /**
   * @brief Bitwise inversion.
   *
   * @snippet constexprs.cpp inv
   */
  #define SPRITE_UNOP ~
  #define SPRITE_OP_NAME "bitwise inversion"
  #define SPRITE_OP_INT_FLAG_CHECK SPRITE_ALLOW_NO_FLAGS
  #define SPRITE_OP_CONST_INT_IMPL(arg,flags) ConstantExpr::getNot(arg)
  #define SPRITE_OP_VALUE_INT_IMPL(arg,flags) \
      current_builder().CreateNot(arg, "")    \
    /**/
  #include "sprite/backend/core/detail/unary_operator.def"

  /**
   * @brief Unary plus.
   *
   * @snippet constexprs.cpp pos
   */
  #define SPRITE_UNOP +
  #define SPRITE_OP_NAME "unary plus"
  #define SPRITE_OP_INT_FLAG_CHECK SPRITE_NO_FLAG_CHECK
  #define SPRITE_OP_CONST_INT_IMPL(arg,flags) arg
  #define SPRITE_OP_VALUE_INT_IMPL(arg,flags) arg
  #define SPRITE_OP_FP_FLAG_CHECK SPRITE_NO_FLAG_CHECK
  #define SPRITE_OP_CONST_FP_IMPL(arg,flags) arg
  #define SPRITE_OP_VALUE_FP_IMPL(arg,flags) arg
  #include "sprite/backend/core/detail/unary_operator.def"

  /**
   * @brief Integer or floating-point addition.
   *
   * @snippet constexprs.cpp add
   */
  #define SPRITE_INPLACE_OP +=
  #define SPRITE_OP_NAME "addition"
  #define SPRITE_OP_INT_FLAG_CHECK SPRITE_ALLOW_NSW_NUW_FLAGS
  #define SPRITE_OP_CONST_INT_IMPL(lhs,rhs,flags)              \
      ConstantExpr::getAdd(lhs, rhs, flags.nuw(), flags.nsw()) \
    /**/
  #define SPRITE_OP_VALUE_INT_IMPL(lhs,rhs,flags)                         \
      current_builder().CreateAdd(lhs, rhs, "", flags.nuw(), flags.nsw()) \
    /**/
  #define SPRITE_OP_FP_FLAG_CHECK SPRITE_ALLOW_SIGNED_FLAG
  #define SPRITE_OP_CONST_FP_IMPL(lhs,rhs,flags) ConstantExpr::getFAdd(lhs,rhs)
  #define SPRITE_OP_VALUE_FP_IMPL(lhs,rhs,flags) \
      current_builder().CreateFAdd(lhs,rhs,"")   \
    /**/
  #include "sprite/backend/core/detail/operator.def"
  //
  #define SPRITE_BINOP +
  #include "sprite/backend/core/detail/binop_from_inplace.def"

  /**
   * @brief Integer or floating-point subtraction.
   *
   * @snippet constexprs.cpp sub
   */
  #define SPRITE_INPLACE_OP -=
  #define SPRITE_OP_NAME "subtraction"
  #define SPRITE_OP_INT_FLAG_CHECK SPRITE_ALLOW_NSW_NUW_FLAGS
  #define SPRITE_OP_CONST_INT_IMPL(lhs,rhs,flags)              \
      ConstantExpr::getSub(lhs, rhs, flags.nuw(), flags.nsw()) \
    /**/
  #define SPRITE_OP_VALUE_INT_IMPL(lhs,rhs,flags)                         \
      current_builder().CreateSub(lhs, rhs, "", flags.nuw(), flags.nsw()) \
    /**/
  #define SPRITE_OP_FP_FLAG_CHECK SPRITE_ALLOW_SIGNED_FLAG
  #define SPRITE_OP_CONST_FP_IMPL(lhs,rhs,flags) ConstantExpr::getFSub(lhs,rhs)
  #define SPRITE_OP_VALUE_FP_IMPL(lhs,rhs,flags) \
      current_builder().CreateFSub(lhs,rhs,"")   \
    /**/
  #include "sprite/backend/core/detail/operator.def"
  //
  #define SPRITE_BINOP -
  #include "sprite/backend/core/detail/binop_from_inplace.def"

  /**
   * @brief Integer or floating-point multiplication.
   *
   * @snippet constexprs.cpp mul
   */
  #define SPRITE_INPLACE_OP *=
  #define SPRITE_OP_NAME "multiplication"
  #define SPRITE_OP_INT_FLAG_CHECK SPRITE_ALLOW_NSW_NUW_FLAGS
  #define SPRITE_OP_CONST_INT_IMPL(lhs,rhs,flags)              \
      ConstantExpr::getMul(lhs, rhs, flags.nuw(), flags.nsw()) \
    /**/
  #define SPRITE_OP_VALUE_INT_IMPL(lhs,rhs,flags)                         \
      current_builder().CreateMul(lhs, rhs, "", flags.nuw(), flags.nsw()) \
    /**/
  #define SPRITE_OP_FP_FLAG_CHECK SPRITE_ALLOW_SIGNED_FLAG
  #define SPRITE_OP_CONST_FP_IMPL(lhs,rhs,flags) ConstantExpr::getFMul(lhs,rhs)
  #define SPRITE_OP_VALUE_FP_IMPL(lhs,rhs,flags) \
      current_builder().CreateFMul(lhs,rhs,"")   \
    /**/
  #include "sprite/backend/core/detail/operator.def"
  //
  #define SPRITE_BINOP *
  #include "sprite/backend/core/detail/binop_from_inplace.def"

  /**
   * @brief Integer or floating-point division.
   *
   * @snippet constexprs.cpp div
   */
  #define SPRITE_INPLACE_OP /=
  #define SPRITE_OP_NAME "division"
  #define SPRITE_OP_INT_FLAG_CHECK SPRITE_ALLOW_DIV_FLAGS
  #define SPRITE_OP_CONST_INT_IMPL(lhs,rhs,flags)        \
      flags.signed_()                                    \
        ? ConstantExpr::getSDiv(lhs, rhs, flags.exact()) \
        : ConstantExpr::getUDiv(lhs, rhs, flags.exact()) \
    /**/
  #define SPRITE_OP_VALUE_INT_IMPL(lhs,rhs,flags)                   \
      flags.signed_()                                               \
        ? current_builder().CreateSDiv(lhs, rhs, "", flags.exact()) \
        : current_builder().CreateUDiv(lhs, rhs, "", flags.exact()) \
    /**/
  #define SPRITE_OP_FP_FLAG_CHECK SPRITE_ALLOW_SIGNED_FLAG
  #define SPRITE_OP_CONST_FP_IMPL(lhs,rhs,flags) ConstantExpr::getFDiv(lhs,rhs)
  #define SPRITE_OP_VALUE_FP_IMPL(lhs,rhs,flags) \
      current_builder().CreateFDiv(lhs,rhs,"")   \
    /**/
  #include "sprite/backend/core/detail/operator.def"
  //
  #define SPRITE_BINOP /
  #include "sprite/backend/core/detail/binop_from_inplace.def"

  /**
   * @brief Integer or floating-point remainder.
   *
   * @snippet constexprs.cpp rem
   */
  #define SPRITE_INPLACE_OP %=
  #define SPRITE_OP_NAME "remainder"
  #define SPRITE_OP_INT_FLAG_CHECK SPRITE_REQUIRE_SIGNED_UNSIGNED_FLAG
  #define SPRITE_OP_CONST_INT_IMPL(lhs,rhs,flags) \
      flags.signed_()                             \
        ? ConstantExpr::getSRem(lhs, rhs)         \
        : ConstantExpr::getURem(lhs, rhs)         \
    /**/
  #define SPRITE_OP_VALUE_INT_IMPL(lhs,rhs,flags)  \
      flags.signed_()                              \
        ? current_builder().CreateSRem(lhs,rhs,"") \
        : current_builder().CreateURem(lhs,rhs,"") \
    /**/
  #define SPRITE_OP_FP_FLAG_CHECK SPRITE_ALLOW_SIGNED_FLAG
  #define SPRITE_OP_CONST_FP_IMPL(lhs,rhs,flags) ConstantExpr::getFRem(lhs,rhs)
  #define SPRITE_OP_VALUE_FP_IMPL(lhs,rhs,flags) \
      current_builder().CreateFRem(lhs,rhs,"")   \
    /**/
  #include "sprite/backend/core/detail/operator.def"
  //
  #define SPRITE_BINOP %
  #include "sprite/backend/core/detail/binop_from_inplace.def"

  /**
   * @brief Bitwise AND.
   *
   * @snippet constexprs.cpp and
   */
  #define SPRITE_INPLACE_OP &=
  #define SPRITE_OP_NAME "bitwise AND"
  #define SPRITE_OP_INT_FLAG_CHECK SPRITE_ALLOW_NO_FLAGS
  #define SPRITE_OP_CONST_INT_IMPL(lhs,rhs,flags) ConstantExpr::getAnd(lhs, rhs)
  #define SPRITE_OP_VALUE_INT_IMPL(lhs,rhs,flags) \
      current_builder().CreateAnd(lhs,rhs,"")     \
    /**/
  #include "sprite/backend/core/detail/operator.def"
  //
  #define SPRITE_BINOP &
  #include "sprite/backend/core/detail/binop_from_inplace.def"

  /**
   * @brief Bitwise OR.
   *
   * @snippet constexprs.cpp or
   */
  #define SPRITE_INPLACE_OP |=
  #define SPRITE_OP_NAME "bitwise OR"
  #define SPRITE_OP_INT_FLAG_CHECK SPRITE_ALLOW_NO_FLAGS
  #define SPRITE_OP_CONST_INT_IMPL(lhs,rhs,flags) ConstantExpr::getOr(lhs, rhs)
  #define SPRITE_OP_VALUE_INT_IMPL(lhs,rhs,flags) \
      current_builder().CreateOr(lhs,rhs,"")      \
    /**/
  #include "sprite/backend/core/detail/operator.def"
  //
  #define SPRITE_BINOP |
  #include "sprite/backend/core/detail/binop_from_inplace.def"

  /**
   * @brief Bitwise XOR.
   *
   * @snippet constexprs.cpp or
   */
  #define SPRITE_INPLACE_OP ^=
  #define SPRITE_OP_NAME "bitwise XOR"
  #define SPRITE_OP_INT_FLAG_CHECK SPRITE_ALLOW_NO_FLAGS
  #define SPRITE_OP_CONST_INT_IMPL(lhs,rhs,flags) ConstantExpr::getXor(lhs, rhs)
  #define SPRITE_OP_VALUE_INT_IMPL(lhs,rhs,flags) \
      current_builder().CreateXor(lhs,rhs,"")     \
    /**/
  #include "sprite/backend/core/detail/operator.def"
  //
  #define SPRITE_BINOP ^
  #include "sprite/backend/core/detail/binop_from_inplace.def"

  /**
   * @brief Left shift.
   *
   * @snippet constexprs.cpp shl
   */
  #define SPRITE_INPLACE_OP <<=
  #define SPRITE_OP_NAME "left shift"
  #define SPRITE_OP_INT_FLAG_CHECK SPRITE_ALLOW_NSW_NUW_FLAGS
  #define SPRITE_OP_CONST_INT_IMPL(lhs,rhs,flags)              \
      ConstantExpr::getShl(lhs, rhs, flags.nuw(), flags.nsw()) \
    /**/
  #define SPRITE_OP_VALUE_INT_IMPL(lhs,rhs,flags)                         \
      current_builder().CreateShl(lhs, rhs, "", flags.nuw(), flags.nsw()) \
    /**/
  #include "sprite/backend/core/detail/operator.def"
  //
  #define SPRITE_BINOP <<
  #include "sprite/backend/core/detail/binop_from_inplace.def"

  /**
   * @brief Right shift.
   *
   * @snippet constexprs.cpp shr
   */
  #define SPRITE_INPLACE_OP >>=
  #define SPRITE_OP_NAME "right shift"
  #define SPRITE_OP_INT_FLAG_CHECK SPRITE_ALLOW_SHR_FLAGS
  #define SPRITE_OP_CONST_INT_IMPL(lhs,rhs,flags)        \
      flags.arithmetic()                                 \
        ? ConstantExpr::getAShr(lhs, rhs, flags.exact()) \
        : ConstantExpr::getLShr(lhs, rhs, flags.exact()) \
    /**/
  #define SPRITE_OP_VALUE_INT_IMPL(lhs,rhs,flags)                   \
      flags.arithmetic()                                            \
        ? current_builder().CreateAShr(lhs, rhs, "", flags.exact()) \
        : current_builder().CreateLShr(lhs, rhs, "", flags.exact()) \
    /**/
  #include "sprite/backend/core/detail/operator.def"
  //
  #define SPRITE_BINOP >>
  #include "sprite/backend/core/detail/binop_from_inplace.def"

  /**
   * @brief Less than.
   */
  #define SPRITE_CMPOP <
  #define SPRITE_OP_NAME "less-than"
  #define SPRITE_CMPOP_CODE LT
  #define SPRITE_CMPOP_IS_ORDERING 1
  #include "sprite/backend/core/detail/comparison_operator.def"

  /**
   * @brief Less than or equal to.
   */
  #define SPRITE_CMPOP <=
  #define SPRITE_OP_NAME "less-than-or-equal-to"
  #define SPRITE_CMPOP_CODE LE
  #define SPRITE_CMPOP_IS_ORDERING 1
  #include "sprite/backend/core/detail/comparison_operator.def"

  /**
   * @brief Greater than.
   */
  #define SPRITE_CMPOP >
  #define SPRITE_OP_NAME "greater-than"
  #define SPRITE_CMPOP_CODE GT
  #define SPRITE_CMPOP_IS_ORDERING 1
  #include "sprite/backend/core/detail/comparison_operator.def"

  /**
   * @brief Greater than or equal to.
   */
  #define SPRITE_CMPOP >=
  #define SPRITE_OP_NAME "greater-than-or-equal-to"
  #define SPRITE_CMPOP_CODE GE
  #define SPRITE_CMPOP_IS_ORDERING 1
  #include "sprite/backend/core/detail/comparison_operator.def"

  /**
   * @brief Equal to.
   */
  #define SPRITE_CMPOP ==
  #define SPRITE_OP_NAME "equal-to"
  #define SPRITE_CMPOP_CODE EQ
  #define SPRITE_CMPOP_IS_ORDERING 0
  #include "sprite/backend/core/detail/comparison_operator.def"

  /**
   * @brief Not equal to.
   */
  #define SPRITE_CMPOP !=
  #define SPRITE_OP_NAME "not-equal-to"
  #define SPRITE_CMPOP_CODE NE
  #define SPRITE_CMPOP_IS_ORDERING 0
  #include "sprite/backend/core/detail/comparison_operator.def"

  /// Appends a return instruction to the active label scope.
  template<typename T>
  instruction return_(T && arg)
  {
    llvm::IRBuilder<> & bldr = current_builder();
    type const retty = type(bldr.GetInsertBlock()->getParent()->getReturnType());
    value const x = get_value(retty, arg);
    return instruction(SPRITE_APICALL(bldr.CreateRet(x.ptr())));
  }

  //@{
  /// Appends a conditional branch instruction to the active label scope.
  template<typename T>
  instruction if_(T && cond, label && true_, label && false_)
  {
    llvm::IRBuilder<> & bldr = current_builder();
    value const cond_ = get_value(types::bool_(), cond);
    auto rv = SPRITE_APICALL(
        bldr.CreateCondBr(cond_.ptr(), true_.ptr(), false_.ptr())
      );
    label next;
    scope::replace_label(next);
    true_._set_continuation(next);
    false_._set_continuation(next);
    return instruction(rv);
  }

  template<typename T>
  instruction if_(T && cond, label && true_)
  {
    label next;
    llvm::IRBuilder<> & bldr = current_builder();
    value const cond_ = get_value(types::bool_(), cond);
    auto rv = SPRITE_APICALL(
        bldr.CreateCondBr(cond_.ptr(), true_.ptr(), next.ptr())
      );
    scope::replace_label(next);
    true_._set_continuation(next);
    return instruction(rv);
  }
  //@}

  /// Appends an unconditional branch instruction to the active label scope.
  inline instruction goto_(label const & target)
  {
    llvm::IRBuilder<> & bldr = current_builder();
    return instruction(SPRITE_APICALL(bldr.CreateBr(target.ptr())));
  }

  //@{
  /// Allocates a local variable.  Returns a pointer.
  template<typename T>
  inline typename std::enable_if<is_value_initializer<T>::value, value>::type
  local(
      type const & ty, T const & size, unsigned alignment=0
    )
  {
    llvm::IRBuilder<> & bldr = current_builder();
    llvm::AllocaInst * px =
        SPRITE_APICALL(bldr.CreateAlloca(ty.ptr(), get_value(size).ptr()));
    if(alignment) SPRITE_APICALL(px->setAlignment(alignment));
    return value(px);
  }

  inline value local(type const & ty) { return local(ty, value(nullptr)); }
  //@}
}}

