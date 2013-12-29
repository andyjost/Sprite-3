/**
 * @file
 * @brief Contains examples and tests for the generic operators.
 */

#include "sprite/backend.hpp"
#include <cassert>
#include <limits>

int main()
{
  using namespace sprite::backend;

  // Note that the generic type Type in each wrapper makes operator% go through
  // the generic dispatch.
  type_factory const types;
  type bool_ = types.int_(1);
  type char_ = types.int_(8);
  type i32 = types.int_(32);
  type float_ = types.float_();
  type double_ = types.double_();

  #define MYCHECK(expr, expected)                            \
      {                                                      \
        auto const tmp = expr;                               \
        assert(tmp.value<decltype(expected)>() == expected); \
      }                                                      \
    /**/

  #define MYCHECKNAN(expr, type)               \
      {                                        \
        auto const tmp = expr;                 \
        auto const x = tmp.value<type>();      \
        assert(x != x && "x should be a nan"); \
      }                                        \
    /**/

  MYCHECK(bool_ % null, false);
  MYCHECK(bool_ % true, true);
  MYCHECK(bool_ % false, false);
  MYCHECK(bool_ % 0, false);
  MYCHECK(bool_ % 1, true);

  MYCHECK(char_ % 'a', 'a');
  MYCHECK(char_ % 100, 100);

  MYCHECK(i32 % null, 0);
  MYCHECK(i32 % -1, -1);
  MYCHECK(i32 % 42, 42);
  MYCHECK(i32 % 0xdeadbeef, 0xdeadbeef);
  MYCHECK(i32 % 0777, 0777);
  MYCHECK(i32 % "42", 42);
  MYCHECK(i32 % "0xdeadbeef", 0xdeadbeef);
  MYCHECK(i32 % "0777", 0777);
  MYCHECK(i32 % "b101", 5);
  MYCHECK(i32 % llvm::APInt(8, 37), 37);
  MYCHECK(i32 % llvm::APInt(8, -2, true), -2);

  MYCHECK(float_ % null, 0.0f);
  MYCHECK(float_ % 1, 1.0f);
  MYCHECK(float_ % 1.0, 1.0f);
  MYCHECK(float_ % 1.0f, 1.0f);
  MYCHECK(float_ % "1.0", 1.0f);
  MYCHECK(float_ % inf_, std::numeric_limits<float>::infinity());
  MYCHECK(float_ % -inf_, -std::numeric_limits<float>::infinity());
  MYCHECKNAN(float_ % nan_, float);
  MYCHECKNAN(float_ % qnan_, float);
  MYCHECKNAN(float_ % snan_, float);

  MYCHECK(double_ % null, 0.0);
  MYCHECK(double_ % 1, 1.0);
  MYCHECK(double_ % 1.0, 1.0);
  MYCHECK(double_ % 1.0f, 1.0);
  MYCHECK(double_ % "1.0", 1.0);
  MYCHECK(double_ % inf_, std::numeric_limits<double>::infinity());
  MYCHECK(double_ % -inf_, -std::numeric_limits<double>::infinity());
  MYCHECKNAN(double_ % nan_, double);
  MYCHECKNAN(double_ % qnan_, double);
  MYCHECKNAN(double_ % snan_, double);

  return 0;
}
