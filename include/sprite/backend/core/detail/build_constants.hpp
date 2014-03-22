#pragma once
#include "sprite/backend/config.hpp"
#include "sprite/backend/core/constant.hpp"
#include "sprite/backend/core/type.hpp"
#include "sprite/backend/support/array_ref.hpp"
#include <tuple>
#include <vector>

namespace sprite { namespace backend { namespace aux
{
  /// Terminating case for building constant elements from a tuple.
  template<size_t I=0, typename C, typename E, typename...T>
  inline typename std::enable_if<I == sizeof...(T), void>::type
  build_uniform_constants(C &, E const &, std::tuple<T...> const &)
  {}

  /**
   * @brief Iterating case for building constant elements from a tuple.
   *
   * Used when there is a single element type, @p E, to instantiate multiple
   * times.
   *
   * Extracts initializer value @p I from tuple @p t, uses it to construct a
   * constant value of type @p e, and loads the result into back-insertable
   * container @p c.
   */
  template<size_t I=0, typename C, typename E, typename...T>
  inline typename std::enable_if<I < sizeof...(T), void>::type
  build_uniform_constants(C & c, E const & e, std::tuple<T...> const & t)
  {
    static_assert(
        !std::is_same<
            typename std::tuple_element<I, std::tuple<T...>>::type
          , Constant*
          >::value
      , "Expected raw initializer value or constant, not llvm::Constant *"
      );
    c.push_back((e % std::get<I>(t)).ptr());
    build_uniform_constants<I+1, C, E, T...>(c, e, t);
  }

  /// Build constants from an array_ref.
  template<typename C, typename E, typename T>
  inline void build_uniform_constants(
      C & c, E const & e, array_ref<T> const & values
    )
  {
    static_assert(
        !std::is_same<T, Constant*>::value
      , "Expected raw initializer value or constant, not llvm::Constant *"
      );
    for(auto const & value: values) { c.push_back((e % value).ptr()); }
  }

  /// Terminating case for building constant elements from a tuple.
  template<size_t I=0, typename C, typename Ty, typename...T>
  inline typename std::enable_if<I == sizeof...(T), bool>::type
  build_nonuniform_constants(C &, Ty const &, std::tuple<T...> const &)
    { return true; }

  /**
   * @brief Iterating case for building constant elements from a tuple.
   *
   * Used when there is an indexable sequence, @p Ty, of types to instantiate.
   *
   * Extracts initializer value @p I from tuple @p t, uses it to construct a
   * constant value of type @p ty[I], and loads the result into
   * back-insertable container @p c.
   *
   * Returns false if instantiation failed due to too many initializer
   * values.
   */
  template<size_t I=0, typename C, typename Ty, typename...T>
  inline typename std::enable_if<I < sizeof...(T), bool>::type
  build_nonuniform_constants(C & c, Ty const & ty, std::tuple<T...> const & t)
  {
    using initializer_type =
        typename std::tuple_element<I,std::tuple<T...>>::type;
    static_assert(
        !std::is_same<initializer_type, Constant*>::value
      , "Expected raw initializer value or constant, not llvm::Constant *"
      );
    size_t const i = I;
    assert(ty->indexValid(i));
    c.push_back((element_type(ty, i) % std::get<I>(t)).ptr());
    return build_nonuniform_constants<I+1, C, Ty, T...>(c, ty, t);
  }

  /// Builds a struct from initializers in an @p array_ref.
  template<size_t I=0, typename C, typename Ty, typename T>
  inline void build_nonuniform_constants(
      C & c, Ty const & ty, array_ref<T> const & values
    )
  {
    static_assert(
        !std::is_same<T, Constant*>::value
      , "Expected raw initializer value or constant, not llvm::Constant *"
      );
    size_t i = 0;
    for(auto const & value: values)
    {
      auto const elem_ty = element_type(ty, i++);
      c.push_back((elem_ty % value).ptr());
    }
  }

  /**
   * @brief Generic constant array builder.
   *
   * Even if @p T is a @p constant, the @p % operator is applied in case a
   * conversion is needed.
   *
   * This function is distinct from the specialized versions that create @p
   * ConstantDataArrays because there is no @p Tgt parameter.
   */
  template<typename C>
  constant generic_build_array(
      array_type_with_flags const & ty, C const & values, size_t size
    )
  {
    auto const elem_ty = element_type(ty);
    llvm::SmallVector<Constant*, 8> args;
    args.reserve(size);
    aux::build_uniform_constants(args, elem_ty, values);
    auto const px = SPRITE_APICALL(ConstantArray::get(ty.ptr(), args));
    return constant(px);
  }

  /**
   * @brief Generic constant struct builder.
   *
   * Even if @p T is a @p constant, the @p % operator is applied in case a
   * conversion is needed.
   */
  template<typename C>
  constant generic_build_struct(struct_type const & ty, C const & values, size_t size)
  {
    if(size != ty->getStructNumElements())
      throw value_error("Wrong number of elements in struct initializer");
    llvm::SmallVector<Constant*, 8> args;
    args.reserve(size);
    aux::build_nonuniform_constants(args, ty, values);
    auto const px = SPRITE_APICALL(ConstantStruct::get(ty.ptr(), args));
    return constant(px);
  }
}}}

