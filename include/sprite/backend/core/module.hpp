/**
 * @file
 * @brief Defines the module class.
 */

#pragma once
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "sprite/backend/core/object.hpp"
#include "sprite/backend/core/wrappers.hpp"
#include "sprite/backend/support/wrap.hpp"

namespace sprite { namespace backend
{
  struct Scaffolding {}; // To be deleted.

  /**
   * @brief
   * Represents an LLVM module.
   *
   * @details
   * Provides a convenient interface for building LLVM types.  LLVM types are
   * created using named type-creation methods.
   *
   * Example:
   *
   * @snippet types.cpp Creating basic types
   */
  struct module : object<llvm::Module, Scaffolding>
  {
    typedef object<llvm::Module, Scaffolding> base_type;

    // The address space that pointer types will belong to.
    unsigned _addrSpace;

  public:

    /**
     * @brief Creates a new module.
     *
     * If the module is NULL, then a new module is created.
     */
    explicit module(llvm::Module * module=0, unsigned addrSpace=0);

    /// Creates a new module.
    explicit module(string_ref const &, unsigned addrSpace=0);

    // Default copy, assignment, and destructor are fine.

    /// Gets the address space associated with this factory.
    unsigned addrSpace() const { return _addrSpace; }

    /// Gets the associated LLVM context.
    llvm::LLVMContext & context() const
      { return ptr()->getContext(); }

    friend bool operator==(module const & lhs, module const & rhs)
    {
      return lhs.ptr() == rhs.ptr()
        && lhs.addrSpace() == rhs.addrSpace();
    }

    friend bool operator!=(module const & lhs, module const & rhs)
      { return !(lhs == rhs); }

    // ====== Type-Creation Methods.

    /// Creates an integer type.
    integer_type int_(unsigned numBits) const;

    /// Creates a bool type.
    integer_type bool_() const { return this->int_(1); }

    /// Creates a char type.
    integer_type char_() const { return this->int_(8); }

    /// Creates the 32-bit float type.
    fp_type float_() const;

    /// Creates the 64-bit float type.
    fp_type double_() const;

    /// Creates the void type.
    type void_() const;

    // ====== Type-Inspection Methods.

    /// Get the size, in bytes of a type.
    uint64_t sizeof_(Type * type) const
    {
      llvm::DataLayout layout("");
      return layout.getTypeAllocSize(type);
    }

    /// Freestanding @p sizeof_ function for wrapper objects.
    template<typename T>
    friend uint64_t sizeof_(typeobj<T> const & wrapper);

    /** 
     * @brief Creates an anonymous struct (uniqued by structural equivalence).
     *
     * @snippet types.cpp Creating an anonymous struct
     */
    struct_type
    struct_(array_ref<type> const & elements) const;

    /**
     * @brief Gets a struct by name.
     *
     * If the struct has not been created, a new opaque struct will be created.
     *
     * @snippet types.cpp Creating opaque structs
     */
    struct_type
    struct_(string_ref const & name) const;

    /**
     * @brief Defines a struct.
     *
     * The struct must not already have a body (though, it may exist and be
     * opaque).
     *
     * @snippet types.cpp Creating structs
     */
    struct_type
    struct_(
        string_ref const & name
      , array_ref<type> const & elements
      ) const;
  };
}}

#include "sprite/backend/core/module_impl.hpp"

