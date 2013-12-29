/**
 * @file
 * @brief Implements constant expression operators.
 */

#pragma once
#include "sprite/backend/core/wrappers.hpp"
#include "sprite/backend/operators/preamble.hpp"
#include "sprite/backend/support/enablers.hpp"
#include "sprite/backend/support/miscellaneous.hpp"

/**
 * @brief Checks thet only the expected flags are set.
 *
 * The first argument is an instance of @p arg_with_flags.
 */
#define SPRITE_ALLOW_FLAGS(arg, what, allowed)                  \
    {                                                           \
      using namespace aux;                                      \
      if(arg.flags().value & ~(allowed))                        \
        { throw parameter_error("Unexpected flags for " what); } \
    }                                                           \
  /**/

namespace sprite { namespace backend
{
  namespace aux
  {
    struct operator_flags;

    template<typename Arg>
    struct arg_with_flags : std::tuple<Arg, operator_flags>
    {
      using std::tuple<Arg, operator_flags>::tuple;
      arg_with_flags(Arg const & arg) : std::tuple<Arg, operator_flags>(arg, operator_flags()) {}
      Arg const & arg() const { return std::get<0>(*this); }
      operator_flags const & flags() const { return std::get<1>(*this); }
    };

    struct operator_flags
    {
      enum Values { NUW = 1, NSW = 2, EXACT = 4, SIGNED = 8, UNSIGNED = 16
        , ARITHMETIC = 32, LOGICAL = 64
        };
      int value;

      operator_flags(int v=0) : value(v) {}

      bool nuw() const { return value & NUW; }
      bool nsw() const { return value & NSW; }
      bool exact() const { return value & EXACT; }
      bool signed_() const { return value & SIGNED; }
      bool unsigned_() const { return value & UNSIGNED; }
      bool arithmetic() const { return value & ARITHMETIC; }
      bool logical() const { return value & LOGICAL; }

      friend operator_flags operator,(operator_flags const & lhs, operator_flags const & rhs)
      {
        operator_flags tmp;
        tmp.value = lhs.value | rhs.value;
        return tmp;
      }

      template<typename T>
      arg_with_flags<constantobj<T>> operator()(constantobj<T> const & arg) const
        { return std::make_tuple(arg, *this); }
    };

    inline void check_for_exactly_one_signed_flag(
        operator_flags const & flags, string_ref const & what
      )
    {
      if(flags.signed_() && flags.unsigned_())
      {
        throw parameter_error(
            "Got both signed_ and unsigned_ flags for " + what
          );
      }

      if(!flags.signed_() && !flags.unsigned_())
      {
        throw parameter_error(
            "Need the signed_ or unsigned_ flag for " + what
          );
      }
    }

    inline void check_for_exactly_one_arithmetic_flag(
        operator_flags const & flags, string_ref const & what
      )
    {
      if(flags.arithmetic() && flags.logical())
      {
        throw parameter_error(
            "Got both arithmetic and logical flags for " + what
          );
      }

      if(!flags.arithmetic() && !flags.logical())
      {
        throw parameter_error(
            "Need the arithmetic or logical flag for " + what
          );
      }
    }

    template<typename Type, typename Arg>
    inline bool has_arg_types(Arg const & arg)
      { return llvm::dyn_cast<Type>(ptr(arg)); }

    template<typename Type, typename Lhs, typename Rhs>
    inline bool has_arg_types(Lhs const & lhs, Rhs const & rhs)
      { return has_arg_types<Type>(lhs) && has_arg_types<Type>(rhs); }
  }

  aux::operator_flags const nuw = aux::operator_flags(aux::operator_flags::NUW);
  aux::operator_flags const nsw = aux::operator_flags(aux::operator_flags::NSW);
  aux::operator_flags const exact = aux::operator_flags(aux::operator_flags::EXACT);
  aux::operator_flags const signed_ = aux::operator_flags(aux::operator_flags::SIGNED);
  aux::operator_flags const unsigned_ = aux::operator_flags(aux::operator_flags::UNSIGNED);
  aux::operator_flags const arithmetic = aux::operator_flags(aux::operator_flags::ARITHMETIC);
  aux::operator_flags const logical = aux::operator_flags(aux::operator_flags::LOGICAL);

  /**
   * @brief Computes the alignment of a type.
   *
   * @snippet constexprs.cpp alignof_
   */
  inline constant alignof_(type const & tp)
    { return wrap(tp.factory(), ConstantExpr::getAlignOf(tp.ptr())); }

