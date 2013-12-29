/**
 * @file
 * @brief Implements the modulo (%) operator.
 *
 * The modulo operator is used for type instantiation.
 */

#pragma once
#include "sprite/llvm/core/wrappers.hpp"
#include "sprite/llvm/operators/preamble.hpp"
#include "sprite/llvm/support/generic_support.hpp"
#include "sprite/llvm/support/wrap.hpp"

namespace sprite { namespace llvm
{
  namespace aux
  {
    /// Terminating case for building constant elements from a tuple.
    template<size_t I=0, typename C, typename E, typename...T>
    inline typename std::enable_if<I == sizeof...(T), void>::type
    build_uniform_constants(C &, E const &, std::tuple<T...> const &)
    {}

    /**
     * @brief Iterating case for building constant elements from a tuple.
     *
     * Used when there is a single element type, @p E, to instantiate multiple
     * times.
     *
     * Extracts initializer value @p I from tuple @p t, uses it to construct a
     * constant value of type @p e, and loads the result into back-insertable
     * container @p c.
     */
    template<size_t I=0, typename C, typename E, typename...T>
    inline typename std::enable_if<I < sizeof...(T), void>::type
    build_uniform_constants(C & c, E const & e, std::tuple<T...> const & t)
    {
      c.push_back((e % std::get<I>(t)).ptr());
      build_uniform_constants<I+1, C, E, T...>(c, e, t);
    }

    /// Terminating case for building constant elements from a tuple.
    template<size_t I=0, typename C, typename S, typename...T>
    inline typename std::enable_if<I == sizeof...(T), bool>::type
    build_nonuniform_constants(C &, S const &, std::tuple<T...> const &)
      { return true; }

    /**
     * @brief Iterating case for building constant elements from a tuple.
     *
     * Used when there is an indexable sequence, @p S, of types to instantiate.
     *
     * Extracts initializer value @p I from tuple @p t, uses it to construct a
     * constant value of type @p s[I], and loads the result into
     * back-insertable container @p c.
     *
     * Returns false if instantiation failed due to too many initializer
     * values.
     */
    template<size_t I=0, typename C, typename S, typename...T>
    inline typename std::enable_if<I < sizeof...(T), bool>::type
    build_nonuniform_constants(C & c, S const & s, std::tuple<T...> const & t)
    {
      size_t const i = I;
      if(!s->indexValid(i)) return false;
      auto const elem_ty = wrap(s.factory(), s->getTypeAtIndex(i));
      c.push_back((elem_ty % std::get<I>(t)).ptr());
      return build_nonuniform_constants<I+1, C, S, T...>(c, s, t);
    }
  }

  // ====== Instantiation functions for IntegerType ======

  /**
   * @brief Instantiates an integer with a null (zero) value.
   *
   * @snippet constants.cpp Instantiating a NULL integer
   */
  SPRITE_BINOP_PREAMBLE(ConstantInt, T, IntegerType, U, null_arg)
  operator%(typeobj<T> const & tp, U const &)
    { return tp % 0; }

  /**
   * @brief Instantiates an integer type with a numeric or Boolean constant.
   *
   * If the given type is a Boolean (i.e., @p i1), then the value is coerced to
   * a true/false value.  Otherwise, it is treated as a normal integer, and the
   * extend mode is determined based on whether @p U is a signed or unsigned
   * type.
   *
   * @snippet constants.cpp Instantiating simple types
   */
  SPRITE_BINOP_PREAMBLE(ConstantInt, T, IntegerType, U, uint64_t)
  operator%(typeobj<T> const & tp, U const & value)
  {
    // Handle Booleans.
    if(tp->isIntegerTy(1))
    {
      Constant * p =
          value ? ConstantInt::getTrue(tp.ptr()) : ConstantInt::getFalse(tp.ptr());
      return wrap<ConstantInt>(tp.factory(), p);
    }

    // Handle other types.
    bool const signed_ = std::numeric_limits<U>::is_signed;
    return wrap(tp.factory(), ConstantInt::get(tp.ptr(), value, signed_));
  }

