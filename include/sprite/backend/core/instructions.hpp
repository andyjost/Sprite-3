/**
 * @file
 * @brief Constructs for inserting instructions into a context.
 */

#pragma once
#include "sprite/backend/core/context.hpp"
#include "sprite/backend/core/wrappers.hpp"
#include "sprite/backend/operators/create.hpp"

#define SPRITE_INSTRUCTION_PREAMBLE_WRAPPED(RetTy, ArgTy)       \
    template<typename Arg>                                      \
    inline typename std::enable_if<                             \
        std::is_convertible<Arg, constantobj<ArgTy>>::value \
      , instruction<RetTy>                               \
      >::type                                                   \
  /**/

#define SPRITE_INSTRUCTION_PREAMBLE_UNWRAPPED(RetTy, ArgTy)      \
    template<typename Arg>                                       \
    inline typename std::enable_if<                              \
        !std::is_convertible<Arg, constantobj<ArgTy>>::value \
      , instruction<RetTy>                                \
      >::type                                                    \
  /**/

namespace sprite { namespace backend
{
  /**
   * @brief Inserts a return instruction into the active context.
   */
  instruction<llvm::ReturnInst> return_(Value * value)
  {
    auto const & cxt = active_context();
    return wrap(cxt.factory(), cxt.builder().CreateRet(value));
  }

  /**
   * @brief Inserts a return instruction into the active context.
   *
   * The argument is a @p constantobj.
   */
  SPRITE_INSTRUCTION_PREAMBLE_WRAPPED(llvm::ReturnInst, Constant)
  return_(Arg const & arg)
    { return return_(ptr(arg)); }

  /**
   * @brief Inserts a return instruction into the active context.
   *
   * The argument is not a @p constantobj.  It is converted to a @p Value
   * @p * by constructing the active function's return type using the supplied
   * value.
   */
  SPRITE_INSTRUCTION_PREAMBLE_UNWRAPPED(llvm::ReturnInst, Constant)
  return_(Arg const & arg)
  {
    auto const & cxt = active_context();
    auto const retty = wrap(
        cxt.factory()
      , cxt.builder().GetInsertBlock()->getParent()->getReturnType()
      );
    Value * value = ptr(retty % arg);
    return return_(value);
  }
}}
