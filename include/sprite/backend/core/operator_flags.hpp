#pragma once
#include "sprite/backend/config.hpp"
#include "sprite/backend/core/object.hpp"
#include "sprite/backend/support/exceptions.hpp"
#include "sprite/backend/support/type_traits.hpp"
#include <tuple>


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
    template<typename Arg> struct arg_with_flags;

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
      typename std::enable_if<is_raw_initializer<T>::value, arg_with_flags<T>>::type
      operator()(T const & arg) const
        { return std::make_tuple(std::cref(arg), *this); }

      template<typename T>
      arg_with_flags<constobj<T>> operator()(constobj<T> const & arg) const
        { return std::make_tuple(arg, *this); }

      template<typename T>
      arg_with_flags<typeobj<T>> operator()(typeobj<T> const & arg) const
        { return std::make_tuple(arg, *this); }
    };

    template<typename Arg
      , bool = is_typeobj<Arg>::value
      , bool = is_raw_initializer<Arg>::value
      >
    struct decorated_arg;

    template<typename Arg>
    struct arg_with_flags : decorated_arg<Arg>, operator_flags
    {
      using base = decorated_arg<Arg>;

      template<typename U
        , typename = typename std::enable_if<
              std::is_constructible<base, U>::value
            >::type
        >
      arg_with_flags(U const & arg, operator_flags const & f = operator_flags())
        : base(arg), operator_flags(f)
      {}

      template<typename U>
      arg_with_flags(arg_with_flags<U> const & arg)
        : base(arg.arg()), operator_flags(arg.flags())
      {}

      template<typename U>
      arg_with_flags(std::tuple<U, operator_flags> const & x)
        : base(std::get<0>(x)), operator_flags(std::get<1>(x))
      {}

      Arg const & arg() const { return *this; }
      operator_flags const & flags() const { return *this; }
    };

    //@{
    /**
     * @brief Injects a specialization of @p operator[] for @p arg_with_flags
     * when @p Arg is a type argument.
     *
     * A type argument having the @p signed_ or @p unsigned_ flag is
     * implemented as <tt>arg_with_flags<typeobj<T>></tt> for some @p T.  The
     * flag is used when a @p typecast is requested, for example, in the last
     * statment of the following example:
     *
     *     auto i8 = types::int_(8);   // using i8 = int8_t;
     *     auto i32 = types::int_(32); // using i32 = int32_t;
     *     signed_(i32) % (i8 % -1);   // i8 a = -1; i32 b = a;
     *
     * The sign flag is needed to know how to perform the integer extension
     * from 8 bits to 32.
     *
     * When forming arrays, the sign flag can be propagated without introducing
     * any ambiguity; the sign applies to the element type and not to the array
     * extent.  Therefore, <tt>operator[]</tt> is specialized for type
     * arguments with flags.
     *
     * When forming pointers and functions, however, propagating the flag is
     * meaningless: neither type is subject to  bit-extending conversions.  The
     * inherited versions of <tt>operator*</tt> and the overload of
     * <tt>operator()</tt> used for function type formation do the right thing,
     * which is discard the flags.
     */
    // Arg is a raw initializer.
    template<typename Arg> struct decorated_arg<Arg, false, true>
      : std::reference_wrapper<typename std::add_const<Arg>::type>
    {
      using std::reference_wrapper<
          typename std::add_const<Arg>::type
         >::reference_wrapper;
    };
    // Arg is an LLVM Value.
    template<typename Arg> struct decorated_arg<Arg, false, false>
      : Arg
    {
      using Arg::Arg;
    };
    // Arg is an LLVM Type.
    template<typename Arg, bool IsRawInitializer>
    struct decorated_arg<Arg, true, IsRawInitializer>
      : Arg
    {
      using Arg::Arg;

      /**
       * @brief Overrides @p operator[] to propagate the flags.
       *
       * The flags apply to the array element type, so, for example,
       * <tt>signed_(i64[2])</tt> is an array of two signed 64-bit integers.
       */
      auto operator[](size_t size) const
        -> arg_with_flags<decltype(this->Arg::operator[](size))>
      {
        using Derived = arg_with_flags<Arg>;
        Derived const * this_ = static_cast<Derived const *>(this);
        return std::make_tuple(
            this->Arg::operator[](size), this_->flags()
          );
      }
    };
    //@}

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
  }

  aux::operator_flags const no_flags;
  aux::operator_flags const nuw = aux::operator_flags(aux::operator_flags::NUW);
  aux::operator_flags const nsw = aux::operator_flags(aux::operator_flags::NSW);
  aux::operator_flags const exact = aux::operator_flags(aux::operator_flags::EXACT);
  aux::operator_flags const signed_ = aux::operator_flags(aux::operator_flags::SIGNED);
  aux::operator_flags const unsigned_ = aux::operator_flags(aux::operator_flags::UNSIGNED);
  aux::operator_flags const arithmetic = aux::operator_flags(aux::operator_flags::ARITHMETIC);
  aux::operator_flags const logical = aux::operator_flags(aux::operator_flags::LOGICAL);

  /// Overloads @p ptr function for @p arg_with_flags.
  template<typename T>
    auto ptr(aux::arg_with_flags<T> const & x) -> decltype(ptr(x.arg()))
    { return ptr(x.arg()); }

  //@{
  // Returns the flags associated with any argument.
  inline aux::operator_flags const & flags(...) { return no_flags; }

  template<typename T>
  inline aux::operator_flags const & flags(object<T> const &)
    { return no_flags; }

  template<typename T>
  inline aux::operator_flags const & flags(aux::arg_with_flags<T> const & x)
    { return x.flags(); }
  //@}

}}