  /**
   * @brief Instantiates an integer type with a string constant.
   *
   * The radix is determined by examining the beginning of the string constant.
   * If it begins with "0x" or "0X", then the radix is 16.  Otherwise, if it
   * begins with 0, then the radix is 8.  Otherwise, if it begins with "b" or
   * "B", then the radix is 2.  Otherwise, the radix is 10.
   *
   * @snippet constants.cpp Instantiating integer types from strings
   */
  SPRITE_BINOP_PREAMBLE(ConstantInt, T, IntegerType, U, StringRef)
  operator%(typeobj<T> const & tp, U const & value_)
  {
    StringRef value(value_);
    int radix = 10;
    if(value.size() > 0)
    {
      switch(value.front())
      {
        case '0':
          // No need to check size > 1, since there must be a NULL character in
          // any case.
          if(value.data()[1] == 'x' || value.data()[1] == 'X')
          {
            radix = 16;
            value = StringRef(value.data() + 2);
          }
          else
            radix = 8;

          break;

        case 'b': case 'B':
          radix = 2;
          value = StringRef(value.data() + 1);
      }
    }

    return wrap(tp.factory(), ConstantInt::get(tp.ptr(), value, radix));
  }

  /**
   * @brief Instantiates tn integer type from an arbitrary-precision constant.
   *
   * @snippet constants.cpp Instantiating simple types
   */
  SPRITE_BINOP_PREAMBLE(ConstantInt, T, IntegerType, U, llvm_::APInt)
  operator%(typeobj<T> const & tp, U const & value)
  {
    return wrap(
        tp.factory()
      , ConstantInt::get(
            tp.factory().context(), static_cast<llvm_::APInt const &>(value)
          )
      );
  }


  // ====== Instantiation functions for FPType ======

  /// Instantiates a floating-point type with a null value.
  SPRITE_BINOP_PREAMBLE(ConstantFP, T, FPType, U, null_arg)
  operator%(typeobj<T> const & tp, U const &)
    { return tp % 0.0; }

  /**
   * @brief Instantiates a floating-point type from a numeric constant.
   *
   * @snippet constants.cpp Instantiating simple types
   */
  SPRITE_BINOP_PREAMBLE(ConstantFP, T, FPType, U, double)
  operator%(typeobj<T> const & tp, U const & value)
  {
    return wrap<ConstantFP>(
        tp.factory(), ConstantFP::get(tp.ptr(), static_cast<double>(value))
      );
  }

  /**
   * @brief Instantiates a floating-point type from a string constant.
   *
   * @snippet constants.cpp Instantiating simple types
   */
  SPRITE_BINOP_PREAMBLE(ConstantFP, T, FPType, U, StringRef)
  operator%(typeobj<T> const & tp, U const & value)
  {
    return wrap<ConstantFP>(
        tp.factory()
      , ConstantFP::get(tp.ptr(), static_cast<StringRef const &>(value))
      );
  }

  /**
   * @brief Instantiates a floating-point type from an arbitrary-precision constant.
   *
   * @snippet constants.cpp Instantiating simple types
   */
  SPRITE_BINOP_PREAMBLE(ConstantFP, T, FPType, U, llvm_::APFloat)
  operator%(typeobj<T> const & tp, U const & value)
  {
    return wrap(
        tp.factory()
      , ConstantFP::get(
            tp.factory().context(), static_cast<llvm_::APFloat const &>(value)
          )
      );
  }

