// Tests the operators.
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
        value const one = i64(1);
        value const two = i64(2);
        value nil = (*i64)(nullptr);
        clib.fprintf(arg("file"), "a:%d.%d.%d", one, two, nil);

        // (+)
        {
          value two_a = one + 1; 
          // FIXME
          // value two_b = 1 + one; // parameter_error: Need the signed_ or unsigned_ flag for integer extension
          value two_c = one + one;
          clib.fprintf(arg("file"), " b:%d.%d", two_a, two_c);
        }

        // (-)
        {
          value one_a = two - 1;
          // value one_b = 2 - one; // parameter_error: Need the signed_ or unsigned_ flag for integer extension
          value one_c = two - one;
          clib.fprintf(arg("file"), " c:%d.%d", one_a, one_c);
        }

        // (==) int
        {
          value true_a = (one == one);
          value true_b = (one == 1);
          value true_c = (1 == one);
          // value true_d = (1 == 1); // FIXME
          value false_a = (one == two);
          value false_b = (one == 2);
          value false_c = (1 == two);
          // value false_a = (1 == 2); // FIXME
          clib.fprintf(
              arg("file")
            , " d:%d.%d.%d.%d.%d.%d"
            , true_a, true_b, true_c, false_a, false_b, false_c
            );
        }

        // (==) pointer
        {
          value true_a = (nil == nil);
          // value true_b = (nil == nullptr); // FIXME
          // value true_c = (nullptr == nil); // FIXME
          // value true_d = (nullptr == nullptr); // FIXME
          clib.fprintf(arg("file"), " e:%d", true_a);
        }
        // value address = char_(nullptr);
        // clib.fprintf(arg("file"), "%p.", address);

        // ref x = local(i64);
        // x = 42;

        // clib.fprintf(arg("file"), "%d.", x);
        // x = 97;
        // clib.fprintf(arg("file"), "%d.", x);
        // ++x;
        // clib.fprintf(arg("file"), "%d.", x);
        return_(0);
      }
    , "a:1.2.0 b:2.2 c:1.1 d:1.1.1.0.0.0 e:1"
    );
}
