#pragma once
#include "sprite/backend/config.hpp"
#include "sprite/backend/core/object.hpp"
#include "sprite/backend/support/casting.hpp"
#include "llvm/IR/Value.h"
#include "llvm/IR/Instruction.h"

namespace sprite { namespace backend
{
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
      Derived const & get_metadata(string_ref kind) const
      {
        Derived const * this_ = static_cast<Derived const *>(this);
        assert(this_->ptr());
        return dyn_cast<instruction &>(*this_).get_metadata(kind);
      }
      // FIXME: this looks totally wrong.
      Derived & get_metadata(string_ref kind)
      {
        metadata_support const * this_ =
            static_cast<metadata_support const *>(this);
        return const_cast<Derived &>(this_->get_metadata(kind));
      }
      bool has_metadata(string_ref kind) const
        { return this->get_metadata.ptr(); }
    };
  }

  template<>
  struct valueobj<llvm::Value>
    : object<llvm::Value>, aux::metadata_support<valueobj<llvm::Value>>
  {
    using basic_type = Value;
    using object<llvm::Value>::object;

    // Define in-place operators.
    #define SPRITE_LHS_TYPE value
    #include "sprite/backend/core/detail/declare_class_operators.def"
  };

  template<>
  struct valueobj<llvm::Instruction> : object<llvm::Instruction>
  {
    using basic_type = Instruction;
    using object<llvm::Instruction>::object;

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
}}
