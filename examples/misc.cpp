/**
 * @file
 * @brief Contains example code referenced throughout the documentation.
 */

// Note: no 80-column rule for this file.  Doxygen already uses a wide layout,
// so there is no need to wrap these examples.

#include "sprite/llvm.hpp"
#include <cmath>
#include <limits>

int main()
{
  using namespace sprite::llvm;

  {
    /// [Using cast]
    TypeFactory const types;
    TypeWrapper<llvm_::Type> tp = types.int_(8); // A generic type wrapper.
    TypeWrapper<llvm_::IntegerType> int_tp = cast<llvm_::IntegerType>(tp); // A more specific type wrapper.
    /// [Using cast]
    (void) int_tp;
  }

  {
    /// [Using dyn_cast]
    TypeFactory const types;
    TypeWrapper<llvm_::Type> tp = types.int_(8); // A generic type wrapper.
    TypeWrapper<llvm_::IntegerType> int_tp = dyn_cast<llvm_::IntegerType>(tp); // A more specific type wrapper.
    /// [Using dyn_cast]
    (void) int_tp;
  }

  {
    /// [Using _a and _t]
    TypeFactory const types;
    auto const i32 = types.int_(32);
    auto myarray = i32[2] % _a{1, 2};
    auto myarray2 = i32[3] % _t(1, 'a', 1.0f);
    /// [Using _a and _t]
    (void) myarray;
    (void) myarray2;
  }

  return 0;
}
