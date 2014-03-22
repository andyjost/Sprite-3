#pragma once
#include "sprite/backend/core/operator_flags.hpp"
#include "sprite/backend/core/fwd.hpp"

namespace sprite { namespace backend
{
  /**
   * @brief Performs a type-casting operation.
   *
   * Creates one of the following LLVM instructions: trunc, sext, zext,
   * fptrunc, fpextend, uitofp, sitofp, fptoui, fpsosi, ptrtoint, or inttoptr.
   *
   * @snippet constexprs.cpp typecast
   */
  constant typecast(constant const &, aux::arg_with_flags<type> const &);

  /**
   * @brief Performs a bit-casting operation.
   *
   * @snippet constexprs.cpp bitcast
   */
  constant bitcast(constant const &, type const &);
}}

