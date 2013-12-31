/**
 * @file
 * @brief Implementation details for sprite/backend/factory.hpp.
 */

#include "sprite/backend/core/wrappers.hpp"
#include <vector>

namespace sprite { namespace backend
{
  inline module::module(llvm::Module * module, unsigned addrSpace)
    : base_type(module, Scaffolding()), _addrSpace(addrSpace)
  {
    if(!this->px)
      this->px = new llvm::Module("module", llvm::getGlobalContext());
  }

  inline module::module(string_ref const & name, unsigned addrSpace)
    : base_type(
          new llvm::Module(name, llvm::getGlobalContext()), Scaffolding()
        )
    , _addrSpace(addrSpace)
  {
    this->px = new llvm::Module(name, llvm::getGlobalContext());
  }

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

}}

