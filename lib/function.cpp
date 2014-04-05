#include "sprite/backend/core/function.hpp"
#include "sprite/backend/core/scope.hpp"
#include "sprite/backend/core/detail/current_builder.hpp"

namespace sprite { namespace backend { namespace aux
{
  value invoke(Function * f, array_ref<Value*> const & args)
    { return value(SPRITE_APICALL(current_builder().CreateCall(f, args))); }
}}}

