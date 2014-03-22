#pragma once
#include "sprite/backend/core/value.hpp"

namespace sprite { namespace backend
{
  template<typename T> struct instrobj : valueobj<T>
  {
    using basic_type = Instruction;
    using valueobj<T>::valueobj;

  private:

    static_assert(
        std::is_base_of<basic_type, T>::value, "Expected an LLVM Instrution object"
      );
  };
}}
