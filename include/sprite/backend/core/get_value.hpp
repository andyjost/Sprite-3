#pragma once
#include "sprite/backend/core/get_constant.hpp"
#include "sprite/backend/core/castexpr.hpp"
#include "sprite/backend/core/value.hpp"

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
  // Applies when T can produce an LLVM Value.
  template<typename T>
  inline typename std::enable_if<is_valuearg<T>::value, value>::type
  get_value(T && arg
    )
  { return value(ptr(std::forward<T>(arg))); }

  // Applies when T is a raw initializer (e.g., an int).  Builds a constant.
  template<typename T>
  inline typename std::enable_if<!is_valuearg<T>::value, constant>::type
  get_value(T && arg
    )
  {
    using BasicT = typename std::remove_reference<T>::type;
    using T_ = typename std::decay<BasicT>::type;
    return get_constant<T_>(std::forward<T>(arg));
  }
  //@}

  //@{
  /**
   * @brief Prepares a value of the specified type from any suitable argument.
   *
   * If the input object is already an LLVM Value, then its API pointer value
   * is extracted and converted.  Otherwise, i.e., in the case of a raw
   * initializer, a constant value is created using @p get_constant.
   */
  // Applies when T can produce an LLVM Value.
  template<typename T>
  inline typename std::enable_if<is_valuearg<T>::value, value>::type
  get_value(aux::arg_with_flags<type> const & ty, T && arg
    )
  { return typecast(get_value(arg), ty); }

  // Applies when T is a raw initializer (e.g., an int).  Builds a constant.
  template<typename T>
  inline typename std::enable_if<!is_valuearg<T>::value, constant>::type
  get_value(aux::arg_with_flags<type> const & ty, T && arg
    )
  { return ty % std::forward<T>(arg); }
  //@}
}}