  // No wrapper for constant expression getSizeOf.  Use i64 % sizeof_(ty)
  // instead.

  /**
   * @brief Computes the offset of a field in a struct.
   *
   * @snippet constexprs.cpp offsetof_
   */
  inline constant offsetof_(type const & tp, unsigned FieldNo)
  {
    auto const p = dyn_cast<StructType>(tp);
    return wrap(tp.factory(), ConstantExpr::getOffsetOf(p.ptr(), FieldNo));
  }

  /**
   * @brief Computes the offset of a field in a struct.
   *
   * @snippet constexprs.cpp offsetof_
   */
  inline constant offsetof_(type const & tp, Constant * FieldNo)
   { return wrap(tp.factory(), ConstantExpr::getOffsetOf(tp.ptr(), FieldNo)); }

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
      return wrap(
          c.arg().factory()
        , ConstantExpr::getNeg(
              ptr(c), c.flags().nuw(), c.flags().nsw()
            )
        );
    }
    else if(aux::has_arg_types<ConstantFP>(c))
    {
      SPRITE_ALLOW_FLAGS(c, "negation", operator_flags::SIGNED)
      return wrap(
          c.arg().factory(), ConstantExpr::getFNeg(ptr(c))
        );
    }
    throw type_error("Expected ConstantInt or ConstantFP for negation");
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
      return wrap(c.factory(), ConstantExpr::getNot(ptr(c)));
    throw type_error("Expected ConstantInt for bitwise inversion");
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
    inline constant getlhs(Constant * lhs, object<T> const & rhs)
      { return wrap(rhs.factory(), lhs); }

    inline constant getrhs(constant const & lhs, Constant * rhs)
      { return wrap(lhs.factory(), rhs); }

    inline type
    getrhs(constant const & lhs, Type * rhs)
      { return wrap(lhs.factory(), rhs); }

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

  /**
   * @brief Integer or floating-point addition.
   *
   * @snippet constexprs.cpp add
   */
  template<typename Lhs, typename Arg>
  inline typename std::enable_if<
      aux::is_constarg<Lhs>(), constant
    >::type
  operator+(Lhs const & lhs, aux::arg_with_flags<Arg> const & rhs)
  {
    if(aux::has_arg_types<ConstantInt>(lhs, rhs))
    {
      SPRITE_ALLOW_FLAGS(rhs, "addition", operator_flags::NUW | operator_flags::NSW)
      return wrap(
          rhs.arg().factory()
        , ConstantExpr::getAdd(
              ptr(lhs), ptr(rhs), rhs.flags().nuw(), rhs.flags().nsw()
            )
        );
    }
    else if(aux::has_arg_types<ConstantFP>(lhs, rhs))
    {
      SPRITE_ALLOW_FLAGS(rhs, "addition", operator_flags::SIGNED)
      return wrap(
          rhs.arg().factory()
        , ConstantExpr::getFAdd(ptr(lhs), ptr(rhs))
        );
    }
    throw type_error("Expected ConstantInt or ConstantFP for addition");
  }

  /**
   * @brief Integer or floating-point addition.
   *
   * @snippet constexprs.cpp add
   */
  template<typename Lhs, typename Rhs>
  inline typename std::enable_if<
      aux::is_constarg<Lhs>() && aux::is_constarg<Rhs>(), constant
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
  inline typename std::enable_if<aux::is_constarg<Lhs>(), constant>::type
  operator-(Lhs const & lhs, aux::arg_with_flags<Arg> const & rhs)
  {
    if(aux::has_arg_types<ConstantInt>(lhs, rhs))
    {
      SPRITE_ALLOW_FLAGS(rhs, "subtraction", operator_flags::NUW | operator_flags::NSW)
      return wrap(
          rhs.arg().factory()
        , ConstantExpr::getSub(
              ptr(lhs), ptr(rhs), rhs.flags().nuw(), rhs.flags().nsw()
            )
        );
    }
    else if(aux::has_arg_types<ConstantFP>(lhs, rhs))
    {
      SPRITE_ALLOW_FLAGS(rhs, "subtraction", operator_flags::SIGNED)
      return wrap(
          rhs.arg().factory()
        , ConstantExpr::getFSub(ptr(lhs), ptr(rhs))
        );
    }
    throw type_error("Expected ConstantInt or ConstantFP for subtraction");
  }

