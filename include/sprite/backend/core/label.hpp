#pragma once
#include "sprite/backend/config.hpp"
#include "sprite/backend/core/object.hpp"

namespace sprite { namespace backend
{
  template<>
  struct valueobj<llvm::BasicBlock> : object<llvm::BasicBlock>
  {
    using object<llvm::BasicBlock>::object;
  };
}}

