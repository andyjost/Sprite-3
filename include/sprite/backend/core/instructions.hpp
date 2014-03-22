/**
 * @file
 * @brief Constructs for inserting instructions into a scope.
 */

#pragma once
#include "sprite/backend/core/constant.hpp"
#include "sprite/backend/core/detail/current_builder.hpp"
#include "sprite/backend/core/get_constant.hpp"
#include "sprite/backend/core/instruction.hpp"
#include "sprite/backend/core/scope.hpp"
#include "sprite/backend/core/type.hpp"
#include "llvm/IR/Instructions.h"

#define SPRITE_INSTRUCTION_PREAMBLE_WRAPPED(RetTy, ArgTy) \
    template<typename Arg>                                \
    inline typename std::enable_if<                       \
        std::is_convertible<Arg, constobj<ArgTy>>::value  \
      , instrobj<RetTy>                                   \
      >::type                                             \
  /**/

#define SPRITE_INSTRUCTION_PREAMBLE_UNWRAPPED(RetTy, ArgTy) \
    template<typename Arg>                                  \
    inline typename std::enable_if<                         \
        !std::is_convertible<Arg, constobj<ArgTy>>::value   \
      , instrobj<RetTy>                                     \
      >::type                                               \
  /**/

namespace sprite { namespace backend
{
  /**
   * @brief Inserts a return instrobj into the active scope.
   */
  instrobj<llvm::ReturnInst> return_(Value * value)
  {
    llvm::IRBuilder<> & bldr = current_builder();
    return instrobj<llvm::ReturnInst>(
        SPRITE_APICALL(bldr.CreateRet(value))
      );
  }

  /**
   * @brief Inserts a return instruction into the active scope.
   *
   * The argument is a @p constobj.
   */
  SPRITE_INSTRUCTION_PREAMBLE_WRAPPED(llvm::ReturnInst, Constant)
  return_(Arg const & arg)
    { return return_(ptr(arg)); }

  /**
   * @brief Inserts a return instruction into the active scope.
   *
   * The argument is not a @p constobj.  It is converted to a @p Value
   * @p * by constructing the active function's return type using the supplied
   * value.
   */
  SPRITE_INSTRUCTION_PREAMBLE_UNWRAPPED(llvm::ReturnInst, Constant)
  return_(Arg const & arg)
  {
    llvm::IRBuilder<> & bldr = current_builder();
    type const retty = type(bldr.GetInsertBlock()->getParent()->getReturnType());
    Value * value = ptr(retty % arg);
    return return_(value);
  }
}}
