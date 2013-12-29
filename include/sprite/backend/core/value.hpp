#pragma once
#include "sprite/backend/support/casting.hpp"
#include "sprite/backend/config.hpp"
#include "sprite/backend/core/wrappers_fwd.hpp"
#include "sprite/backend/support/exceptions.hpp"
#include "sprite/backend/support/typenames.hpp"
#include <type_traits>
#include <utility>

namespace sprite { namespace backend
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
  value(constantobj<T, Factory> const & c)
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
      throw value_error("Integer value is too large to return");
    }
    throw type_error(
        "Expected integer value, not " + llvm_typename(c->getType())
      );
  }

  /// Get the value of a constant as an APInt.
  template<typename Target, typename T, typename Factory>
  inline
  typename std::enable_if<
      std::is_constructible<Target, APInt>::value
    , Target
    >::type
  value(constantobj<T, Factory> const & c)
  {
    if(ConstantInt * ci = dyn_cast<ConstantInt>(c.ptr()))
      return ci->getValue();
    throw type_error(
        "Expected integer value, not " + llvm_typename(c->getType())
      );
  }

  /// Get the value of a constant as an APFloat.
  template<typename Target, typename T, typename Factory>
  inline
  typename std::enable_if<
      std::is_same<Target, APFloat>::value, Target const &
    >::type
  value(constantobj<T, Factory> const & c)
  {
    if(ConstantFP * cfp = dyn_cast<ConstantFP>(c.ptr()))
      return cfp->getValueAPF();
    throw type_error("Expected floating-point value");
  }

  /// Get the value of a constant as a float.
  template<typename Target, typename T, typename Factory>
  inline
  typename std::enable_if<
      std::is_same<Target, float>::value, float
    >::type
  value(constantobj<T, Factory> const & c)
  {
    APFloat const & val = value<APFloat>(c);
    llvm::fltSemantics const & sem = val.getSemantics();
    if(&sem == &APFloat::IEEEsingle)
      return val.convertToFloat();
    else if(&sem == &APFloat::IEEEdouble)
      return static_cast<float>(val.convertToDouble());
    throw value_error("Unsupported float semantics");
  }

  /// Get the value of a constant as a double.
  template<typename Target, typename T, typename Factory>
  inline
  typename std::enable_if<
      std::is_same<Target, double>::value, double
    >::type
  value(constantobj<T, Factory> const & c)
  {
    APFloat const & val = value<APFloat>(c);
    llvm::fltSemantics const & sem = val.getSemantics();
    if(&sem == &APFloat::IEEEsingle)
      return val.convertToFloat();
    else if(&sem == &APFloat::IEEEdouble)
      return val.convertToDouble();
    throw value_error("Unsupported float semantics");
  }
}}
