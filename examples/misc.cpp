/**
 * @file
 * @brief Contains example code referenced throughout the documentation.
 */

// Note: no 80-column rule for this file.  Doxygen already uses a wide layout,
// so there is no need to wrap these examples.

#include "sprite/backend.hpp"
#include <cmath>
#include <limits>

int main()
{
  using namespace sprite::backend;

  {
    /// [Using cast]
    module const mod;
    type tp = mod.int_(8); // A generic type wrapper.
    integer_type int_tp = cast<IntegerType>(tp); // A more specific type wrapper.
    /// [Using cast]
    (void) int_tp;
  }

  {
    /// [Using dyn_cast]
    module const mod;
    type tp = mod.int_(8); // A generic type wrapper.
    integer_type int_tp = dyn_cast<IntegerType>(tp); // A more specific type wrapper.
    /// [Using dyn_cast]
    (void) int_tp;
  }

  {
    /// [Using _a and _t]
    module const mod;
    auto const i32 = mod.int_(32);
    auto myarray = i32[2] % _a{1, 2};
    auto myarray2 = i32[3] % _t(1, 'a', 1.0f);
    /// [Using _a and _t]
    (void) myarray;
    (void) myarray2;
  }

  return 0;
}
