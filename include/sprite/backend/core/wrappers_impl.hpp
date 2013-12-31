/**
 * @file
 * @brief Implementation details for sprite/backend/wrappers.hpp.
 */

#include <limits>
#include "sprite/backend/operators/preamble.hpp"
#include "sprite/backend/support/exceptions.hpp"
#include "sprite/backend/support/casting.hpp"
#include <string>
#include <utility>

namespace sprite { namespace backend { namespace aux
{
  /**
   * @brief Loads function arguments into an array; handles an @p ellipsis as the final
   * argument.
   */
  template<typename TargetType>
  struct function_arg_loader
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
    TargetType ** operator()(TargetType** p, ellipsis const &) const
      { return p; }
  };
}}}

namespace sprite { namespace backend
{
  // ====== Implementation details for typeobj.

  template<typename T, typename Factory>
  typeobj<PointerType, Factory>
  typeobj<T, Factory>::operator*() const
    { return wrap(this->factory(), (*this)->getPointerTo()); }
    
  template<typename T, typename Factory>
  typeobj<ArrayType, Factory>
  typeobj<T, Factory>::operator[](uint64_t size) const
    { return wrap(this->factory(), ArrayType::get(this->ptr(), size)); }

  template<typename T, typename Factory>
  template<typename... Args>
  typeobj<FunctionType, Factory>
  typeobj<T, Factory>::operator()(Args &&... argtypes) const
  {
    Type * tmp[sizeof...(argtypes)];
    Type ** end = aux::function_arg_loader<Type>()(&tmp[0], argtypes...);
    array_ref<Type*> args(&tmp[0], end);
    bool const varargs = (args.size() == sizeof...(argtypes) - 1);
    assert(varargs || args.size() == sizeof...(argtypes));
    return wrap(this->factory(), FunctionType::get(this->ptr(), args, varargs));
  }

  template<typename Factory>
  template<typename... Args>
  instruction<llvm::CallInst, Factory>
  globalobj<Function, Factory>::operator()(Args &&... args) const
  {
    Value * tmp[sizeof...(args)];
    Value ** end = aux::function_arg_loader<Value>()(&tmp[0], args...);
    array_ref<Value*> args_(&tmp[0], end);
    if(args_.size() != sizeof...(args))
    {
      throw parameter_error(
          "An ellipsis cannot be supplied to a function call."
        );
    }
    context_frame<Factory> const & cxt = active_context();
    return wrap(cxt.factory(), cxt.builder().CreateCall(this->px, args_));
  }
}}
