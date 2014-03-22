#include "sprite/backend/core/get_value.hpp"
#include "sprite/backend/support/exceptions.hpp"

namespace sprite { namespace backend
{
  namespace aux
  {
    // Performs the call into the LLVM API.
    instrobj<llvm::CallInst> invoke(Function *, array_ref<Value*> const &);
  }

  template<typename... Args, typename>
  instrobj<llvm::CallInst>
  globalobj<Function>::operator()(Args &&... args) const
  {
    // The is an array initialized with the result of calling @p get_value
    // for each argument.
    Value * tmp[sizeof...(args)] {get_value(std::forward<Args>(args))...};
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
