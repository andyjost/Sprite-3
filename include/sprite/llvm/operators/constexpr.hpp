/**
 * @file
 * @brief Implements constant expression operators.
 */

#pragma once
#include "sprite/llvm/core/wrappers.hpp"
#include "sprite/llvm/operators/preamble.hpp"
#include "sprite/llvm/support/enablers.hpp"
#include "sprite/llvm/support/miscellaneous.hpp"

/**
 * @brief Checks thet only the expected flags are set.
 *
 * The first argument is an instance of @p ArgWithFlags.
 */
#define SPRITE_ALLOW_FLAGS(arg, what, allowed)                  \
    {                                                           \
      using namespace aux;                                      \
      if(arg.flags().value & ~(allowed))                        \
        { throw ParameterError("Unexpected flags for " what); } \
    }                                                           \
  /**/

namespace sprite { namespace llvm
{
  namespace aux
  {
    struct Flags;

    template<typename Arg>
    struct ArgWithFlags : std::tuple<Arg, Flags>
    {
      using std::tuple<Arg, Flags>::tuple;
      ArgWithFlags(Arg const & arg) : std::tuple<Arg, Flags>(arg, Flags()) {}
      Arg const & arg() const { return std::get<0>(*this); }
      Flags const & flags() const { return std::get<1>(*this); }
    };

    struct Flags
    {
      enum Values { NUW = 1, NSW = 2, EXACT = 4, SIGNED = 8, UNSIGNED = 16
        , ARITHMETIC = 32, LOGICAL = 64
        };
      int value;

      Flags(int v=0) : value(v) {}

      bool nuw() const { return value & NUW; }
      bool nsw() const { return value & NSW; }
      bool exact() const { return value & EXACT; }
      bool signed_() const { return value & SIGNED; }
      bool unsigned_() const { return value & UNSIGNED; }
      bool arithmetic() const { return value & ARITHMETIC; }
      bool logical() const { return value & LOGICAL; }

      friend Flags operator,(Flags const & lhs, Flags const & rhs)
      {
        Flags tmp;
        tmp.value = lhs.value | rhs.value;
        return tmp;
      }

      template<typename T>
      ArgWithFlags<ConstantWrapper<T>> operator()(ConstantWrapper<T> const & arg) const
        { return std::make_tuple(arg, *this); }
    };

    inline void check_for_exactly_one_signed_flag(
        Flags const & flags, StringRef const & what
      )
    {
      if(flags.signed_() && flags.unsigned_())
      {
        throw ParameterError(
            "Got both signed_ and unsigned_ flags for " + what
          );
      }

      if(!flags.signed_() && !flags.unsigned_())
      {
        throw ParameterError(
            "Need the signed_ or unsigned_ flag for " + what
          );
      }
    }

    inline void check_for_exactly_one_arithmetic_flag(
        Flags const & flags, StringRef const & what
      )
    {
      if(flags.arithmetic() && flags.logical())
      {
        throw ParameterError(
            "Got both arithmetic and logical flags for " + what
          );
      }

      if(!flags.arithmetic() && !flags.logical())
      {
        throw ParameterError(
            "Need the arithmetic or logical flag for " + what
          );
      }
    }

    template<typename Type, typename Arg>
    inline bool has_arg_types(Arg const & arg)
      { return llvm_::dyn_cast<Type>(ptr(arg)); }

    template<typename Type, typename Lhs, typename Rhs>
    inline bool has_arg_types(Lhs const & lhs, Rhs const & rhs)
      { return has_arg_types<Type>(lhs) && has_arg_types<Type>(rhs); }
  }

  aux::Flags const nuw = aux::Flags(aux::Flags::NUW);
  aux::Flags const nsw = aux::Flags(aux::Flags::NSW);
  aux::Flags const exact = aux::Flags(aux::Flags::EXACT);
  aux::Flags const signed_ = aux::Flags(aux::Flags::SIGNED);
  aux::Flags const unsigned_ = aux::Flags(aux::Flags::UNSIGNED);
  aux::Flags const arithmetic = aux::Flags(aux::Flags::ARITHMETIC);
  aux::Flags const logical = aux::Flags(aux::Flags::LOGICAL);

