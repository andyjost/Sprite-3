/**
 * @file
 * @brief Defines support for defining special values, such as NULL and inf.
 */

#pragma once

namespace sprite { namespace backend
{
  /// Represents the ... token in function type definitions.
  struct ellipsis {};

  /// May be used to refer to an ellipsis in a function type declaration.
  ellipsis const dots;

  /// Represents the value of a NULL pointer.
  struct null_arg {};
  /// May be used to refer to a NULL pointer value.
  null_arg const null;

  /// Used to construct non-finite floating-point values.
  struct non_finite_value
  {
    enum Kind { Inf, Nan, Qnan, Snan };

    /// Constructor.
    non_finite_value(Kind kind, bool negative = false)
      : m_kind(kind), m_negative(negative) {}

    /// Returns the kind of non-finite value.
    Kind kind() const { return m_kind; }

    /// Returns the sign of the non-finite value (true if negative).
    bool negative() const { return m_negative; }

    /// Indicates a positive sign.
    non_finite_value operator+() const { return non_finite_value(kind(), false); }

    /// Indicates a negative sign.
    non_finite_value operator-() const { return non_finite_value(kind(), true); }

  private:

    Kind m_kind;
    bool m_negative;
  };

  /// Used to construct infinite floating-point values.
  non_finite_value const inf_ = non_finite_value::Inf;

  /// Used to construct NaN floating-point values.
  non_finite_value const nan_ = non_finite_value::Nan;

  /// Used to construct quiet NaN floating-point values.
  non_finite_value const qnan_ = non_finite_value::Qnan;

  /// Used to construct signaling NaN floating-point values.
  non_finite_value const snan_ = non_finite_value::Snan;

}}
