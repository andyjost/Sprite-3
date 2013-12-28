/**
 * @file
 * @brief Implements the assignment (=) operator.
 *
 * The assignment operator is used for setting an initializer.
 *
 * @note The assignment operator is required to be defined as a class member.
 * Setting the initializer is implemented as a named function @p
 * set_initializer that @p GlobalValueWrapper::operator= delegates to.
 */

#pragma once
#include "sprite/llvm/core/wrappers.hpp"
#include "sprite/llvm/operators/preamble.hpp"
#include "sprite/llvm/operators/create.hpp"

namespace sprite { namespace llvm
{
  namespace aux
  {
    /**
     * @brief Helper used by GlobalValueWrapper to get a global variable and
     * its element type.
     */
    template<typename T, typename Factory>
    std::pair<llvm_::GlobalVariable *, TypeWrapper<Type, Factory>>
    get_global_and_type(T * arg, Factory const & f)
    {
      if(auto const global = dyn_cast<llvm_::GlobalVariable>(arg))
      {
        // Global variables are always pointer types.  Get the underlying
        // element type.
        auto const ty_p = cast<PointerType>(global->getType());
        auto const ty = wrap(f, ty_p->getElementType());

        return std::make_pair(global, ty);
      }
      else
        throw TypeError("GlobalVariable required");
    }
  }

  template<typename T, typename Factory>
  template<typename U>
  GlobalValueWrapper<T, Factory> &
  GlobalValueWrapper<T, Factory>::set_initializer(U const & value)
  {
    auto pair = aux::get_global_and_type(this->ptr(), this->factory());
    pair.first->setInitializer((pair.second % value).ptr());
    return *this;
  }

  /**
   * @brief Sets the initializer for a global variable using an aggregate.
   *
   * Synonymous with @p operator=.
   */
  template<typename T, typename Factory>
  template<typename U>
  GlobalValueWrapper<T, Factory> &
  GlobalValueWrapper<T, Factory>::set_initializer(std::initializer_list<U> const & values)
  {
    auto pair = aux::get_global_and_type(this->ptr(), this->factory());
    pair.first->setInitializer((pair.second % values).ptr());
    return *this;
  }

  /**
   * @brief Sets the initializer for a global variable from heterogeneous data.
   *
   * Synonymous with @p operator=.
   */
  template<typename T, typename Factory>
  template<typename...U>
  GlobalValueWrapper<T, Factory> &
  GlobalValueWrapper<T, Factory>::set_initializer(std::tuple<U...> const & values)
  {
    auto pair = aux::get_global_and_type(this->ptr(), this->factory());
    pair.first->setInitializer((pair.second % values).ptr());
    return *this;
  }
}}