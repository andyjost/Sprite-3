#pragma once
#include "sprite/backend/core/castexpr.hpp"
#include "sprite/backend/core/constant.hpp"
#include "sprite/backend/core/detail/build_constants.hpp"
#include "sprite/backend/core/module.hpp"
#include "sprite/backend/core/operator_flags.hpp"
#include "sprite/backend/core/scope.hpp"
#include "sprite/backend/core/type.hpp"
#include "sprite/backend/support/array_ref.hpp"
#include "sprite/backend/support/casting.hpp"
#include "sprite/backend/support/generic_support.hpp"
#include "sprite/backend/support/type_erasures.hpp"
#include "sprite/backend/support/type_traits.hpp"
#include <cstddef>
#include <vector>

#define SPRITE_GETCONST_PREAMBLE(RetTy, Lhs, EnTy, Rhs, ArgTy) \
    template<typename Lhs, typename Rhs>                       \
    inline typename std::enable_if<                            \
        std::is_base_of<EnTy, Lhs>::value                      \
            && std::is_convertible<Rhs, ArgTy>::value          \
      , RetTy                                                  \
      >::type                                                  \
  /**/

#define SPRITE_GETCONST_PREAMBLE2(RetTy, T, EnTy) \
    typename std::enable_if<                      \
        std::is_base_of<EnTy, T>::value, RetTy    \
      >::type                                     \
  /**/

#define SPRITE_GETCONST_PREAMBLE3(RetTy, Lhs, EnTy, Rhs, ArgTy, NotArgTy) \
    template<typename Lhs, typename Rhs>                                  \
    inline typename std::enable_if<                                       \
        std::is_base_of<EnTy, Lhs>::value                                 \
            && std::is_convertible<Rhs, ArgTy>::value                     \
            && !std::is_convertible<Rhs, NotArgTy>::value                 \
      , RetTy                                                             \
      >::type                                                             \
  /**/

#define SPRITE_GETCONST_PREAMBLE4(RetTy, Rhs, ArgTy) \
    template<typename Rhs>                           \
    inline typename std::enable_if<                  \
        std::is_convertible<Rhs, ArgTy>::value       \
      , RetTy                                        \
      >::type                                        \
  /**/

namespace sprite { namespace backend
{
  // TODO add comment: generic one needs to be predeclared.
  template<typename T>
  inline constant get_constant_impl(type_with_flags const & ty, T const & arg);

  // ====== Instantiation functions for IntegerType ======

  /**
   * @brief Instantiates an integer with a null (zero) value.
   *
   * @snippet constants.cpp Instantiating a NULL integer
   */
  SPRITE_GETCONST_PREAMBLE(constant_int, T, IntegerType, U, null_arg)
  get_constant_impl(typeobj<T> const & ty, U const &)
    { return get_constant_impl(ty, 0); }

  SPRITE_GETCONST_PREAMBLE(constant, T, IntegerType, U, constant)
  get_constant_impl(typeobj_with_flags<T> const & ty, U const & value)
    { return typecast(value, ty); }

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
  SPRITE_GETCONST_PREAMBLE(constant_int, T, IntegerType, U, uint64_t)
  get_constant_impl(typeobj<T> const & ty, U const & value)
  {
    // Handle Booleans.
    if(ty->isIntegerTy(1))
    {
      return constant_int(
          dyn_cast<ConstantInt>(
              value
                ? SPRITE_APICALL(ConstantInt::getTrue(ty.ptr()))
                : SPRITE_APICALL(ConstantInt::getFalse(ty.ptr()))
            )
        );
    }

    // Handle other types.
    bool const signed_ = std::numeric_limits<U>::is_signed;
    return constant_int(SPRITE_APICALL(
        ConstantInt::get(ty.ptr(), value, signed_)
      ));
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
  SPRITE_GETCONST_PREAMBLE(constant_int, T, IntegerType, U, string_ref)
  get_constant_impl(typeobj<T> const & ty, U const & value_)
  {
    string_ref value(value_);
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
            value = string_ref(value.data() + 2);
          }
          else
            radix = 8;

          break;

        case 'b': case 'B':
          radix = 2;
          value = string_ref(value.data() + 1);
      }
    }

