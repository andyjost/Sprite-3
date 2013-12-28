#pragma once
#include "sprite/llvm/core/wrappers_fwd.hpp"
#include "sprite/llvm/support/enablers.hpp"

namespace sprite { namespace llvm
{
  // ====== Methods for wrapping LLVM objects.

  /// Wraps an llvm_::Type object using the specified factory as context.
  template<typename T, typename Factory>
  TypeWrapper<T,Factory> wrap(
      Factory const & f, T * arg, typename aux::EnableIfLlvmType<T>::type=0
    )
  { return TypeWrapper<T, Factory>(arg, f); }

  /// Wraps an llvm_::Constant object using the specified factory as context.
  template<typename T, typename Factory>
  ConstantWrapper<T, Factory> wrap(
      Factory const & f, T * arg
    , typename aux::EnableIfLlvmConstantNotGlobal<T>::type=0
    )
  { return ConstantWrapper<T, Factory>(arg, f); }

  /// Wraps an llvm_::GlobalValue object using the specified factory as context.
  template<typename T, typename Factory>
  GlobalValueWrapper<T, Factory> wrap(
      Factory const & f, T * arg
    , typename aux::EnableIfLlvmGlobalValue<T>::type=0
    )
  { return GlobalValueWrapper<T, Factory>(arg, f); }

  /// Wraps an llvm_::BasicBlock object.
  template<typename Factory>
  BasicBlockWrapper<Factory> wrap(
      Factory const & f, llvm_::BasicBlock * arg
    )
  { return BasicBlockWrapper<Factory>(arg, f); }

  /// Wraps an llvm_::Instruction object.
  template<typename T, typename Factory>
  InstructionWrapper<T> wrap(
      Factory const & f, T * arg
    , typename aux::EnableIfLlvmInstruction<T>::type=0
    )
  { return InstructionWrapper<T, Factory>(arg, f); }

  /**
   * @brief Combines a @p wrap operation with @p cast.
   *
   * Useful when LLVM returns a less specific object, such as <tt>Constant
   * *</tt>, that we know must in fact be a more specific one, say
   * <tt>ConstantFP *</tt>, in some circumstance.
   */
  template<typename Tgt, typename Src, typename Factory>
  auto wrap(Factory const & f, Src * arg) -> decltype(wrap(f, (Tgt *)(0)))
    { return wrap(f, llvm_::cast<Tgt>(arg)); }
}}

