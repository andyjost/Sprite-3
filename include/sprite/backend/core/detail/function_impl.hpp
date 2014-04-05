#include "sprite/backend/core/get_value.hpp"
#include "sprite/backend/support/exceptions.hpp"
#include "sprite/backend/core/scope.hpp"
#include <algorithm>

namespace sprite { namespace backend
{
  namespace aux
  {
    // Performs the call into the LLVM API.
    value invoke(Function *, array_ref<Value*> const &);

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
          // Get the LLVM type from the function signature.
          return get_value(*m_begin++, std::forward<T>(arg)).ptr();
      }
    };
  }

  template<typename... Args, typename>
  value globalobj<Function>::operator()(Args &&... args) const
  {
    // The is an array initialized with the result of calling @p get_value
    // for each argument.
    auto fun = (*this)->getFunctionType();
    aux::parameter_builder get_param(*fun);
    Value * tmp[sizeof...(args)] {get_param(std::forward<Args>(args))...};
    return aux::invoke(this->px, tmp);
  }

  inline label globalobj<Function>::entry() const
  {
    assert(this->px);
    if(this->px->empty())
    {
      SPRITE_APICALL(llvm::BasicBlock::Create(
          scope::current_context(), ".entry", this->px
        ));
    }
    assert(!this->px->empty());
    return label(&this->px->front());
  }
}}
