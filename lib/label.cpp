#include "sprite/backend/core/function.hpp"
#include "sprite/backend/core/label.hpp"
#include "sprite/backend/core/scope.hpp"

namespace sprite { namespace backend
{
  llvm::BasicBlock * valueobj<llvm::BasicBlock>::init(twine const & name)
  {
    function f = scope::current_function();
    llvm::LLVMContext & cxt = scope::current_context();
    return SPRITE_APICALL(BasicBlock::Create(cxt, name, f.ptr()));
  }
}}
