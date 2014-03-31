#pragma once
#include "sprite/backend/config.hpp"
#include "sprite/backend/core/type.hpp"
#include "sprite/backend/core/value.hpp"
#include "llvm/IR/Constants.h"

namespace sprite { namespace backend
{
  // API: constant
  template<>
  struct constobj<llvm::Constant> : valueobj<llvm::Constant>
  {
    using basic_type = Constant;
    using valueobj<llvm::Constant>::valueobj;

    /// Gets an element of a constant aggregate.
    constant operator[](size_t i) const
    {
      auto const ii = static_cast<unsigned>(i);
      return constant(SPRITE_APICALL((*this)->getAggregateElement(ii)));
    }

    // Define operators.
    #define SPRITE_INPLACE_OP +=
    #define SPRITE_LHS_TYPE constant
    #include "sprite/backend/core/detail/operator.def"
    #define SPRITE_INPLACE_OP -=
    #define SPRITE_LHS_TYPE constant
    #include "sprite/backend/core/detail/operator.def"
    #define SPRITE_INPLACE_OP *=
    #define SPRITE_LHS_TYPE constant
    #include "sprite/backend/core/detail/operator.def"
    #define SPRITE_INPLACE_OP /=
    #define SPRITE_LHS_TYPE constant
    #include "sprite/backend/core/detail/operator.def"
    #define SPRITE_INPLACE_OP %=
    #define SPRITE_LHS_TYPE constant
    #include "sprite/backend/core/detail/operator.def"
  };

  template<typename T>
  struct constobj : valueobj<T>
  {
    using basic_type = Constant;
    using valueobj<T>::valueobj;

    /// Gets an element of a constant aggregate.
    constant operator[](size_t i) const
    {
      auto const ii = static_cast<unsigned>(i);
      return constant(SPRITE_APICALL((*this)->getAggregateElement(ii)));
    }

  private:

    static_assert(
        std::is_base_of<basic_type, T>::value, "Expected an LLVM Constant object"
      );
  };
}}
