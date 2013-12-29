/**
 * @file
 * @brief Implementation details for sprite/backend/factory.hpp.
 */

#include "sprite/backend/core/wrappers.hpp"
#include <vector>

namespace sprite { namespace backend
{
  inline type_factory::type_factory(llvm::Module * module, unsigned addrSpace)
    : _module(module), _addrSpace(addrSpace)
  {
    if(!_module)
      _module = new llvm::Module("module", llvm::getGlobalContext());
  }

  inline integer_type
  type_factory::int_(unsigned numBits) const
  {
    return wrap(
        *this, llvm::IntegerType::get(_module->getContext(), numBits)
      );
  }

  inline fp_type type_factory::float_() const
  {
    auto const p = llvm::Type::getFloatTy(_module->getContext());
    return wrap(*this, reinterpret_cast<FPType*>(p));
  }

  inline fp_type type_factory::double_() const
  {
    auto const p = llvm::Type::getDoubleTy(_module->getContext());
    return wrap(*this, reinterpret_cast<FPType*>(p));
  }

  inline type type_factory::void_() const
    { return wrap(*this, llvm::Type::getVoidTy(_module->getContext())); }

  inline struct_type
  type_factory::struct_(ArrayRef<type> const & elements) const
  {
    std::vector<llvm::Type*> tmp;
    for(auto e: elements) { tmp.emplace_back(e.ptr()); }
    return wrap(*this, llvm::StructType::get(_module->getContext(),tmp));
  }

  inline struct_type
  type_factory::struct_(llvm::StringRef const & name) const
  {
    llvm::StructType * type = _module->getTypeByName(name);
    if(!type)
      type = llvm::StructType::create(_module->getContext(), name);
    return wrap(*this, type);
  }

  inline struct_type
  type_factory::struct_(
      llvm::StringRef const & name
    , ArrayRef<type> const & elements 
    ) const
  {
    auto const type = this->struct_(name);
    std::vector<llvm::Type*> tmp;
    for(auto e: elements) { tmp.emplace_back(e.ptr()); }
    type->setBody(tmp, /*isPacked*/ false);
    return type;
  }

  template<typename T>
  uint64_t sizeof_(typeobj<T> const & wrapper)
    { return wrapper.factory().sizeof_(wrapper.ptr()); }

}}

