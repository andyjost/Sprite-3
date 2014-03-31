#pragma once
#include "sprite/backend/config.hpp"
#include "sprite/backend/core/object.hpp"
#include "llvm/IR/Value.h"

namespace sprite { namespace backend
{
  template<>
  struct valueobj<llvm::Value> : object<llvm::Value>
  {
    using basic_type = Value;
    using object<llvm::Value>::object;

    // Define operators.
    #define SPRITE_INPLACE_OP +=
    #define SPRITE_LHS_TYPE value
    #include "sprite/backend/core/detail/operator.def"
    #define SPRITE_INPLACE_OP -=
    #define SPRITE_LHS_TYPE value
    #include "sprite/backend/core/detail/operator.def"
    #define SPRITE_INPLACE_OP *=
    #define SPRITE_LHS_TYPE value
    #include "sprite/backend/core/detail/operator.def"
    #define SPRITE_INPLACE_OP /=
    #define SPRITE_LHS_TYPE value
    #include "sprite/backend/core/detail/operator.def"
  };

  template<typename T>
  struct valueobj : object<T>
  {
    using basic_type = Value;
    using object<T>::object;

  private:

    static_assert(
        std::is_base_of<basic_type, T>::value, "Expected an LLVM Value object"
      );
  };

  // FIXME: rename typeof_.
  /// Returns the type of a value object.
  template<typename T>
  inline type get_type(valueobj<T> const & arg)
    { return type(arg->getType()); }

}}
