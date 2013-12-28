/**
 * @file
 * @brief Defines the TypeFactory class.
 */

#pragma once
#include "llvm/ADT/ArrayRef.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "sprite/llvm/core/wrappers.hpp"
#include "sprite/llvm/support/wrap.hpp"

namespace sprite { namespace llvm
{
  /**
   * @brief
   * Provides a convenient interface for building LLVM types.
   *
   * @details
   * LLVM types are created using named type-creation methods.
   *
   * Example:
   *
   * @snippet types.cpp Creating basic types
   */
  class TypeFactory
  {
    /// The module that types will be gotten from.
    llvm_::Module * _module; 

    // The address space that pointer types will belong to.
    unsigned _addrSpace;

  public:

    /**
     * @brief Creates a new TypeFactory.
     *
     * If the module is NULL, then a new module is created.
     */
    explicit TypeFactory(llvm_::Module * module=0, unsigned addrSpace=0);

    // Default copy, assignment, and destructor are fine.

    /// Gets the Module associated with this factory.
    llvm_::Module * module() const { return _module; }

    /// Gets the address space associated with this factory.
    unsigned addrSpace() const { return _addrSpace; }

    /// Gets the associated LLVM context.
    llvm_::LLVMContext & context() const
      { return this->module()->getContext(); }

    friend bool operator==(TypeFactory const & lhs, TypeFactory const & rhs)
    {
      return lhs.module() == rhs.module()
        && lhs.addrSpace() == rhs.addrSpace();
    }

    friend bool operator!=(TypeFactory const & lhs, TypeFactory const & rhs)
      { return !(lhs == rhs); }

    // ====== Type-Creation Methods.

    /// Creates an integer type.
    TypeWrapper<llvm_::IntegerType> int_(unsigned numBits) const;

    /// Creates a bool type.
    TypeWrapper<llvm_::IntegerType> bool_() const { return this->int_(1); }

    /// Creates a char type.
    TypeWrapper<llvm_::IntegerType> char_() const { return this->int_(8); }

    /// Creates the 32-bit float type.
    TypeWrapper<FPType> float_() const;

    /// Creates the 64-bit float type.
    TypeWrapper<FPType> double_() const;

    /// Creates the void type.
    TypeWrapper<llvm_::Type> void_() const;

    // ====== Type-Inspection Methods.

    /// Get the size, in bytes of a type.
    uint64_t sizeof_(llvm_::Type * type) const
    {
      llvm_::DataLayout layout("");
      return layout.getTypeAllocSize(type);
    }

    /// Freestanding @p sizeof_ function for wrapper objects.
    template<typename T>
    friend uint64_t sizeof_(TypeWrapper<T> const & wrapper);

    /** 
     * @brief Creates an anonymous struct (uniqued by structural equivalence).
     *
     * @snippet types.cpp Creating an anonymous struct
     */
    TypeWrapper<llvm_::StructType>
    struct_(ArrayRef<TypeWrapper<llvm_::Type>> const & elements) const;

    /**
     * @brief Gets a struct by name.
     *
     * If the struct has not been created, a new opaque struct will be created.
     *
     * @snippet types.cpp Creating opaque structs
     */
    TypeWrapper<llvm_::StructType>
    struct_(llvm_::StringRef const & name) const;

    /**
     * @brief Defines a struct.
     *
     * The struct must not already have a body (though, it may exist and be
     * opaque).
     *
     * @snippet types.cpp Creating structs
     */
    TypeWrapper<llvm_::StructType>
    struct_(
        llvm_::StringRef const & name
      , ArrayRef<TypeWrapper<llvm_::Type>> const & elements
      ) const;
  };
}}

#include "sprite/llvm/core/type_factory_impl.hpp"

