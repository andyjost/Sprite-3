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

  // No wrapper for constant expression getSizeOf.  Use i64 % sizeof_(ty)
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
  inline constant offsetof_(type const & ty, Constant * FieldNo)
  {
    return constant(
        SPRITE_APICALL(ConstantExpr::getOffsetOf(ty.ptr(), FieldNo))
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

  template<typename Rhs>
  typename std::enable_if<is_raw_initializer<Rhs>::value, constant &>::type
  constobj<llvm::Constant>::operator+=(Rhs const & rhs)
  {
    auto const rhs_ = get_constant(get_type(*this), rhs);
    (*this) += rhs_;
    return *this;
  }

  template<typename Rhs>
  typename std::enable_if<is_constarg<Rhs>::value, constant &>::type
  constobj<llvm::Constant>::operator+=(Rhs const & rhs)
  {
    (*this) +=aux::operator_flags() (rhs);
    return *this;
  }

  template<typename Rhs>
  typename std::enable_if<is_constarg<Rhs>::value, constant &>::type
  constobj<llvm::Constant>::operator+=(aux::arg_with_flags<Rhs> const & rhs)
  {
    type const ty = coerce(get_type(*this), get_type(rhs));
    auto const lhs_ = get_value(ty, *this);
    auto const rhs_ = get_value(ty, rhs);
    if(ty->isIntegerTy())
    {
      SPRITE_ALLOW_FLAGS(rhs, "addition", operator_flags::NUW | operator_flags::NSW)
      *this = constant(SPRITE_APICALL(
          ConstantExpr::getAdd(
              lhs_.ptr(), rhs_.ptr(), rhs.flags().nuw(), rhs.flags().nsw()
            )
        ));
      return *this;
    }
    else if(ty->isFloatingPointTy())
    {
      SPRITE_ALLOW_FLAGS(rhs, "addition", operator_flags::SIGNED)
      *this = constant(SPRITE_APICALL(
          ConstantExpr::getFAdd(lhs_.ptr(), rhs_.ptr())
        ));
      return *this;
    }
    throw type_error("Expected integer or floating-point for addition.");
  }

  /**
   * @brief Integer or floating-point addition.
   *
   * @snippet constexprs.cpp add
   */
  template<typename Lhs, typename Arg>
  inline typename std::enable_if<
      is_constarg<Lhs>::value, constant
    >::type
  operator+(Lhs const & lhs, aux::arg_with_flags<Arg> const & rhs)
  {
    if(aux::has_arg_types<ConstantInt>(lhs, rhs))
    {
      SPRITE_ALLOW_FLAGS(rhs, "addition", operator_flags::NUW | operator_flags::NSW)
      return constant(SPRITE_APICALL(
          ConstantExpr::getAdd(
              ptr(lhs), ptr(rhs), rhs.flags().nuw(), rhs.flags().nsw()
            )
        ));
    }
    else if(aux::has_arg_types<ConstantFP>(lhs, rhs))
    {
      SPRITE_ALLOW_FLAGS(rhs, "addition", operator_flags::SIGNED)
      return constant(SPRITE_APICALL(
          ConstantExpr::getFAdd(ptr(lhs), ptr(rhs))
        ));
    }
    throw type_error("Expected ConstantInt or ConstantFP for addition.");
  }

  /**
   * @brief Integer or floating-point addition.
   *
   * @snippet constexprs.cpp add
   */
  template<typename Lhs, typename Rhs>
  inline typename std::enable_if<
      is_constarg<Lhs>::value && is_constarg<Rhs>(), constant
    >::type
  operator+(Lhs const & lhs, Rhs const & rhs)
  {
    constant lhs_ = aux::getlhs(lhs, rhs);
    constant rhs_ = aux::getrhs(lhs, rhs);
    return lhs_ +aux::operator_flags() (rhs_);
  }

  /**
   * @brief Integer or floating-point subtraction.
   *
   * @snippet constexprs.cpp sub
   */
  template<typename Lhs, typename Arg>
  inline typename std::enable_if<is_constarg<Lhs>::value, constant>::type
  operator-(Lhs const & lhs, aux::arg_with_flags<Arg> const & rhs)
  {
    if(aux::has_arg_types<ConstantInt>(lhs, rhs))
    {
      SPRITE_ALLOW_FLAGS(rhs, "subtraction", operator_flags::NUW | operator_flags::NSW)
      return constant(SPRITE_APICALL(
          ConstantExpr::getSub(
              ptr(lhs), ptr(rhs), rhs.flags().nuw(), rhs.flags().nsw()
            )
        ));
    }
    else if(aux::has_arg_types<ConstantFP>(lhs, rhs))
    {
      SPRITE_ALLOW_FLAGS(rhs, "subtraction", operator_flags::SIGNED)
      return constant(SPRITE_APICALL(
          ConstantExpr::getFSub(ptr(lhs), ptr(rhs))
        ));
    }
    throw type_error("Expected ConstantInt or ConstantFP for subtraction.");
  }

  /**
   * @brief Integer or floating-point subtraction.
   *
   * @snippet constexprs.cpp sub
   */
  template<typename Lhs, typename Rhs>
  inline typename std::enable_if<
      is_constarg<Lhs>::value && is_constarg<Rhs>::value, constant
    >::type
  operator-(Lhs const & lhs, Rhs const & rhs)
  {
    constant lhs_ = aux::getlhs(lhs, rhs);
    constant rhs_ = aux::getrhs(lhs, rhs);
    return lhs_ -aux::operator_flags() (rhs_);
  }

  /**
   * @brief Integer or floating-point multiplication.
   *
   * @snippet constexprs.cpp mul
   */
  template<typename Lhs, typename Arg>
  inline typename std::enable_if<is_constarg<Lhs>::value, constant>::type
  operator*(Lhs const & lhs, aux::arg_with_flags<Arg> const & rhs)
  {
    if(aux::has_arg_types<ConstantInt>(lhs, rhs))
    {
      SPRITE_ALLOW_FLAGS(rhs, "multiplication", operator_flags::NUW | operator_flags::NSW)
      return constant(SPRITE_APICALL(
          ConstantExpr::getMul(
              ptr(lhs), ptr(rhs), rhs.flags().nuw(), rhs.flags().nsw()
            )
        ));
    }
    else if(aux::has_arg_types<ConstantFP>(lhs, rhs))
    {
      SPRITE_ALLOW_FLAGS(rhs, "multiplication", operator_flags::SIGNED)
      return constant(SPRITE_APICALL(
          ConstantExpr::getFMul(ptr(lhs), ptr(rhs))
        ));
    }
    throw type_error("Expected ConstantInt or ConstantFP for multiplication.");
  }

  /**
   * @brief Integer or floating-point multiplication.
   *
   * @snippet constexprs.cpp mul
   */
  template<typename Lhs, typename Rhs>
  inline typename std::enable_if<
      is_constarg<Lhs>::value && is_constarg<Rhs>::value, constant
    >::type
  operator*(Lhs const & lhs, Rhs const & rhs)
  {
    constant lhs_ = aux::getlhs(lhs, rhs);
    constant rhs_ = aux::getrhs(lhs, rhs);
    return lhs_ *aux::operator_flags() (rhs_);
  }

  /**
   * @brief Integer or floating-point division.
   *
   * @snippet constexprs.cpp div
   */
  template<typename Lhs, typename Arg>
  typename std::enable_if<is_constarg<Lhs>::value, constant>::type
  operator/(Lhs const & lhs, aux::arg_with_flags<Arg> const & rhs)
  {
    if(aux::has_arg_types<ConstantInt>(lhs, rhs))
    {
      SPRITE_ALLOW_FLAGS(rhs, "integer division"
        , operator_flags::SIGNED | operator_flags::UNSIGNED | operator_flags::EXACT
        )
      check_for_exactly_one_signed_flag(rhs.flags(), "integer division");

      if(rhs.flags().signed_())
      {
        return constant(SPRITE_APICALL(
            ConstantExpr::getSDiv(
                ptr(lhs), ptr(rhs), rhs.flags().exact()
              )
          ));
      }
      else
      {
        return constant(SPRITE_APICALL(
            ConstantExpr::getUDiv(
                ptr(lhs), ptr(rhs), rhs.flags().exact()
              )
          ));
      }
    }
    else if(aux::has_arg_types<ConstantFP>(lhs, rhs))
    {
      SPRITE_ALLOW_FLAGS(rhs, "floating-point division", operator_flags::SIGNED)
      return constant(SPRITE_APICALL(
          ConstantExpr::getFDiv(ptr(lhs), ptr(rhs))
        ));
    }
    throw type_error("Expected ConstantInt or ConstantFP for division.");
  }

  /**
   * @brief Integer or floating-point division.
   *
   * @snippet constexprs.cpp div
   */
  template<typename Lhs, typename Rhs>
  inline typename std::enable_if<
      is_constarg<Lhs>::value && is_constarg<Rhs>::value, constant
    >::type
  operator/(Lhs const & lhs, Rhs const & rhs)
  {
    constant lhs_ = aux::getlhs(lhs, rhs);
    constant rhs_ = aux::getrhs(lhs, rhs);
    return lhs_ /aux::operator_flags() (rhs_);
  }

  /**
   * @brief Integer or floating-point remainder.
   *
   * @snippet constexprs.cpp rem
   */
  template<typename Lhs, typename Arg>
  typename std::enable_if<is_constarg<Lhs>::value, constant>::type
  operator%(Lhs const & lhs, aux::arg_with_flags<Arg> const & rhs)
  {
    if(aux::has_arg_types<ConstantInt>(lhs, rhs))
    {
      SPRITE_ALLOW_FLAGS(rhs, "integer remainder"
        , operator_flags::SIGNED | operator_flags::UNSIGNED
        )
      check_for_exactly_one_signed_flag(rhs.flags(), "integer remainder");

      if(rhs.flags().signed_())
      {
        return constant(SPRITE_APICALL(
            ConstantExpr::getSRem(ptr(lhs), ptr(rhs))
          ));
      }
      else
      {
        return constant(SPRITE_APICALL(
            ConstantExpr::getURem(ptr(lhs), ptr(rhs))
          ));
      }
    }
    else if(aux::has_arg_types<ConstantFP>(lhs, rhs))
    {
      SPRITE_ALLOW_FLAGS(rhs, "floating-point remainder", operator_flags::SIGNED)
      return constant(SPRITE_APICALL(
          ConstantExpr::getFRem(ptr(lhs), ptr(rhs))
        ));
    }
    throw type_error("Expected ConstantInt or ConstantFP for remainder.");
  }

  /**
   * @brief Integer or floating-point remainder.
   *
   * @snippet constexprs.cpp rem
   */
  template<typename Lhs, typename Rhs>
  inline typename std::enable_if<
      is_constarg<Lhs>::value && is_constarg<Rhs>::value, constant
    >::type
  operator%(Lhs const & lhs, Rhs const & rhs)
  {
    constant lhs_ = aux::getlhs(lhs, rhs);
    constant rhs_ = aux::getrhs(lhs, rhs);
    return lhs_ %aux::operator_flags() (rhs_);
  }

  /**
   * @brief Bitwise AND.
   *
   * @snippet constexprs.cpp and
   */
  template<typename Lhs, typename Rhs>
  inline typename std::enable_if<
      is_constarg<Lhs>::value && is_constarg<Rhs>::value, constant
    >::type
  operator&(Lhs const & lhs, Rhs const & rhs)
  {
    constant lhs_ = aux::getlhs(lhs, rhs);
    constant rhs_ = aux::getrhs(lhs, rhs);

    if(aux::has_arg_types<ConstantInt>(lhs_, rhs_))
    {
      return constant(SPRITE_APICALL(
          ConstantExpr::getAnd(ptr(lhs_), ptr(rhs_))
        ));
    }
    throw type_error("Expected ConstantInt for bitwise AND.");
  }

  /**
   * @brief Bitwise OR.
   *
   * @snippet constexprs.cpp or
   */
  template<typename Lhs, typename Rhs>
  inline typename std::enable_if<
      is_constarg<Lhs>::value && is_constarg<Rhs>::value, constant
    >::type
  operator|(Lhs const & lhs, Rhs const & rhs)
  {
    constant lhs_ = aux::getlhs(lhs, rhs);
    constant rhs_ = aux::getrhs(lhs, rhs);

    if(aux::has_arg_types<ConstantInt>(lhs_, rhs_))
    {
      return constant(SPRITE_APICALL(
          ConstantExpr::getOr(ptr(lhs_), ptr(rhs_))
        ));
    }
    throw type_error("Expected ConstantInt for bitwise OR.");
  }

  /**
   * @brief Bitwise XOR.
   *
   * @snippet constexprs.cpp or
   */
  template<typename Lhs, typename Rhs>
  inline typename std::enable_if<
      is_constarg<Lhs>::value && is_constarg<Rhs>::value, constant
    >::type
  operator^(Lhs const & lhs, Rhs const & rhs)
  {
    constant lhs_ = aux::getlhs(lhs, rhs);
    constant rhs_ = aux::getrhs(lhs, rhs);

    if(aux::has_arg_types<ConstantInt>(lhs_, rhs_))
    {
      return constant(SPRITE_APICALL(
          ConstantExpr::getXor(ptr(lhs_), ptr(rhs_))
        ));
    }
    throw type_error("Expected ConstantInt for bitwise XOR.");
  }

  /**
   * @brief Left shift.
   *
   * @snippet constexprs.cpp shl
   */
  template<typename Lhs, typename Arg>
  inline typename std::enable_if<is_constarg<Lhs>::value, constant>::type
  operator<<(Lhs const & lhs, aux::arg_with_flags<Arg> const & rhs)
  {
    if(aux::has_arg_types<ConstantInt>(lhs, rhs))
    {
      SPRITE_ALLOW_FLAGS(rhs, "left shift", operator_flags::NUW | operator_flags::NSW)
      return constant(SPRITE_APICALL(
          ConstantExpr::getShl(
              ptr(lhs), ptr(rhs), rhs.flags().nuw(), rhs.flags().nsw()
            )
        ));
    }
    throw type_error("Expected ConstantInt for left shift.");
  }

  /**
   * @brief Left shift.
   *
   * @snippet constexprs.cpp shl
   */
  template<typename Lhs, typename Rhs>
  inline typename std::enable_if<
      is_constarg<Lhs>::value && is_constarg<Rhs>::value, constant
    >::type
  operator<<(Lhs const & lhs, Rhs const & rhs)
  {
    constant lhs_ = aux::getlhs(lhs, rhs);
    constant rhs_ = aux::getrhs(lhs, rhs);
    return lhs_ <<aux::operator_flags() (rhs_);
  }

  /**
   * @brief Right shift.
   *
   * @snippet constexprs.cpp shr
   */
  template<typename Lhs, typename Arg>
  inline typename std::enable_if<is_constarg<Lhs>::value, constant>::type
  operator>>(Lhs const & lhs, aux::arg_with_flags<Arg> const & rhs)
  {
    if(aux::has_arg_types<ConstantInt>(lhs, rhs))
    {
      SPRITE_ALLOW_FLAGS(rhs, "right shift"
        , operator_flags::EXACT | operator_flags::ARITHMETIC | operator_flags::LOGICAL
        );
      check_for_exactly_one_arithmetic_flag(rhs.flags(), "right shift");

      if(rhs.flags().arithmetic())
      {
        return constant(SPRITE_APICALL(
            ConstantExpr::getAShr(
                ptr(lhs), ptr(rhs), rhs.flags().exact()
              )
          ));
      }
      else
      {
        return constant(SPRITE_APICALL(
            ConstantExpr::getLShr(
                ptr(lhs), ptr(rhs), rhs.flags().exact()
              )
          ));
      }
    }
    throw type_error("Expected ConstantInt for right shift.");
  }

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

