#pragma once

#define SPRITE_NO_FLAG_CHECK(name)

#define SPRITE_ALLOW_NSW_NUW_FLAGS(name)           \
    SPRITE_ALLOW_FLAGS(arg, name                   \
      , operator_flags::NUW | operator_flags::NSW  \
      )                                            \
  /**/

#define SPRITE_ALLOW_SIGNED_FLAG(name)                    \
    SPRITE_ALLOW_FLAGS(arg, name, operator_flags::SIGNED) \
  /**/

#define SPRITE_ALLOW_DIV_FLAGS(name)                      \
    SPRITE_ALLOW_FLAGS(arg, name                          \
      , operator_flags::SIGNED | operator_flags::UNSIGNED \
          | operator_flags::EXACT                         \
      )                                                   \
   check_for_exactly_one_signed_flag(arg.flags(), name);  \
  /**/

#define SPRITE_ALLOW_SHR_FLAGS(name)                         \
    SPRITE_ALLOW_FLAGS(arg, name                             \
      , operator_flags::ARITHMETIC | operator_flags::LOGICAL \
          | operator_flags::EXACT                            \
      )                                                      \
   check_for_exactly_one_arithmetic_flag(arg.flags(), name); \
  /**/

#define SPRITE_REQUIRE_SIGNED_UNSIGNED_FLAG(name)         \
    SPRITE_ALLOW_FLAGS(arg, name                          \
      , operator_flags::SIGNED | operator_flags::UNSIGNED \
      )                                                   \
   check_for_exactly_one_signed_flag(arg.flags(), name);  \
  /**/

