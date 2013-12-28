/**
 * @file
 * @brief Implementation details for sprite/llvm/factory.hpp.
 */

#include "sprite/llvm/core/wrappers.hpp"
#include <vector>

namespace sprite { namespace llvm
{
  inline TypeFactory::TypeFactory(llvm_::Module * module, unsigned addrSpace)
    : _module(module), _addrSpace(addrSpace)
  {
    if(!_module)
      _module = new llvm_::Module("module", llvm_::getGlobalContext());
  }

  inline TypeWrapper<llvm_::IntegerType>
  TypeFactory::int_(unsigned numBits) const
  {
    return wrap(
        *this, llvm_::IntegerType::get(_module->getContext(), numBits)
      );
  }

  inline TypeWrapper<FPType> TypeFactory::float_() const
  {
    auto const p = llvm_::Type::getFloatTy(_module->getContext());
    return wrap(*this, reinterpret_cast<FPType*>(p));
  }

  inline TypeWrapper<FPType> TypeFactory::double_() const
  {
    auto const p = llvm_::Type::getDoubleTy(_module->getContext());
    return wrap(*this, reinterpret_cast<FPType*>(p));
  }

  inline TypeWrapper<llvm_::Type> TypeFactory::void_() const
    { return wrap(*this, llvm_::Type::getVoidTy(_module->getContext())); }

  inline TypeWrapper<llvm_::StructType>
  TypeFactory::struct_(ArrayRef<TypeWrapper<llvm_::Type>> const & elements) const
  {
    std::vector<llvm_::Type*> tmp;
    for(auto e: elements) { tmp.emplace_back(e.ptr()); }
    return wrap(*this, llvm_::StructType::get(_module->getContext(),tmp));
  }

  inline TypeWrapper<llvm_::StructType>
  TypeFactory::struct_(llvm_::StringRef const & name) const
  {
    llvm_::StructType * type = _module->getTypeByName(name);
    if(!type)
      type = llvm_::StructType::create(_module->getContext(), name);
    return wrap(*this, type);
  }

  inline TypeWrapper<llvm_::StructType>
  TypeFactory::struct_(
      llvm_::StringRef const & name
    , ArrayRef<TypeWrapper<llvm_::Type>> const & elements 
    ) const
  {
    auto const type = this->struct_(name);
    std::vector<llvm_::Type*> tmp;
    for(auto e: elements) { tmp.emplace_back(e.ptr()); }
    type->setBody(tmp, /*isPacked*/ false);
    return type;
  }

  template<typename T>
  uint64_t sizeof_(TypeWrapper<T> const & wrapper)
    { return wrapper.factory().sizeof_(wrapper.ptr()); }

}}