  /**
   * @brief Integer or floating-point subtraction.
   *
   * @snippet constexprs.cpp sub
   */
  template<typename Lhs, typename Rhs>
  inline typename std::enable_if<
      aux::is_constarg<Lhs>() && aux::is_constarg<Rhs>(), constant
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
  inline typename std::enable_if<aux::is_constarg<Lhs>(), constant>::type
  operator*(Lhs const & lhs, aux::arg_with_flags<Arg> const & rhs)
  {
    if(aux::has_arg_types<ConstantInt>(lhs, rhs))
    {
      SPRITE_ALLOW_FLAGS(rhs, "multiplication", operator_flags::NUW | operator_flags::NSW)
      return wrap(
          rhs.arg().factory()
        , ConstantExpr::getMul(
              ptr(lhs), ptr(rhs), rhs.flags().nuw(), rhs.flags().nsw()
            )
        );
    }
    else if(aux::has_arg_types<ConstantFP>(lhs, rhs))
    {
      SPRITE_ALLOW_FLAGS(rhs, "multiplication", operator_flags::SIGNED)
      return wrap(
          rhs.arg().factory()
        , ConstantExpr::getFMul(ptr(lhs), ptr(rhs))
        );
    }
    throw type_error("Expected ConstantInt or ConstantFP for multiplication");
  }