  /**
   * @brief Computes the alignment of a type.
   *
   * @snippet constexprs.cpp alignof_
   */
  inline ConstantWrapper<Constant>
  alignof_(TypeWrapper<Type> const & tp)
    { return wrap(tp.factory(), llvm_::ConstantExpr::getAlignOf(tp.ptr())); }

  // No wrapper for constant expression getSizeOf.  Use i64 % sizeof_(ty)
  // instead.

  /**
   * @brief Computes the offset of a field in a struct.
   *
   * @snippet constexprs.cpp offsetof_
   */
  inline ConstantWrapper<Constant>
  offsetof_(TypeWrapper<Type> const & tp, unsigned FieldNo)
  {
    auto const p = dyn_cast<StructType>(tp);
    return wrap(tp.factory(), llvm_::ConstantExpr::getOffsetOf(p.ptr(), FieldNo));
  }

  /**
   * @brief Computes the offset of a field in a struct.
   *
   * @snippet constexprs.cpp offsetof_
   */
  inline ConstantWrapper<Constant>
  offsetof_(TypeWrapper<Type> const & tp, Constant * FieldNo)
  {
    return wrap(tp.factory(), llvm_::ConstantExpr::getOffsetOf(tp.ptr(), FieldNo));
  }

  /**
   * @brief Integer or floating-point negation.
   *
   * @snippet constexprs.cpp neg
   */
  template<typename Arg>
  inline ConstantWrapper<Constant> operator-(aux::ArgWithFlags<Arg> const & c)
  {
    if(aux::has_arg_types<ConstantInt>(c))
    {
      SPRITE_ALLOW_FLAGS(c, "negation", Flags::NUW | Flags::NSW)
      return wrap(
          c.arg().factory()
        , llvm_::ConstantExpr::getNeg(
              ptr(c), c.flags().nuw(), c.flags().nsw()
            )
        );
    }
    else if(aux::has_arg_types<ConstantFP>(c))
    {
      SPRITE_ALLOW_FLAGS(c, "negation", Flags::SIGNED)
      return wrap(
          c.arg().factory(), llvm_::ConstantExpr::getFNeg(ptr(c))
        );
    }
    throw TypeError("Expected ConstantInt or ConstantFP for negation");
  }

  /**
   * @brief Integer or floating-point negation.
   *
   * @snippet constexprs.cpp neg
   */
  inline ConstantWrapper<Constant> operator-(ConstantWrapper<Constant> const & c)
    { return -aux::Flags()(c); }

  /**
   * @brief Bitwise inversion.
   *
   * @snippet constexprs.cpp inv
   */
  inline ConstantWrapper<Constant> operator~(ConstantWrapper<Constant> const & c)
  {
    if(aux::has_arg_types<ConstantInt>(c))
      return wrap(c.factory(), llvm_::ConstantExpr::getNot(ptr(c)));
    throw TypeError("Expected ConstantInt for bitwise inversion");
  }

  /**
   * @brief Unary plus.
   *
   * @snippet constexprs.cpp pos
   */
  inline ConstantWrapper<Constant> operator+(ConstantWrapper<Constant> const & c)
    { return c; }

  namespace aux
  {
    // These functions help binary operators accept any combination of Constant
    // * and ConstantWrapper, so long as at least one argument is a wrapper.
    inline ConstantWrapper<Constant>
    getlhs(ConstantWrapper<Constant> const & lhs, void *)
      { return lhs; }

    template<typename T>
    inline ConstantWrapper<Constant>
    getlhs(ConstantWrapper<Constant> const & lhs, Wrapper<T> const &)
      { return lhs; }

    template<typename T>
    inline ConstantWrapper<Constant>
    getlhs(Constant * lhs, Wrapper<T> const & rhs)
      { return wrap(rhs.factory(), lhs); }

    inline ConstantWrapper<Constant>
    getrhs(ConstantWrapper<Constant> const & lhs, Constant * rhs)
      { return wrap(lhs.factory(), rhs); }

    inline TypeWrapper<Type>
    getrhs(ConstantWrapper<Constant> const & lhs, Type * rhs)
      { return wrap(lhs.factory(), rhs); }

