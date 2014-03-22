#include <iostream>
#include "sprite/backend.hpp"
#include <array>

using namespace sprite::backend;

// Test nested_initializer_list_for.
#define CHECK_NIL(type_in, type_out)                            \
    static_assert(                                               \
        std::is_same<                                            \
            nested_initializer_list_for<type_in>::type, type_out \
          >::value                                               \
      , "Error in nested_initializer_list_for."                  \
      );                                                         \
  /**/

CHECK_NIL(int, int)
CHECK_NIL(int&, int&)
CHECK_NIL(int*, int*)
CHECK_NIL(std::vector<int>, std::vector<int>)
CHECK_NIL(int[], std::initializer_list<int>)
CHECK_NIL(int[2], std::initializer_list<int>)
CHECK_NIL(int[2][3], std::initializer_list<std::initializer_list<int>>)
CHECK_NIL(int[][3], std::initializer_list<std::initializer_list<int>>)

template<typename T>
void mytest(array_ref<array_ref<T>> const & seq)
{
  for(auto const & a: seq)
  {
    std::cout << "array_ref<int> @" << &a << " with storage @" << a.data() << std::endl;
    for(auto const & b: a)
    {
      std::cout << "int @" << &b << std::endl;
    }
  }
}

template<typename T, typename U>
void check_2d(T const & seq, U const & ans)
{
  std::vector<int> out;
  for(auto const & a: seq)
    for(auto const & b: a)
      out.push_back(b);
  assert(out == ans);
}

template<typename T, typename U>
void check_3d(T const & seq, U const & ans)
{
  std::vector<int> out;
  for(auto const & a: seq)
    for(auto const & b: a)
      for(auto const & c: b)
        out.push_back(c);
  assert(out == ans);
}

int main()
{
  // Test array_ref.
  (void) array_ref<int>({1,2});
  (void) array_ref<char>("hello");
  {
    int x[2] = {1,2};
    (void) array_ref<int>(x);
  }
  {
    std::vector<int> x{1,2};
    (void) array_ref<int>(x);
    check_2d(
        array_ref<array_ref<int>>{x,x}
      , std::vector<int>{1,2,1,2}
      );
  }
  {
    std::vector<int> x{1,2};
    check_3d(
        array_ref<array_ref<array_ref<int>>>{{x,x},{x,x}}
      , std::vector<int>{1,2,1,2,1,2,1,2}
      );
  }
  {
    array_ref<array_ref<int>> seq{{1,2},{3,4}};
    check_2d(
        array_ref<array_ref<int>>{{1,2},{3,4}}
      , std::vector<int>{1,2,3,4}
      );
  }
  (void) array_ref<array_ref<array_ref<int>>>{{{1,2},{3,4}},{{1,2},{3,4}}};
  (void) array_ref<int[2]>{{1,2},{3,4}};
  (void) array_ref<int[]>{{1},{2,3}}; // irregular ok with unspecified extent
  (void) array_ref<int[2][2]>{{{1,2},{3,4}},{{1,2},{3,4}}};
  try {
    (void) array_ref<int[2][2]>{{{1,2,2},{3,4}},{{1,2},{3,4}}};
    assert(0);
  } catch(value_error const &) { /* ok: bad initializer extent */ }

  (void) array_ref<double[1][1][2]>{{{{1,2}}}};

  // Test any_array_ref.
  (void) any_array_ref(std::vector<int>{1,2});
  (void) any_array_ref(llvm::SmallVector<int,4>());
  {
    std::vector<int> x{1,2};
    std::vector<int> y{3,4};
    (void) any_array_ref({x,y}); // 2D
    (void) any_array_ref({{x,y},{y,x}}); // 3D
    std::vector<std::vector<int>> xy{x,y};
    (void) any_array_ref(xy);
    (void) any_array_ref({xy});
  }
  {
    int x[] = {1,2};
    (void) any_array_ref(x);
  }
  (void) any_array_ref({1,2}); // 1D
  (void) any_array_ref{{1,2},{3,4}}; // 2D
  (void) any_array_ref{{{1,2},{3,4}},{{5,6},{7,8}}}; // 3D
  (void) any_array_ref{{{{1,2},{3,4}},{{5,6},{7,8}}},{{{1,2},{3,4}},{{5,6},{7,8}}}}; // 4D
  (void) any_array_ref{{1,2},{3,4,5}}; // 2D irregular (allowed).
  return 0;
}