  /**
   * @brief Integer or floating-point multiplication.
   *
   * @snippet constexprs.cpp mul
   */
  template<typename Lhs, typename Rhs>
  inline typename std::enable_if<
      aux::is_constarg<Lhs>() && aux::is_constarg<Rhs>(), constant
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
  typename std::enable_if<aux::is_constarg<Lhs>(), constant>::type
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
        return wrap(
            rhs.arg().factory()
          , ConstantExpr::getSDiv(
                ptr(lhs), ptr(rhs), rhs.flags().exact()
              )
          );
      }
      else
      {
        return wrap(
            rhs.arg().factory()
          , ConstantExpr::getUDiv(
                ptr(lhs), ptr(rhs), rhs.flags().exact()
              )
          );
      }
    }
    else if(aux::has_arg_types<ConstantFP>(lhs, rhs))
    {
      SPRITE_ALLOW_FLAGS(rhs, "floating-point division", operator_flags::SIGNED)
      return wrap(
          rhs.arg().factory()
        , ConstantExpr::getFDiv(ptr(lhs), ptr(rhs))
        );
    }
    throw type_error("Expected ConstantInt or ConstantFP for division");
  }

  /**
   * @brief Integer or floating-point division.
   *
   * @snippet constexprs.cpp div
   */
  template<typename Lhs, typename Rhs>
  inline typename std::enable_if<
      aux::is_constarg<Lhs>() && aux::is_constarg<Rhs>(), constant
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
  typename std::enable_if<aux::is_constarg<Lhs>(), constant>::type
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
        return wrap(
            rhs.arg().factory()
          , ConstantExpr::getSRem(ptr(lhs), ptr(rhs))
          );
      }
      else
      {
        return wrap(
            rhs.arg().factory()
          , ConstantExpr::getURem(ptr(lhs), ptr(rhs))
          );
      }
    }
    else if(aux::has_arg_types<ConstantFP>(lhs, rhs))
    {
      SPRITE_ALLOW_FLAGS(rhs, "floating-point remainder", operator_flags::SIGNED)
      return wrap(
          rhs.arg().factory()
        , ConstantExpr::getFRem(ptr(lhs), ptr(rhs))
        );
    }
    throw type_error("Expected ConstantInt or ConstantFP for remainder");
  }

  /**
   * @brief Integer or floating-point remainder.
   *
   * @snippet constexprs.cpp rem
   */
  template<typename Lhs, typename Rhs>
  inline typename std::enable_if<
      aux::is_constarg<Lhs>() && aux::is_constarg<Rhs>(), constant
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
      aux::is_constarg<Lhs>() && aux::is_constarg<Rhs>(), constant
    >::type
  operator&(Lhs const & lhs, Rhs const & rhs)
  {
    constant lhs_ = aux::getlhs(lhs, rhs);
    constant rhs_ = aux::getrhs(lhs, rhs);

    if(aux::has_arg_types<ConstantInt>(lhs_, rhs_))
    {
      return wrap(
          rhs_.factory()
        , ConstantExpr::getAnd(ptr(lhs_), ptr(rhs_))
        );
    }
    throw type_error("Expected ConstantInt for bitwise AND");
  }

  /**
   * @brief Bitwise OR.
   *
   * @snippet constexprs.cpp or
   */
  template<typename Lhs, typename Rhs>
  inline typename std::enable_if<
      aux::is_constarg<Lhs>() && aux::is_constarg<Rhs>(), constant
    >::type
  operator|(Lhs const & lhs, Rhs const & rhs)
  {
    constant lhs_ = aux::getlhs(lhs, rhs);
    constant rhs_ = aux::getrhs(lhs, rhs);

    if(aux::has_arg_types<ConstantInt>(lhs_, rhs_))
    {
      return wrap(
          rhs_.factory()
        , ConstantExpr::getOr(ptr(lhs_), ptr(rhs_))
        );
    }
    throw type_error("Expected ConstantInt for bitwise OR");
  }

  /**
   * @brief Bitwise XOR.
   *
   * @snippet constexprs.cpp or
   */
  template<typename Lhs, typename Rhs>
  inline typename std::enable_if<
      aux::is_constarg<Lhs>() && aux::is_constarg<Rhs>(), constant
    >::type
  operator^(Lhs const & lhs, Rhs const & rhs)
  {
    constant lhs_ = aux::getlhs(lhs, rhs);
    constant rhs_ = aux::getrhs(lhs, rhs);

    if(aux::has_arg_types<ConstantInt>(lhs_, rhs_))
    {
      return wrap(
          rhs_.factory()
        , ConstantExpr::getXor(ptr(lhs_), ptr(rhs_))
        );
    }
    throw type_error("Expected ConstantInt for bitwise XOR");
  }

  /**
   * @brief Left shift.
   *
   * @snippet constexprs.cpp shl
   */
  template<typename Lhs, typename Arg>
  inline typename std::enable_if<aux::is_constarg<Lhs>(), constant>::type
  operator<<(Lhs const & lhs, aux::arg_with_flags<Arg> const & rhs)
  {
    if(aux::has_arg_types<ConstantInt>(lhs, rhs))
    {
      SPRITE_ALLOW_FLAGS(rhs, "left shift", operator_flags::NUW | operator_flags::NSW)
      return wrap(
          rhs.arg().factory()
        , ConstantExpr::getShl(
              ptr(lhs), ptr(rhs), rhs.flags().nuw(), rhs.flags().nsw()
            )
        );
    }
    throw type_error("Expected ConstantInt for left shift");
  }

  /**
   * @brief Left shift.
   *
   * @snippet constexprs.cpp shl
   */
  template<typename Lhs, typename Rhs>
  inline typename std::enable_if<
      aux::is_constarg<Lhs>() && aux::is_constarg<Rhs>(), constant
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
  inline typename std::enable_if<aux::is_constarg<Lhs>(), constant>::type
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
        return wrap(
            rhs.arg().factory()
          , ConstantExpr::getAShr(
                ptr(lhs), ptr(rhs), rhs.flags().exact()
              )
          );
      }
      else
      {
        return wrap(
            rhs.arg().factory()
          , ConstantExpr::getLShr(
                ptr(lhs), ptr(rhs), rhs.flags().exact()
              )
          );
      }
    }
    throw type_error("Expected ConstantInt for right shift");
  }

  /**
   * @brief Performs a type-casting operation.
   *
   * Creates one of the following LLVM instructions: trunc, sext, zext,
   * fptrunc, fpextend, uitofp, sitofp, fptoui, fpsosi, ptrtoint, or inttoptr.
   *
   * @snippet constexprs.cpp typecast
   */
  template<typename Arg, typename Rhs>
  typename std::enable_if<aux::is_typearg<Rhs>(), constant>::type
  typecast(aux::arg_with_flags<Arg> const & lhs, Rhs const & rhs)
  {
    Type * const type = ptr(rhs);
    assert(type);

    if(ConstantInt * const lhs_ = llvm::dyn_cast<ConstantInt>(ptr(lhs)))
    {
      if(type->getScalarType()->isIntegerTy())
      {
        unsigned const lhsz = lhs_->getBitWidth();
        unsigned const rhsz = cast<IntegerType>(type)->getBitWidth();
        if(lhsz < rhsz)
        {
          SPRITE_ALLOW_FLAGS(
              lhs, "integer extension", operator_flags::SIGNED | operator_flags::UNSIGNED
            )
          check_for_exactly_one_signed_flag(lhs.flags(), "integer extension");

          if(lhs.flags().signed_())
          {
            return wrap(
                lhs.arg().factory()
              , ConstantExpr::getSExt(ptr(lhs), type)
              );
          }
          else
          {
            return wrap(
                lhs.arg().factory()
              , ConstantExpr::getZExt(ptr(lhs), type)
              );
          }
        }
        else if(rhsz < lhsz)
        {
          SPRITE_ALLOW_FLAGS(lhs, "integer truncation", 0)
          return wrap(
              lhs.arg().factory()
            , ConstantExpr::getTrunc(ptr(lhs), type)
            );
        }
        return lhs.arg(); // no-op
      }
      else if(type->getScalarType()->isFloatingPointTy())
      {
        SPRITE_ALLOW_FLAGS(
            lhs, "integer-to-floating-point conversion"
          , operator_flags::SIGNED | operator_flags::UNSIGNED
          )
        check_for_exactly_one_signed_flag(
            lhs.flags(), "integer-to-floating-point conversion"
          );

        if(lhs.flags().signed_())
        {
          return wrap(
              lhs.arg().factory()
            , ConstantExpr::getSIToFP(ptr(lhs), type)
            );
        }
        else
        {
          return wrap(
              lhs.arg().factory()
            , ConstantExpr::getUIToFP(ptr(lhs), type)
            );
        }
      }
      else if(type->getScalarType()->isPointerTy())
      {
        SPRITE_ALLOW_FLAGS(lhs, "integer-to-pointer conversion", 0)
        return wrap(
            lhs.arg().factory()
          , ConstantExpr::getIntToPtr(ptr(lhs), type)
          );
      }
      throw type_error(
          "Expected integer, floating-point or pointer type as the target of "
          "typecast from an integer type"
        );
    }
    else if(ConstantFP * const lhs_ = llvm::dyn_cast<ConstantFP>(ptr(lhs)))
    {
      if(type->getScalarType()->isIntegerTy())
      {
        SPRITE_ALLOW_FLAGS(
            lhs, "floating-point-to-integer conversion"
          , operator_flags::SIGNED | operator_flags::UNSIGNED
          )
        check_for_exactly_one_signed_flag(
            lhs.flags(), "floating-point-to-integer conversion"
          );

        if(lhs.flags().signed_())
        {
          return wrap(
              lhs.arg().factory()
            , ConstantExpr::getFPToSI(ptr(lhs), type)
            );
        }
        else
        {
          return wrap(
              lhs.arg().factory()
            , ConstantExpr::getFPToUI(ptr(lhs), type)
            );
        }
      }
      else if(type->getScalarType()->isFloatingPointTy())
      {
        unsigned const lhsz = getFPBitWidth(lhs_->getType());
        unsigned const rhsz = getFPBitWidth(type);
        if(lhsz < rhsz)
        {
          SPRITE_ALLOW_FLAGS(lhs, "floating-point extension", operator_flags::SIGNED)
          return wrap(
              lhs.arg().factory()
            , ConstantExpr::getFPExtend(ptr(lhs), type)
            );
        }
        else if(rhsz < lhsz)
        {
          SPRITE_ALLOW_FLAGS(lhs, "floating-point truncation", operator_flags::SIGNED)
          return wrap(
              lhs.arg().factory()
            , ConstantExpr::getFPTrunc(ptr(lhs), type)
            );
        }
        return lhs.arg(); // no-op
      }
      throw type_error(
          "Expected integer or floating-point type as the target of typecast "
          "from a floating-point type"
        );
    }
    else if(ptr(lhs)->getType()->isPointerTy())
    {
      if(type->getScalarType()->isIntegerTy())
      {
        SPRITE_ALLOW_FLAGS(lhs, "pointer-to-integer conversion", 0)
        return wrap(
            lhs.arg().factory()
          , ConstantExpr::getPtrToInt(ptr(lhs), type)
          );
      }
      throw type_error(
          "Expected integer type as the target of typecast from integer"
        );
    }
    throw type_error(
        "Expected integer, floating-point, or pointer type as the source "
        "of typecast"
      );
  }

  /**
   * @brief Performs a type-casting operation.
   *
   * Creates one of the following LLVM instructions: trunc, sext, zext,
   * fptrunc, fpextend, uitofp, sitofp, fptoui, fpsosi, ptrtoint, or inttoptr.
   *
   * @snippet constexprs.cpp typecast
   */
  template<typename Lhs, typename Rhs>
  inline typename std::enable_if<
      aux::is_constarg<Lhs>() && aux::is_typearg<Rhs>(), constant
    >::type
  typecast(Lhs const & lhs, Rhs const & rhs)
  {
    constant lhs_ = aux::getlhs(lhs, rhs);
    type rhs_ = aux::getrhs(lhs, rhs);
    return typecast(aux::operator_flags()(lhs_), rhs_);
  }

  /**
   * @brief Performs a bit-casting operation.
   *
   * @snippet constexprs.cpp bitcast
   */
  template<typename Lhs, typename Rhs>
  inline typename std::enable_if<
      aux::is_constarg<Lhs>() && aux::is_typearg<Rhs>(), constant
    >::type
  bitcast(Lhs const & lhs, Rhs const & rhs)
  {
    constant lhs_ = aux::getlhs(lhs, rhs);
    return wrap(
        lhs_.factory()
      , ConstantExpr::getBitCast(ptr(lhs_), ptr(rhs))
      );
  }

  /**
   * @brief Performs a select operation.
   *
   * @snippet constexprs.cpp select
   */
  template<typename If, typename Then, typename Else>
  inline typename std::enable_if<
      aux::is_constarg<If>() && aux::is_constarg<Then>()
        && aux::is_constarg<Else>()
    , constant
    >::type
  select(If const & if_, Then const & then, Else const & else_)
  {
    constant if__ = aux::getlhs(aux::getlhs(if_, then), else_);
    return wrap(
        if__.factory()
      , ConstantExpr::getSelect(ptr(if_), ptr(then), ptr(else_))
      );
  }

  template<typename T, typename Factory>
  inline constantobj<Constant, Factory>
  global_value_proxy<T, Factory>::operator&() const
  {
    return wrap(
        this->base.factory()
      , ConstantExpr::getGetElementPtr(
            ptr(this->base), this->indices
          )
      );
  }

  template<typename T, typename Factory>
  inline constantobj<Constant, Factory>
  address_inbounds(global_value_proxy<T, Factory> const & gvp)
  {
    return wrap(
        gvp.get_base().factory()
      , ConstantExpr::getInBoundsGetElementPtr(
            ptr(gvp.get_base()), gvp.get_indices()
          )
      );
  }

  template<typename T, typename Factory>
  template<typename Index>
  inline
  typename std::enable_if<
      aux::is_constarg<Index>(), global_value_proxy<T, Factory>
    >::type
  global_value_proxy<T, Factory>::operator[](Index const & i) const
  {
    global_value_proxy tmp = *this;
    tmp.indices.push_back(ptr(i));
    return std::move(tmp);
  }

  template<typename T, typename Factory>
  inline global_value_proxy<T, Factory>
  global_value_proxy<T, Factory>::operator[](int64_t i) const
  {
    global_value_proxy<T, Factory> tmp = *this;
    auto const i64 = base.factory().int_(64);
    tmp.indices.push_back(ptr(i64 % i));
    return std::move(tmp);
  }

  template<typename T, typename Factory>
  inline constantobj<Constant, Factory>
  globalobj<T, Factory>::operator&() const
  {
    auto const i64 = this->factory().int_(64);
    return wrap(
        this->factory()
      , ConstantExpr::getGetElementPtr(this->ptr(), ptr(i64 % 0))
      );
  }

  template<typename T, typename Factory>
  template<typename Index>
  inline
  typename std::enable_if<
      aux::is_constarg<Index>(), global_value_proxy<T, Factory>
    >::type
  globalobj<T, Factory>::operator[](Index const & i) const
  {
    auto const i64 = this->factory().int_(64);
    global_value_proxy<T, Factory> tmp(*this);
    tmp.indices.push_back(ptr(i64 % 0));
    tmp.indices.push_back(ptr(i));
    return std::move(tmp);
  }

  template<typename T, typename Factory>
  inline global_value_proxy<T, Factory>
  globalobj<T, Factory>::operator[](int64_t i) const
  {
    auto const i64 = this->factory().int_(64);
    global_value_proxy<T, Factory> tmp(*this);
    tmp.indices.push_back(ptr(i64 % 0));
    tmp.indices.push_back(ptr(i64 % i));
    return std::move(tmp);
  }
}}