    inline ConstantWrapper<Constant>
    getrhs(ConstantWrapper<Constant> const &, ConstantWrapper<Constant> const & rhs)
      { return rhs; }

    inline TypeWrapper<Type>
    getrhs(ConstantWrapper<Constant> const &, TypeWrapper<Type> const & rhs)
      { return rhs; }

    inline ConstantWrapper<Constant>
    getrhs(Constant *, ConstantWrapper<Constant> const & rhs)
      { return rhs; }

    inline TypeWrapper<Type>
    getrhs(Constant *, TypeWrapper<Type> const & rhs)
      { return rhs; }
  }

  /**
   * @brief Integer or floating-point addition.
   *
   * @snippet constexprs.cpp add
   */
  template<typename Lhs, typename Arg>
  inline typename std::enable_if<
      aux::is_constarg<Lhs>(), ConstantWrapper<Constant>
    >::type
  operator+(Lhs const & lhs, aux::ArgWithFlags<Arg> const & rhs)
  {
    if(aux::has_arg_types<ConstantInt>(lhs, rhs))
    {
      SPRITE_ALLOW_FLAGS(rhs, "addition", Flags::NUW | Flags::NSW)
      return wrap(
          rhs.arg().factory()
        , llvm_::ConstantExpr::getAdd(
              ptr(lhs), ptr(rhs), rhs.flags().nuw(), rhs.flags().nsw()
            )
        );
    }
    else if(aux::has_arg_types<ConstantFP>(lhs, rhs))
    {
      SPRITE_ALLOW_FLAGS(rhs, "addition", Flags::SIGNED)
      return wrap(
          rhs.arg().factory()
        , llvm_::ConstantExpr::getFAdd(ptr(lhs), ptr(rhs))
        );
    }
    throw TypeError("Expected ConstantInt or ConstantFP for addition");
  }

  /**
   * @brief Integer or floating-point addition.
   *
   * @snippet constexprs.cpp add
   */
  template<typename Lhs, typename Rhs>
  inline typename std::enable_if<
      aux::is_constarg<Lhs>() && aux::is_constarg<Rhs>(), ConstantWrapper<Constant>
    >::type
  operator+(Lhs const & lhs, Rhs const & rhs)
  {
    ConstantWrapper<Constant> lhs_ = aux::getlhs(lhs, rhs);
    ConstantWrapper<Constant> rhs_ = aux::getrhs(lhs, rhs);
    return lhs_ +aux::Flags() (rhs_);
  }

  /**
   * @brief Integer or floating-point subtraction.
   *
   * @snippet constexprs.cpp sub
   */
  template<typename Lhs, typename Arg>
  inline typename std::enable_if<
      aux::is_constarg<Lhs>(), ConstantWrapper<Constant>
    >::type
  operator-(Lhs const & lhs, aux::ArgWithFlags<Arg> const & rhs)
  {
    if(aux::has_arg_types<ConstantInt>(lhs, rhs))
    {
      SPRITE_ALLOW_FLAGS(rhs, "subtraction", Flags::NUW | Flags::NSW)
      return wrap(
          rhs.arg().factory()
        , llvm_::ConstantExpr::getSub(
              ptr(lhs), ptr(rhs), rhs.flags().nuw(), rhs.flags().nsw()
            )
        );
    }
    else if(aux::has_arg_types<ConstantFP>(lhs, rhs))
    {
      SPRITE_ALLOW_FLAGS(rhs, "subtraction", Flags::SIGNED)
      return wrap(
          rhs.arg().factory()
        , llvm_::ConstantExpr::getFSub(ptr(lhs), ptr(rhs))
        );
    }
    throw TypeError("Expected ConstantInt or ConstantFP for subtraction");
  }

  /**
   * @brief Integer or floating-point subtraction.
   *
   * @snippet constexprs.cpp sub
   */
  template<typename Lhs, typename Rhs>
  inline typename std::enable_if<
      aux::is_constarg<Lhs>() && aux::is_constarg<Rhs>(), ConstantWrapper<Constant>
    >::type
  operator-(Lhs const & lhs, Rhs const & rhs)
  {
    ConstantWrapper<Constant> lhs_ = aux::getlhs(lhs, rhs);
    ConstantWrapper<Constant> rhs_ = aux::getrhs(lhs, rhs);
    return lhs_ -aux::Flags() (rhs_);
  }

