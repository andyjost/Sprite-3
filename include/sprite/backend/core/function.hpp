#pragma once
#include "sprite/backend/core/global.hpp"
#include "sprite/backend/core/instruction.hpp"
#include "sprite/backend/core/label.hpp"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"

namespace sprite { namespace backend
{
  template<> struct globalobj<Function> : constobj<Function>
  {
    using basic_type = Function;
    using constobj<Function>::constobj;

    /**
     * @brief Inserts a call instruction in the current context.
     *
     * Each argument can be a value, or raw initializer.  A raw initializer is
     * any object -- such as a built-in integer or floating-point value, to
     * name two -- that can be used to initialize a constant.
     */
    template<
        typename... Args
      , SPRITE_ENABLE_FOR_VALUE_INITIALIZERS(Args...)
      >
    instrobj<llvm::CallInst> operator()(Args &&... args) const;

    /// Returns the function entry point.
    label entry() const;
  };
}}

#include "sprite/backend/core/detail/function_impl.hpp"

