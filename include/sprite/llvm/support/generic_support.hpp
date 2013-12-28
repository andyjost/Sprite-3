/**
 * @file
 * @brief Contains support constructs for implementing generic operators.
 */

#pragma once
#include "sprite/llvm/core/wrappers.hpp"
#include "sprite/llvm/support/exceptions.hpp"
#include <sstream>

namespace sprite { namespace llvm { namespace generics
{
  /**
   * @brief Used to specify one case for @p GenericHandler.
   *
   * @param[in] Lhs
   *     The type to match in the LHS position.
   * @param[in] Us...
   *     the RHS types that are allows if the LHS matches.
   */
  template<typename Lhs, typename...Us>
  struct Case
  {
    /// The target type for LHS conversion.
    typedef Lhs Target;

    /// The dispatcher type to call when the LHS matches.
    template<template<typename...> class What>
    struct Dispatcher { typedef What<Lhs, Us...> type;
    };
  };

  namespace aux
  {
    /// A dummy return value that converts to anything (to fool the type-checker).
    struct WeakReturn
    {
      /// Converts to anything (for type-checking; cannot actually be called).
      template<typename T> operator T() const
        { throw RuntimeError("Illegal conversion of WeakReturn"); }
    };

    /// Implements the generic modulo (%) operator.
    template<typename EH, typename Lhs, typename...Us> struct ModuloImpl;
    
    /// Iterating specialization.
    template<typename EH, typename Lhs, typename U, typename...Us>
    struct ModuloImpl<EH, Lhs, U, Us...> : ModuloImpl<EH, Lhs, Us...>
    {
      using ModuloImpl<EH, Lhs, Us...>::operator();
    
      auto operator()(
          TypeWrapper<Lhs> const & lhs, U const & arg
        ) const -> decltype(lhs % arg)
      { return lhs % arg; }
    };
    
    /// Terminal specialization.
    template<typename EH, typename Lhs> struct ModuloImpl<EH, Lhs>
    {
      // Called when no other case (above) matches.
      template<typename V>
      typename std::enable_if<EH::template MatchErrors<V>::value, WeakReturn>::type
      operator()(TypeWrapper<Lhs> const & lhs, V const & arg) const
        { throw EH::error(lhs, arg); }
    };

    // True when T cannot convert to anything in Args...
    template<typename T, typename...Args> struct ConvertsToNone;
    template<typename T> struct ConvertsToNone<T> : std::true_type {};
    template<typename T, typename Arg, typename...Args>
      struct ConvertsToNone<T,Arg,Args...>
      { enum { value = !std::is_convertible<T, Arg>::value && ConvertsToNone<T,Args...>::value }; };

    /**
     * @brief Prepares a nice error message when generic modulo fails to make a
     * match.
     */
    template<typename...Us> struct ModuloEH
    {
      // The predicate to use when enabling the error catcher.
      template<typename T> using MatchErrors = ConvertsToNone<T, Us...>;

      template<typename Lhs, typename T>
      static TypeError error(Wrapper<Lhs> const &, T const & arg)
      {
        std::stringstream ss;
        ss << "While dispatching generic operator % for LHS match of type "
           << Typename<Lhs>::name() << ", expecting RHS of type ";
        append_typenames<Us...>(ss);
        ss << ", but got " + typename_(arg);
        return TypeError(ss.str());
      }

    private:

      // One U.
      template<typename U, typename Os>
      static void append_typenames(Os & os)
        { os << Typename<U>::name(); }

      // Two Us.
      template<typename U0, typename U1, typename Os>
      static void append_typenames(Os & os)
      {
        os  << Typename<U0>::name()
            << ", or "
            << Typename<U1>::name()
          ;
      }

      // More than two Us.
      template<
          typename U0, typename U1, typename U2, typename...Us_
        , typename Os
        >
      static void append_typenames(Os & os)
      {
        os << Typename<U0>::name() << ", ";
        append_typenames<U1, U2, Us_...>(os);
      }
    };
  }
  
  /**
   * @brief Dispatches to the modulo (@p %) operator.  Used in conjunction with
   * @p GenericHandler.
   */
  template<typename Lhs, typename...Us>
  struct Modulo
    : aux::ModuloImpl<aux::ModuloEH<Us...>, Lhs, Us...>
  {
  };

  namespace aux
  {
    template<typename EH, typename ReturnType, template<typename...> class What, typename...Cases>
    struct GenericHandlerImpl;
    
    /// Iterating specialization.
    template<typename EH, typename ReturnType, template<typename...> class What, typename Case, typename...Cases>
    struct GenericHandlerImpl<EH, ReturnType, What, Case, Cases...>
    {
      template<typename T, typename U>
      ReturnType operator()(T const & lhs, U const & arg) const
      {
        // If the generic LHS argument can be cast to the target type, then
        // dispatch the operation.
        if(auto const p = dyn_cast<typename Case::Target>(lhs))
        {
          static typename Case::template Dispatcher<What>::type const dispatcher;
          return dispatcher(p, arg);
        }
    
        // Otherwise, try the next case.
        static GenericHandlerImpl<EH, ReturnType, What, Cases...> const next;
        return next(lhs, arg);
      }
    };
    
    /// Terminal specialization.
    template<typename EH, typename ReturnType, template<typename...> class What>
    struct GenericHandlerImpl<EH, ReturnType, What>
    {
      template<typename T, typename U>
      ReturnType operator()(T const & badtype, U const &) const
        { throw EH::error(badtype); }
    };

    /// Generates a good error message for @p GenericHandler.
    template<typename...Cases> struct GenericHandlerEH
    {
      template<typename T>
      static TypeError error(Wrapper<T> const & arg)
      {
        std::stringstream ss;
        ss << "Expecting LHS of type ";
        append_typenames<Cases...>(ss);
        ss << ", but got " + typename_(*arg.ptr());
        return TypeError(ss.str());
      }

    private:

      // One case.
      template<typename Case, typename Os>
      static void append_typenames(Os & os)
        { os << Typename<typename Case::Target>::name(); }

      // Two cases.
      template<typename Case0, typename Case1, typename Os>
      static void append_typenames(Os & os)
      {
        os  << Typename<typename Case0::Target>::name()
            << ", or "
            << Typename<typename Case1::Target>::name()
          ;
      }

      // More than two cases.
      template<
          typename Case0, typename Case1, typename Case2, typename...Cases_
        , typename Os
        >
      static void append_typenames(Os & os)
      {
        os << Typename<typename Case0::Target>::name() << ", ";
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
   * For example, consider type instantiation using the module (@p %) operator.
   * Each LHS type has a set of allowed RHS types.  @p IntegerType may be
   * instantiated with an @p int, @p FPType may be instantiated with a @p
   * double, and @p ArrayType may be instantiated with an @p ArrayRef, just to
   * name a few.  This class makes it easy to write a generic handler that
   * takes a base <tt>Type *</tt> and and a templatized RHS argument.  It
   * performs a double dispatch that calls the appropriate implementation when
   * the types are compatible, or rasies an error otherwise.
   */
  template<typename ReturnType, template<typename...> class What, typename...Cases>
  struct GenericHandler
    : aux::GenericHandlerImpl<
          aux::GenericHandlerEH<Cases...>, ReturnType, What, Cases...
        >
  {
  };
}}}

