/**
 * @file
 * @brief Constructs for inserting instructions into a context.
 */

#pragma once
#include "sprite/llvm/core/context.hpp"
#include "sprite/llvm/core/wrappers.hpp"
#include "sprite/llvm/operators/create.hpp"

#define SPRITE_INSTRUCTION_PREAMBLE_WRAPPED(RetTy, ArgTy)       \
    template<typename Arg>                                      \
    inline typename std::enable_if<                             \
        std::is_convertible<Arg, ConstantWrapper<ArgTy>>::value \
      , InstructionWrapper<RetTy>                               \
      >::type                                                   \
  /**/

#define SPRITE_INSTRUCTION_PREAMBLE_UNWRAPPED(RetTy, ArgTy)      \
    template<typename Arg>                                       \
    inline typename std::enable_if<                              \
        !std::is_convertible<Arg, ConstantWrapper<ArgTy>>::value \
      , InstructionWrapper<RetTy>                                \
      >::type                                                    \
  /**/

namespace sprite { namespace llvm
{
  /**
   * @brief Inserts a return instruction into the active context.
   */
  InstructionWrapper<llvm_::ReturnInst> return_(Value * value)
  {
    auto const & cxt = activeContext();
    return wrap(cxt.factory(), cxt.builder().CreateRet(value));
  }

  /**
   * @brief Inserts a return instruction into the active context.
   *
   * The argument is a @p ConstantWrapper.
   */
  SPRITE_INSTRUCTION_PREAMBLE_WRAPPED(llvm_::ReturnInst, Constant)
  return_(Arg const & arg)
    { return return_(ptr(arg)); }

  /**
   * @brief Inserts a return instruction into the active context.
   *
   * The argument is not a @p ConstantWrapper.  It is converted to a @p Value
   * @p * by constructing the active function's return type using the supplied
   * value.
   */
  SPRITE_INSTRUCTION_PREAMBLE_UNWRAPPED(llvm_::ReturnInst, Constant)
  return_(Arg const & arg)
  {
    auto const & cxt = activeContext();
    auto const retty = wrap(
        cxt.factory()
      , cxt.builder().GetInsertBlock()->getParent()->getReturnType()
      );
    Value * value = ptr(retty % arg);
    return return_(value);
  }
}}