  /**
   * @brief Integer or floating-point multiplication.
   *
   * @snippet constexprs.cpp mul
   */
  template<typename Lhs, typename Arg>
  inline typename std::enable_if<
      aux::is_constarg<Lhs>(), ConstantWrapper<Constant>
    >::type
  operator*(Lhs const & lhs, aux::ArgWithFlags<Arg> const & rhs)
  {
    if(aux::has_arg_types<ConstantInt>(lhs, rhs))
    {
      SPRITE_ALLOW_FLAGS(rhs, "multiplication", Flags::NUW | Flags::NSW)
      return wrap(
          rhs.arg().factory()
        , llvm_::ConstantExpr::getMul(
              ptr(lhs), ptr(rhs), rhs.flags().nuw(), rhs.flags().nsw()
            )
        );
    }
    else if(aux::has_arg_types<ConstantFP>(lhs, rhs))
    {
      SPRITE_ALLOW_FLAGS(rhs, "multiplication", Flags::SIGNED)
      return wrap(
          rhs.arg().factory()
        , llvm_::ConstantExpr::getFMul(ptr(lhs), ptr(rhs))
        );
    }
    throw TypeError("Expected ConstantInt or ConstantFP for multiplication");
  }

  /**
   * @brief Integer or floating-point multiplication.
   *
   * @snippet constexprs.cpp mul
   */
  template<typename Lhs, typename Rhs>
  inline typename std::enable_if<
      aux::is_constarg<Lhs>() && aux::is_constarg<Rhs>(), ConstantWrapper<Constant>
    >::type
  operator*(Lhs const & lhs, Rhs const & rhs)
  {
    ConstantWrapper<Constant> lhs_ = aux::getlhs(lhs, rhs);
    ConstantWrapper<Constant> rhs_ = aux::getrhs(lhs, rhs);
    return lhs_ *aux::Flags() (rhs_);
  }

  /**
   * @brief Integer or floating-point division.
   *
   * @snippet constexprs.cpp div
   */
  template<typename Lhs, typename Arg>
  typename std::enable_if<
      aux::is_constarg<Lhs>(), ConstantWrapper<Constant>
    >::type
  operator/(Lhs const & lhs, aux::ArgWithFlags<Arg> const & rhs)
  {
    if(aux::has_arg_types<ConstantInt>(lhs, rhs))
    {
      SPRITE_ALLOW_FLAGS(rhs, "integer division"
        , Flags::SIGNED | Flags::UNSIGNED | Flags::EXACT
        )
      check_for_exactly_one_signed_flag(rhs.flags(), "integer division");

      if(rhs.flags().signed_())
      {
        return wrap(
            rhs.arg().factory()
          , llvm_::ConstantExpr::getSDiv(
                ptr(lhs), ptr(rhs), rhs.flags().exact()
              )
          );
      }
      else
      {
        return wrap(
            rhs.arg().factory()
          , llvm_::ConstantExpr::getUDiv(
                ptr(lhs), ptr(rhs), rhs.flags().exact()
              )
          );
      }
    }
    else if(aux::has_arg_types<ConstantFP>(lhs, rhs))
    {
      SPRITE_ALLOW_FLAGS(rhs, "floating-point division", Flags::SIGNED)
      return wrap(
          rhs.arg().factory()
        , llvm_::ConstantExpr::getFDiv(ptr(lhs), ptr(rhs))
        );
    }
    throw TypeError("Expected ConstantInt or ConstantFP for division");
  }

  /**
   * @brief Integer or floating-point division.
   *
   * @snippet constexprs.cpp div
   */
  template<typename Lhs, typename Rhs>
  inline typename std::enable_if<
      aux::is_constarg<Lhs>() && aux::is_constarg<Rhs>(), ConstantWrapper<Constant>
    >::type
  operator/(Lhs const & lhs, Rhs const & rhs)
  {
    ConstantWrapper<Constant> lhs_ = aux::getlhs(lhs, rhs);
    ConstantWrapper<Constant> rhs_ = aux::getrhs(lhs, rhs);
    return lhs_ /aux::Flags() (rhs_);
  }

