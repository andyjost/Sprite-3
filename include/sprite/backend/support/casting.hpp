/**
 * @file
 * @brief Provides support for casting operations.
 */

#pragma once
#include "llvm/Support/Casting.h"
#include "sprite/backend/core/wrappers_fwd.hpp"
#include "sprite/backend/support/special_values.hpp"
#include "sprite/backend/support/wrap.hpp"
#include <type_traits>

// The specializations below are needed for the wrapper types to pass argument
// substitution when llvm::cast and llvm::dyn_cast are considered (they should
// never match).
namespace llvm
{
  /// Disables llvm::simplify_type for wrappers.
  template<typename T, typename Factory, template<typename,typename> class Wrapper_>
  struct simplify_type<Wrapper_<T,Factory>>
    : std::enable_if<
          std::is_base_of<
              sprite::backend::object<T, Factory>, Wrapper_<T, Factory>
            >::value
        , sprite::backend::null_arg
        >::type
  { typedef sprite::backend::null_arg SimpleType; };

  /// Disables llvm::cast_retty for wrappers.
  template<
      class To, class From, typename Factory
    , template<typename,typename> class Wrapper_
    >
  struct cast_retty<
      To
    , Wrapper_<From, Factory>
    >
    : std::enable_if<
          std::is_base_of<
              sprite::backend::object<From, Factory>, Wrapper_<From, Factory>
            >::value
        , sprite::backend::null_arg
        >::type
  {};
}

namespace sprite { namespace backend
{
  /**
   * @brief Enables a casting function.
   *
   * Assumes the presence of Tgt, Src, and Wrapper_ template parameters.
   */
  #define SPRITE_CAST_RETURN                                                \
      typename std::enable_if<                                              \
          std::is_base_of<                                                  \
              sprite::backend::object<Src, Factory>, Wrapper_<Src, Factory> \
            >::value                                                        \
        , Wrapper_<Tgt, Factory>                                            \
        >::type                                                             \
    /**/

  /**
   * @brief Performs a cast (unchecked) between wrapped types.
   *
   * Intended to overload with @p llvm::cast.
   *
   * @snippet misc.cpp Using cast
   */
  template<
      typename Tgt, typename Src
    , typename Factory, template<typename, typename> class Wrapper_
    >
  inline SPRITE_CAST_RETURN cast(Wrapper_<Src, Factory> const & src)
  {
    static_assert(
        !std::is_pointer<Tgt>::value
      , "The target type must not be a pointer (did you add a * by accident?)"
      );
    return wrap<Tgt>(src.factory(), src.ptr());
  }

  /**
   * @brief Performs a dynamic (checked) cast between wrapped types.
   *
   * Intended to overload with @p llvm::dyn_cast.
   *
   * @snippet misc.cpp Using dyn_cast
   */
  template<
      typename Tgt, typename Src
    , typename Factory, template<typename, typename> class Wrapper_>
  inline SPRITE_CAST_RETURN dyn_cast(Wrapper_<Src, Factory> const & src)
  {
    static_assert(
        !std::is_pointer<Tgt>::value
      , "The target type must not be a pointer (did you add a * by accident?)"
      );
    return wrap(src.factory(), llvm::dyn_cast<Tgt>(src.ptr()));
  }

  #undef SPRITE_CAST_RETURN

}}
