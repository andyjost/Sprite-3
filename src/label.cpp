#include "sprite/backend/core/global.hpp"
#include "sprite/backend/core/label.hpp"
#include "sprite/backend/core/metadata.hpp"
#include "sprite/backend/core/operations.hpp"
#include "sprite/backend/core/scope.hpp"
#include "llvm/IR/InstrTypes.h"

namespace sprite { namespace backend
{
  llvm::BasicBlock * label::init(twine const & name)
  {
    function f = scope::current_function();
    llvm::LLVMContext & cxt = scope::current_context();
    return SPRITE_APICALL(BasicBlock::Create(cxt, name, f.ptr()));
  }
}}