  /**
   * @brief Integer or floating-point remainder.
   *
   * @snippet constexprs.cpp rem
   */
  template<typename Lhs, typename Arg>
  typename std::enable_if<
      aux::is_constarg<Lhs>(), ConstantWrapper<Constant>
    >::type
  operator%(Lhs const & lhs, aux::ArgWithFlags<Arg> const & rhs)
  {
    if(aux::has_arg_types<ConstantInt>(lhs, rhs))
    {
      SPRITE_ALLOW_FLAGS(rhs, "integer remainder"
        , Flags::SIGNED | Flags::UNSIGNED
        )
      check_for_exactly_one_signed_flag(rhs.flags(), "integer remainder");

      if(rhs.flags().signed_())
      {
        return wrap(
            rhs.arg().factory()
          , llvm_::ConstantExpr::getSRem(ptr(lhs), ptr(rhs))
          );
      }
      else
      {
        return wrap(
            rhs.arg().factory()
          , llvm_::ConstantExpr::getURem(ptr(lhs), ptr(rhs))
          );
      }
    }
    else if(aux::has_arg_types<ConstantFP>(lhs, rhs))
    {
      SPRITE_ALLOW_FLAGS(rhs, "floating-point remainder", Flags::SIGNED)
      return wrap(
          rhs.arg().factory()
        , llvm_::ConstantExpr::getFRem(ptr(lhs), ptr(rhs))
        );
    }
    throw TypeError("Expected ConstantInt or ConstantFP for remainder");
  }

  /**
   * @brief Integer or floating-point remainder.
   *
   * @snippet constexprs.cpp rem
   */
  template<typename Lhs, typename Rhs>
  inline typename std::enable_if<
      aux::is_constarg<Lhs>() && aux::is_constarg<Rhs>(), ConstantWrapper<Constant>
    >::type
  operator%(Lhs const & lhs, Rhs const & rhs)
  {
    ConstantWrapper<Constant> lhs_ = aux::getlhs(lhs, rhs);
    ConstantWrapper<Constant> rhs_ = aux::getrhs(lhs, rhs);
    return lhs_ %aux::Flags() (rhs_);
  }

  /**
   * @brief Bitwise AND.
   *
   * @snippet constexprs.cpp and
   */
  template<typename Lhs, typename Rhs>
  inline typename std::enable_if<
      aux::is_constarg<Lhs>() && aux::is_constarg<Rhs>(), ConstantWrapper<Constant>
    >::type
  operator&(Lhs const & lhs, Rhs const & rhs)
  {
    ConstantWrapper<Constant> lhs_ = aux::getlhs(lhs, rhs);
    ConstantWrapper<Constant> rhs_ = aux::getrhs(lhs, rhs);

    if(aux::has_arg_types<ConstantInt>(lhs_, rhs_))
    {
      return wrap(
          rhs_.factory()
        , llvm_::ConstantExpr::getAnd(ptr(lhs_), ptr(rhs_))
        );
    }
    throw TypeError("Expected ConstantInt for bitwise AND");
  }

  /**
   * @brief Bitwise OR.
   *
   * @snippet constexprs.cpp or
   */
  template<typename Lhs, typename Rhs>
  inline typename std::enable_if<
      aux::is_constarg<Lhs>() && aux::is_constarg<Rhs>(), ConstantWrapper<Constant>
    >::type
  operator|(Lhs const & lhs, Rhs const & rhs)
  {
    ConstantWrapper<Constant> lhs_ = aux::getlhs(lhs, rhs);
    ConstantWrapper<Constant> rhs_ = aux::getrhs(lhs, rhs);

    if(aux::has_arg_types<ConstantInt>(lhs_, rhs_))
    {
      return wrap(
          rhs_.factory()
        , llvm_::ConstantExpr::getOr(ptr(lhs_), ptr(rhs_))
        );
    }
    throw TypeError("Expected ConstantInt for bitwise OR");
  }

