#pragma once
#include "sprite/backend/config.hpp"

namespace sprite { namespace backend
{
  class type_factory;
  template<typename T, typename Factory=type_factory> class object;
  template<typename T=Type, typename Factory=type_factory> class typeobj;
  template<typename T=Constant, typename Factory=type_factory> class constantobj;
  template<typename T=GlobalValue, typename Factory=type_factory> class globalobj;
  template<typename T=Instruction, typename Factory=type_factory> class instruction;

  // Always wraps an llvm::BasicBlock.
  template<typename Factory=type_factory> class basic_block;

  template<typename Factory=type_factory>
    using function = globalobj<Function, Factory>;

  using type = typeobj<>;
  using array_type = typeobj<ArrayType>;
  using integer_type = typeobj<IntegerType>;
  using function_type = typeobj<FunctionType>;
  using fp_type = typeobj<FPType>;
  using pointer_type = typeobj<PointerType>;
  using struct_type = typeobj<StructType>;

  using global = globalobj<>;

  namespace aux
  {
    struct operator_flags;
    template<typename Arg> struct arg_with_flags;
  }

}}
