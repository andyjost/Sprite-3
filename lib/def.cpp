#include "sprite/backend/core/def.hpp"
#include "sprite/backend/core/module.hpp"
#include "sprite/backend/core/operations.hpp"
#include "sprite/backend/core/control_flow.hpp"
#include "sprite/backend/core/scope.hpp"

namespace
{
  using namespace sprite::backend;

  // Used to find an existing, matching global definition, which is relevant
  // only for rigid names.
  template<typename ResultType, typename Subtype>
  inline ResultType find_existing(
      globalname const & name, type const & matching_type
    )
  {
    if(!name.is_flexible)
    {
      if(auto gv = scope::current_module().ptr()->getNamedValue(name))
      {
        if(gv->getType() != (*matching_type).ptr())
          throw type_error("Type mismatch");
        return ResultType(llvm::dyn_cast<Subtype>(gv));
      }
    }
    return ResultType(nullptr);
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

    // For rigid names, first try to look up an existing global variable.
    auto existing = find_existing<global, llvm::GlobalVariable>(name, ty);
    if(existing.ptr()) return existing;

    // Create a new global variable.
    auto rv = global(SPRITE_APICALL(
        new GlobalVariable(
            /* Module      */ *scope::current_module().ptr()
          , /* Type        */ ty.ptr()
          , /* isConstant  */ false
          , /* Linkage     */ linkage
          , /* Initializer */ nullptr
          , /* Name        */ name
          )
      ));
    assert(name.is_flexible || rv->getName() == name);
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
    // For rigid names, first try to look up an existing function.
    function fun = find_existing<function, llvm::Function>(name, ty);
    bool const is_existing = static_cast<bool>(fun.ptr());

    // Create a new function, if needed.
    if(!fun.ptr())
    {
      fun = function(SPRITE_APICALL(
          Function::Create(ty.ptr(), linkage, name, scope::current_module().ptr())
        ));
      assert(name.is_flexible || fun->getName() == name);
    }

    if(arg_names.size() > fun->arg_size())
      throw value_error("Too many arguments names.");

    // Set the argument names.  If an existing definition was found, then the
    // argument names must match any name previously given.
    llvm::Function::arg_iterator arg = fun->arg_begin();
    for(auto const & arg_name : arg_names)
    {
      if(is_existing && arg->hasName() && arg->getName() != arg_name.str())
        throw name_error("Conflicting parameter name.");
      SPRITE_APICALL((arg++)->setName(arg_name));
    }

    // Evaluate the function body, if one was supplied.
    if(body)
    {
      // The function must currently have no body.
      if(!fun->empty())
        throw compile_error("Multiply-defined function.");

      scope _ = fun;
      body();

      // If the function returns void, then the terminator can be implied.
      if(fun.return_type()->isVoidTy() && !scope::current_label()->getTerminator())
        return_();
    }

    return fun;
  }
}}
