/**
 * @file
 * @brief Contains example code showing how to create and call functions.
 */

// These tests cover various ways of defining and calling functions.  At a
// minimum, the following parameters must be covered:
//
//     Linkage: extern, inline, static.
//     Return types: void, non-void.
//     Arity: zero, non-zero.

// Note: no 80-column rule for this file.  Doxygen already uses a wide layout,
// so there is no need to wrap these examples.

#include "sprite/backend.hpp"
#include <cmath>
#include <limits>

int main()
{
  using namespace sprite::backend;
  module const m("misc");
  scope _ = m;

  type int_ = types::int_();
  // type void_ = types::void_();


  // A nullary function returning void.
  // function a = extern_(void_(), "a", []{return_();});

  // A nullary function returning int.
  function b = inline_(int_(), "b", []{return_(0);});

  // A main function to test calling the others.
  function main = static_(int_(), "main");
  {
    scope _ = main;
    // a();
    return_(b());
  }

  m->dump();
}
