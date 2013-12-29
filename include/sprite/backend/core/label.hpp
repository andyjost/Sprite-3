#pragma once
#include "sprite/backend/core/wrappers.hpp"
#include "sprite/backend/core/type_factory.hpp"

namespace sprite { namespace backend
{
  struct label
  {
    label(llvm::Twine const & name = "");

    basic_block<type_factory> block() const { return m_block; }

  private:

    basic_block<type_factory> m_block;
  };
}}
