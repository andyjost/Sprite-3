/**
 * @file
 * @brief Provides support for casting operations.
 */

#pragma once
#include "llvm/Support/Casting.h"
#include "sprite/backend/core/fwd.hpp"
#include "sprite/backend/support/special_values.hpp"
#include "sprite/backend/support/type_traits.hpp"
#include <type_traits>

// The specializations below are needed for the wrapper types to pass argument
// substitution when llvm::cast and llvm::dyn_cast are considered (they should
// never match).
namespace llvm
{
  /// Disables llvm::simplify_type for sprite::backend::object.
  template<typename T, template<typename> class Object>
  struct simplify_type<Object<T>>
    : std::enable_if<
          std::is_base_of<sprite::backend::object<T>, Object<T>>::value
            || std::is_base_of<sprite::backend::aux::arg_with_flags<T>, Object<T>>::value
        , sprite::backend::null_arg
        >::type
  { typedef sprite::backend::null_arg SimpleType; };

  /// Disables llvm::cast_retty for sprite::backend::object.
  template<
      class To, class From
    , template<typename> class Object
    >
  struct cast_retty<To, Object<From>>
    : std::enable_if<
          std::is_base_of<sprite::backend::object<From>, Object<From>>::value
            || std::is_base_of<sprite::backend::aux::arg_with_flags<From>, Object<From>>::value
        , sprite::backend::null_arg
        >::type
  {};
}

namespace sprite { namespace backend
{
  /**
   * @brief Enables a casting function.
   *
   * Assumes the presence of Tgt, Src, and Object template parameters.
   */
  #define SPRITE_CAST_RETURN                                                \
      typename std::enable_if<                                              \
          std::is_base_of<sprite::backend::object<Src>, Object<Src>>::value \
        , Object<typename remove_object_wrapper<Tgt>::type>                 \
        >::type                                                             \
    /**/

  /**
   * @brief Performs a cast (unchecked) between wrapped types.
   *
   * Intended to overload with @p llvm::cast.
   *
   * @snippet misc.cpp Using cast
   */
  template<typename Tgt, typename Src, template<typename> class Object>
  inline SPRITE_CAST_RETURN cast(Object<Src> const & src)
  {
    using Tgt_ = typename remove_object_wrapper<Tgt>::type;
    static_assert(
        !std::is_pointer<Tgt_>::value
      , "The target type must not be a pointer (did you add a * by accident?)"
      );
    return Object<Tgt_>(llvm::cast<Tgt_>(src.ptr()));
  }

  /// Performs a cast involving @p arg_with_flags, preserving the flags.
  template<typename Tgt, typename Src, template<typename> class Object>
  inline auto cast(aux::arg_with_flags<Object<Src>> const & src)
    -> aux::arg_with_flags<Object<typename remove_object_wrapper<Tgt>::type>>
    { return std::make_tuple(cast<Tgt>(src.arg()), src.flags()); }

  /**
   * @brief Performs a dynamic (checked) cast between wrapped types.
   *
   * Intended to overload with @p llvm::dyn_cast.
   *
   * @snippet misc.cpp Using dyn_cast
   */
  template<typename Tgt, typename Src, template<typename> class Object>
  inline SPRITE_CAST_RETURN dyn_cast(Object<Src> const & src)
  {
    using Tgt_ = typename remove_object_wrapper<Tgt>::type;
    static_assert(
        !std::is_pointer<Tgt_>::value
      , "The target type must not be a pointer (did you add a * by accident?)"
      );
    return Object<Tgt_>(llvm::dyn_cast<Tgt_>(src.ptr()));
  }

  /// Performs a dyn_cast involving @p arg_with_flags, preserving the flags.
  template<typename Tgt, typename Src, template<typename> class Object>
  inline auto dyn_cast(aux::arg_with_flags<Object<Src>> const & src)
    -> aux::arg_with_flags<Object<typename remove_object_wrapper<Tgt>::type>>
    { return std::make_tuple(dyn_cast<Tgt>(src.arg()), src.flags()); }

  #undef SPRITE_CAST_RETURN

}}
