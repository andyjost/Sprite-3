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
    /// [Creating basic types]
    // Create a new module and module.
    module const mod("mymodule");

    // Get a few basic types.
    auto const char_ = mod.int_(8); // get the char type
    auto const double_ = mod.double_(); // get the 64-bit double type
    auto const float_ = mod.float_(); // get the 32-bit float type
    auto const i32 = mod.int_(32); // get the 32-bit integer type
    auto const void_ = mod.void_(); // get the void type
    /// [Creating basic types]
    (void) char_;
    (void) double_;
    (void) float_;
    (void) i32;
    (void) void_;
  }

  {
    /// [Creating array types]
    module const mod;
    auto const i32 = mod.int_(32);
    auto const i32_x2 = i32[2]; // get the i32[2] type
    /// [Creating array types]
    (void) i32_x2;
  }

  {
    /// [Creating pointer types]
    module const mod;
    auto const i32 = mod.int_(32);
    auto const i32_p = *i32; // get the i32 * type
    /// [Creating pointer types]
    (void) i32_p;
  }

  {
    /// [Creating function types]
    module const mod;
    auto const char_ = mod.int_(8);
    auto const float_ = mod.float_();
    auto const i32 = mod.int_(32);
    auto const void_ = mod.void_();

    // Type of function taking i32 and float, returning nothing.
    auto const func1_t = void_(i32, float_);

    // Type of a function taking a char* and varargs, returning an i32.
    auto const func2_t = i32(*char_, dots);
    /// [Creating function types]
    (void) func1_t;
    (void) func2_t;
  }

  {
    /// [Creating an anonymous struct]
    // A struct consisting of two 32-bit integers.
    module const mod;
    auto const i32 = mod.int_(32);
    auto const anon = mod.struct_({i32, i32});
    /// [Creating an anonymous struct]
    (void) anon;
  }

  {
    /// [Creating opaque structs]
    // Creates an opaque struct.
    module const mod;
    auto const opaque = mod.struct_("OpaqueStruct");
    /// [Creating opaque structs]
    (void) opaque;
  }

  {
    /// [Creating structs]
    module const mod;

    // Creates an empty (not opaque) struct.
    auto const empty = mod.struct_("empty", {});

    // Creates a named struct consisting of a pointer to function pointers (taking varargs and returning i32), and an i32.
    auto const i32 = mod.int_(32);
    auto const my_struct = mod.struct_("MyStruct", {**(i32(dots)), i32});

    // Gets the previously-created named struct.
    auto const my_struct2 = mod.struct_("MyStruct");
    /// [Creating structs]
    (void) empty;
    (void) my_struct;
    (void) my_struct2;
  }


  return 0;
}
