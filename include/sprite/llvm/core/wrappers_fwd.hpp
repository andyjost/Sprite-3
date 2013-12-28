#pragma once
#include "sprite/llvm/config.hpp"

namespace sprite { namespace llvm
{
  class TypeFactory;
  template<typename T, typename Factory=TypeFactory> class Wrapper;
  template<typename T, typename Factory=TypeFactory> class TypeWrapper;
  template<typename T, typename Factory=TypeFactory> class ConstantWrapper;
  template<typename T, typename Factory=TypeFactory> class GlobalValueWrapper;
  template<typename T, typename Factory=TypeFactory> class InstructionWrapper;

  // Always wraps an llvm::BasicBlock.
  template<typename Factory=TypeFactory> class BasicBlockWrapper;

  template<typename Factory=TypeFactory>
    using FunctionWrapper = GlobalValueWrapper<llvm_::Function, Factory>;

  namespace aux
  {
    struct Flags;
    template<typename Arg> struct ArgWithFlags;
  }

}}
