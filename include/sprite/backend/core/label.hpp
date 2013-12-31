#pragma once
#include "sprite/backend/core/wrappers.hpp"
#include "sprite/backend/core/module.hpp"

namespace sprite { namespace backend
{
  struct label
  {
    label(twine const & name = "");

    basic_block<module> block() const { return m_block; }

  private:

    basic_block<module> m_block;
  };
}}
