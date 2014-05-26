/**
 * @file
 * @brief Defines the module class.
 */

#pragma once
#include "sprite/backend/config.hpp"
#include "sprite/backend/core/object.hpp"
#include "sprite/backend/support/array_ref.hpp"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"

namespace sprite { namespace backend
{
  /**
   * @brief
   * Represents an LLVM module.
   *
   * @details
   * Among other things, provides a convenient interface for building LLVM
   * types.  LLVM types are created using named type-creation methods.
   *
   * Example:
   *
   * @snippet types.cpp Creating basic types
   */
  struct module : object<llvm::Module>
  {
    typedef object<llvm::Module> base_type;

  public:

    using object<llvm::Module>::object;

    /// Creates a new module.
    explicit module(
        string_ref const & name=".anon"
      , llvm::LLVMContext & context = SPRITE_APICALL(llvm::getGlobalContext())
      )
      : base_type(SPRITE_APICALL(new llvm::Module(name, context)))
    {}

    // Default copy, assignment, and destructor are fine.

    /// Gets the associated LLVM context.
    llvm::LLVMContext & context() const
    {
      return px ? px->getContext() : SPRITE_APICALL(llvm::getGlobalContext());
    }

    friend bool operator==(module const & lhs, module const & rhs)
      { return lhs.ptr() == rhs.ptr(); }

    friend bool operator!=(module const & lhs, module const & rhs)
      { return !(lhs == rhs); }
  };
}}

