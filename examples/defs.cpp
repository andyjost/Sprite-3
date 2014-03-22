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
  module const m("defs");
  scope _ = m;

  {
    /// [Global definitions with linkage]
    // Creates an external function named myfunction, taking no arguments and returning void.
    auto const void_ = types::void_();
    auto const myfunction = def(GlobalValue::ExternalLinkage, void_(), "myfunction");

    // Creates a static array, ga, of three int32 values with no initializer.
    auto const i32 = types::int_(32);
    auto const ga = def(GlobalValue::InternalLinkage, i32[3], "ga");

    // Creates an external array, ga2, of three int32 values with an initializer.
    auto const ga2 =
        extern_(i32[3], "ga2").set_initializer({1, 2, 3});
    /// [Global definitions with linkage]
    (void) myfunction;
    (void) ga;
    (void) ga2;
  }

  {
    /// [Initializing a global from a value]
    // Create a global integer variable initialized to the value 5.
    auto const i32 = types::int_(32);
    extern_(i32).set_initializer(5);
    /// [Initializing a global from a value]
  }

  {
    /// [Initializing a global from an aggregate]
    // Create a global array variable initialized to the value {1, 2}.
    auto const i32 = types::int_(32);
    extern_(i32[2]).set_initializer({1, 2});
    /// [Initializing a global from an aggregate]
  }

  {
    /// [Initializing a global from heterogeneous data]
    // Create a global array varible initialized to the value {1.0, 3.14, 97.0}.
    auto const double_ = types::double_();
    extern_(double_[3]).set_initializer(_t(1, 3.14, 'a'));
    /// [Initializing a global from heterogeneous data]
  }

  {
    /// [Using extern_]
    // Creates an external function named myfunction, taking no arguments and returning void.
    auto const void_ = types::void_();
    auto const myfunction = extern_(void_(), "myfunction");

    // Creates an external array, ga, of three int32 values with no initializer.
    auto const i32 = types::int_(32);
    auto ga = extern_(i32[3], "ga");
    ga.set_initializer({1, 2, 3}); // Set the initializer.

    // Creates an external array named hello_world containing C-string pointers "hello" and "world".
    auto const char_ = types::int_(8);
    auto hello_world =
        extern_((*char_)[2], "hello_world").set_initializer({"hello", "world"});

    // Creates an external array from different initializer types.
    std::string const world("world");
    auto hello_world2 =
        extern_((*char_)[2], "hello_world").set_initializer(_t("hello", world));

    /// [Using extern_]
    (void) myfunction;
    (void) ga;
    (void) hello_world;
    (void) hello_world2;
  }

  {
    /// [Using static_]
    // Creates an internal function named myfunction, taking no arguments and returning void.
    auto const void_ = types::void_();
    auto const myfunction = static_(void_(), "myfunction");

    // Creates a global, internal float variable initialized to the value 1.0.
    auto const float_ = types::float_();
    auto global_float = 
        static_(float_, "global_float").set_initializer(1.0);
    /// [Using static_]
    (void) myfunction;
    (void) global_float;
  }

  {
    /// [Using inline_]
    // Creates an inline function named myfunction, taking no arguments and returning void.
    auto const void_ = types::void_();
    auto const myfunction = inline_(void_(), "myfunction");
    /// [Using inline_]
    (void) myfunction;
  }

  return 0;
}
