// Tests branch instructions.
#include "sprite/backend/support/testing.hpp"

using namespace sprite::backend;
using namespace sprite::backend::testing;

int main()
{
  test_function(
      [](clib_h const & clib)
      {
        type i64 = types::int_(64);
        ref x = *local(i64);
        x = 42;
        clib.fprintf(arg("file"), "%d.", x);
        x = 97;
        clib.fprintf(arg("file"), "%d.", x);
        ++x;
        clib.fprintf(arg("file"), "%d.", x);
        return_(0);
      }
    , "42.97.98."
    );

}

