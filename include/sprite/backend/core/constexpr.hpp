/**
 * @file
 * @brief Implements constant expression operators.
 */

#pragma once
#include "sprite/backend/core/constant.hpp"
#include "sprite/backend/core/get_value.hpp"
#include "sprite/backend/core/global.hpp"
#include "sprite/backend/core/operator_flags.hpp"
#include "sprite/backend/support/exceptions.hpp"
#include "sprite/backend/support/type_traits.hpp"

namespace sprite { namespace backend
{
  namespace aux
  {
    template<typename Type, typename Arg>
    inline bool has_arg_types(Arg const & arg)
      { return llvm::dyn_cast<Type>(ptr(arg)); }

    template<typename Type, typename Lhs, typename Rhs>
    inline bool has_arg_types(Lhs const & lhs, Rhs const & rhs)
      { return has_arg_types<Type>(lhs) && has_arg_types<Type>(rhs); }
  }

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
   * @brief Integer or floating-point negation.
   *
   * @snippet constexprs.cpp neg
   */
  template<typename Arg>
  inline constant operator-(aux::arg_with_flags<Arg> const & c)
  {
    if(aux::has_arg_types<ConstantInt>(c))
    {
      SPRITE_ALLOW_FLAGS(c, "negation", operator_flags::NUW | operator_flags::NSW)
      return constant(SPRITE_APICALL(
          ConstantExpr::getNeg(
              ptr(c), c.flags().nuw(), c.flags().nsw()
            )
        ));
    }
    else if(aux::has_arg_types<ConstantFP>(c))
    {
      SPRITE_ALLOW_FLAGS(c, "negation", operator_flags::SIGNED)
      return constant(SPRITE_APICALL(ConstantExpr::getFNeg(ptr(c))));
    }
    throw type_error("Expected ConstantInt or ConstantFP for negation.");
  }

  /**
   * @brief Integer or floating-point negation.
   *
   * @snippet constexprs.cpp neg
   */
  inline constant operator-(constant const & c)
    { return -aux::operator_flags()(c); }

  /**
   * @brief Bitwise inversion.
   *
   * @snippet constexprs.cpp inv
   */
  inline constant operator~(constant const & c)
  {
    if(aux::has_arg_types<ConstantInt>(c))
      return constant(SPRITE_APICALL(ConstantExpr::getNot(ptr(c))));
    throw type_error("Expected ConstantInt for bitwise inversion.");
  }

  /**
   * @brief Unary plus.
   *
   * @snippet constexprs.cpp pos
   */
  inline constant operator+(constant const & c)
    { return c; }

  namespace aux
  {
    // These functions help binary operators accept any combination of Constant
    // * and constantobj, so long as at least one argument is a wrapper.
    inline constant getlhs(constant const & lhs, void *)
      { return lhs; }

    template<typename T>
    inline constant getlhs(constant const & lhs, object<T> const &)
      { return lhs; }

    template<typename T>
    inline constant getlhs(Constant * lhs, object<T> const &)
      { return constant(lhs); }

    inline constant getrhs(constant const & lhs, Constant * rhs)
      { return constant(rhs); }

    inline type
    getrhs(constant const & lhs, Type * rhs)
      { return type(rhs); }

    inline constant getrhs(constant const &, constant const & rhs)
      { return rhs; }

    inline type
    getrhs(constant const &, type const & rhs)
      { return rhs; }

    inline constant getrhs(Constant *, constant const & rhs)
      { return rhs; }

    inline type
    getrhs(Constant *, type const & rhs)
      { return rhs; }
  }

  //@{
  /**
   * @brief Integer or floating-point addition.
   *
   * @snippet constexprs.cpp add
   */
  #define SPRITE_OP +
  #define SPRITE_INPLACE_OP +=
  #define SPRITE_CLASS_CONTEXT constobj<llvm::Constant>::
  #define SPRITE_LHS_TYPE constant
  #define SPRITE_OP_NAME "addition"
  #define SPRITE_OP_INT_FLAG_CHECK SPRITE_ALLOW_NSW_NUW_FLAGS
  #define SPRITE_OP_INT_IMPL(lhs,rhs,flags)                    \
      ConstantExpr::getAdd(lhs, rhs, flags.nuw(), flags.nsw()) \
    /**/
  #define SPRITE_OP_FP_FLAG_CHECK SPRITE_ALLOW_SIGNED_FLAG
  #define SPRITE_OP_FP_IMPL(lhs,rhs,flags) ConstantExpr::getFAdd(lhs,rhs)
  #include "sprite/backend/core/detail/operator.def"

