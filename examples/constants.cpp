/**
 * @file
 * @brief Contains example code referenced throughout the documentation.
 */

#undef NDEBUG // assert() is assumed to "use" a variable.

// Note: no 80-column rule for this file.  Doxygen already uses a wide layout,
// so there is no need to wrap these examples.

#include "sprite/backend.hpp"
#include <cmath>
#include <limits>

int main()
{
  using namespace sprite::backend;
  module const m("constants");
  scope _ = m;

  {
    /// [Instantiating a NULL pointer]
    // Instantiate a null pointer.
    auto const i32 = types::int_(32);
    auto const null_i32_p = (*i32)(null);
    /// [Instantiating a NULL pointer]
    (void) null_i32_p;
  }

  {
    /// [Instantiating a NULL struct]
    // Instantiate a null struct.
    auto const i32 = types::int_(32);
    auto const tp = types::struct_("mystruct.1", {i32, i32, i32});
    auto const null_struct = tp(null);
    /// [Instantiating a NULL struct]
    (void) null_struct;
  }

  {
    /// [Instantiating a NULL integer]
    // Instantiate a null array.
    auto const i32 = types::int_(32);
    auto const null_int = i32(null);
    /// [Instantiating a NULL integer]
    assert(valueof<int>(null_int) == 0);
  }

  {
    /// [Instantiating a NULL array]
    // Instantiate a null array.
    auto const i32 = types::int_(32);
    auto const null_array = i32[2](null);
    /// [Instantiating a NULL array]
    (void) null_array;
  }

  {
    /// [Instantiating simple types]
    auto const bool_ = types::int_(1);
    auto const char_ = types::int_(8);
    auto const i32 = types::int_(32);
    auto const float_ = types::float_();

    auto const true_ = bool_(true);
    auto const false_ = bool_(false);
    auto const a = char_('a');
    auto const fortytwo = i32(42);
    auto const fortytwo2 = i32(APInt(32, 42));
    auto const pi = float_(APFloat(3.1415));
    auto const zero = float_(0.0f);
    auto const zero2 = float_(0);
    auto const zero3 = float_(null);

    // Integer and floating-point constants can also be created from string representations.
    auto const fortytwo_fromstring = i32("42");
    auto const pi_fromstring = float_("3.1415");
    /// [Instantiating simple types]
    assert(valueof<bool>(true_) == true);
    assert(valueof<bool>(false_) == false);
    assert(valueof<char>(a) == 'a');
    assert(valueof<int>(fortytwo) == 42);
    assert(valueof<int>(fortytwo2) == 42);
    assert(valueof<float>(pi) == 3.1415f);
    assert(valueof<float>(zero) == 0);
    assert(valueof<float>(zero2) == 0);
    assert(valueof<float>(zero3) == 0);
    assert(valueof<int>(fortytwo_fromstring) == 42);
    assert(valueof<float>(pi_fromstring) == 3.1415f);
  }

  {
    /// [Instantiating non-finite floating-point types]
    auto const float_ = types::float_();
    auto const double_ = types::double_();

    // Instantiate infinite values.
    auto const pfinf = float_(+inf_); // explicitly positive
    auto const nfinf = float_(-inf_);
    auto const pdinf = double_(inf_); // positive by default
    auto const ndinf = double_(-inf_);

    // Instantiate nan values (can also use qnan or snan for "quiet" or "signaling" flavors).
    auto const fnan = float_(nan_);
    auto const dnan = double_(nan_);
    auto const ndnan = double_(-nan_); // negative/positive are defined for NaN types.
    /// [Instantiating non-finite floating-point types]
    assert(valueof<float>(pfinf) == std::numeric_limits<float>::infinity());
    assert(valueof<float>(nfinf) == -std::numeric_limits<float>::infinity());
    assert(valueof<double>(pdinf) == std::numeric_limits<double>::infinity());
    assert(valueof<double>(ndinf) == -std::numeric_limits<double>::infinity());
    assert(std::isnan(valueof<float>(fnan)));
    assert(std::isnan(valueof<double>(dnan)));
    assert(std::isnan(valueof<double>(ndnan)));
  }

  {
    /// [Instantiating integer types from strings]
    auto const i32 = types::int_(32);

    auto const decimal = i32("197");
    auto const hex = i32("0xdeadbeef");
    auto const binary = i32("b01001");
    auto const octal = i32("0777");
    /// [Instantiating integer types from strings]
    assert(valueof<int>(decimal) == 197);
    assert(valueof<unsigned>(hex) == 0xdeadbeef);
    assert(valueof<int>(binary) == 9);
    assert(valueof<unsigned>(octal) == 0777);
  }

  {
    /// [Instantiating types]
    auto const char_ = types::int_(8);
    auto const i32 = types::int_(32);
    auto const float_ = types::float_();
    auto const struct_ = types::struct_("mystruct.2", {i32, float_});

    auto const fortytwo = i32(42);
    auto const hello = char_[12]("hello world");
    auto const pi = float_(3.141592654);
    auto const st = struct_(_t(i32(1), pi));
    /// [Instantiating types]
    assert(valueof<int>(fortytwo) == 42);
    (void) hello;
    assert(valueof<float>(pi) == 3.141592654f);
    (void) st;
  }

  {
    /// [Instantiating arrays as aggregates]
    auto const char_ = types::int_(8);
    auto const ab = char_[2]({'a', 'b'}); // same as char_[2](char_('a'), char_('b'));
    /// [Instantiating arrays as aggregates]
    (void) ab;
  }

  {
    /// [Instantiating arrays from tuples]
    auto const double_ = types::double_();
    auto const doubles = double_[2](_t(1.0f, 2.0));
    /// [Instantiating arrays from tuples]
    (void) doubles;
  }

  {
    /// [Instantiating char pointers]
    // Expands to an expression that evaluates the length of a string constant.
    #define NELEM(arg) \
        cast<GlobalVariable>(arg->stripPointerCasts())->getInitializer()->getType()->getArrayNumElements() \
      /**/
    auto const char_ = types::int_(8);
    // A null-terminated string.
    auto const hello = (*char_)("hello world"); // alloc length: 12
    assert(NELEM(hello) == 12);

    // A null-terminated string from an array.
    char const letters[] = {'h', 'e', 'l', 'l', 'o', '\0'};
    auto const hello2 = (*char_)(letters); // alloc length: 6
    auto const hello3 = (*char_)(array_ref<char>(letters)); // alloc length: 6
    assert(NELEM(hello2) == 6);
    assert(NELEM(hello3) == 6);

    // Arrays of char *.
    char const * init[2] = {"hello", "world"};
    auto const hello4 = ((*char_)[2])(init);
    auto const hello5 = ((*char_)[2])({"hello", "world"});
    auto const hello6 = ((*char_)[2])(_t("hello", "world"));
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
    auto const i32 = types::int_(32);
    auto const double_ = types::double_();

    auto const array1 = (*i32)({1, 2, 3, 4});
    auto const array2 = (*double_)(std::vector<double>{1.0, 2.0, 3.0, 4.0});
    auto const array3 = (*double_)(_t(1, 2, 3.0, 4.0f));
    /// [Instantiating pointers as global arrays]
    (void) array1;
    (void) array2;
    (void) array3;
  }

  {
    /// [Instantiating constant data arrays]
    auto a = get_constant<uint8_t[]>({'a', 'b', 'c'});
    auto b = get_constant<uint16_t[]>({1, 2, 3, 4});
    auto c = get_constant<uint32_t[]>({1, 2, 3, 4});
    auto d = get_constant<uint64_t[]>({1, 2, 3, 4});
    auto e = get_constant<float[]>({1, 2, 3, 4});
    auto f = get_constant<double[]>({1, 2, 3, 4});
    auto g = get_constant<char[]>("hello");
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