  /**
   * @brief Instantiates a floating-point infinity or negative infinity value.
   *
   * @snippet constants.cpp Instantiating non-finite floating-point types
   */
  SPRITE_BINOP_PREAMBLE(ConstantFP, T, FPType, U, non_finite_value)
  operator%(typeobj<T> const & tp, U const & nfv_)
  {
    non_finite_value const nfv = nfv_;
    auto const & sem = tp->getFltSemantics();
    switch(nfv.kind())
    {
      case non_finite_value::Inf:
        return tp % llvm_::APFloat::getInf(sem, nfv.negative());
      case non_finite_value::Nan:
        return tp % llvm_::APFloat::getNaN(sem, nfv.negative());
      case non_finite_value::Qnan:
        return tp % llvm_::APFloat::getQNaN(sem, nfv.negative());
      case non_finite_value::Snan:
        return tp % llvm_::APFloat::getSNaN(sem, nfv.negative());
    }
    throw runtime_error("Bad non-finite value specifier");
  }

  // ====== Instantiation functions for StructType ======

  /**
   * @brief Instantiates a struct with a null value.
   *
   * All bits are set to zero.
   *
   * @snippet constants.cpp Instantiating a NULL struct
   */
  SPRITE_BINOP_PREAMBLE(ConstantAggregateZero, T, StructType, U, null_arg)
  operator%(typeobj<T> const & tp, U const &)
    { return wrap(tp.factory(), ConstantAggregateZero::get(tp.ptr())); }

  /**
   * @brief Instantiates a struct type from an array.
   *
   * @snippet constants.cpp Instantiating types
   */
  SPRITE_BINOP_PREAMBLE(Constant, T, StructType, U, any_array_ref)
  operator%(typeobj<T> const & tp, U const & values_)
  {
    any_array_ref values(values_);
    return values._accept_modulo(tp);
  }

  /**
   * @brief Instantiates a struct type from a tuple.
   *
   * @snippet constants.cpp Instantiating types
   */
  SPRITE_BINOP_PREAMBLE(Constant, T, StructType, U, any_tuple_ref)
  operator%(typeobj<T> const & tp, U const & values_)
  {
    any_tuple_ref values(values_);
    return values._accept_modulo(tp);
  }

  /**
   * @brief Implements operator @p % between a struct type and a sequence of
   * instantiated values.
   */
  constantobj<Constant>
  _modulo(struct_type const & tp, ArrayRef<Constant*> const & values)
  {
    auto const p = ConstantStruct::get(
        tp.ptr(), static_cast<ArrayRef<Constant*> const &>(values)
      );
    return wrap(tp.factory(), p);
  }

  template<typename T>
  constantobj<Constant>
  _modulo(struct_type const & tp, ArrayRef<T> const & values)
  {
    if(values.size() > 0 && tp->indexValid(values.size() - 1))
      throw type_error("Too many values to extract in struct instantiation");
    std::vector<Constant*> args;
    size_t i=0;
    for(auto arg: values)
    {
      auto const elem_ty = wrap(tp.factory(), tp->getTypeAtIndex(i++));
      args.push_back((elem_ty % arg).ptr());
    }
    return _modulo(tp, args);
  }

  template<typename...T>
  constantobj<Constant>
  _modulo(struct_type const & tp, std::tuple<T...> const & value)
  {
    std::vector<Constant*> args;
    if(!aux::build_nonuniform_constants(args, tp, value))
      throw type_error("Too many values to extract in struct instantiation");
    return _modulo(tp, args);
  }

  // ====== Instantiation functions for ArrayType ======

  /**
   * @brief Instantiates an array with a null value.
   *
   * All bits are set to zero.
   *
   * @snippet constants.cpp Instantiating a NULL array
   */
  SPRITE_BINOP_PREAMBLE(ConstantAggregateZero, T, ArrayType, U, null_arg)
  operator%(typeobj<T> const & tp, U const &)
    { return wrap(tp.factory(), ConstantAggregateZero::get(tp.ptr())); }

  /**
   * @brief Instantiates an array type using an initializer list.
   *
   * @param[in] values
   *   An initializer list used to initialize the array values.  Each element
   *   of the list is used to construct a constant value of the appropriate
   *   type.
   *
   * @snippet constants.cpp Instantiating arrays as aggregates
   * @snippet constants.cpp Instantiating an array from a sequence
   */
  SPRITE_BINOP_PREAMBLE(Constant, T, ArrayType, U, any_array_ref)
  operator%(typeobj<T> const & tp, U const & values)
  {
    any_array_ref values_(values);
    return values_._accept_modulo(tp);
  }

