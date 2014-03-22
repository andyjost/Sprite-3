#include "sprite/backend/core/def.hpp"
#include "sprite/backend/core/module.hpp"
#include "sprite/backend/core/scope.hpp"

namespace sprite { namespace backend
{
  global def(
      GlobalValue::LinkageTypes linkage
    , type const & tp
    , twine const & name
    )
  {
    // Create a function for function types.
    if(auto const fun_type = dyn_cast<FunctionType>(tp))
      return def(linkage, fun_type, name);

    // Create a global variable for other types.
    module const mod = scope::current_module();
    return global(SPRITE_APICALL(
        new GlobalVariable(
            /* Module      */ *mod.ptr()
          , /* Type        */ tp.ptr()
          , /* isConstant  */ false
          , /* Linkage     */ linkage
          , /* Initializer */ nullptr
          , /* Name        */ name
          )
      ));
  }

  function def(
      GlobalValue::LinkageTypes linkage
    , function_type const & fun
    , twine const & name
    )
  {
    module const mod = scope::current_module();
    return function(SPRITE_APICALL(
        Function::Create(fun.ptr(), linkage, name, mod.ptr())
      ));
  }
}}
