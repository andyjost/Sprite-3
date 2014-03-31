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
  module const m("contexprs");
  scope _ = m;

  {
    /// [alignof_]
    auto i32 = types::int_(32);
    auto four = alignof_(i32);
    /// [alignof_]
    (void) four;
  }

  {
    /// [offsetof_]
    auto i32 = types::int_(32);
    auto st = types::struct_({i32, i32});
    auto four = offsetof_(st, 1);
    /// [offsetof_]
    (void) four;
  }

  {
    /// [neg]
    auto i32 = types::int_(32);
    auto six = i32(6);
    auto nsix = -six;
    auto nsix_nsw = -nuw (six);

    auto float_ = types::float_();
    auto two = float_(2.0f);
    auto ntwo = -two;
    /// [neg]
    (void) nsix;
    (void) nsix_nsw;
    (void) ntwo;
  }

  {
    /// [pos]
    auto i32 = types::int_(32);
    auto six = i32(6);
    auto still_six = +six;
    /// [pos]
    (void) still_six;
  }

  {
    /// [inv]
    auto i8 = types::int_(8);
    auto b11010010 = i8("b11010010");
    auto b00101101 = ~b11010010;
    /// [inv]
    (void) b00101101;
  }

  {
    /// [add]
    auto i32 = types::int_(32);
    auto six = i32(6);
    auto twelve = six + six;
    auto also_twelve = six +nuw (six);
    auto multiple_flags = six +(nuw,nsw) (six);
    /// [add]
    (void) twelve;
    (void) also_twelve;
    (void) multiple_flags;
  }

  {
    /// [sub]
    auto i32 = types::int_(32);
    auto six = i32(6);
    auto zero = six - six;
    auto also_zero = six -nuw (six);
    auto multiple_flags = six -(nuw,nsw) (six);
    /// [sub]
    (void) zero;
    (void) also_zero;
    (void) multiple_flags;
  }

  {
    /// [mul]
    auto i32 = types::int_(32);
    auto six = i32(6);
    auto thirtysix = six * six;
    auto also_thirtysix = six *nuw (six);
    auto multiple_flags = six *(nuw,nsw) (six);
    /// [mul]
    (void) thirtysix;
    (void) also_thirtysix;
    (void) multiple_flags;
  }

  {
    /// [div]
    auto i32 = types::int_(32);
    auto six = i32(6);
    auto neg_one = i32(-1);
    auto neg_six = six /signed_ (neg_one);
    auto zero = six /unsigned_ (neg_one);
    auto one = six /(unsigned_,exact) (six);
    /// [div]
    (void) neg_six;
    (void) zero;
    (void) one;
  }

  {
    /// [rem]
    auto i32 = types::int_(32);
    auto six = i32(6);
    auto four = i32(4);
    auto neg_one = i32(-1);
    auto also_six = six %signed_ (neg_one);
    auto two = six %unsigned_ (four);
    /// [rem]
    (void) also_six;
    (void) two;
  }

  {
    /// [and]
    auto i32 = types::int_(32);
    auto twelve = i32("01100");
    auto ten = i32("01010");
    auto eight = twelve & ten;
    /// [and]
    (void) eight;
  }

  {
    /// [or]
    auto i32 = types::int_(32);
    auto twelve = i32("01100");
    auto ten = i32("01010");
    auto fourteen = twelve | ten;
    /// [or]
    (void) fourteen;
  }

  {
    /// [xor]
    auto i32 = types::int_(32);
    auto twelve = i32("01100");
    auto ten = i32("01010");
    auto six = twelve ^ ten;
    /// [xor]
    (void) six;
  }

  {
    /// [shl]
    auto i32 = types::int_(32);
    auto one = i32(1);
    auto two = one << one;
    auto also_two = one <<nuw (one);
    auto still_two = one <<(nuw,nsw) (one);
    /// [shl]
    (void) two;
    (void) also_two;
    (void) still_two;
  }

  {
    /// [shr]
    auto i8 = types::int_(8);
    auto neg_one = i8(-1); // 11111111
    auto four = i8(4);
    auto fifteen = neg_one >>logical (four); // 00001111
    auto one = i8(1);
    auto two = four >>(logical,exact) (one);
    auto also_neg_one = neg_one >>(arithmetic) (four); // 11111111
    /// [shr]
    assert(valueof<int>(fifteen) == 15);
    assert(valueof<int>(two) == 2);
    assert(valueof<int>(also_neg_one) == -1);
  }

  {
    /// [typecast]
    auto i8 = types::int_(8);
    auto i32 = types::int_(32);
    auto float_ = types::float_();
    auto double_ = types::double_();

    auto trunc = typecast(i32(256), i8); // i8 1
    auto sext = typecast(i8(-1), signed_(i32)); // i32 -1
    auto zext = typecast(i8(255), unsigned_(i32)); // i32 255
    auto fptrunc = typecast(double_(1.5), float_); // float 1.5f
    auto fpextend = typecast(float_(1.5f), double_); // double 1.5
    auto uitofp = typecast(i8(-1), unsigned_(float_)); // float 255.0f
    auto sitofp = typecast(i8(-1), signed_(float_)); // float -1.0f
    auto fptoui = typecast(float_(1.0), unsigned_(i32)); // i32 1
    auto fptosi = typecast(float_(-1.0), signed_(i32)); // i32 -1
    auto ptrtoint = typecast((*i8)(null), i32); // i32 0
    auto inttoptr = typecast(i32("0xdeadbeef"), *i8); // i8* 0xdeadbeef

    /// [typecast]
    (void) trunc;
    (void) sext;
    (void) zext;
    (void) fptrunc;
    (void) fpextend;
    (void) uitofp;
    (void) sitofp;
    (void) fptoui;
    (void) fptosi;
    (void) ptrtoint;
    (void) inttoptr;
  }

  {
    /// [bitcast]
    auto i32 = types::int_(32);
    auto float_ = types::float_();
    auto qnan = bitcast(i32(-1), float_);
    /// [bitcast]
    (void) qnan;
  }

  {
    /// [select]
    auto i32 = types::int_(32);
    auto bool_ = types::bool_();
    auto one = select(bool_(true), i32(1), i32(2));
    /// [select]
    (void) one;
  }

  {
    /// [Computing addresses]
    auto i32 = types::int_(32);
    auto myarray = (extern_(i32[2], "myarray").set_initializer({1,2}));
    auto address_of_myarray = &myarray;
    auto address_of_two = &myarray[1];
    /// [Computing addresses]
    (void) myarray;
    (void) address_of_myarray;
    (void) address_of_two;
  }
  
  
  #if 0
  auto f = static_(i32(), "f");
  {
    scope f_body(f);
    auto array = static_(i32[2]) = {1,2};
    auto index = static_(i32 = 0);
    auto tmp = alloca(i32, "tmp")
    auto one = i32(1);
    store(&index, index ^ one);
    load(tmp, &array[index]);
    return_(tmp);
  }
  #endif

  #if 0
  {
    #if 0
    auto myarray =
        extern_(i32[2]) = {1, 2};
    i32(v) + 4
    i32[2](_a{1, 2})
    auto ptr = &myarray[0][0]
    myarray + 5
    #endif
  }

  auto ab = struct_({i32|"a", *i8|"b"})((i32(1), *i8("hello")));
  int64 a = valueof<int64>(ab.attr("a"));

  auto Widget = struct_("Widget", {*i8, i32, i1[4]});
  auto awidget = Widget((null, 42, {true, false, false, true}));
  #endif

  return 0;
}
