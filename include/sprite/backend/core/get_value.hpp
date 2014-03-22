#pragma once
#include "sprite/backend/core/get_constant.hpp"

namespace sprite { namespace backend
{
  //@{
  /**
   * @brief Prepares a value from any suitable argument.
   *
   * If the input object is already an LLVM Value, then its API pointer value
   * is extracted.  Otherwise, i.e., in the case of a raw initializer, a
   * constant value is created using @p get_constant.
   */
  // T can already produce an LLVM Value.
  template<typename T>
  inline Value * get_value(T && arg
    , typename std::enable_if<is_valuearg<T>::value>::type* = nullptr
    )
  { return ptr(std::forward<T>(arg)); }

  // T is a raw initializer (e.g., an int).  Use it to make a constant.
  template<typename T>
  inline Value * get_value(T && arg
    , typename std::enable_if<!is_valuearg<T>::value>::type* = nullptr
    )
  {
    using BasicT = typename std::remove_reference<T>::type;
    using T_ = typename std::decay<BasicT>::type;
    return ptr(get_constant<T_>(std::forward<T>(arg)));
  }
  //@}
}}
