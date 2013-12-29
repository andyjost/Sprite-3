/**
 * @file
 * @brief Contains example code referenced throughout the documentation.
 */

// Note: no 80-column rule for this file.  Doxygen already uses a wide layout,
// so there is no need to wrap these examples.

#include "sprite/backend.hpp"
#include <cmath>
#include <limits>

int main()
{
  using namespace sprite::backend;

  {
    /// [Instantiating a NULL pointer]
    // Instantiate a null pointer.
    type_factory const types;
    auto const i32 = types.int_(32);
    auto const null_i32_p = *i32 % null;
    /// [Instantiating a NULL pointer]
    (void) null_i32_p;
  }

  {
    /// [Instantiating a NULL struct]
    // Instantiate a null struct.
    type_factory const types;
    auto const i32 = types.int_(32);
    auto const tp = types.struct_("mystruct.1", {i32, i32, i32});
    auto const null_struct = tp % null;
    /// [Instantiating a NULL struct]
    (void) null_struct;
  }

  {
    /// [Instantiating a NULL integer]
    // Instantiate a null array.
    type_factory const types;
    auto const i32 = types.int_(32);
    auto const null_int = i32 % null;
    /// [Instantiating a NULL integer]
    assert(null_int.value<int>() == 0);
  }

  {
    /// [Instantiating a NULL array]
    // Instantiate a null array.
    type_factory const types;
    auto const i32 = types.int_(32);
    auto const null_array = i32[2] % null;
    /// [Instantiating a NULL array]
    (void) null_array;
  }

  {
    /// [Instantiating simple types]
    type_factory const types;
    auto const bool_ = types.int_(1);
    auto const char_ = types.int_(8);
    auto const i32 = types.int_(32);
    auto const float_ = types.float_();

    auto const true_ = bool_ % true;
    auto const false_ = bool_ % false;
    auto const a = char_ % 'a';
    auto const fortytwo = i32 % 42;
    auto const fortytwo2 = i32 % APInt(32, 42);
    auto const pi = float_ % APFloat(3.1415);
    auto const zero = float_ % 0.0f;
    auto const zero2 = float_ % 0;
    auto const zero3 = float_ % null;

    // Integer and floating-point constants can also be created from string representations.
    auto const fortytwo_ = i32 % "42";
    auto const pi_ = float_ % "3.1415";
    /// [Instantiating simple types]
    assert(true_.value<bool>() == true);
    assert(false_.value<bool>() == false);
    assert(a.value<char>() == 'a');
    assert(fortytwo.value<int>() == 42);
    assert(fortytwo2.value<int>() == 42);
    assert(pi.value<float>() == 3.1415f);
    assert(zero.value<float>() == 0);
    assert(zero2.value<float>() == 0);
    assert(zero3.value<float>() == 0);
    assert(fortytwo_.value<int>() == 42);
    assert(pi_.value<float>() == 3.1415f);
  }

  {
    /// [Instantiating non-finite floating-point types]
    type_factory const types;
    auto const float_ = types.float_();
    auto const double_ = types.double_();

    // Instantiate infinite values.
    auto const pfinf = float_ % +inf_; // explicitly positive
    auto const nfinf = float_ % -inf_;
    auto const pdinf = double_ % inf_; // positive by default
    auto const ndinf = double_ % -inf_;

    // Instantiate nan values (can also use qnan or snan for "quiet" or "signaling" flavors).
    auto const fnan = float_ % nan_;
    auto const dnan = double_ % nan_;
    auto const ndnan = double_ % -nan_; // negative/positive are defined for NaN types.
    /// [Instantiating non-finite floating-point types]
    assert(pfinf.value<float>() == std::numeric_limits<float>::infinity());
    assert(nfinf.value<float>() == -std::numeric_limits<float>::infinity());
    assert(pdinf.value<double>() == std::numeric_limits<double>::infinity());
    assert(ndinf.value<double>() == -std::numeric_limits<double>::infinity());
    assert(std::isnan(fnan.value<float>()));
    assert(std::isnan(dnan.value<double>()));
    assert(std::isnan(ndnan.value<double>()));
  }

  {
    /// [Instantiating integer types from strings]
    type_factory const types;
    auto const i32 = types.int_(32);

    auto const decimal = i32 % "197";
    auto const hex = i32 % "0xdeadbeef";
    auto const binary = i32 % "b01001";
    auto const octal = i32 % "0777";
    /// [Instantiating integer types from strings]
    assert(decimal.value<int>() == 197);
    assert(hex.value<unsigned>() == 0xdeadbeef);
    assert(binary.value<int>() == 9);
    assert(octal.value<unsigned>() == 0777);
  }

  {
    /// [Instantiating types]
    type_factory const types;
    auto const char_ = types.int_(8);
    auto const i32 = types.int_(32);
    auto const float_ = types.float_();
    auto const struct_ = types.struct_("mystruct.2", {i32, float_});

    auto const fortytwo = i32 % 42;
    auto const hello = char_[12] % "hello world";
    auto const pi = float_ % 3.141592654;
    auto const st12 = struct_ % (i32 % 1, pi);

    /// [Instantiating types]
    assert(fortytwo.value<int>() == 42);
    (void) hello;
    assert(pi.value<float>() == 3.141592654f);
    (void) st12;
  }

  {
    /// [Instantiating an array from a sequence]
    type_factory const types;
    auto const i32 = types.int_(32);
    auto const one_two = i32[2] % (i32 % 1, i32 % 2);
    /// [Instantiating an array from a sequence]
    (void) one_two;
  }

  {
    /// [Instantiating arrays as aggregates]
    type_factory const types;
    auto const char_ = types.int_(8);
    auto const ab = char_[2] % _a{'a', 'b'}; // same as char_[2] % (char_ % 'a', char_ % 'b');
    /// [Instantiating arrays as aggregates]
    (void) ab;
  }

  {
    /// [Instantiating arrays from tuples]
    type_factory const types;
    auto const double_ = types.double_();
    auto const doubles = double_[2] % _t(1.0f, 2.0);
    /// [Instantiating arrays from tuples]
    (void) doubles;
  }

  {
    /// [Instantiating char pointers]
    // Expands to an expression that evaluates the length of a string constant.
    #define NELEM(arg) \
        cast<llvm::GlobalVariable>(arg->stripPointerCasts())->getInitializer()->getType()->getArrayNumElements() \
      /**/
    type_factory const types;
    auto const char_ = types.int_(8);
    // A null-terminated string.
    auto const hello = *char_ % "hello world"; // length: 12
    assert(NELEM(hello) == 12);

    // Another null-terminated string.
    char const letters[] = {'h', 'e', 'l', 'l', 'o'};
    auto const hello2 = *char_ % letters; // length: 6
    assert(NELEM(hello2) == 6);

    // A non-null-terminated char array.
    auto const hello3 = *char_ % array_ref<char>(letters); // length: 5
    assert(NELEM(hello3) == 5);

    // Arrays of char *.
    char const * init[2] = {"hello", "world"};
    auto const hello4 = (*char_)[2] % init;
    auto const hello5 = (*char_)[2] % _a{"hello", "world"};
    auto const hello6 = (*char_)[2] % _t("hello", "world");
    /// [Instantiating char pointers]
    (void) hello;
    (void) hello2;
    (void) hello3;
    (void) hello4;
    (void) hello5;
    (void) hello6;
  }

  {
    /// [Instantiating pointers as global arrays]
    type_factory const types;
    auto const i32 = types.int_(32);
    auto const double_ = types.double_();

    auto const array1 = *i32 % _a{1, 2, 3, 4};
    auto const array2 = *double_ % std::vector<double>{1.0, 2.0, 3.0, 4.0};
    auto const array3 = *double_ % _t(1, 2, 3.0, 4.0f);
    /// [Instantiating pointers as global arrays]
    (void) array1;
    (void) array2;
    (void) array3;
  }

  {
    /// [Instantiating constant data arrays]
    type_factory const types;
    auto a = types % array_ref<uint8_t>{'a', 'b', 'c'};
    auto b = types % array_ref<uint16_t>{1, 2, 3, 4};
    auto c = types % array_ref<uint32_t>{1, 2, 3, 4};
    auto d = types % array_ref<uint64_t>{1, 2, 3, 4};
    auto e = types % array_ref<float>{1, 2, 3, 4};
    auto f = types % array_ref<double>{1, 2, 3, 4};
    auto g = types % "hello";
    /// [Instantiating constant data arrays]
    (void) a;
    (void) b;
    (void) c;
    (void) d;
    (void) e;
    (void) f;
    (void) g;
  }

  return 0;
}
