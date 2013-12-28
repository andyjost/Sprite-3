/**
 * @file
 * @brief Implementation details for sprite/llvm/wrappers.hpp.
 */

#include <limits>
#include "sprite/llvm/operators/preamble.hpp"
#include "sprite/llvm/support/exceptions.hpp"
#include "sprite/llvm/support/casting.hpp"
#include <string>
#include <utility>

namespace sprite { namespace llvm { namespace aux
{
  /**
   * @brief Loads function arguments into an array; handles an @p Ellipsis as the final
   * argument.
   */
  template<typename TargetType>
  struct FunctionArgLoader
  {
    /// Processes a non-terminal argument.
    template<typename First, typename... Remaining>
    TargetType ** operator()(
        TargetType** p, First && arg, Remaining &&... rem
      ) const
    {
      *p++ = arg.ptr();
      return (*this)(p, rem...);
    }

    /// Terminates with no varargs.
    TargetType ** operator()(TargetType** p) const
      { return p; }

    /// Terminates with varargs.
    TargetType ** operator()(TargetType** p, Ellipsis const &) const
      { return p; }
  };
}}}

namespace sprite { namespace llvm
{
  // ====== Implementation details for TypeWrapper.

  template<typename T, typename Factory>
  TypeWrapper<PointerType, Factory>
  TypeWrapper<T, Factory>::operator*() const
  {
    return wrap(
        this->factory(), (*this)->getPointerTo(this->factory().addrSpace())
      );
  }
    
  template<typename T, typename Factory>
  TypeWrapper<ArrayType, Factory>
  TypeWrapper<T, Factory>::operator[](uint64_t size) const
    { return wrap(this->factory(), ArrayType::get(this->ptr(), size)); }

  template<typename T, typename Factory>
  template<typename... Args>
  TypeWrapper<llvm_::FunctionType, Factory>
  TypeWrapper<T, Factory>::operator()(Args &&... argtypes) const
  {
    Type * tmp[sizeof...(argtypes)];
    Type ** end = aux::FunctionArgLoader<Type>()(&tmp[0], argtypes...);
    ArrayRef<Type*> args(&tmp[0], end);
    bool const varargs = (args.size() == sizeof...(argtypes) - 1);
    assert(varargs || args.size() == sizeof...(argtypes));
    return wrap(this->factory(), FunctionType::get(this->ptr(), args, varargs));
  }

  template<typename Factory>
  template<typename... Args>
  InstructionWrapper<llvm_::CallInst, Factory>
  GlobalValueWrapper<llvm_::Function, Factory>::operator()(Args &&... args) const
  {
    Value * tmp[sizeof...(args)];
    Value ** end = aux::FunctionArgLoader<Value>()(&tmp[0], args...);
    ArrayRef<Value*> args_(&tmp[0], end);
    if(args_.size() != sizeof...(args))
    {
      throw ParameterError(
          "An ellipsis cannot be supplied to a function call."
        );
    }
    ContextFrame<Factory> const & cxt = activeContext();
    return wrap(cxt.factory(), cxt.builder().CreateCall(this->px, args_));
  }
}}
