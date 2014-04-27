#pragma once
#include "sprite/backend/core/global.hpp"
#include "sprite/backend/core/value.hpp"
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
    value operator()(Args &&... args) const;

    /// Returns the function entry point.
    label entry() const;

    /// Returns the last basic block, which is the current insertion point.
    // label insertion() const;

    /// Returns the function return type.
    type return_type() const
      { return type(SPRITE_APICALL(ptr()->getReturnType())); }
  };

  /// Get an argument (by its position) for the function currently in scope.
  value arg(size_t);

  /// Get an argument (by its name) for the function currently in scope.
  value arg(string_ref const &);
}}

#include "sprite/backend/core/detail/function_impl.hpp"

