/**
 * @file
 * @brief Defines macros to help declare operator functions.
 */

#pragma once

#define SPRITE_BINOP_PREAMBLE(RetTy, Lhs, EnTy, Rhs, ArgTy, Quals...) \
    template<typename Lhs, typename Rhs>                              \
    inline Quals typename std::enable_if<                             \
        std::is_base_of<EnTy, Lhs>::value                             \
            && std::is_convertible<Rhs, ArgTy>::value                 \
      , constantobj<RetTy>                                            \
      >::type                                                         \
  /**/

#define SPRITE_BINOP_PREAMBLE2(RetTy, T, EnTy, More...)     \
    typename std::enable_if<                                \
        std::is_base_of<EnTy, T>::value, constantobj<RetTy> \
      >::type                                               \
  /**/

#define SPRITE_BINOP_PREAMBLE3(RetTy, Lhs, EnTy, Rhs, ArgTy, NotArgTy) \
    template<typename Lhs, typename Rhs>                               \
    inline typename std::enable_if<                                    \
        std::is_base_of<EnTy, Lhs>::value                              \
            && std::is_convertible<Rhs, ArgTy>::value                  \
            && !std::is_convertible<Rhs, NotArgTy>::value              \
      , constantobj<RetTy>                                             \
      >::type                                                          \
  /**/