  /**
   * @brief Instantiates an array type from heterogenous data.
   *
   * @snippet constants.cpp Instantiating arrays from tuples
   */
  SPRITE_BINOP_PREAMBLE(Constant, T, ArrayType, U, any_tuple_ref)
  operator%(typeobj<T> const & tp, U const & values_)
  {
    any_tuple_ref values(values_);
    return values._accept_modulo(tp);
  }

  /**
   * @brief Implements operator @p % between an array type and a sequence of
   * instantiated values.
   */
  constantobj<Constant>
  _modulo(array_type const & tp, ArrayRef<Constant*> const & values)
  {
    auto const p = ConstantArray::get(
        tp.ptr(), static_cast<ArrayRef<Constant*> const &>(values)
      );
    return wrap(tp.factory(), p);
  }

  /**
   * @brief Implements operator @p % between an array type and a sequence of
   * initializer values.
   */
  template<typename T>
  constantobj<Constant>
  _modulo(array_type const & tp, ArrayRef<T> const & values)
  {
    auto const elem_ty = wrap(tp.factory(), tp->getElementType());
    std::vector<Constant*> args;
    for(auto arg: values) { args.push_back((elem_ty % arg).ptr()); }
    return _modulo(tp, args);
  }

  template<typename...T>
  constantobj<Constant>
  _modulo(array_type const & tp, std::tuple<T...> const & value)
  {
    auto const elem_ty = wrap(tp.factory(), tp->getElementType());
    std::vector<Constant*> args;
    aux::build_uniform_constants(args, elem_ty, value);
    return _modulo(tp, args);
  }


  // ====== Instantiation functions for PointerType ======

  /**
   * @brief Instantiates a pointer with a null value.
   *
   * @snippet constants.cpp Instantiating a NULL pointer
   */
  SPRITE_BINOP_PREAMBLE(ConstantPointerNull, T, PointerType, U, null_arg)
  operator%(typeobj<T> const & tp, U const &)
    { return wrap(tp.factory(), ConstantPointerNull::get(tp.ptr())); }

  namespace aux
  {
    template<typename T, typename U>
    constantobj<Constant>
    create_global_pointer_from_constants(
        typeobj<T> const & tp, U const & values
      )
    {
      auto const elem_ty = wrap(tp.factory(), tp->getElementType());
      auto const array_ty = elem_ty[values.size()];
      auto const global = new llvm_::GlobalVariable(
          /* Module      */ *tp.factory().module()
        , /* Type        */ array_ty.ptr()
        , /* isConstant  */ true
        , /* Linkage     */ GlobalValue::PrivateLinkage
        , /* Initializer */ (array_ty % values).ptr()
        , /* Name        */ ".str"
        );

      // llc -march=cpp sets the alignment for small element types.  This may
      // not really be required.
      switch(sizeof_(elem_ty))
      {
        case 1:  global->setAlignment(1); break;
        case 2:  global->setAlignment(2); break;
        case 4:  global->setAlignment(4); break;
        case 8:  global->setAlignment(8); break;
        case 16: global->setAlignment(16); break;
      }
      auto const zero = (tp.factory().int_(32) % 0).ptr();
      auto const ptr = llvm_::ConstantExpr::getGetElementPtr(
          global, ArrayRef<Constant *>{zero, zero}
        );
      return wrap(tp.factory(), ptr);
    }
  }

  /**
   * @brief Instantiates a pointer to char from a string.
   *
   * @snippet constants.cpp Instantiating char pointers
   */
  SPRITE_BINOP_PREAMBLE(Constant, T, PointerType, U, StringRef)
  operator%(typeobj<T> const & tp, U const & value)
  {
    StringRef const str(value);
    ArrayRef<char> const values(str.data(), str.size() + 1);
    return aux::create_global_pointer_from_constants(tp, values);
  }

