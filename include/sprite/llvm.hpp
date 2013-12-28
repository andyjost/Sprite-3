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
  namespace llvm {}
}


#include <sprite/llvm/core/type_factory.hpp>
#include <sprite/llvm/core/def.hpp>
#include <sprite/llvm/core/value.hpp>
#include <sprite/llvm/core/wrappers.hpp>
#include <sprite/llvm/core/instructions.hpp>
#include <sprite/llvm/operators/create.hpp>
#include <sprite/llvm/operators/initialize.hpp>
#include <sprite/llvm/operators/constexpr.hpp>

