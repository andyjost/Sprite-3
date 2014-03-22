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

    // ====== Type-Creation Methods.

    #if 0
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
    struct_type struct_(array_ref<type> const & elements) const;

    /**
     * @brief Gets a struct by name.
     *
     * If the struct has not been created, a new opaque struct will be created.
     *
     * @snippet types.cpp Creating opaque structs
     */
    struct_type struct_(string_ref const & name) const;

    /**
     * @brief Defines a struct.
     *
     * The struct must not already have a body (though, it may exist and be
     * opaque).
     *
     * @snippet types.cpp Creating structs
     */
    struct_type struct_(
        string_ref const & name, array_ref<type> const & elements
      ) const;
    #endif
  };

  #if 0
  inline integer_type
  module::int_(unsigned numBits) const
  {
    return wrap(
        *this, IntegerType::get(ptr()->getContext(), numBits)
      );
  }

  inline fp_type module::float_() const
  {
    auto const p = Type::getFloatTy(ptr()->getContext());
    return wrap(*this, reinterpret_cast<FPType*>(p));
  }

  inline fp_type module::double_() const
  {
    auto const p = Type::getDoubleTy(ptr()->getContext());
    return wrap(*this, reinterpret_cast<FPType*>(p));
  }

  inline type module::void_() const
    { return wrap(*this, Type::getVoidTy(ptr()->getContext())); }

  inline struct_type
  module::struct_(array_ref<type> const & elements) const
  {
    std::vector<Type*> tmp;
    for(auto e: elements) { tmp.emplace_back(e.ptr()); }
    return wrap(*this, StructType::get(ptr()->getContext(),tmp));
  }

  inline struct_type
  module::struct_(string_ref const & name) const
  {
    StructType * type = ptr()->getTypeByName(name);
    if(!type)
      type = StructType::create(ptr()->getContext(), name);
    return wrap(*this, type);
  }

  inline struct_type
  module::struct_(
      string_ref const & name
    , array_ref<type> const & elements 
    ) const
  {
    auto const type = this->struct_(name);
    std::vector<Type*> tmp;
    for(auto e: elements) { tmp.emplace_back(e.ptr()); }
    type->setBody(tmp, /*isPacked*/ false);
    return type;
  }

  template<typename T>
  uint64_t sizeof_(typeobj<T> const & wrapper)
    { return wrapper.factory().sizeof_(wrapper.ptr()); }
  #endif
}}

