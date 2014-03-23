#pragma once
#include "sprite/backend/core/constant.hpp"
#include "sprite/backend/core/operator_flags.hpp"
#include "sprite/backend/core/value.hpp"

namespace sprite { namespace backend
{
  //@{
  /**
   * @brief Performs a type-casting operation.
   *
   * Creates one of the following LLVM instructions: trunc, sext, zext,
   * fptrunc, fpextend, uitofp, sitofp, fptoui, fpsosi, ptrtoint, or inttoptr.
   *
   * @snippet constexprs.cpp typecast
   */
  constant typecast(constant const &, aux::arg_with_flags<type> const &);
  value typecast(value const &, aux::arg_with_flags<type> const &);

  template<typename T>
  constant typecast(constobj<T> const & obj, aux::arg_with_flags<type> const & ty)
    { return typecast(constant(obj), ty); }

  template<typename T>
  value typecast(valueobj<T> const & obj, aux::arg_with_flags<type> const & ty)
    { return typecast(value(obj), ty); }

  //@}

  //@{
  /**
   * @brief Performs a bit-casting operation.
   *
   * @snippet constexprs.cpp bitcast
   */
  constant bitcast(constant const &, type const &);
  value bitcast(value const &, type const &);

  template<typename T>
  constant bitcast(constobj<T> const & obj, type const & ty)
    { return bitcast(constant(obj), ty); }

  template<typename T>
  value bitcast(valueobj<T> const & obj, type const & ty)
    { return bitcast(value(obj), ty); }

  //@}
}}

