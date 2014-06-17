#pragma once
#include "sprite/backend/config.hpp"
#include "sprite/backend/core/type.hpp"
#include "sprite/backend/support/casting.hpp"
#include "llvm/IR/Value.h"
#include "llvm/IR/Instruction.h"

namespace sprite { namespace backend
{
  /// Specifies an attribute to attach to an instruction.
  enum attribute { tailcall };

  namespace aux
  {
    /**
     * @brief Mixes in metadata support for objects that might be convertible
     * to instructions.
     */
    template<typename Derived>
    struct metadata_support
    {
      Derived & set_metadata(string_ref kind, metadata const & arg)
      {
        Derived * this_ = static_cast<Derived *>(this);
        assert(this_->ptr());
        dyn_cast<instruction &>(*this_).set_metadata(kind, arg);
        return *this_;
      }
      Derived & set_metadata(string_ref kind)
      {
        Derived * this_ = static_cast<Derived *>(this);
        assert(this_->ptr());
        dyn_cast<instruction &>(*this_).set_metadata(kind);
        return *this_;
      }
      // Defined in metadata.hpp.
      metadata get_metadata(string_ref kind) const;
      bool has_metadata(string_ref kind) const
        { return this->get_metadata(kind).ptr(); }
    };
  }

  template<>
  struct valueobj<llvm::Value>
    : object<llvm::Value>, aux::metadata_support<valueobj<llvm::Value>>
  {
    using basic_type = Value;
    using object<llvm::Value>::object;

    valueobj() : object<llvm::Value>::object(nullptr) {}

    // Define in-place operators.
    #define SPRITE_LHS_TYPE value
    #include "sprite/backend/core/detail/declare_class_operators.def"

    /// See function::operator().
    template<
        typename... Args
      , SPRITE_ENABLE_FOR_ALL_VALUE_INITIALIZERS(Args...)
      >
    value operator()(Args &&... args) const;

    /// Performs pointer dereference followed by member access into a struct.
    ref arrow(uint32_t i) const;

    /// Sets an attribute.
    value const & set_attribute(attribute) const;
  };

  template<>
  struct valueobj<llvm::Instruction> : object<llvm::Instruction>
  {
    using basic_type = Instruction;
    using object<llvm::Instruction>::object;

    // Defined in metadata.hpp.
    instruction & set_metadata(string_ref kind);
    instruction & set_metadata(string_ref kind, metadata const &);
    metadata get_metadata(string_ref kind) const;
    bool has_metadata(string_ref kind) const;

  private:

    static_assert(
        std::is_base_of<basic_type, llvm::Instruction>::value
      , "Expected an LLVM Instruction object"
      );
  };

  template<typename T>
  struct valueobj : object<T>, aux::metadata_support<valueobj<T>>
  {
    using basic_type = Value;
    using object<T>::object;

  private:

    static_assert(
        std::is_base_of<basic_type, T>::value, "Expected an LLVM Value object"
      );
  };

  /// Get the type of a value.
  template<typename T>
  type typeof_(valueobj<T> const & arg)
    { return type(SPRITE_APICALL(arg.ptr()->getType())); }
}}
