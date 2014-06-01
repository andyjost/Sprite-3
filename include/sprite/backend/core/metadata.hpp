#pragma once
#include "sprite/backend/core/scope.hpp"
#include "sprite/backend/core/value.hpp"
#include "llvm/IR/Metadata.h"

namespace sprite { namespace backend
{
  namespace aux
  {
    template<typename T>
    typename std::enable_if<
        !std::is_convertible<T, string_ref>::value, Value *
      >::type
    get_md_value(T && arg)
      { return get_value(std::forward<T>(arg)).ptr(); }

    template<typename T>
    typename std::enable_if<
        std::is_convertible<T, string_ref>::value, Value *
      >::type
    get_md_value(T && arg)
      { return llvm::MDString::get(scope::current_context(), arg); }
  }

  // API: metadata
  template<>
  struct valueobj<llvm::MDNode> : object<llvm::MDNode>
  {
    using basic_type = MDNode;
    using object<llvm::MDNode>::object;

    template<typename...Args>
    valueobj<llvm::MDNode>(Args&&...args) : object<llvm::MDNode>(nullptr)
    {
      Value * elts[sizeof...(args)]
          {aux::get_md_value(std::forward<Args>(args))...};
      this->px = llvm::MDNode::get(
          scope::current_context()
        , array_ref<Value*>(elts) // works if elts has zero elements
        );
    }

    size_t size() const
      { return (*this)->getNumOperands(); }

    value operator[](unsigned i)
      { return value((*this)->getOperand(i)); }

    // FIXME: should be able to use ref instead.
    template<typename T
      , typename = typename std::enable_if<is_value_initializer<T>::value>::type
      >
    void set(unsigned i, T const & arg)
      { return (*this)->replaceOperandWith(i, get_value(arg).ptr()); }

  private:

    static_assert(
        std::is_base_of<basic_type, llvm::MDNode>::value
      , "Expected an LLVM MDNode object"
      );
  };

  inline instruction & valueobj<llvm::Instruction>::set_metadata(string_ref kind)
    { return this->set_metadata(kind, metadata()); }

  inline instruction & valueobj<llvm::Instruction>::set_metadata(
      string_ref kind, metadata const & arg
    )
  {
    (*this)->setMetadata(kind, arg.ptr());
    return *this;
  }

  inline metadata
  valueobj<llvm::Instruction>::get_metadata(string_ref kind) const
    { return metadata((*this)->getMetadata(kind)); }

  inline bool
  valueobj<llvm::Instruction>::has_metadata(string_ref kind) const
      { return this->get_metadata(kind).ptr(); }

  namespace aux
  {
    template<typename Derived>
    metadata metadata_support<Derived>::get_metadata(string_ref kind) const
    {
      Derived const * this_ = static_cast<Derived const *>(this);
      assert(this_->ptr());
      return dyn_cast<instruction const &>(*this_).get_metadata(kind);
    }
  }
}}