    return constant_int(SPRITE_APICALL(
        ConstantInt::get(ty.ptr(), value, radix)
      ));
  }

  /**
   * @brief Instantiates tn integer type from an arbitrary-precision constant.
   *
   * @snippet constants.cpp Instantiating simple types
   */
  SPRITE_GETCONST_PREAMBLE(constant_int, T, IntegerType, U, APInt)
  get_constant_impl(typeobj<T> const & ty, U const & value)
  {
    return constant_int(SPRITE_APICALL(
        ConstantInt::get(
            scope::current_context(), static_cast<APInt const &>(value)
          )
      ));
  }


  // ====== Instantiation functions for FPType ======

  /// Instantiates a floating-point type with a null value.
  SPRITE_GETCONST_PREAMBLE(constant_fp, T, FPType, U, null_arg)
  get_constant_impl(typeobj<T> const & ty, U const &)
    { return get_constant_impl(ty, 0.0); }

  SPRITE_GETCONST_PREAMBLE(constant, T, FPType, U, constant)
  get_constant_impl(typeobj_with_flags<T> const & ty, U const & value)
    { return typecast(value, ty); }

  /**
   * @brief Instantiates a floating-point type from a numeric constant.
   *
   * @snippet constants.cpp Instantiating simple types
   */
  SPRITE_GETCONST_PREAMBLE(constant_fp, T, FPType, U, double)
  get_constant_impl(typeobj<T> const & ty, U const & value)
  {
    return constant_fp(
        dyn_cast<ConstantFP>(SPRITE_APICALL(
            ConstantFP::get(ty.ptr(), static_cast<double>(value))
          ))
      );
  }

  /**
   * @brief Instantiates a floating-point type from a string constant.
   *
   * @snippet constants.cpp Instantiating simple types
   */
  SPRITE_GETCONST_PREAMBLE(constant_fp, T, FPType, U, string_ref)
  get_constant_impl(typeobj<T> const & ty, U const & value)
  {
    return constant_fp(
        dyn_cast<ConstantFP>(SPRITE_APICALL(
            ConstantFP::get(ty.ptr(), static_cast<string_ref const &>(value))
          ))
      );
  }

  /**
   * @brief Instantiates a floating-point type from an arbitrary-precision constant.
   *
   * @snippet constants.cpp Instantiating simple types
   */
  SPRITE_GETCONST_PREAMBLE(constant_fp, T, FPType, U, APFloat)
  get_constant_impl(typeobj<T> const & ty, U const & value)
  {
    return constant_fp(
        dyn_cast<ConstantFP>(SPRITE_APICALL(
            ConstantFP::get(
                scope::current_context(), static_cast<APFloat const &>(value)
              )
          ))
      );
  }

  /**
   * @brief Instantiates a floating-point infinity or negative infinity value.
   *
   * @snippet constants.cpp Instantiating non-finite floating-point types
   */
  SPRITE_GETCONST_PREAMBLE(constant_fp, T, FPType, U, non_finite_value)
  get_constant_impl(typeobj<T> const & ty, U const & nfv_)
  {
    non_finite_value const nfv = nfv_;
    auto const & sem = ty->getFltSemantics();
    switch(nfv.kind())
    {
      case non_finite_value::Inf:
        return get_constant_impl(ty, SPRITE_APICALL(APFloat::getInf(sem, nfv.negative())));
      case non_finite_value::Nan:
        return get_constant_impl(ty, SPRITE_APICALL(APFloat::getNaN(sem, nfv.negative())));
      case non_finite_value::Qnan:
        return get_constant_impl(ty, SPRITE_APICALL(APFloat::getQNaN(sem, nfv.negative())));
      case non_finite_value::Snan:
        return get_constant_impl(ty, SPRITE_APICALL(APFloat::getSNaN(sem, nfv.negative())));
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
  SPRITE_GETCONST_PREAMBLE(constobj<ConstantAggregateZero>, T, StructType, U, null_arg)
  get_constant_impl(typeobj<T> const & ty, U const &)
  {
    return constobj<ConstantAggregateZero>(SPRITE_APICALL(
        ConstantAggregateZero::get(ty.ptr())
      ));
  }

  /**
   * @brief Instantiates a struct type from an array.
   *
   * @snippet constants.cpp Instantiating types
   */
  SPRITE_GETCONST_PREAMBLE(constant, T, StructType, U, any_array_ref)
  get_constant_impl(typeobj<T> const & ty, U const & values_)
  {
    any_array_ref values(values_);
    return values._accept_get_constant_impl(ty);
  }

  /**
   * @brief Instantiates a struct type from a tuple.
   *
   * @snippet constants.cpp Instantiating types
   */
  SPRITE_GETCONST_PREAMBLE(constant, T, StructType, U, any_tuple_ref)
  get_constant_impl(typeobj<T> const & ty, U const & values_)
  {
    any_tuple_ref values(values_);
    return values._accept_get_constant_impl(ty);
  }

  template<typename T>
  constant get_constant_impl(struct_type const & ty, array_ref<T> const & values)
    { return aux::generic_build_struct(ty, values, values.size()); }

  template<typename...T>
  constant get_constant_impl(struct_type const & ty, std::tuple<T...> const & values)
    { return aux::generic_build_struct(ty, values, sizeof...(T)); }

  // ====== Instantiation functions for ArrayType ======

  /**
   * @brief Instantiates an array with a null value.
   *
   * All bits are set to zero.
   *
   * @snippet constants.cpp Instantiating a NULL array
   */
  SPRITE_GETCONST_PREAMBLE(constobj<ConstantAggregateZero>, T, ArrayType, U, null_arg)
  get_constant_impl(typeobj<T> const & ty, U const &)
  {
    return constobj<ConstantAggregateZero>(SPRITE_APICALL(
        ConstantAggregateZero::get(ty.ptr())
      ));
  }

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
  SPRITE_GETCONST_PREAMBLE(constant, T, ArrayType, U, any_array_ref)
  get_constant_impl(typeobj_with_flags<T> const & ty, U const & values)
  {
    any_array_ref values_(values);
    return values_._accept_get_constant_impl(ty);
  }

  /**
   * @brief Instantiates an array type from heterogenous data.
   *
   * @snippet constants.cpp Instantiating arrays from tuples
   */
  SPRITE_GETCONST_PREAMBLE(constant, T, ArrayType, U, any_tuple_ref)
  get_constant_impl(typeobj_with_flags<T> const & ty, U const & values_)
  {
    any_tuple_ref values(values_);
    return values._accept_get_constant_impl(ty);
  }

  namespace aux
  {
    // If Src and Tgt are both integers of the same size,, but they are not the
    // same type, then it's safe to reinterpret.
    template<typename Tgt, typename Src>
    bool constexpr can_reinterpret()
    {
      using namespace std;
      return is_integral<Tgt>::value
          && is_integral<Src>::value
          && (sizeof(Tgt) == sizeof(Src))
          && !is_same<Tgt, Src>::value;
    }

    //@{
    // Exact match.
    template<typename Tgt, typename Src>
    constant build_data_array(
        array_type_with_flags const & ty, array_ref<Src> const & values
      , typename std::enable_if<std::is_same<Src, Tgt>::value>::type* = nullptr
      )
    {
      static_assert(
          is_simple_data_type<Tgt>::value, "Expected simple target data"
        );

      return constant(SPRITE_APICALL(
          llvm::ConstantDataArray::get(scope::current_context(), values)
        ));
    }

    // Integer bitwidth but not signedness match, e.g., Tgt = uint64_t and Src
    // = int64_t.
    template<typename Tgt, typename Src>
    constant build_data_array(
        array_type_with_flags const & ty, array_ref<Src> const & values
      , typename std::enable_if<can_reinterpret<Src, Tgt>()>::type* = nullptr
      )
    {
      // OK to reinterpret the data type.
      array_ref<Tgt> const data(
          reinterpret_cast<Tgt const *>(values.data()), values.size()
        );
      return build_data_array<Tgt, Tgt>(ty, data);
    }

    // Explicitly convertible.
    template<typename Tgt, typename Src>
    constant build_data_array(
        array_type_with_flags const & ty, array_ref<Src> const & values
      , typename std::enable_if<
            std::is_constructible<Tgt, Src>::value
              && !can_reinterpret<Tgt, Src>()
              && !std::is_same<Tgt, Src>::value
          >::type* = nullptr
      )
    {
      std::vector<Tgt> const data(values.begin(), values.end());
      return build_data_array<Tgt, Tgt>(ty, data);
    }

    template<typename Tgt, typename Src>
    constant build_data_array(
        array_type_with_flags const & ty, array_ref<Src> const & values
      , typename std::enable_if<
            !std::is_convertible<Src, Tgt>::value
              && !std::is_same<Src, Tgt>::value
          >::type* = nullptr
      )
    { return generic_build_array(ty, values, values.size()); }
    //@}
  }

  /**
   * @brief Implements @p get_constant_impl between an array type and a
   * sequence of initializer values.
   */
  template<typename T>
  inline constant get_constant_impl(
      array_type_with_flags const & ty, array_ref<T> const & values
    )
  {
    auto const elem_ty = element_type(ty);
    if(elem_ty->isFloatTy())
      return aux::build_data_array<float>(ty, values);
    if(elem_ty->isDoubleTy())
      return aux::build_data_array<double>(ty, values);
    if(integer_type const ity = dyn_cast<integer_type>(elem_ty))
    {
      switch (ity->getBitWidth())
      {
      case 8:  return aux::build_data_array<uint8_t>(ty, values);
      case 16: return aux::build_data_array<uint16_t>(ty, values);
      case 32: return aux::build_data_array<uint32_t>(ty, values);
      case 64: return aux::build_data_array<uint64_t>(ty, values);
      }
    }

    // No specialized match.
    return aux::generic_build_array(ty, values, values.size());
  }

  template<typename T>
  inline constant get_constant_impl(array_type const & ty, array_ref<T> const & values)
    { return get_constant_impl(array_type_with_flags(ty), values); }

  template<typename...T>
  inline constant get_constant_impl(
      array_type_with_flags const & ty, std::tuple<T...> const & values
    )
  { return aux::generic_build_array(ty, values, sizeof...(T)); }

  template<typename...T>
  constant get_constant_impl(
      array_type const & ty, std::tuple<T...> const & values
    )
  { return get_constant_impl(array_type_with_flags(ty), values); }


  // ====== Instantiation functions for PointerType ======

  /**
   * @brief Instantiates a pointer with a null value.
   *
   * @snippet constants.cpp Instantiating a NULL pointer
   */
  SPRITE_GETCONST_PREAMBLE(nullptr_, T, PointerType, U, null_arg)
  get_constant_impl(typeobj<T> const & ty, U const &)
    { return nullptr_(SPRITE_APICALL(ConstantPointerNull::get(ty.ptr()))); }

  /// Instantiates a pointer with a null value using @p std::nullptr_t.
  template<typename T>
  inline SPRITE_GETCONST_PREAMBLE2(nullptr_, T, PointerType)
  get_constant_impl(typeobj<T> const & ty, std::nullptr_t const &)
    { return nullptr_(SPRITE_APICALL(ConstantPointerNull::get(ty.ptr()))); }

  /// Instantiates a pointer from a constant, such as an integer.
  SPRITE_GETCONST_PREAMBLE(constant, T, PointerType, U, constant)
  get_constant_impl(typeobj_with_flags<T> const & ty, U const & value)
    { return typecast(value, ty); }

  namespace aux
  {
    template<typename T, typename U>
    constant create_global_pointer_from_constants(
        typeobj<T> const & ty, U const & values
      )
    {
      auto const elem_ty = element_type(ty);
      auto const array_ty = elem_ty[values.size()];
      auto const global = SPRITE_APICALL(new GlobalVariable(
          /* Module      */ *scope::current_module().ptr()
        , /* Type        */ array_ty.ptr()
        , /* isConstant  */ true
        , /* Linkage     */ GlobalValue::PrivateLinkage
        , /* Initializer */ get_constant_impl(array_ty, values).ptr()
        , /* Name        */ ".str"
        ));

      global->setUnnamedAddr(true);

      // llc -march=cpp sets the alignment for small element types.  This may
      // not really be required.
      switch(sizeof_(elem_ty))
      {
        case 1:  SPRITE_APICALL(global->setAlignment(1)); break;
        case 2:  SPRITE_APICALL(global->setAlignment(2)); break;
        case 4:  SPRITE_APICALL(global->setAlignment(4)); break;
        case 8:  SPRITE_APICALL(global->setAlignment(8)); break;
        case 16: SPRITE_APICALL(global->setAlignment(16)); break;
      }
      auto const zero = get_constant_impl(types::int_(32), 0).ptr();
      auto const ptr = SPRITE_APICALL(ConstantExpr::getGetElementPtr(
          global, array_ref<Constant *>{zero, zero}
        ));
      return constant(ptr);
    }
  }

  /**
   * @brief Instantiates a pointer to char from a string.
   *
   * @snippet constants.cpp Instantiating char pointers
   */
  SPRITE_GETCONST_PREAMBLE(constant, T, PointerType, U, string_ref)
  get_constant_impl(typeobj<T> const & ty, U const & value)
  {
    string_ref const str(value);
    assert(str.begin()[str.size()] == 0
      && "Expected a null terminator for C string."
      );
    array_ref<char> const values(str.data(), str.size() + 1);
    return aux::create_global_pointer_from_constants(ty, values);
  }

  /**
   * @brief Instantiates a pointer from an array type.
   *
   * A constant global array is created, and a pointer to that is returned.  If
   * a char array is created, no null terminator will be appended.
   *
   * Note: this function is not available through the generic get_constant_impl.
   *
   * @snippet constants.cpp Instantiating pointers as global arrays
   */
  SPRITE_GETCONST_PREAMBLE3(constant, T, PointerType, U, any_array_ref, string_ref)
  get_constant_impl(typeobj<T> const & ty, U const & values_)
  {
    any_array_ref const values(values_);
    return aux::create_global_pointer_from_constants(ty, values);
  }

  /**
   * @brief Instantiates a pointer from a tuple type.
   *
   * A constant global array is created, and a pointer to that is returned.  If
   * a char array is created, no null terminator will be appended.
   *
   * @snippet constants.cpp Instantiating pointers as global arrays
   */
  SPRITE_GETCONST_PREAMBLE(constant, T, PointerType, U, any_tuple_ref)
  get_constant_impl(typeobj<T> const & ty, U const & values_)
  {
    any_tuple_ref const values(values_);
    return aux::create_global_pointer_from_constants(ty, values);
  }

  /**
   * @brief Generic version of get_constant_impl for @p typeobj.
   *
   * Instantiates a pointer, array, struct, integer, or floating-point type
   * with any allowed value.
   *
   * Due to the dynamic nature of this function, mismatches between the LHS
   * type and RHS value lead to an exception rather than a compile-time
   * failure.
   */
  template<typename T>
  inline constant get_constant_impl(type_with_flags const & ty, T const & arg)
  {
    using namespace generics;
    generic_handler<
        /* Return type */     constant
      , /* Action on match */ get_constant_action

        /*    LHS Match     Allowed RHSs */
        /*    ------------  ------------ */
      , case_< IntegerType,  null_arg, constant, uint64_t, string_ref, APInt>
      , case_< FPType,       null_arg, constant, double, string_ref, APFloat, non_finite_value>
      , case_< StructType,   null_arg, any_array_ref, any_tuple_ref>
      , case_< ArrayType,    null_arg, any_array_ref, any_tuple_ref>
      , case_< PointerType,  null_arg, constant, std::nullptr_t, string_ref>

      > const handler;

    return handler(ty, arg);
  }

  //@{
  /**
   * @brief Produces constant values known at runtime.
   *
   * The type of the constant is passed as the first argument.
   */
  template<
      typename Ty, typename U
    , typename = typename std::enable_if<is_typearg<Ty>::value>::type
    , typename = DISABLE_IF_ARRAY_LIKE(U)
    >
  inline auto get_constant(Ty const & ty, U const & value)
    -> decltype(get_constant_impl(ty, value))
    { return get_constant_impl(ty, value); }

  // Explicit mention of any_array_ref makes this accept nested initializer_lists.
  template<
      typename Ty
    , typename Enable = typename std::enable_if<is_typearg<Ty>::value>::type
    >
  inline auto get_constant(Ty const & ty, any_array_ref const & value)
    -> decltype(get_constant_impl(ty, value))
  {
    if(ty->isArrayTy())
    {
      size_t const n = len(ty);
      size_t const n_init = value.size();
      if(n == 0)
        return get_constant_impl(element_type(ty)[n_init], value);
      else
      {
        if(n != n_init)
          throw value_error("Wrong number of initializers for array type.");
        return get_constant_impl(ty, value);
      }
    }
    // If ty is not an array type, then it must be an integer, floating-point,
    // or char pointer type.  And in that case, the value must be a string.
    else if(ty->isIntegerTy() || ty->isFloatingPointTy() || ty->isPointerTy())
    {
      // Try to initialize the constant using a char* first.  If ty is
      // a char*, this will correctly keep the null terminator.
      string_ref const str = value.string();
      if(str.data())
        return get_constant_impl(ty, str);

      // Otherwise, initialize the pointer with the values.  That is used when
      // an non-char pointer is initialized with an array type.
      else if (ty->isPointerTy())
        return get_constant_impl(ty, value);
      else
        throw value_error(
            "Expected a character array when initializing an integer or "
            "floating-point type with an array-like initializer."
          );
    }
    else if(ty->isStructTy())
      return get_constant_impl(ty, value);
    throw type_error(
        "Expected array, integer, floating-point, or pointer type."
      );
  }
  //@}

  namespace aux
  {
    /// Selects the appropriate initializer type for a given T.
    template<typename T> struct initializer_for
      : std::conditional<
            std::is_array<T>::value, any_array_ref
          , typename std::conditional<
                is_tuple<T>::value, any_tuple_ref, T
              >::type
          >
    {};
  }

  //@{
  /**
   * @brief Produces constant values known at compile time.
   *
   * The type of the constant is passed in the template parameter argument list.
   */
  /// Initializes (or default-initializes) a constant of type @p T with the given value.

  // Simple version.
  template<
      typename T, typename U=null_arg
    , typename = typename std::enable_if<!std::is_constructible<any_array_ref, U>::value>::type
    >
  inline auto get_constant(U && value = null)
    -> decltype(get_constant_impl(get_type<T>(), value))
    { return get_constant(get_type<T>(), std::forward<U>(value)); }

 
  // Array version.
  template<typename T>
  inline typename std::enable_if<std::is_array<T>::value, constant>::type
  get_constant(any_array_ref const & value)
  {
    constexpr size_t n = std::extent<T>::value;
    if(n == 0)
    {
      using base_type = typename std::remove_extent<T>::type;
      size_t const deduced_n = value.size();
      return get_constant(get_type<base_type>()[deduced_n], value);
    }
    else
    {
      if(n != value.size())
        throw value_error("Wrong number of initializers for array type.");
      return get_constant(get_type<T>(), value);
    }
  }

  // Pointer version.
  template<typename T>
  inline typename std::enable_if<std::is_pointer<T>::value, constant>::type
  get_constant(any_array_ref const & value)
    { return get_constant(get_type<T>(), value); }

  // Tuple version.
  template<typename...T>
  inline typename std::enable_if<sizeof...(T) != 1, constant>::type
  get_constant(typename aux::initializer_for<T>::type const && ... args)
    { return get_constant<std::tuple<T...>>(std::make_tuple(args...)); }
  //@}

  // See declaration in type.hpp.
  template<typename T>
  template<typename Arg, typename>
  constant typeobj<T>::operator()(Arg && arg) const
    { return get_constant(*this, std::forward<Arg>(arg)); }

  // See declaration in type.hpp.
  template<typename T>
  constant typeobj<T>::operator()(any_array_ref const & array) const
    { return get_constant(*this, array); }

  namespace aux
  {
    // See declaration in operator_flags.hpp.
    template<typename Arg, bool IsRawInitializer>
    template<typename T>
    constant
    decorated_arg<Arg,true,IsRawInitializer>::operator()(T && arg) const
    {
      using Derived = arg_with_flags<Arg>;
      Derived const * this_ = static_cast<Derived const *>(this);
      return get_constant(*this_, std::forward<T>(arg));
    }

    // See declaration in operator_flags.hpp.
    template<typename Arg, bool IsRawInitializer>
    constant
    decorated_arg<Arg,true,IsRawInitializer>::operator()(
        any_array_ref const & arg
      ) const
    {
      using Derived = arg_with_flags<Arg>;
      Derived const * this_ = static_cast<Derived const *>(this);
      return get_constant(*this_, arg);
    }
  }
}}

