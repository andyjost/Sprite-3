#pragma once
#include "sprite/llvm/core/wrappers.hpp"
#include "sprite/llvm/core/type_factory.hpp"

namespace sprite { namespace llvm
{
  struct Label
  {
    Label(llvm_::Twine const & name = "");

    BasicBlockWrapper<TypeFactory> block() const { return m_block; }

  private:

    BasicBlockWrapper<TypeFactory> m_block;
  };
}}
