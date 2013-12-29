#pragma once
#include "sprite/llvm/core/wrappers_fwd.hpp"
#include "sprite/llvm/support/enablers.hpp"

namespace sprite { namespace llvm
{
  // ====== Methods for wrapping LLVM objects.

  /// Wraps an llvm_::Type object using the specified factory as context.
  template<typename T, typename Factory>
  typeobj<T,Factory> wrap(
      Factory const & f, T * arg, typename aux::enable_if_type<T>::type=0
    )
  { return typeobj<T, Factory>(arg, f); }

  /// Wraps an llvm_::Constant object using the specified factory as context.
  template<typename T, typename Factory>
  constantobj<T, Factory> wrap(
      Factory const & f, T * arg
    , typename aux::enable_if_constant_not_global<T>::type=0
    )
  { return constantobj<T, Factory>(arg, f); }

  /// Wraps an GlobalValue object using the specified factory as context.
  template<typename T, typename Factory>
  globalobj<T, Factory> wrap(
      Factory const & f, T * arg
    , typename aux::enable_if_global_value<T>::type=0
    )
  { return globalobj<T, Factory>(arg, f); }

  /// Wraps an llvm_::BasicBlock object.
  template<typename Factory>
  basic_block<Factory> wrap(
      Factory const & f, llvm_::BasicBlock * arg
    )
  { return basic_block<Factory>(arg, f); }

  /// Wraps an llvm_::Instruction object.
  template<typename T, typename Factory>
  instruction<T> wrap(
      Factory const & f, T * arg
    , typename aux::enable_if_instruction<T>::type=0
    )
  { return instruction<T, Factory>(arg, f); }

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