  /**
   * @brief Integer or floating-point subtraction.
   *
   * @snippet constexprs.cpp sub
   */
  #define SPRITE_OP -
  #define SPRITE_INPLACE_OP -=
  #define SPRITE_CLASS_CONTEXT constobj<llvm::Constant>::
  #define SPRITE_LHS_TYPE constant
  #define SPRITE_OP_NAME "subtraction"
  #define SPRITE_OP_INT_FLAG_CHECK SPRITE_ALLOW_NSW_NUW_FLAGS
  #define SPRITE_OP_INT_IMPL(lhs,rhs,flags)                    \
      ConstantExpr::getSub(lhs, rhs, flags.nuw(), flags.nsw()) \
    /**/
  #define SPRITE_OP_FP_FLAG_CHECK SPRITE_ALLOW_SIGNED_FLAG
  #define SPRITE_OP_FP_IMPL(lhs,rhs,flags) ConstantExpr::getFSub(lhs,rhs)
  #include "sprite/backend/core/detail/operator.def"

  /**
   * @brief Integer or floating-point multiplication.
   *
   * @snippet constexprs.cpp mul
   */
  #define SPRITE_OP *
  #define SPRITE_INPLACE_OP *=
  #define SPRITE_CLASS_CONTEXT constobj<llvm::Constant>::
  #define SPRITE_LHS_TYPE constant
  #define SPRITE_OP_NAME "multiplication"
  #define SPRITE_OP_INT_FLAG_CHECK SPRITE_ALLOW_NSW_NUW_FLAGS
  #define SPRITE_OP_INT_IMPL(lhs,rhs,flags)                    \
      ConstantExpr::getMul(lhs, rhs, flags.nuw(), flags.nsw()) \
    /**/
  #define SPRITE_OP_FP_FLAG_CHECK SPRITE_ALLOW_SIGNED_FLAG
  #define SPRITE_OP_FP_IMPL(lhs,rhs,flags) ConstantExpr::getFMul(lhs,rhs)
  #include "sprite/backend/core/detail/operator.def"

  /**
   * @brief Integer or floating-point division.
   *
   * @snippet constexprs.cpp div
   */
  #define SPRITE_OP /
  #define SPRITE_INPLACE_OP /=
  #define SPRITE_CLASS_CONTEXT constobj<llvm::Constant>::
  #define SPRITE_LHS_TYPE constant
  #define SPRITE_OP_NAME "division"
  #define SPRITE_OP_INT_FLAG_CHECK SPRITE_ALLOW_DIV_FLAGS
  #define SPRITE_OP_INT_IMPL(lhs,rhs,flags)              \
      flags.signed_()                                    \
        ? ConstantExpr::getSDiv(lhs, rhs, flags.exact()) \
        : ConstantExpr::getUDiv(lhs, rhs, flags.exact()) \
    /**/
  #define SPRITE_OP_FP_FLAG_CHECK SPRITE_ALLOW_SIGNED_FLAG
  #define SPRITE_OP_FP_IMPL(lhs,rhs,flags) ConstantExpr::getFDiv(lhs,rhs)
  #include "sprite/backend/core/detail/operator.def"

  /**
   * @brief Integer or floating-point remainder.
   *
   * @snippet constexprs.cpp rem
   */
  #define SPRITE_OP %
  #define SPRITE_INPLACE_OP %=
  #define SPRITE_CLASS_CONTEXT constobj<llvm::Constant>::
  #define SPRITE_LHS_TYPE constant
  #define SPRITE_OP_NAME "remainder"
  #define SPRITE_OP_INT_FLAG_CHECK SPRITE_REQUIRE_SIGNED_UNSIGNED_FLAG
  #define SPRITE_OP_INT_IMPL(lhs,rhs,flags) \
      flags.signed_()                       \
        ? ConstantExpr::getSRem(lhs, rhs)   \
        : ConstantExpr::getURem(lhs, rhs)   \
    /**/
  #define SPRITE_OP_FP_FLAG_CHECK SPRITE_ALLOW_SIGNED_FLAG
  #define SPRITE_OP_FP_IMPL(lhs,rhs,flags) ConstantExpr::getFRem(lhs,rhs)
  #include "sprite/backend/core/detail/operator.def"

