#include "sprite/backend/core/def.hpp"
#include "sprite/backend/core/module.hpp"
#include "sprite/backend/core/operations.hpp"
#include "sprite/backend/core/control_flow.hpp"
#include "sprite/backend/core/scope.hpp"

namespace
{
  using namespace sprite::backend;

  // Checks that the desired name matches the given object name, conditional on
  // the @p is_flexible property.
  template<typename T>
  inline void check_for_conflicts(globalname const & name, T const & obj)
  {
    if(!name.is_flexible && obj->getName() != name)
    {
      obj->eraseFromParent();
      throw name_error("name conflict");
    }
  }
}

namespace sprite { namespace backend
{
  global def(
      GlobalValue::LinkageTypes linkage
    , type const & ty
    , globalname const & name
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
    auto rv = global(SPRITE_APICALL(
        new GlobalVariable(
            /* Module      */ *mod.ptr()
          , /* Type        */ ty.ptr()
          , /* isConstant  */ false
          , /* Linkage     */ linkage
          , /* Initializer */ nullptr
          , /* Name        */ name
          )
      ));
    check_for_conflicts(name, rv);
    return rv;
  }

  function def(
      GlobalValue::LinkageTypes linkage
    , function_type const & ty
    , globalname const & name
    , array_ref<twine> const & arg_names
    , codeblock const & body
    )
  {
    module const mod = scope::current_module();
    function fun(SPRITE_APICALL(
        Function::Create(ty.ptr(), linkage, name, mod.ptr())
      ));
    check_for_conflicts(name, fun);

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
