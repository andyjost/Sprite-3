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
    type const ty((&*this)->getType()->getPointerElementType());
    (&*this)->setInitializer(get_constant_impl(ty, value).ptr());
    return *this;
  }

  inline globalvar & globalobj<GlobalVariable>::set_initializer(any_array_ref const & value)
  {
    type const ty((&*this)->getType()->getPointerElementType());
    (&*this)->setInitializer(get_constant_impl(ty, value).ptr());
    return *this;
  }

  template<typename T>
  template<typename U, typename>
  globalvar globalobj<T>::set_initializer(U const & value)
  {
    // TODO support/casting.hpp probably needs to understand basic_reference.
    if(auto * g = dyn_cast<GlobalVariable>(this->ptr()))
      return globalvar(globalvaraddr(g)).set_initializer(value);
    // auto g = dyn_cast<globalvar>(*this);
    // if(g.ptr())
    //   return g.set_initializer(value);
    throw type_error("Expected GlobalVariable.");
  }

  template<typename T>
  globalvar globalobj<T>::set_initializer(any_array_ref const & value)
  {
    // TODO support/casting.hpp probably needs to understand basic_reference.
    if(auto * g = dyn_cast<GlobalVariable>(this->ptr()))
      return globalvar(globalvaraddr(g)).set_initializer(value);
  //   auto g = dyn_cast<globalvar>(*this);
  //   if(g.ptr())
  //     return g.set_initializer(value);
    throw type_error("Expected GlobalVariable.");
  }
}}
