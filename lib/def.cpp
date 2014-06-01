#include "sprite/backend/core/def.hpp"
#include "sprite/backend/core/module.hpp"
#include "sprite/backend/core/operations.hpp"
#include "sprite/backend/core/scope.hpp"

namespace sprite { namespace backend
{
  global def(
      GlobalValue::LinkageTypes linkage
    , type const & ty
    , twine const & name
    , array_ref<twine> const & arg_names
    , codeblock const & body
    )
  {
    // Create a function for function types.
    auto const fun_type = dyn_cast<FunctionType>(ty);
    if(fun_type.ptr())
      return def(linkage, fun_type, name, arg_names);

    if(!arg_names.empty())
    {
      throw value_error(
          "No arg names may be supplied for a global variable definition."
        );
    }

    if(body)
    {
      throw value_error(
          "No function body may be supplied for a global variable definition."
        );
    }

    // Create a global variable for other types.
    module const mod = scope::current_module();
    return global(SPRITE_APICALL(
        new GlobalVariable(
            /* Module      */ *mod.ptr()
          , /* Type        */ ty.ptr()
          , /* isConstant  */ false
          , /* Linkage     */ linkage
          , /* Initializer */ nullptr
          , /* Name        */ name
          )
      ));
  }

  function def(
      GlobalValue::LinkageTypes linkage
    , function_type const & ty
    , twine const & name
    , array_ref<twine> const & arg_names
    , codeblock const & body
    )
  {
    module const mod = scope::current_module();
    function fun(SPRITE_APICALL(
        Function::Create(ty.ptr(), linkage, name, mod.ptr())
      ));

    if(arg_names.size() > fun->arg_size())
      throw value_error("Too many arguments names.");

    llvm::Function::arg_iterator arg = fun->arg_begin();
    for(auto const & name : arg_names)
      SPRITE_APICALL((arg++)->setName(name));

    // Evaluate the function body, if one was supplied.
    if(body)
    {
      scope _ = fun;
      body();

      // If the function returns void, then the terminator can be implied.
      if(fun.return_type()->isVoidTy() && !scope::current_label()->getTerminator())
        return_();
    }

    return fun;
  }
}}
