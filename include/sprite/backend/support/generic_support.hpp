/**
 * @file
 * @brief Contains support constructs for implementing generic operators.
 */

#pragma once
#include "sprite/backend/core/type.hpp"
#include "sprite/backend/support/exceptions.hpp"
#include <sstream>

namespace sprite { namespace backend { namespace generics
{
  /**
   * @brief Used to specify one case for @p generic_handler.
   *
   * @param[in] Lhs
   *     The type to match in the LHS position.
   * @param[in] Us...
   *     the RHS types that are allows if the LHS matches.
   */
  template<typename Lhs, typename...Us>
  struct case_
  {
    /// The target type for LHS conversion.
    typedef Lhs Target;

    /// The dispatcher type to call when the LHS matches.
    template<template<typename...> class What>
    struct dispatcher { typedef What<Us...> type;
    };
  };

  namespace aux
  {
    /// A dummy return value that converts to anything (to fool the type-checker).
    struct weak_return
    {
      /// Converts to anything (for type-checking; cannot actually be called).
      template<typename T> operator T() const
        { throw compile_error("Illegal conversion of weak_return"); }
    };

    /// Implements the generic get_constant_impl function.
    template<typename EH, typename...Us> struct get_constant_action_impl;
    
    /// Iterating specialization.
    template<typename EH, typename U, typename...Us>
    struct get_constant_action_impl<EH, U, Us...>
      : get_constant_action_impl<EH, Us...>
    {
      using get_constant_action_impl<EH, Us...>::operator();
    
      template<typename Lhs>
      auto operator()(
          Lhs const & lhs, U const & arg
        ) const -> decltype(get_constant_impl(lhs, arg))
      { return get_constant_impl(lhs, arg); }
    };
    
    /// Terminal specialization.
    template<typename EH> struct get_constant_action_impl<EH>
    {
      // Called when no other case (above) matches.
      template<typename Lhs, typename V>
      typename std::enable_if<EH::template MatchErrors<V>::value, weak_return>::type
      operator()(Lhs const & lhs, V const & arg) const
        { throw EH::error(lhs, arg); }
    };

    // True when T cannot convert to anything in Args...
    template<typename T, typename...Args> struct converts_to_none;
    template<typename T> struct converts_to_none<T> : std::true_type {};
    template<typename T, typename Arg, typename...Args>
      struct converts_to_none<T,Arg,Args...>
      { enum { value = !std::is_convertible<T, Arg>::value && converts_to_none<T,Args...>::value }; };

    /**
     * @brief Prepares a nice error message when generic get_constant_action
     * fails to make a match.
     */
    template<typename...Us> struct get_constant_action_eh
    {
      // The predicate to use when enabling the error catcher.
      template<typename T> using MatchErrors = converts_to_none<T, Us...>;

      template<typename Lhs, typename T>
      static type_error error(object<Lhs> const &, T const & arg)
      {
        std::stringstream ss;
        ss << "While dispatching get_constant for LHS match of type "
           << typename_impl<Lhs>::name() << ", expecting RHS of type ";
        append_typenames<Us...>(ss);
        ss << ", but got " + typename_(arg);
        return type_error(ss.str());
      }

    private:

      // One U.
      template<typename U, typename Os>
      static void append_typenames(Os & os)
        { os << typename_impl<U>::name(); }

      // Two Us.
      template<typename U0, typename U1, typename Os>
      static void append_typenames(Os & os)
      {
        os  << typename_impl<U0>::name()
            << ", or "
            << typename_impl<U1>::name()
          ;
      }

      // More than two Us.
      template<
          typename U0, typename U1, typename U2, typename...Us_
        , typename Os
        >
      static void append_typenames(Os & os)
      {
        os << typename_impl<U0>::name() << ", ";
        append_typenames<U1, U2, Us_...>(os);
      }
    };
  }
  
  /**
   * @brief Dispatches to the @p get_constant_impl function.  Used in
   * conjunction with @p generic_handler.
   */
  template<typename...Us>
  struct get_constant_action
    : aux::get_constant_action_impl<aux::get_constant_action_eh<Us...>, Us...>
  {
  };

  namespace aux
  {
    template<typename EH, typename ReturnType, template<typename...> class What, typename...Cases>
    struct generic_handler_impl;
    
    /// Iterating specialization.
    template<
        typename EH, typename ReturnType, template<typename...> class What
      , typename Case, typename...Cases
      >
    struct generic_handler_impl<EH, ReturnType, What, Case, Cases...>
    {
      template<typename T, typename U>
      ReturnType operator()(T const & lhs, U const & arg) const
      {
        // If the generic LHS argument can be cast to the target type, then
        // dispatch the operation.
        if(auto const p = dyn_cast<typename Case::Target>(lhs))
        {
          static typename Case::template dispatcher<What>::type const dispatcher;
          return dispatcher(p, arg);
        }
    
        // Otherwise, try the next case.
        static generic_handler_impl<EH, ReturnType, What, Cases...> const next;
        return next(lhs, arg);
      }
    };
    
    /// Terminal specialization.
    template<typename EH, typename ReturnType, template<typename...> class What>
    struct generic_handler_impl<EH, ReturnType, What>
    {
      template<typename T, typename U>
      ReturnType operator()(T const & badtype, U const &) const
        { throw EH::error(badtype); }
    };

    /// Generates a good error message for @p generic_handler.
    template<typename...Cases> struct generic_handler_eh
    {
      template<typename T>
      static type_error error(object<T> const & arg)
      {
        std::stringstream ss;
        ss << "Expecting LHS of type ";
        append_typenames<Cases...>(ss);
        ss << ", but got " + typename_(*arg.ptr());
        return type_error(ss.str());
      }

    private:

      // One case.
      template<typename Case, typename Os>
      static void append_typenames(Os & os)
        { os << typename_impl<typename Case::Target>::name(); }

      // Two cases.
      template<typename Case0, typename Case1, typename Os>
      static void append_typenames(Os & os)
      {
        os  << typename_impl<typename Case0::Target>::name()
            << ", or "
            << typename_impl<typename Case1::Target>::name()
          ;
      }

      // More than two cases.
      template<
          typename Case0, typename Case1, typename Case2, typename...Cases_
        , typename Os
        >
      static void append_typenames(Os & os)
      {
        os << typename_impl<typename Case0::Target>::name() << ", ";
        append_typenames<Case1, Case2, Cases_...>(os);
      }
    };
  }

  /**
   * @brief A generic double-dispatch construct.
   *
   * Used to dispatch operators based on generic types, when the implementation
   * is provided by more specific versions.
   *
   * For example, consider type instantiation using get_constant_impl.
   * Each LHS type has a set of allowed RHS types.  @p IntegerType may be
   * instantiated with an @p int, @p FPType may be instantiated with a @p
   * double, and @p ArrayType may be instantiated with an @p ArrayRef, just to
   * name a few.  This class makes it easy to write a generic handler that
   * takes a base <tt>Type *</tt> and and a templatized RHS argument.  It
   * performs a double dispatch that calls the appropriate implementation when
   * the types are compatible, or rasies an error otherwise.
   */
  template<typename ReturnType, template<typename...> class What, typename...Cases>
  struct generic_handler
    : aux::generic_handler_impl<
          aux::generic_handler_eh<Cases...>, ReturnType, What, Cases...
        >
  {
  };
}}}

