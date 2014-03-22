#include "sprite/backend/core/constant.hpp"
#include <type_traits>

namespace sprite { namespace backend
{
  namespace aux
  {
    // The integer type requested (when returning a value) must be no larger
    // than that largest supported by LLVM.
    template<typename T>
    constexpr bool is_ok_integer_for_value()
    {
      typedef decltype(std::declval<ConstantInt*>()->getSExtValue()) return_type;
      return std::is_integral<T>::value && sizeof(T) <= sizeof(return_type);
    }

    template<typename T>
    constexpr bool is_ok_fp_for_value()
    {
      return std::is_same<T, float>::value
        || std::is_same<T, double>::value
        || std::is_same<T, long double>::value;
    }
  }

  //@{
  /// Gets the value of a literal constant as an APInt.
  template<typename Target>
  inline typename std::enable_if<
      can_get_as_cref<Target, APInt>::value, APInt const &
    >::type
  valueof(constant_int const & li)
    { return li->getValue(); }

  template<typename Target>
  inline typename std::enable_if<
      can_get_as_cref<Target, APInt>::value, APInt const &
    >::type
  valueof(constant const & c)
  {
    if(constant_int const ci = dyn_cast<constant_int>(c))
      return valueof<Target>(ci);
    throw type_error(
        "Expected integer value, not " + typename_(c->getType())
      );
  }
  //@}

  //@{
  /// Gets the value of a literal constant as a user-specified integral type.
  template<typename Target>
  inline typename std::enable_if<
      aux::is_ok_integer_for_value<Target>(), Target
    >::type
  valueof(constant_int const & li)
  {
    if(li->getBitWidth() <= sizeof(Target) * 8)
    {
      return (std::is_signed<Target>::value)
        ? li->getSExtValue() : li->getZExtValue();
    }
    throw value_error("Integer value is too large to return.");
  }

  template<typename Target>
  inline typename std::enable_if<
      aux::is_ok_integer_for_value<Target>(), Target
    >::type
  valueof(constant const & c)
  {
    if(constant_int const ci = dyn_cast<constant_int>(c))
      return valueof<Target>(ci);
    throw type_error(
        "Expected integer value, not " + typename_(c->getType())
      );
  }
  //@}

  //@{
  /// Gets the value of a literal constant as an APFloat.
  template<typename Target>
  inline typename std::enable_if<
      can_get_as_cref<Target, APFloat>::value, APFloat const &
    >::type
  valueof(constant_fp const & lfp)
    { return lfp->getValueAPF(); }

  template<typename Target>
  inline typename std::enable_if<
      can_get_as_cref<Target, APFloat>::value, APFloat const &
    >::type
  valueof(constant const & c)
  {
    if(constant_fp const lfp = dyn_cast<constant_fp>(c))
      return valueof<Target>(lfp);
    throw type_error("Expected floating-point value");
  }
  //@}

  /// Gets the value of a constant as a float.
  template<typename Target>
  inline typename std::enable_if<
      std::is_same<Target, float>::value, Target
    >::type
  valueof(constant const & c)
  {
    APFloat const & val = valueof<APFloat>(c);
    llvm::fltSemantics const & sem = SPRITE_APICALL(val.getSemantics());
    if(&sem == &APFloat::IEEEsingle)
      return val.convertToFloat();
    else if(&sem == &APFloat::IEEEdouble)
      return static_cast<float>(val.convertToDouble());
    throw value_error("Unsupported float semantics");
  }

  /// Gets the value of a constant as a double.
  template<typename Target>
  inline typename std::enable_if<
      std::is_same<Target, double>::value, Target
    >::type
  valueof(constant const & c)
  {
    APFloat const & val = valueof<APFloat>(c);
    llvm::fltSemantics const & sem = SPRITE_APICALL(val.getSemantics());
    if(&sem == &APFloat::IEEEsingle)
      return val.convertToFloat();
    else if(&sem == &APFloat::IEEEdouble)
      return val.convertToDouble();
    throw value_error("Unsupported float semantics");
  }
}}
