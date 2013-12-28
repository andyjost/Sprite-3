#pragma once
#include "sprite/llvm/support/casting.hpp"
#include "sprite/llvm/config.hpp"
#include "sprite/llvm/core/wrappers_fwd.hpp"
#include "sprite/llvm/support/exceptions.hpp"
#include "sprite/llvm/support/typenames.hpp"
#include <type_traits>
#include <utility>

namespace sprite { namespace llvm
{
  namespace aux
  {
    template<typename T>
    constexpr bool is_ok_integer_for_value()
    {
      typedef decltype(std::declval<ConstantInt*>()->getSExtValue()) return_type;
      return std::is_integral<T>::value && sizeof(T) <= sizeof(return_type);
    }

    template<typename T>
    constexpr bool is_ok_fp_for_value()
      { return std::is_same<T, double>::value || std::is_same<T, float>::value; }
  }

  /// Get the value of a constant as a user-specified integral type.
  template<typename Target, typename T, typename Factory>
  inline
  typename std::enable_if<aux::is_ok_integer_for_value<Target>(), Target>::type
  value(ConstantWrapper<T, Factory> const & c)
  {
    if(ConstantInt * ci = dyn_cast<ConstantInt>(c.ptr()))
    {
      if(ci->getBitWidth() <= sizeof(Target) * 8)
      {
        if(std::is_signed<Target>::value)
          return ci->getSExtValue();
        else
          return ci->getZExtValue();
      }
      throw ValueError("Integer value is too large to return");
    }
    throw TypeError(
        "Expected integer value, not " + llvm_typename(c->getType())
      );
  }

  /// Get the value of a constant as an APInt.
  template<typename Target, typename T, typename Factory>
  inline
  typename std::enable_if<
      std::is_constructible<Target, llvm_::APInt>::value
    , Target
    >::type
  value(ConstantWrapper<T, Factory> const & c)
  {
    if(ConstantInt * ci = dyn_cast<ConstantInt>(c.ptr()))
      return ci->getValue();
    throw TypeError(
        "Expected integer value, not " + llvm_typename(c->getType())
      );
  }

  /// Get the value of a constant as an llvm::APFloat.
  template<typename Target, typename T, typename Factory>
  inline
  typename std::enable_if<
      std::is_same<Target, llvm_::APFloat>::value, Target const &
    >::type
  value(ConstantWrapper<T, Factory> const & c)
  {
    if(ConstantFP * cfp = dyn_cast<ConstantFP>(c.ptr()))
      return cfp->getValueAPF();
    throw TypeError("Expected floating-point value");
  }

  /// Get the value of a constant as a float.
  template<typename Target, typename T, typename Factory>
  inline
  typename std::enable_if<
      std::is_same<Target, float>::value, float
    >::type
  value(ConstantWrapper<T, Factory> const & c)
  {
    llvm_::APFloat const & val = value<llvm_::APFloat>(c);
    llvm_::fltSemantics const & sem = val.getSemantics();
    if(&sem == &llvm_::APFloat::IEEEsingle)
      return val.convertToFloat();
    else if(&sem == &llvm_::APFloat::IEEEdouble)
      return static_cast<float>(val.convertToDouble());
    throw ValueError("Unsupported float semantics");
  }

  /// Get the value of a constant as a double.
  template<typename Target, typename T, typename Factory>
  inline
  typename std::enable_if<
      std::is_same<Target, double>::value, double
    >::type
  value(ConstantWrapper<T, Factory> const & c)
  {
    llvm_::APFloat const & val = value<llvm_::APFloat>(c);
    llvm_::fltSemantics const & sem = val.getSemantics();
    if(&sem == &llvm_::APFloat::IEEEsingle)
      return val.convertToFloat();
    else if(&sem == &llvm_::APFloat::IEEEdouble)
      return val.convertToDouble();
    throw ValueError("Unsupported float semantics");
  }
}}
