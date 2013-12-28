/**
 * @file
 * @brief Defines support for defining special values, such as NULL and inf.
 */

#pragma once

namespace sprite { namespace llvm
{
  /// Represents the ... token in function type definitions.
  struct Ellipsis {};

  /// May be used to refer to an ellipsis in a function type declaration.
  Ellipsis const dots;

  /// Represents the value of a NULL pointer.
  struct Null {};
  /// May be used to refer to a NULL pointer value.
  Null const null;

  /// Used to construct non-finite floating-point values.
  struct NonFiniteValue
  {
    enum Kind { Inf, Nan, Qnan, Snan };

    /// Constructor.
    NonFiniteValue(Kind kind, bool negative = false)
      : m_kind(kind), m_negative(negative) {}

    /// Returns the kind of non-finite value.
    Kind kind() const { return m_kind; }

    /// Returns the sign of the non-finite value (true if negative).
    bool negative() const { return m_negative; }

    /// Indicates a positive sign.
    NonFiniteValue operator+() const { return NonFiniteValue(kind(), false); }

    /// Indicates a negative sign.
    NonFiniteValue operator-() const { return NonFiniteValue(kind(), true); }

  private:

    Kind m_kind;
    bool m_negative;
  };

  /// Used to construct infinite floating-point values.
  NonFiniteValue const inf_ = NonFiniteValue::Inf;

  /// Used to construct NaN floating-point values.
  NonFiniteValue const nan_ = NonFiniteValue::Nan;

  /// Used to construct quiet NaN floating-point values.
  NonFiniteValue const qnan_ = NonFiniteValue::Qnan;

  /// Used to construct signaling NaN floating-point values.
  NonFiniteValue const snan_ = NonFiniteValue::Snan;

}}
