// Tests the ref class.
#include "sprite/backend/support/testing.hpp"

using namespace sprite::backend;
using namespace sprite::backend::testing;

int main()
{
  // Basic test.
  test_function(
      [](clib_h const & clib)
      {
        type i64 = types::int_(64);
        ref x = local(i64);
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

  // // Test the conversion from an indexed global variable to ref and value.
  // test_function(
  //     [](clib_h const & clib)
  //     {
  //       type i64 = types::int_(64);
  //       global table = static_(i64[2], "table").set_initializer({0,0});
  //       clib.fprintf(arg("file"), "%d-%d.", table[0], table[1]);
  //       table[0] = 100;
  //       table[1] = 200;
  //       clib.fprintf(arg("file"), "%d-%d.", table[0], table[1]);
  //       ++table[0];

  //       return_(0);
  //     }
  //   , "0-0.100.200.101-200."
  //   );
}

