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
    /// [Global definitions with linkage]
    TypeFactory const types;

    // Creates an external function named myfunction, taking no arguments and returning void.
    auto const void_ = types.void_();
    auto const myfunction = def(llvm_::GlobalValue::ExternalLinkage, void_(), "myfunction");

    // Creates a static array, ga, of three int32 values with no initializer.
    auto const i32 = types.int_(32);
    auto const ga = def(llvm_::GlobalValue::InternalLinkage, i32[3], "ga");

    // Creates an external array, ga2, of three int32 values with an initializer.
    auto const ga2 =
        extern_(i32[3], "ga2") = { 1, 2, 3 };
    /// [Global definitions with linkage]
    (void) myfunction;
    (void) ga;
    (void) ga2;
  }

  {
    /// [Initializing a global from a value]
    // Create a global integer variable initialized to the value 5.
    TypeFactory const types;
    auto const i32 = types.int_(32);
    extern_(i32) = 5;
    /// [Initializing a global from a value]
  }

  {
    /// [Initializing a global from an aggregate]
    // Create a global array variable initialized to the value {1, 2}.
    TypeFactory const types;
    auto const i32 = types.int_(32);
    extern_(i32[2]) = {1, 2};
    /// [Initializing a global from an aggregate]
  }

  {
    /// [Initializing a global from heterogeneous data]
    // Create a global array varible initialized to the value {1.0, 3.14, 97.0}.
    TypeFactory const types;
    auto const double_ = types.double_();
    extern_(double_[3]) = _t(1, 3.14, 'a');
    /// [Initializing a global from heterogeneous data]
  }

  {
    /// [Using extern_]
    TypeFactory const types;

    // Creates an external function named myfunction, taking no arguments and returning void.
    auto const void_ = types.void_();
    auto const myfunction = extern_(void_(), "myfunction");

    // Creates an external array, ga, of three int32 values with no initializer.
    auto const i32 = types.int_(32);
    auto ga = extern_(i32[3], "ga");
    ga = {1, 2, 3}; // Set the initializer.
    ga.set_initializer({1, 2, 3}); // Synonym for operator= (sets the initializer).

    // Creates an external array named hello_world containing C-string pointers "hello" and "world".
    auto const char_ = types.int_(8);
    auto hello_world =
        extern_((*char_)[2], "hello_world") = {"hello", "world"};

    // Creates an external array from different initializer types.
    std::string const world("world");
    auto hello_world2 =
        extern_((*char_)[2], "hello_world") = _t("hello", world);

    /// [Using extern_]
    (void) myfunction;
    (void) ga;
    (void) hello_world;
    (void) hello_world2;
  }

  {
    /// [Using static_]
    TypeFactory const types;

    // Creates an internal function named myfunction, taking no arguments and returning void.
    auto const void_ = types.void_();
    auto const myfunction = static_(void_(), "myfunction");

    // Creates a global, internal float variable initialized to the value 1.0.
    auto const float_ = types.float_();
    auto global_float = 
        static_(float_, "global_float") = 1.0;
    /// [Using static_]
    (void) myfunction;
    (void) global_float;
  }

  {
    /// [Using inline_]
    // Creates an inline function named myfunction, taking no arguments and returning void.
    TypeFactory const types;
    auto const void_ = types.void_();
    auto const myfunction = inline_(void_(), "myfunction");
    /// [Using inline_]
    (void) myfunction;
  }

  return 0;
}
