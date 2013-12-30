/**
 * @file
 * @brief Defines class @p object.
 */

#pragma once
#include <type_traits>

namespace sprite { namespace backend
{
  /**
   * @brief Base class used to wrap an LLVM API object.
   *
   * It is assumed LLVM will manage the lifetime of the raw pointer.  An
   * instance of this type can be created directly, though normally it's easier
   * to use one of the type_factory::wrap methods.
   */
  template<typename T, typename Factory> class object
  {
  protected:

    /// The wrapped pointer to an LLVM object.
    T * px;

    /// The @p type_factory associated with this object.
    Factory fx;

  public:

    /// The underlying LLVM type.
    typedef T element_type;

    /// The type factory type.
    typedef Factory factory_type;

    /// Regular constructor to capture an LLVM API object.
    template<
        typename U
      , typename = typename std::enable_if<std::is_base_of<T,U>::value>::type
      >
    object(U * p, Factory const & factory) : px(p), fx(factory) {}

    /// Implicitly-converting constructor.
    template<
        typename U
      , typename = typename std::enable_if<std::is_base_of<T,U>::value>::type
      >
    object(object<U, Factory> const & arg)
      : px(arg.ptr()), fx(arg.factory())
    {}

    // Default copy and assignment are OK.

    friend bool operator==(object const & lhs, object const & rhs)
      { return lhs.px == rhs.px && lhs.fx == rhs.fx; }

    friend bool operator!=(object const & lhs, object const & rhs)
      { return !(lhs == rhs); }

    /// Explicit conversion to @p bool.  True if the wrapped pointer is valid.
    explicit operator bool() const { return px; }

    /// Conversion to the LLVM object.
    explicit operator T *() const { assert(px); return px; }

    /// Named conversion to the LLVM object.
    T * ptr() const { assert(px); return px; }

    /// Member access for the LLVM object.
    T * operator->() const { assert(px); return px; }

    /// Get the associated type factory.
    Factory const & factory() const { return fx; }
  };
}}

