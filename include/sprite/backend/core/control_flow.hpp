/**
 * @file
 * @brief Implements functions that influence control flow.
 */

#pragma once
#include "sprite/backend/core/descriptors.hpp"
#include "sprite/backend/core/detail/current_builder.hpp"
#include "sprite/backend/core/function.hpp"
#include "sprite/backend/core/get_value.hpp"
#include "sprite/backend/core/label.hpp"
#include "sprite/backend/core/scope.hpp"
#include "sprite/backend/core/type.hpp"
#include "llvm/IR/Instructions.h"

namespace sprite { namespace backend
{
  //@{
  /// Appends a return instruction to the active label scope.
  instruction return_();

  template<typename T>
  instruction return_(T && arg)
  {
    type const retty = scope::current_function().return_type();
    if(retty->isVoidTy())
    {
      throw type_error(
          "A return value may not be supplied for functions returning void."
        );
    }
    value const x = get_value(retty, arg);
    return instruction(SPRITE_APICALL(current_builder().CreateRet(x.ptr())));
  }
  //@}

  //@{
  /// Appends a conditional branch instruction to the active label scope.
  instruction if_(
      branch_condition const & cond
    , labeldescr const & true_, labeldescr const & false_
    );

  instruction if_(branch_condition const & cond, labeldescr const & true_);
  //@}

  /// Appends an unconditional branch instruction to the active label scope.
  inline instruction goto_(label const & target)
  {
    llvm::IRBuilder<> & bldr = current_builder();
    return instruction(SPRITE_APICALL(bldr.CreateBr(target.ptr())));
  }

  /// Creates a while loop.
  instruction while_(loop_condition const & cond, labeldescr const & body);

  /// Creates an unconditional branch that escapes the nearest enclosing loop.
  instruction break_();
  
}}