  /**
   * @brief Bitwise AND.
   *
   * @snippet constexprs.cpp and
   */
  #define SPRITE_OP &
  #define SPRITE_INPLACE_OP &=
  #define SPRITE_CLASS_CONTEXT constobj<llvm::Constant>::
  #define SPRITE_LHS_TYPE constant
  #define SPRITE_OP_NAME "bitwise AND"
  #define SPRITE_OP_INT_IMPL(lhs,rhs,flags) ConstantExpr::getAnd(lhs, rhs)
  #include "sprite/backend/core/detail/operator.def"

  /**
   * @brief Bitwise OR.
   *
   * @snippet constexprs.cpp or
   */
  #define SPRITE_OP |
  #define SPRITE_INPLACE_OP |=
  #define SPRITE_CLASS_CONTEXT constobj<llvm::Constant>::
  #define SPRITE_LHS_TYPE constant
  #define SPRITE_OP_NAME "bitwise OR"
  #define SPRITE_OP_INT_IMPL(lhs,rhs,flags) ConstantExpr::getOr(lhs, rhs)
  #include "sprite/backend/core/detail/operator.def"

  /**
   * @brief Bitwise XOR.
   *
   * @snippet constexprs.cpp or
   */
  #define SPRITE_OP ^
  #define SPRITE_INPLACE_OP ^=
  #define SPRITE_CLASS_CONTEXT constobj<llvm::Constant>::
  #define SPRITE_LHS_TYPE constant
  #define SPRITE_OP_NAME "bitwise XOR"
  #define SPRITE_OP_INT_IMPL(lhs,rhs,flags) ConstantExpr::getXor(lhs, rhs)
  #include "sprite/backend/core/detail/operator.def"

  /**
   * @brief Left shift.
   *
   * @snippet constexprs.cpp shl
   */
  #define SPRITE_OP <<
  #define SPRITE_INPLACE_OP <<=
  #define SPRITE_CLASS_CONTEXT constobj<llvm::Constant>::
  #define SPRITE_LHS_TYPE constant
  #define SPRITE_OP_NAME "left shift"
  #define SPRITE_OP_INT_FLAG_CHECK SPRITE_ALLOW_NSW_NUW_FLAGS
  #define SPRITE_OP_INT_IMPL(lhs,rhs,flags)                    \
      ConstantExpr::getShl(lhs, rhs, flags.nuw(), flags.nsw()) \
    /**/
  #include "sprite/backend/core/detail/operator.def"

  /**
   * @brief Right shift.
   *
   * @snippet constexprs.cpp shr
   */
  #define SPRITE_OP >>
  #define SPRITE_INPLACE_OP >>=
  #define SPRITE_CLASS_CONTEXT constobj<llvm::Constant>::
  #define SPRITE_LHS_TYPE constant
  #define SPRITE_OP_NAME "right shift"
  #define SPRITE_OP_INT_FLAG_CHECK SPRITE_ALLOW_SHR_FLAGS
  #define SPRITE_OP_INT_IMPL(lhs,rhs,flags)              \
      flags.arithmetic()                                 \
        ? ConstantExpr::getAShr(lhs, rhs, flags.exact()) \
        : ConstantExpr::getLShr(lhs, rhs, flags.exact()) \
    /**/
    /**/
  #include "sprite/backend/core/detail/operator.def"

  /**
   * @brief Performs a select operation.
   *
   * @snippet constexprs.cpp select
   */
  template<typename If, typename Then, typename Else>
  inline typename std::enable_if<
      is_constarg<If>::value && is_constarg<Then>::value
        && is_constarg<Else>::value
    , constant
    >::type
  select(If const & if_, Then const & then, Else const & else_)
  {
    return constant(SPRITE_APICALL(
        ConstantExpr::getSelect(ptr(if_), ptr(then), ptr(else_))
      ));
  }
}}

