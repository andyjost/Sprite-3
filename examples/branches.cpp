// Tests branch instructions.
#include "sprite/backend/support/testing.hpp"

using namespace sprite::backend;
using namespace sprite::backend::testing;

int main()
{
  // Test the "hello world!" program.
  test_function(
      [](clib_h const & clib)
      {
        clib.fprintf(arg("file"), "hello world!");
        return_(0);
      }
    , "hello world!"
    );

  // Test goto.
  test_function(
      [](clib_h const & clib)
      {
        value file = arg("file");
        clib.fprintf(file, "a");
        label l;
        goto_(l);
        { // l:
          scope _ = l;
          clib.fprintf(file, "b");
          return_(0);
        }
      }
    , "ab"
    );

  // Test if with two branches.
  test_function(
      [](clib_h const & clib)
      {
        value file = arg("file");
        clib.fprintf(file, "a");
        if_(1
          , [&]{clib.fprintf(file, "b"); return_(0);}
          , [&]{clib.fprintf(file, "c"); return_(0);}
          );
      }
    , "ab"
    );
  test_function(
      [](clib_h const & clib)
      {
        value file = arg("file");
        clib.fprintf(file, "a");
        if_(1, [&]{clib.fprintf(file, "b");}, [&]{clib.fprintf(file, "c");});
        clib.fprintf(file, "d");
        return_(0);
      }
    , "abd"
    );
  test_function(
      [](clib_h const & clib)
      {
        value file = arg("file");
        clib.fprintf(file, "a");
        if_(0, [&]{clib.fprintf(file, "b");}, [&]{clib.fprintf(file, "c");});
        clib.fprintf(file, "d");
        return_(0);
      }
    , "acd"
    );
  test_function(
      [](clib_h const & clib)
      {
        value file = arg("file");
        clib.fprintf(file, "a");
        if_(1, [&]{clib.fprintf(file, "b"); return_(0);}, [&]{clib.fprintf(file, "c");});
        clib.fprintf(file, "d");
        return_(0);
      }
    , "ab"
    );
  // Test creating branches inside a label.
  test_function(
      [](clib_h const & clib)
      {
        value file = arg("file");
        clib.fprintf(file, "a");
        label l([&]{
            if_(0
              , [&]{clib.fprintf(file, "b"); return_(0);}
              , [&]{clib.fprintf(file, "c"); return_(0);}
              );
          });
        goto_(l);
      }
    , "ac"
    );
  // Test implicit continuation with nested branches.
  test_function(
      [](clib_h const & clib)
      {
        value file = arg("file");
        clib.fprintf(file, "a");
        if_(1
          , [&]{
              clib.fprintf(file, "b");
              if_(1, [&]{clib.fprintf(file, "c");});
              clib.fprintf(file, "d");
            }
          );
        clib.fprintf(file, "e");
        return_(0);
      }
    , "abcde"
    );
}

// TODO add this test.
// while_([]{return true;}, []{break_();});