  /**
   * @brief Instantiates a pointer from an array type.
   *
   * A constant global array is created, and a pointer to that is returned.  If
   * a char array is created, no null terminator will be appended.
   *
   * Note: this function is not available through the generic operator%.
   *
   * @snippet constants.cpp Instantiating pointers as global arrays
   */
  SPRITE_BINOP_PREAMBLE3(Constant, T, PointerType, U, any_array_ref, StringRef)
  operator%(typeobj<T> const & tp, U const & values_)
  {
    any_array_ref const values(values_);
    return aux::create_global_pointer_from_constants(tp, values);
  }

  /**
   * @brief Instantiates a pointer from a tuple type.
   *
   * A constant global array is created, and a pointer to that is returned.  If
   * a char array is created, no null terminator will be appended.
   *
   * @snippet constants.cpp Instantiating pointers as global arrays
   */
  SPRITE_BINOP_PREAMBLE(Constant, T, PointerType, U, any_tuple_ref)
  operator%(typeobj<T> const & tp, U const & values_)
  {
    any_tuple_ref const values(values_);
    return aux::create_global_pointer_from_constants(tp, values);
  }

  /**
   * @brief Generic version of operator% for @p typeobj.
   *
   * Instantiates a pointer, array, struct, integer, or floating-point type
   * with any allowed value.
   *
   * Due to the dynamic nature of this function, mismatches between the LHS
   * type and RHS value lead to an exception rather than a compile-time
   * failure.
   */
  template<typename T>
  inline constantobj<Constant>
  operator%(type const & tp, T const & arg)
  {
    using namespace generics;
    generic_handler<
        /* Return type */     constantobj<Constant>
      , /* Action on match */ modulo

        /*    LHS Match     Allowed RHSs */
        /*    ------------  ------------ */
      , case_< IntegerType,  null_arg, uint64_t, StringRef, llvm_::APInt>
      , case_< FPType,       null_arg, double, StringRef, llvm_::APFloat, non_finite_value>
      , case_< StructType,   null_arg, any_array_ref, any_tuple_ref>
      , case_< ArrayType,    null_arg, any_array_ref, any_tuple_ref>
      , case_< PointerType,  null_arg, StringRef>

      > const handler;

    return handler(tp, arg);
  }

  /**
   * @brief Instantiates a constant data array.
   *
   * This method takes a @p type_factory, not a @p typeobj, as the left-hand
   * side.  An array of the appropriate element type and length is deduced.
   *
   * The right-hand side type is constrained by @p ConstantDataArray, meaning
   * only simple 1/2/4/8-byte integer or float/double types are accepted.
   * 
   * @snippet constants.cpp Instantiating constant data arrays
   */

  template<typename T>
  inline typename std::enable_if<
      std::is_convertible<T, ArrayRef<uint8_t>>::value
        || std::is_convertible<T, ArrayRef<uint16_t>>::value
        || std::is_convertible<T, ArrayRef<uint32_t>>::value
        || std::is_convertible<T, ArrayRef<uint64_t>>::value
        || std::is_convertible<T, ArrayRef<float>>::value
        || std::is_convertible<T, ArrayRef<double>>::value
        || !std::is_convertible<T, StringRef>::value
    , constantobj<Constant>
    >::type
  operator%(type_factory const & tf, T const & value)
    { return wrap(tf, llvm_::ConstantDataArray::get(tf.context(), value)); }

  /**
   * @brief Instantiates a constant data array from string data.
   *
   * A null-terminator will be appended.  To avoid this behavior, pass the data
   * explicitly as @p ArrayRef<char>.
   *
   * @snippet constants.cpp Instantiating constant data arrays
   */
  inline constantobj<Constant>
  operator%(type_factory const & tf, StringRef const & value)
    { return wrap(tf, llvm_::ConstantDataArray::getString(tf.context(), value)); }
}}

