#include "sprite/backend.hpp"
#include <cmath>
#include <limits>
int main()
{
  using namespace sprite::backend;
  module const hello_module("hello");
  scope _ = hello_module;
  auto const char_ = types::char_();
  auto const i32 = types::int_(32);
  auto const puts = extern_(i32(*char_), "puts");
  auto const main = extern_(i32(), "main");
  {
    scope _ = main;
    puts("hello world\n");
    return_(0);
  }
  // module.write("hello.ll");
  return 0;
}
