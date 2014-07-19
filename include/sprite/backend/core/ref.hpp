#pragma once
#include "sprite/backend/config.hpp"
#include "sprite/backend/core/detail/current_builder.hpp"
#include "sprite/backend/core/get_constant.hpp"
#include "sprite/backend/core/get_value.hpp"
#include "sprite/backend/core/operations.hpp"
#include "sprite/backend/core/value.hpp"
#include "sprite/backend/core/operations.hpp"
#include "sprite/backend/support/exceptions.hpp"

namespace sprite { namespace backend
{
  /**
   * @brief Provides a value-like interface to the target of a pointer value.
   */
  template<typename ValueType>
  struct basic_reference
  {
    basic_reference(std::nullptr_t) : m_value(nullptr) {}

    basic_reference(ValueType const & arg) : m_value(nullptr)
      { this->reset(arg); }

    void reset(ValueType const & arg)
    {
      m_value = arg;
      if(m_value.ptr() && !m_value->getType()->isPointerTy())
        throw type_error("Pointer type required to form a reference.");
    }
    template<typename T>
    void reset(basic_reference<T> const & arg)
      { this->reset(arg.m_value); }

    // Default copy is okay.

    basic_reference & operator=(basic_reference const & arg)
      { return (*this = ValueType(arg)); }

    template<typename T, SPRITE_ENABLE_FOR_ALL_VALUE_INITIALIZERS(T)>
    ref & operator=(T const & arg);

    /// Performs member access into a struct.
    ref dot(uint32_t i) const;
    ref arrow(uint32_t i) const { return get().arrow(i); }

    /// See function::operator().
    template<
        typename... Args
      , SPRITE_ENABLE_FOR_ALL_VALUE_INITIALIZERS(Args...)
      >
    value operator()(Args &&... args) const
      { return get()(std::forward<Args...>(args)...); }

    /// Load the value from the stored address.
    operator ValueType() const { return ValueType(this->ptr()); }
    ValueType get() const { return *this; }

    /// Get the stored address.
    ValueType operator&() const { return m_value; }
    ValueType address() const { return m_value; }

    template<typename T, SPRITE_ENABLE_FOR_ALL_VALUE_INITIALIZERS(T)>
    ref operator[](T const & arg) const;

    llvm::Value * ptr() const 
      { return SPRITE_APICALL(current_builder().CreateLoad(m_value.ptr())); }

    // TODO: generate operators through file inclusion.
    basic_reference & operator++() { return (*this = get() + 1); }

    template<typename T> value operator<(T const & arg) { return get() < arg; }

  private:

    ValueType m_value;

    static_assert(
        std::is_base_of<llvm::Value, typename ValueType::element_type>::value
      , "Expected an LLVM Value object"
      );
  };

  // Overload ptr() for basic_reference.
  template<typename ValueType>
  inline llvm::Value * ptr(basic_reference<ValueType> const & arg)
    { return arg.ptr(); }

  // The * operator applied to a pointer value is a reference.
  template<typename T>
  typename std::enable_if<is_valuearg<T>::value, basic_reference<T>>::type
  operator*(T const & arg)
    { return basic_reference<T>(arg); }

  template<typename ValueType>
  template<typename T, typename>
  ref basic_reference<ValueType>::operator[](T const & arg) const
  {
    llvm::Value * tmp[2] {get_constant<int32_t>(0).ptr(), get_value(arg).ptr()};
    try
    {
      auto & bldr = current_builder();
      value v(
          SPRITE_APICALL(bldr.CreateGEP(address().ptr(), array_ref<llvm::Value*>(tmp)))
        );
      return ref(v);
    }
    catch(scope_error const &)
    {
      using namespace llvm;
      // If all arguments are constants, then this is just a constant expression.
      if(Constant *pc = dyn_cast<Constant>(address().ptr()))
      if(Constant * arg0 = dyn_cast<Constant>(tmp[0]))
      if(Constant * arg1 = dyn_cast<Constant>(tmp[1]))
      {
        Constant * tmp2[2] {arg0, arg1};
        return value(ConstantFolder().CreateGetElementPtr(pc, tmp2));
      }
      throw;
    }
  }

  template<typename ValueType>
  template<typename T, typename>
  inline ref & basic_reference<ValueType>::operator=(T const & arg)
  {
    type const ty = element_type(get_type(m_value));
    SPRITE_APICALL(
        current_builder().CreateStore(
            get_value(ty, arg).ptr(), m_value.ptr()
          )
      );
    return *this;
  }

  template<typename ValueType>
  inline ref basic_reference<ValueType>::dot(uint32_t i) const
  {
    type const i32 = types::int_(32);
    Value * tmp[2] {get_constant(i32,0).ptr(), get_constant(i32, i).ptr()};
    auto & bldr = current_builder();
    value v(
        SPRITE_APICALL(bldr.CreateGEP(address().ptr(), array_ref<Value*>(tmp)))
      );
    return ref(v);
  }

  inline ref valueobj<llvm::Value>::arrow(uint32_t i) const
  {
    ref r(*this);
    return r.dot(i);
  }

  //@{
  /// Allocates a local variable.  Returns a @p ref.
  template<typename T>
  inline typename std::enable_if<is_value_initializer<T>::value, ref>::type
  local(
      type const & ty, T const & size, unsigned alignment=0
    )
  {
    llvm::IRBuilder<> & bldr = current_builder();
    llvm::AllocaInst * px =
        SPRITE_APICALL(bldr.CreateAlloca(ty.ptr(), get_value(size).ptr()));
    if(alignment) SPRITE_APICALL(px->setAlignment(alignment));
    return *value(px);
  }

  inline ref local(type const & ty) { return local(ty, value(nullptr)); }
  //@}
}}
