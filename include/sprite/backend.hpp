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

#include <sprite/backend/core/constant.hpp>
#include <sprite/backend/core/constexpr.hpp>
#include <sprite/backend/core/def.hpp>
#include <sprite/backend/core/function.hpp>
#include <sprite/backend/core/fwd.hpp>
#include <sprite/backend/core/get_constant.hpp>
#include <sprite/backend/core/global.hpp>
#include <sprite/backend/core/instruction.hpp>
#include <sprite/backend/core/instructions.hpp>
#include <sprite/backend/core/label.hpp>
#include <sprite/backend/core/module.hpp>
#include <sprite/backend/core/object.hpp>
#include <sprite/backend/core/scope.hpp>
#include <sprite/backend/core/type.hpp>
#include <sprite/backend/core/value.hpp>
#include <sprite/backend/core/valueof.hpp>