  /**
   * @brief Bitwise XOR.
   *
   * @snippet constexprs.cpp or
   */
  template<typename Lhs, typename Rhs>
  inline typename std::enable_if<
      aux::is_constarg<Lhs>() && aux::is_constarg<Rhs>(), ConstantWrapper<Constant>
    >::type
  operator^(Lhs const & lhs, Rhs const & rhs)
  {
    ConstantWrapper<Constant> lhs_ = aux::getlhs(lhs, rhs);
    ConstantWrapper<Constant> rhs_ = aux::getrhs(lhs, rhs);

    if(aux::has_arg_types<ConstantInt>(lhs_, rhs_))
    {
      return wrap(
          rhs_.factory()
        , llvm_::ConstantExpr::getXor(ptr(lhs_), ptr(rhs_))
        );
    }
    throw TypeError("Expected ConstantInt for bitwise XOR");
  }

  /**
   * @brief Left shift.
   *
   * @snippet constexprs.cpp shl
   */
  template<typename Lhs, typename Arg>
  inline typename std::enable_if<
      aux::is_constarg<Lhs>(), ConstantWrapper<Constant>
    >::type
  operator<<(Lhs const & lhs, aux::ArgWithFlags<Arg> const & rhs)
  {
    if(aux::has_arg_types<ConstantInt>(lhs, rhs))
    {
      SPRITE_ALLOW_FLAGS(rhs, "left shift", Flags::NUW | Flags::NSW)
      return wrap(
          rhs.arg().factory()
        , llvm_::ConstantExpr::getShl(
              ptr(lhs), ptr(rhs), rhs.flags().nuw(), rhs.flags().nsw()
            )
        );
    }
    throw TypeError("Expected ConstantInt for left shift");
  }

  /**
   * @brief Left shift.
   *
   * @snippet constexprs.cpp shl
   */
  template<typename Lhs, typename Rhs>
  inline typename std::enable_if<
      aux::is_constarg<Lhs>() && aux::is_constarg<Rhs>(), ConstantWrapper<Constant>
    >::type
  operator<<(Lhs const & lhs, Rhs const & rhs)
  {
    ConstantWrapper<Constant> lhs_ = aux::getlhs(lhs, rhs);
    ConstantWrapper<Constant> rhs_ = aux::getrhs(lhs, rhs);
    return lhs_ <<aux::Flags() (rhs_);
  }

