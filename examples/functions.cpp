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

#include "sprite/backend/support/testing.hpp"
#include <cmath>
#include <limits>

using namespace sprite::backend;
using namespace sprite::backend::testing;

#define MYPRINT(expr) clib.fprintf(file, "%d\n", expr)

int main()
{
  test_function(
      [](clib_h const & clib)
      {
        value file = arg("file");

        type int_ = types::int_();
        type void_ = types::void_();

        // Nullary functions.
        function n0 = extern_(void_(), "n0", []{});
        n0();

        function n1 = inline_(int_(), "n1", []{return_(0);});
        MYPRINT(n1()); // prints 0

        function n2 = inline_(int_(), "n2");
        {
          scope _ = n2;
          return_(137);
        }
        MYPRINT(n2()); // prints 137

        // Unary functions.
        function u0 = extern_(void_(int_), "u0", {"a0"}, []{return_();});
        u0(0);

        function u1 = static_(int_(int_), "u1", {"a0"}, []{return_(arg("a0"));});
        MYPRINT(u1(int_(42))); // prints 42

        function u2 = inline_(int_(int_), "u2", {"a0"});
        {
          scope _ = u2;
          value a0 = arg("a0");
          return_(a0+1);
        }
        MYPRINT(u2(1)); // prints 2

        return_(0);
      }
    , "0\n137\n42\n2\n"
    );
}

#undef P
