#include "sprite/backend/core/value.hpp"
#include "llvm/IR/Instructions.h"

namespace sprite { namespace backend
{
  value const & value::set_attribute(attribute attr) const
  {
    switch(attr)
    {
      case tailcall:
        dyn_cast<llvm::CallInst>(ptr())->setTailCall();
        break;
    }
    return *this;
  }
}}