  /**
   * @brief Right shift.
   *
   * @snippet constexprs.cpp shr
   */
  template<typename Lhs, typename Arg>
  inline typename std::enable_if<
      aux::is_constarg<Lhs>(), ConstantWrapper<Constant>
    >::type
  operator>>(Lhs const & lhs, aux::ArgWithFlags<Arg> const & rhs)
  {
    if(aux::has_arg_types<ConstantInt>(lhs, rhs))
    {
      SPRITE_ALLOW_FLAGS(rhs, "right shift"
        , Flags::EXACT | Flags::ARITHMETIC | Flags::LOGICAL
        );
      check_for_exactly_one_arithmetic_flag(rhs.flags(), "right shift");

      if(rhs.flags().arithmetic())
      {
        return wrap(
            rhs.arg().factory()
          , llvm_::ConstantExpr::getAShr(
                ptr(lhs), ptr(rhs), rhs.flags().exact()
              )
          );
      }
      else
      {
        return wrap(
            rhs.arg().factory()
          , llvm_::ConstantExpr::getLShr(
                ptr(lhs), ptr(rhs), rhs.flags().exact()
              )
          );
      }
    }
    throw TypeError("Expected ConstantInt for right shift");
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
  typename std::enable_if<aux::is_typearg<Rhs>(), ConstantWrapper<Constant>>::type
  typecast(aux::ArgWithFlags<Arg> const & lhs, Rhs const & rhs)
  {
    Type * const type = ptr(rhs);
    assert(type);

    if(ConstantInt * const lhs_ = llvm_::dyn_cast<ConstantInt>(ptr(lhs)))
    {
      if(type->getScalarType()->isIntegerTy())
      {
        unsigned const lhsz = lhs_->getBitWidth();
        unsigned const rhsz = cast<IntegerType>(type)->getBitWidth();
        if(lhsz < rhsz)
        {
          SPRITE_ALLOW_FLAGS(
              lhs, "integer extension", Flags::SIGNED | Flags::UNSIGNED
            )
          check_for_exactly_one_signed_flag(lhs.flags(), "integer extension");

          if(lhs.flags().signed_())
          {
            return wrap(
                lhs.arg().factory()
              , llvm_::ConstantExpr::getSExt(ptr(lhs), type)
              );
          }
          else
          {
            return wrap(
                lhs.arg().factory()
              , llvm_::ConstantExpr::getZExt(ptr(lhs), type)
              );
          }
        }
        else if(rhsz < lhsz)
        {
          SPRITE_ALLOW_FLAGS(lhs, "integer truncation", 0)
          return wrap(
              lhs.arg().factory()
            , llvm_::ConstantExpr::getTrunc(ptr(lhs), type)
            );
        }
        return lhs.arg(); // no-op
      }
      else if(type->getScalarType()->isFloatingPointTy())
      {
        SPRITE_ALLOW_FLAGS(
            lhs, "integer-to-floating-point conversion"
          , Flags::SIGNED | Flags::UNSIGNED
          )
        check_for_exactly_one_signed_flag(
            lhs.flags(), "integer-to-floating-point conversion"
          );

        if(lhs.flags().signed_())
        {
          return wrap(
              lhs.arg().factory()
            , llvm_::ConstantExpr::getSIToFP(ptr(lhs), type)
            );
        }
        else
        {
          return wrap(
              lhs.arg().factory()
            , llvm_::ConstantExpr::getUIToFP(ptr(lhs), type)
            );
        }
      }
      else if(type->getScalarType()->isPointerTy())
      {
        SPRITE_ALLOW_FLAGS(lhs, "integer-to-pointer conversion", 0)
        return wrap(
            lhs.arg().factory()
          , llvm_::ConstantExpr::getIntToPtr(ptr(lhs), type)
          );
      }
      throw TypeError(
          "Expected integer, floating-point or pointer type as the target of "
          "typecast from an integer type"
        );
    }
    else if(ConstantFP * const lhs_ = llvm_::dyn_cast<ConstantFP>(ptr(lhs)))
    {
      if(type->getScalarType()->isIntegerTy())
      {
        SPRITE_ALLOW_FLAGS(
            lhs, "floating-point-to-integer conversion"
          , Flags::SIGNED | Flags::UNSIGNED
          )
        check_for_exactly_one_signed_flag(
            lhs.flags(), "floating-point-to-integer conversion"
          );

        if(lhs.flags().signed_())
        {
          return wrap(
              lhs.arg().factory()
            , llvm_::ConstantExpr::getFPToSI(ptr(lhs), type)
            );
        }
        else
        {
          return wrap(
              lhs.arg().factory()
            , llvm_::ConstantExpr::getFPToUI(ptr(lhs), type)
            );
        }
      }
      else if(type->getScalarType()->isFloatingPointTy())
      {
        unsigned const lhsz = getFPBitWidth(lhs_->getType());
        unsigned const rhsz = getFPBitWidth(type);
        if(lhsz < rhsz)
        {
          SPRITE_ALLOW_FLAGS(lhs, "floating-point extension", Flags::SIGNED)
          return wrap(
              lhs.arg().factory()
            , llvm_::ConstantExpr::getFPExtend(ptr(lhs), type)
            );
        }
        else if(rhsz < lhsz)
        {
          SPRITE_ALLOW_FLAGS(lhs, "floating-point truncation", Flags::SIGNED)
          return wrap(
              lhs.arg().factory()
            , llvm_::ConstantExpr::getFPTrunc(ptr(lhs), type)
            );
        }
        return lhs.arg(); // no-op
      }
      throw TypeError(
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
          , llvm_::ConstantExpr::getPtrToInt(ptr(lhs), type)
          );
      }
      throw TypeError(
          "Expected integer type as the target of typecast from integer"
        );
    }
    throw TypeError(
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
      aux::is_constarg<Lhs>() && aux::is_typearg<Rhs>(), ConstantWrapper<Constant>
    >::type
  typecast(Lhs const & lhs, Rhs const & rhs)
  {
    ConstantWrapper<Constant> lhs_ = aux::getlhs(lhs, rhs);
    TypeWrapper<Type> rhs_ = aux::getrhs(lhs, rhs);
    return typecast(aux::Flags()(lhs_), rhs_);
  }

  /**
   * @brief Performs a bit-casting operation.
   *
   * @snippet constexprs.cpp bitcast
   */
  template<typename Lhs, typename Rhs>
  inline typename std::enable_if<
      aux::is_constarg<Lhs>() && aux::is_typearg<Rhs>(), ConstantWrapper<Constant>
    >::type
  bitcast(Lhs const & lhs, Rhs const & rhs)
  {
    ConstantWrapper<Constant> lhs_ = aux::getlhs(lhs, rhs);
    return wrap(
        lhs_.factory()
      , llvm_::ConstantExpr::getBitCast(ptr(lhs_), ptr(rhs))
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
    , ConstantWrapper<Constant>
    >::type
  select(If const & if_, Then const & then, Else const & else_)
  {
    ConstantWrapper<Constant> if__ = aux::getlhs(aux::getlhs(if_, then), else_);
    return wrap(
        if__.factory()
      , llvm_::ConstantExpr::getSelect(ptr(if_), ptr(then), ptr(else_))
      );
  }

  template<typename T, typename Factory>
  inline ConstantWrapper<Constant, Factory>
  GlobalValueProxy<T, Factory>::operator&() const
  {
    return wrap(
        this->base.factory()
      , llvm_::ConstantExpr::getGetElementPtr(
            ptr(this->base), this->indices
          )
      );
  }

  template<typename T, typename Factory>
  inline ConstantWrapper<Constant, Factory>
  address_inbounds(GlobalValueProxy<T, Factory> const & gvp)
  {
    return wrap(
        gvp.get_base().factory()
      , llvm_::ConstantExpr::getInBoundsGetElementPtr(
            ptr(gvp.get_base()), gvp.get_indices()
          )
      );
  }

  template<typename T, typename Factory>
  template<typename Index>
  inline
  typename std::enable_if<
      aux::is_constarg<Index>(), GlobalValueProxy<T, Factory>
    >::type
  GlobalValueProxy<T, Factory>::operator[](Index const & i) const
  {
    GlobalValueProxy tmp = *this;
    tmp.indices.push_back(ptr(i));
    return std::move(tmp);
  }

  template<typename T, typename Factory>
  inline GlobalValueProxy<T, Factory>
  GlobalValueProxy<T, Factory>::operator[](int64_t i) const
  {
    GlobalValueProxy<T, Factory> tmp = *this;
    auto const i64 = base.factory().int_(64);
    tmp.indices.push_back(ptr(i64 % i));
    return std::move(tmp);
  }

  template<typename T, typename Factory>
  inline ConstantWrapper<Constant, Factory>
  GlobalValueWrapper<T, Factory>::operator&() const
  {
    auto const i64 = this->factory().int_(64);
    return wrap(
        this->factory()
      , llvm_::ConstantExpr::getGetElementPtr(this->ptr(), ptr(i64 % 0))
      );
  }

  template<typename T, typename Factory>
  template<typename Index>
  inline
  typename std::enable_if<
      aux::is_constarg<Index>(), GlobalValueProxy<T, Factory>
    >::type
  GlobalValueWrapper<T, Factory>::operator[](Index const & i) const
  {
    auto const i64 = this->factory().int_(64);
    GlobalValueProxy<T, Factory> tmp(*this);
    tmp.indices.push_back(ptr(i64 % 0));
    tmp.indices.push_back(ptr(i));
    return std::move(tmp);
  }

  template<typename T, typename Factory>
  inline GlobalValueProxy<T, Factory>
  GlobalValueWrapper<T, Factory>::operator[](int64_t i) const
  {
    auto const i64 = this->factory().int_(64);
    GlobalValueProxy<T, Factory> tmp(*this);
    tmp.indices.push_back(ptr(i64 % 0));
    tmp.indices.push_back(ptr(i64 % i));
    return std::move(tmp);
  }
}}

