/**
 * @file
 * @brief Master include file for the Sprite LLVM extension interface.
 */

#pragma once

/// Contains all definitions made by Sprite.
namespace sprite
{
  /**
   * @brief Contains the LLVM extension interface.
   *
    * The extensions found in this namespace are intended to simplify the LLVM
    * interface, making it more expressive and easier to use.
   */
  namespace backend {}
}


#include <sprite/backend/core/type_factory.hpp>
#include <sprite/backend/core/def.hpp>
#include <sprite/backend/core/value.hpp>
#include <sprite/backend/core/wrappers.hpp>
#include <sprite/backend/core/instructions.hpp>
#include <sprite/backend/operators/create.hpp>
#include <sprite/backend/operators/initialize.hpp>
#include <sprite/backend/operators/constexpr.hpp>

