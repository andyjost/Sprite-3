#include "sprite/backend.hpp"

using namespace sprite::backend;

// Test nested_initializer_list_for.
static_assert(
    std::is_same<
        nested_initializer_list_for<int*>::type
      , int*
      >::value
  , "Error in nested_initializer_list_for."
  );
static_assert(
    std::is_same<
        nested_initializer_list_for<int[]>::type
      , std::initializer_list<int>
      >::value
  , "Error in nested_initializer_list_for."
  );

int main()
{
  module m;
  {
    scope _ = m;

    // Some types needed later.
    type int_ = get_type<int>();
    type const i16 = get_type<int16_t>();
    type const i32 = get_type<int32_t>();
    type const i64 = get_type<int64_t>();
    type const char_ = get_type<char>();
    type const float_ = get_type<float>();

    function main = extern_(int_(), "main");
    {
      scope _ = main;

      // Here's one straightforward example.  The other calls to get_constant are
      // buried in macro expansions.
      get_constant<int>(42);  // Create a get_constant int with value 42.

      #define MYCHECK_TYPE_AND_VALUE(type, value)     \
          {                                           \
            auto const a = get_constant<type>(value); \
            auto const tp = get_type<type>();         \
            auto const b = tp(value);                 \
            assert(get_type(a) == get_type<type>());  \
            assert(valueof<type>(a) == value);        \
            assert(valueof<type>(b) == value);        \
          }                                           \
        /**/

      #define MYCHECK_TYPE(type, ...)                       \
          {                                                 \
            auto const a = get_constant<type>(__VA_ARGS__); \
            auto const tp = get_type<type>();               \
            auto const b = tp(__VA_ARGS__);                 \
            assert(get_type(a) == get_type<type>());        \
            assert(get_type(b) == get_type<type>());        \
          }                                                 \
        /**/

      #define MYCHECK_DEDUCED_TYPE(typeorig, typededuced, ...)  \
          {                                                     \
            auto const a = get_constant<typeorig>(__VA_ARGS__); \
            auto const tp = get_type<typeorig>();               \
            auto const b = tp(__VA_ARGS__);                     \
            assert(get_type(a) == get_type<typededuced>());     \
            assert(get_type(b) == get_type<typededuced>());     \
          }                                                     \
        /**/

      // Integral type.
      MYCHECK_TYPE_AND_VALUE(int, 42)
      MYCHECK_TYPE_AND_VALUE(bool, true)
      MYCHECK_TYPE_AND_VALUE(char, 'a')

      // FP types.
      MYCHECK_TYPE_AND_VALUE(float, 1.0f)
      MYCHECK_TYPE_AND_VALUE(double, 1.0)
      // Note: APFloat-to-long-double conversion is not supported in LLVM 3.3.
      MYCHECK_TYPE(long double, 1.0)

      // Pointer types (note: compile-time constant pointers are only NULL).
      MYCHECK_TYPE(int*, nullptr)

      // Array types.

      // Checks the values in a simple (i.e., no constexpr) constant array.
      #define CHECK_ARRAY(x, n, ty, ...)          \
          {                                       \
            std::vector<ty> test;                 \
            std::vector<ty> correct{__VA_ARGS__}; \
            for(size_t i=0; i<n; ++i)             \
              test.push_back(valueof<ty>(x[i]));  \
            assert(test == correct);              \
          }                                       \
        /**/
      MYCHECK_TYPE(long[2], {1, 2})
      MYCHECK_TYPE(long[2][3], {{1, 2, 3}, {4, 5, 6}})
      MYCHECK_DEDUCED_TYPE(int[][3], int[1][3], {{1, 2, 3}})
      MYCHECK_DEDUCED_TYPE(long[][3], long[3][3], {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}})
      MYCHECK_DEDUCED_TYPE(char[], char[2], std::vector<int>{1, 2})
      MYCHECK_DEDUCED_TYPE(short[], short[2], any_array_ref({1, 2}))

      {
        array_type i_3 = i64[3];
        array_type i_2_3 = i_3[2];
        constant a = get_constant(i_3, {1,2,3});
        CHECK_ARRAY(a, 3, int64_t, 1, 2, 3);
        constant b = get_constant(i_2_3, {{1,2,3},{4,5,6}});
        CHECK_ARRAY(b[0], 3, int64_t, 1, 2, 3);
        CHECK_ARRAY(b[1], 3, int64_t, 4, 5, 6);
      }

      // It should be possible to to use an llvm::Constant in an array
      // initializer.  Note that the initializer itself, however, cannot be a
      // single llvm:Constant.  For example, by analogy with C, a constant of a
      // convertible type can appear in an array initializer, but one array
      // cannot be used as the initilizer for a second array.  These points are
      // illustrated here:
      //
      // float a[2] = { 1.0f, 1 }; // the int, 1, is OK here
      // double b[2] = a; // ILLEGAL
      {
        auto a = signed_(i64)[3];
        assert(a.flags().value == aux::operator_flags::SIGNED);

        // Implicitly convert the array initializers to the signed type.
        auto b = get_constant(signed_(i32)[0], {i64(-1), char_(-1), i16(-1), float_(1.0f)});
        assert(valueof<int32_t>(b[0]) == -1);
        assert(valueof<int32_t>(b[1]) == -1);
        assert(valueof<int32_t>(b[2]) == -1);

        // Same as the last one if signed_ is applied after forming the array.
        auto b_ = get_constant(signed_(i32[0]), {i64(-1), char_(-1), i16(-1), float_(1.0f)});
        assert(valueof<int32_t>(b_[0]) == -1);
        assert(valueof<int32_t>(b_[1]) == -1);
        assert(valueof<int32_t>(b_[2]) == -1);

        auto c = get_constant(unsigned_(i32)[4], {i64(-1), char_(-1), i16(-1), float_(1.0f)});
        assert(valueof<int32_t>(c[0]) == -1);
        assert(valueof<int32_t>(c[1]) == 255);
        assert(valueof<int32_t>(c[2]) == 65535);
      }

      {
        // n.b. i64[3][2] =:= int64_t[2][3]
        auto const x = get_constant(i64[3][2], {{1,2,3},{4,5,6}});
        CHECK_ARRAY(x[0], 3, int64_t, 1, 2, 3);
        CHECK_ARRAY(x[1], 3, int64_t, 4, 5, 6);
      }

      {
        // n.b. int64_t[3][2] =:= i64[2][3]
        auto const x = get_constant<int64_t[3][2]>({{1,2},{3,4},{5,6}});
        CHECK_ARRAY(x[0], 2, int64_t, 1, 2);
        CHECK_ARRAY(x[1], 2, int64_t, 3, 4);
        CHECK_ARRAY(x[2], 2, int64_t, 5, 6);
      }

      {
        auto const x = get_constant<int64_t[3][2]>(
            std::vector<std::vector<int64_t>>{{1,2},{3,4},{5,6}}
          );
        CHECK_ARRAY(x[0], 2, int64_t, 1, 2);
        CHECK_ARRAY(x[1], 2, int64_t, 3, 4);
        CHECK_ARRAY(x[2], 2, int64_t, 5, 6);
      }

      {
        auto const x = get_type<int[1][1]>()(
            std::vector<std::vector<int>>{{42}}
          );
        CHECK_ARRAY(x[0], 1, int64_t, 42);
      }

      // Check that ConstantDataArray is produced, where possible.
      #define CHECK_CDA(type, ...)                          \
          {                                                 \
            auto const x = get_constant<type>(__VA_ARGS__); \
            assert(                                         \
                dyn_cast<llvm::ConstantDataArray>(x.ptr())  \
                  && "expected ConstantDataArray"           \
              );                                            \
          }
        /**/

      CHECK_CDA(int16_t[], {1,1,1,1})
      CHECK_CDA(float[3], {1,2,3})
      CHECK_CDA(float[], {1.f})
      CHECK_CDA(float[], {1.1, 2.2})
      CHECK_CDA(double[2], {1.1, 2.2})
      CHECK_CDA(double[], {1.f, 2.f})
      CHECK_CDA(char[], "hello")

      // With implicit conversion after the initializer_list is formed.
      {
        std::initializer_list<int> a = {1,2,3};
        CHECK_CDA(char[], a)
      }

      {
        auto const x = get_constant<int16_t[2]>({i64(1), i64(1)});
        assert(dyn_cast<llvm::ConstantDataArray>(x.ptr()) && "expected ConstantDataArray");
      }

      // Struct types.
      (void) get_type<>(); // empty struct
      (void) get_type<std::tuple<int>>(); // one-element struct
      (void) get_type<int, int>(); // two-element struct
      (void) get_type<int, int, int>(); // three-element struct
      (void) get_type<std::tuple<int, int, int>>(); // three-element struct
      (void) get_type<int, std::tuple<int, int>, float>(); // nested
      get_type<int, int>()(any_array_ref{0,1});

      #define CHECK_VALUE(x, ty, val)      \
          {                                \
            assert(valueof<ty>(x) == val); \
          }                                \
        /**/

      {
        auto const x = get_constant<int, int>(0, 1);
        CHECK_VALUE(x[0], int, 0);
        CHECK_VALUE(x[1], int, 1);
      }

      {
        auto const x = get_constant<std::tuple<int, int>>(std::make_tuple(0, 1));
        CHECK_VALUE(x[0], int, 0);
        CHECK_VALUE(x[1], int, 1);
      }

      {
        auto const x = get_constant<int[3], int, int>({1,2,3}, 4, 5);
        CHECK_ARRAY(x[0], 3, int, 1, 2, 3)
        CHECK_VALUE(x[1], int, 4);
        CHECK_VALUE(x[2], int, 5);
      }

      {
        auto const x = get_constant<std::tuple<char[2], float>, int[3], int, double>(
            _t(_a{'a', 'b'}, 1.0f), {1,2,3}, 4, 7
          );
        CHECK_ARRAY(x[0][0], 2, char, 'a', 'b');
        CHECK_VALUE(x[0][1], float, 1.0f);
        CHECK_ARRAY(x[1], 3, int, 1, 2, 3);
        CHECK_VALUE(x[2], int, 4);
        CHECK_VALUE(x[3], double, 7.0);
      }

      // Check that the simple form of get_constant with no type argument
      // compiles for a variety of types.
      {
        int a = 0;
        float b = 0.0f;
        int c[3] = {1,2,3};
        char d[] = "hello";
        std::tuple<int, float> e{0, 3.14f};
        int const f = 0;
        get_constant(a);
        get_constant(b);
        get_constant(c);
        get_constant(d);
        get_constant(e);
        get_constant(f);
      }
    }

    // Check multi-step creations involving implicit conversion.
    {
      auto a = signed_(i64)(char_('a'));
      assert(valueof<int64_t>(a) == 'a');
    }
    {
      auto a = signed_(i64)(char_(-1));
      assert(valueof<int64_t>(a) == -1);
    }
    {
      auto a = unsigned_(i64)(char_(-1));
      assert(valueof<uint64_t>(a) == 255);
    }

    // DELETEME
    static_(int_, "x").set_initializer(0);
    static_(int_, "x").set_initializer(get_constant<int>(42));
  }
  return 0;
}
