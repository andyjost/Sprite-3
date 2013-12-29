#pragma once
#include "sprite/llvm/core/wrappers.hpp"
#include "sprite/llvm/core/type_factory.hpp"

namespace sprite { namespace llvm
{
  struct label
  {
    label(llvm_::Twine const & name = "");

    basic_block<type_factory> block() const { return m_block; }

  private:

    basic_block<type_factory> m_block;
  };
}}
