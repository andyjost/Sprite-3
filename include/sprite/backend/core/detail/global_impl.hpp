#include "sprite/backend/core/detail/current_builder.hpp"
#include "sprite/backend/core/get_constant.hpp"
#include "sprite/backend/core/get_value.hpp"
#include "sprite/backend/core/scope.hpp"
#include "sprite/backend/support/casting.hpp"
#include "sprite/backend/support/exceptions.hpp"
#include <algorithm>

namespace sprite { namespace backend
{
  template<typename U, typename>
  globalvar & globalobj<GlobalVariable>::set_initializer(U const & value)
  {
    // Global variables are always pointer types.  Get the underlying
    // element type.
    type const ty((&*this)->getType()->getPointerElementType());
    (&*this)->setInitializer(get_constant_impl(ty, value).ptr());
    return *this;
  }

  inline globalvar & globalobj<GlobalVariable>::set_initializer(any_array_ref const & value)
  {
    type const ty((&*this)->getType()->getPointerElementType());
    (&*this)->setInitializer(get_constant_impl(ty, value).ptr());
    return *this;
  }

  template<typename T>
  globalvar globalobj<T>::as_globalvar() const
  {
    if(auto * g = dyn_cast<GlobalVariable>(this->ptr()))
      return globalvar(globalvaraddr(g));
    throw type_error("Expected GlobalVariable.");
  }

  template<typename T>
  template<typename U, typename>
  globalvar globalobj<T>::set_initializer(U const & value)
  {
    return this->as_globalvar().set_initializer(value);
    // auto g = dyn_cast<globalvar>(*this);
    // if(g.ptr())
    //   return g.set_initializer(value);
  }

  template<typename T>
  globalvar globalobj<T>::set_initializer(any_array_ref const & value)
  {
    return this->as_globalvar().set_initializer(value);
    //   auto g = dyn_cast<globalvar>(*this);
    //   if(g.ptr())
    //     return g.set_initializer(value);
  }

  template<typename T>
  inline constant globalobj<T>::operator&() const
  {
    // TODO support/casting.hpp probably needs to understand basic_reference.
    if(auto * a = dyn_cast<GlobalVariable>(this->ptr()))
      return globalvaraddr(a);
    if(auto * b = dyn_cast<Function>(this->ptr()))
      return &function(b);
    throw type_error("Expected GlobalVariable or Function.");
  }

  namespace aux
  {
    // Performs the call into the LLVM API.
    inline value invoke(Function * f, array_ref<Value*> const & args)
      { return value(SPRITE_APICALL(current_builder().CreateCall(f, args))); }

    /**
     * @brief Constructs function call parameters from arbitrary inputs.
     *
     * This performs any needed conversions so that the parameter types match
     * the function signature.
     */
    struct parameter_builder
    {
      parameter_builder(llvm::FunctionType & f)
        : m_begin(f.param_begin()), m_end(f.param_end())
      {}

    private:

      llvm::FunctionType::param_iterator m_begin, m_end;

    public:

      template<typename T>
      Value * operator()(T && arg)
      {
        if(m_begin == m_end)
          // Infer the LLVM type from the type of T.
          return get_value(std::forward<T>(arg)).ptr();
        else
        {
          // Get the LLVM type from the function signature.
          type const ty(*m_begin++);
          return get_value(ty, std::forward<T>(arg)).ptr();
        }
      }
    };
  }

  template<typename... Args, typename>
  value globalobj<Function>::operator()(Args &&... args) const
  {
    // tmp is an array initialized with the result of calling @p get_value
    // for each argument.
    auto fun = (*this)->getFunctionType();
    aux::parameter_builder get_param(*fun);
    Value * tmp[sizeof...(args)] {get_param(std::forward<Args>(args))...};
    return aux::invoke(this->px, tmp);
  }

  inline label globalobj<Function>::entry() const
  {
    assert(this->px);
    if(!this->px->empty())
      throw compile_error("A function body was already provided.");
    SPRITE_APICALL(llvm::BasicBlock::Create(
        scope::current_context(), ".entry", this->px
      ));
    
    assert(!this->px->empty());
    return label(&this->px->front());
  }

  namespace aux
  {
    inline constant addressof_impl(Constant * ptr)
    {
      auto const i64 = types::int_(64);
      return constant(SPRITE_APICALL(
          ConstantExpr::getInBoundsGetElementPtr(
              ptr, get_constant_impl(i64, 0).ptr()
            )
        ));
    }
  }

  inline constant globalobj<Function>::operator&() const
    { return aux::addressof_impl(this->ptr()); }

  template<typename... Args, typename>
  inline value value::operator()(Args &&... args) const
  {
    // Function.
    function f = dyn_cast<function>(*this);
    if(f.ptr())
      return f(std::forward<Args>(args)...);

    Type * ty = (*this)->getType();
    if(ty && ty->isPointerTy())
    {
      PointerType * pt = cast<PointerType>(ty);
      ty = pt->getPointerElementType();
      if(FunctionType * fun = dyn_cast<FunctionType>(ty))
      {
        aux::parameter_builder get_param(*fun);
        Value * tmp[sizeof...(args)] {get_param(std::forward<Args>(args))...};
        return value(SPRITE_APICALL(current_builder().CreateCall(ptr(), array_ref<Value*>(tmp))));
        // return aux::invoke(this->px, tmp);
      }
    }

    throw type_error("Expected function or function pointer type.");
  }
}}
