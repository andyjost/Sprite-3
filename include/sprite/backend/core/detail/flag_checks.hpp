#pragma once

/**
 * @brief Checks thet only the expected flags are set.
 */
#define SPRITE_ALLOW_FLAGS(flags, what, allowed)                 \
    {                                                            \
      using namespace aux;                                       \
      if(flags.value & ~(allowed))                               \
        { throw parameter_error("Unexpected flags for " what); } \
    }                                                            \
  /**/

#define SPRITE_NO_FLAG_CHECK(flags, name)

#define SPRITE_ALLOW_NO_FLAGS(flags, name) SPRITE_ALLOW_FLAGS(flags, name, 0)

#define SPRITE_ALLOW_NSW_NUW_FLAGS(flags, name)    \
    SPRITE_ALLOW_FLAGS(flags, name                 \
      , operator_flags::NUW | operator_flags::NSW  \
      )                                            \
  /**/

#define SPRITE_ALLOW_SIGNED_FLAG(flags, name)               \
    SPRITE_ALLOW_FLAGS(flags, name, operator_flags::SIGNED) \
  /**/

#define SPRITE_ALLOW_DIV_FLAGS(flags, name)               \
    SPRITE_ALLOW_FLAGS(flags, name                        \
      , operator_flags::SIGNED | operator_flags::UNSIGNED \
          | operator_flags::EXACT                         \
      )                                                   \
   check_for_exactly_one_signed_flag(flags, name);        \
  /**/

#define SPRITE_ALLOW_SHR_FLAGS(flags, name)                  \
    SPRITE_ALLOW_FLAGS(flags, name                           \
      , operator_flags::ARITHMETIC | operator_flags::LOGICAL \
          | operator_flags::EXACT                            \
      )                                                      \
   check_for_exactly_one_arithmetic_flag(flags, name);       \
  /**/

#define SPRITE_REQUIRE_SIGNED_UNSIGNED_FLAG(flags, name)  \
    SPRITE_ALLOW_FLAGS(flags, name                        \
      , operator_flags::SIGNED | operator_flags::UNSIGNED \
      )                                                   \
   check_for_exactly_one_signed_flag(flags, name);        \
  /**/

