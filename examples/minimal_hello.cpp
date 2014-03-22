#include "sprite/backend.hpp"
#include <iostream>

using namespace sprite::backend;

int main()
{
  module hello("hello");
  {
    scope _ = hello;
  
    type char_ = types::char_();
    type int_ = types::int_();
  
    function puts = extern_(int_(*char_), "puts");
    (void) puts;
  
    function main = extern_(int_(), "main");
    {
      scope _ = main;
      // puts("hello world!");
      return_(0);
    }
  }

#if 0
  auto hello_main = hello.compile();
  return hello_main();
#endif
  return 0;
}
