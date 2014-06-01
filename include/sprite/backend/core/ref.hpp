#pragma once
#include "sprite/backend/config.hpp"
#include "sprite/backend/core/detail/current_builder.hpp"
#include "sprite/backend/core/get_value.hpp"
#include "sprite/backend/core/value.hpp"
#include "sprite/backend/support/exceptions.hpp"

namespace sprite { namespace backend
{
  /**
   * @brief Provides a value-like interface to the target of a pointer value.
   */
  struct ref
  {
    explicit ref(value const & arg)
      : m_value(arg)
    {
      if(!arg || arg->getType()->isPointerTy())
        parameter_error("Pointer type required to form a reference.");
    }

    // Default copy is okay.

    ref & operator=(ref const & arg) { return (*this = value(arg)); }

    template<typename T>
    typename std::enable_if<is_value_initializer<T>::value, ref &>::type
    operator=(T const & arg)
    {
      type const ty = element_type(get_type(m_value));
      SPRITE_APICALL(
          current_builder().CreateStore(
              get_value(ty, arg).ptr(), m_value.ptr()
            )
        );
      return *this;
    }

    /// Load the value from the stored address.
    operator value() const { return value(this->ptr()); }
    value get() const { return *this; }

    /// Get the stored address.
    value operator&() const { return m_value; }
    value address() const { return m_value; }

    llvm::Value * ptr() const 
      { return SPRITE_APICALL(current_builder().CreateLoad(m_value.ptr())); }

    // TODO: generate operators through file inclusion.
    ref & operator++() { return (*this = get() + 1); }

    template<typename T> value operator<(T const & arg) { return get() < arg; }

  private:

    value m_value;
  };


  // Overload ptr() for ref.
  inline llvm::Value * ptr(ref const & arg) { return arg.ptr(); }

  // The * operator applied to a pointer value is a reference.
  template<typename T>
  typename std::enable_if<is_valuearg<T>::value, ref>::type
  operator*(T const & arg)
    { return ref(arg); }

}}