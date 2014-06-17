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
  test_function(
      [](clib_h const & clib)
      {
        value file = arg("file");
        clib.fprintf(file, "a");
        label c;
        if_(1
          , [&]{
              clib.fprintf(file, "b");
              if_(1, c);
              clib.fprintf(file, "d");
            }
          );
        {
          scope _ = c;
          clib.fprintf(file, "c");
        }
        clib.fprintf(file, "e");
        return_(0);
      }
    , "abcde"
    );
  // Test a simple while loop.
  test_function(
      [](clib_h const & clib)
      {
        value file = arg("file");
        clib.fprintf(file, "a");
        while_(true, [&]{clib.fprintf(file, "b"); break_();});
        clib.fprintf(file, "c");
        return_(0);
      }
    , "abc"
    );
  // Test break with nested branches.
  test_function(
      [](clib_h const & clib)
      {
        value file = arg("file");
        clib.fprintf(file, "a");
        while_(
            true
          , [&]{
              clib.fprintf(file, "b");
              while_(
                  true
                , [&]{
                    clib.fprintf(file, "c");
                    if_(true, []{break_();});
                  }
                );
              clib.fprintf(file, "d");
              if_(false, []{}, [&]{clib.fprintf(file, "e"); break_();});
              clib.fprintf(file, "f");
              break_();
            }
          );
        clib.fprintf(file, "g");
        return_(0);
      }
    , "abcdeg"
    );
  test_function(
      [](clib_h const & clib)
      {
        value file = arg("file");
        clib.fprintf(file, "a");
        if_(true
          , [&]{
              while_(
                  false
                , [&]{clib.fprintf(file, "b"); break_();}
                );
              while_(
                  true
                , [&]{clib.fprintf(file, "c"); break_();}
                );
            }
          );
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
        type i64 = types::int_(64);
        ref x = local(i64);
        x = 0;
        while_(
            [&]{x<(signed_)(3);}
          , [&]{
              clib.fprintf(arg("file"), "%d.", x);
              ++x;
            }
          );
        clib.fprintf(file, "b");
        return_(0);
      }
    , "a0.1.2.b"
    );
  // Test indirect branches.
  test_function(
      [](clib_h const & clib)
      {
        type char_p = *types::char_();
        type int_ = types::int_();
        value file = arg("file");
        clib.fprintf(file, ".beg");
        label a, b, c, jump;
        globalvar jt = static_(char_p[3]).set_initializer({&a, &b, &c});
        ref i = local(int_);
        i = 0;
        goto_(jump);
        {
          scope _ = jump;
          goto_(jt[i], {a, b, c});
        }

        {
          scope _ = a;
          clib.fprintf(file, ".a");
          i = 1;
          goto_(jump);
        }
        {
          scope _ = b;
          clib.fprintf(file, ".b");
          i = 2;
          goto_(jump);
        }
        {
          scope _ = c;
          clib.fprintf(file, ".c");
          return_(0);
        }
      }
    , ".beg.a.b.c"
    );
  #if 0
  test_function(
      [](clib_h const & clib)
      {
        value file = arg("file");
        clib.fprintf(file, "a");
        type i64 = types::int_(64);
        ref i = local(i64, "i");
        i = 0;
        while_([&]{i<3;}, [&]{clib.fprintf(file, "b"); ++i;});
        clib.fprintf(file, "b");
        return_(0);
      }
    , "abbbc"
    );
  #endif
}

