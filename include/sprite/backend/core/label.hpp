#pragma once
#include "sprite/backend/config.hpp"
#include "sprite/backend/core/scope.hpp"
#include "sprite/backend/core/value.hpp"
#include <sprite/backend/support/type_traits.hpp>

namespace sprite { namespace backend
{
  // API: label
  template<>
  struct valueobj<llvm::BasicBlock> : object<llvm::BasicBlock>
  {
    using object<llvm::BasicBlock>::object;

    /// Creates a new BasicBlock in the current function scope.
    explicit valueobj(twine const & name = "")
      : object(init(name))
    {}

    /// Creates a new BasicBlock and fills it by calling the function.
    // Implicit conversion is allowed.
    template<
        typename T
      , typename = typename std::enable_if<
            is_code_block_specifier<T>::value
          >::type
      >
    valueobj(T && body, twine const & name = "")
      : object(init(name))
    {
      scope _ = *this;
      body();
    }

  private:

    llvm::BasicBlock * init(twine const &);
  };
}}

