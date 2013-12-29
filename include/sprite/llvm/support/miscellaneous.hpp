#pragma once
#include "sprite/llvm/config.hpp"
#include "sprite/llvm/support/exceptions.hpp"

namespace sprite { namespace llvm
{
  inline unsigned getFPBitWidth(Type * type)
  {
    switch(type->getTypeID())
    {
      case Type::HalfTyID: return 16;
      case Type::FloatTyID: return 32;
      case Type::DoubleTyID: return 64;
      case Type::X86_FP80TyID: return 80;
      case Type::FP128TyID: return 128;
      case Type::PPC_FP128TyID: return 128;
      default: throw type_error("Expected floating-point type");
    }
  }
}}
