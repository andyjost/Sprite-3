#include "sprite/backend/support/casting.hpp"
#include "sprite/backend/support/exceptions.hpp"
#include "sprite/backend/core/get_constant.hpp"

namespace sprite { namespace backend
{
  template<typename U, typename>
  globalvar & globalobj<GlobalVariable>::set_initializer(U const & value)
  {
    // Global variables are always pointer types.  Get the underlying
    // element type.
    type const ty((*this)->getType()->getPointerElementType());
    (*this)->setInitializer(get_constant_impl(ty, value).ptr());
    return *this;
  }

  inline globalvar & globalobj<GlobalVariable>::set_initializer(any_array_ref const & value)
  {
    type const ty((*this)->getType()->getPointerElementType());
    (*this)->setInitializer(get_constant_impl(ty, value).ptr());
    return *this;
  }

  template<typename T>
  template<typename U, typename>
  globalvar globalobj<T>::set_initializer(U const & value)
  {
    auto g = dyn_cast<globalvar>(*this);
    if(g.ptr())
      return g.set_initializer(value);
    throw type_error("Expected GlobalVariable.");
  }

  template<typename T>
  globalvar globalobj<T>::set_initializer(any_array_ref const & value)
  {
    auto g = dyn_cast<globalvar>(*this);
    if(g.ptr())
      return g.set_initializer(value);
    throw type_error("Expected GlobalVariable.");
  }

  template<typename T>
  aux::address_calculation<GlobalVariable>
  globalobj<T>::operator[](size_t i) const
  {
    auto g = dyn_cast<globalvar>(*this);
    if(g.ptr())
      return std::move(g[i]);
    throw type_error("Expected GlobalVariable.");
  }

  template<typename T>
  template<typename Index>
  typename std::enable_if<
      is_constarg<Index>::value, aux::address_calculation<GlobalVariable>
    >::type
  globalobj<T>::operator[](Index const & i) const
  {
    auto g = dyn_cast<globalvar>(*this);
    if(g.ptr())
      return std::move(g[i]);
    throw type_error("Expected GlobalVariable.");
  }

  template<typename T>
  constant globalobj<T>::operator&() const
  {
    auto g = dyn_cast<globalvar>(*this);
    if(g.ptr())
      return &g; // Overloaded!  Not the address of a local.
    auto f = dyn_cast<function>(*this);
    if(f.ptr())
      return &f; // Overloaded!  Not the address of a local.
    throw type_error("Expected GlobalVariable or Function.");
  }

  namespace aux
  {
    template<typename T>
    inline constant address_calculation<T>::operator&() const
    {
      return constant(SPRITE_APICALL(
          ConstantExpr::getInBoundsGetElementPtr(
              ptr(this->base), this->indices
            )
        ));
    }

    template<typename T>
    template<typename Index>
    inline
    typename std::enable_if<
        is_constarg<Index>::value, address_calculation<T> &
      >::type
    address_calculation<T>::operator[](Index const & i)
    {
      this->indices.push_back(i.ptr());
      return *this;
    }

    template<typename T>
    inline address_calculation<T> &
    address_calculation<T>::operator[](size_t i)
    {
      auto const i64 = types::int_(64);
      this->indices.push_back(get_constant_impl(i64, i).ptr());
      return *this;
    }
  }

  namespace aux
  {
    inline constant addressof_impl(Constant * ptr)
    {
      auto const i64 = types::int_(64);
      return constant(SPRITE_APICALL(
          ConstantExpr::getInBoundsGetElementPtr(
              ptr, get_constant_impl(i64, 0).ptr()
            )
        ));
    }
  }

  inline constant globalobj<GlobalVariable>::operator&() const
    { return aux::addressof_impl(this->ptr()); }

  template<typename Index>
  inline
  typename std::enable_if<
      is_constarg<Index>::value, aux::address_calculation<GlobalVariable>
    >::type
  globalobj<GlobalVariable>::operator[](Index const & i) const
  {
    auto const i64 = types::int_(64);
    aux::address_calculation<GlobalVariable> tmp(*this);
    tmp.indices.push_back(get_constant_impl(i64, 0).ptr());
    tmp.indices.push_back(i.ptr());
    return std::move(tmp);
  }

  inline aux::address_calculation<GlobalVariable>
  globalobj<GlobalVariable>::operator[](size_t i) const
  {
    auto const i64 = types::int_(64);
    aux::address_calculation<GlobalVariable> tmp(*this);
    tmp.indices.push_back(get_constant_impl(i64, 0).ptr());
    tmp.indices.push_back(get_constant_impl(i64, i).ptr());
    return std::move(tmp);
  }
}}
