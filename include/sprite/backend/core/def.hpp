#pragma once
#include "sprite/backend/core/function.hpp"
#include "sprite/backend/core/global.hpp"
#include "sprite/backend/core/type.hpp"
#include "sprite/backend/support/casting.hpp"

namespace sprite { namespace backend
{
  //@{
  /**
   * @brief Declares a global variable or function with user-specified linkage.
   *
   * For the most common linkage specifiers, corresponding to the @p extern, @p
   * static and @p inline keywords in C++, use the @p extern_, @p static_ and
   * @p inline_ functions, defined below.
   *
   * The returned wrapper object supports operator=, which can be used to set
   * an initializer (see the examples).
   *
   * @param[in] linkage
   *   The linkage specification.
   * @param[in] type
   *   The entity type.  If this parameter holds an instance of llvm::Function
   *   then a function is declared.  Otherwise, a global varaible is declared.
   * @param[in] name
   *   The entity name.
   *
   * @snippet defs.cpp Global definitions with linkage
   */
  global def(
      GlobalValue::LinkageTypes linkage
    , type const & ty
    , twine const & name = ""
    , array_ref<twine> const & = {}
    );

  function def(
      GlobalValue::LinkageTypes linkage
    , function_type const &
    , twine const & name = ""
    , array_ref<twine> const & = {}
    );

  template<typename T>
  inline auto def(
      GlobalValue::LinkageTypes linkage
    , type const & ty
    , twine const & name = ""
    , array_ref<twine> const & arg_names = {}
    )
    -> decltype(dyn_cast<T>(def(linkage, ty, name, arg_names)))
  { return dyn_cast<T>(def(linkage, ty, name, arg_names)); }
  //@}

  //@{
  /**
   * @brief Declares a global variable or function with external linkage.
   *
   * @snippet defs.cpp Using extern_
   */
  inline global extern_(
      type const & ty, twine const & name = ""
    , array_ref<twine> const & arg_names = {}
    )
  { return def(GlobalValue::ExternalLinkage, ty, name, arg_names); }

  inline function extern_(
      function_type const & ty, twine const & name = ""
    , array_ref<twine> const & arg_names = {}
    )
  { return def(GlobalValue::ExternalLinkage, ty, name, arg_names); }

  template<typename T>
  inline auto extern_(
      type const & ty, twine const & name = ""
    , array_ref<twine> const & arg_names = {}
    )
    -> decltype(dyn_cast<T>(extern_(ty, name, arg_names)))
  { return dyn_cast<T>(extern_(ty, name, arg_names)); }
  //@}

  //@{
  /**
   * @brief Declares a global variable or function with internal linkage.
   *
   * @snippet defs.cpp Using static_
   */
  inline global static_(
      type const & ty, twine const & name = ""
    , array_ref<twine> const & arg_names = {}
    )
  { return def(GlobalValue::InternalLinkage, ty, name, arg_names); }

  inline function static_(
      function_type const & ty, twine const & name = ""
    , array_ref<twine> const & arg_names = {}
    )
  { return def(GlobalValue::InternalLinkage, ty, name, arg_names); }

  template<typename T>
  inline auto static_(
      type const & ty, twine const & name = ""
    , array_ref<twine> const & arg_names = {}
    )
    -> decltype(dyn_cast<T>(static_(ty, name, arg_names)))
  { return dyn_cast<T>(static_(ty, name, arg_names)); }
  //@}

  //@{
  /**
   * @brief Declares an inline function.
   *
   * This function only accepts pointer types (inline arrays are not
   * meaningful).
   *
   * @snippet defs.cpp Using inline_
   */
  inline function inline_(
      function_type const & ty, twine const & name = ""
    , array_ref<twine> const & arg_names = {}
    )
  { return def(GlobalValue::LinkOnceAnyLinkage, ty, name, arg_names); }

  inline function inline_(
      type const & ty, twine const & name = ""
    , array_ref<twine> const & arg_names = {}
    )
  {
    // Only functions may be inline.
    if(function_type fun_type = dyn_cast<FunctionType>(ty))
      return inline_(fun_type, name, arg_names);
    throw type_error("Function type required for inline definition.");
  }

  /**
   * @brief Version of inline_ that takes a return type specifier.
   */
  template<typename T>
  inline auto inline_(
      type const & ty, twine const & name = ""
    , array_ref<twine> const & arg_names = {}
    )
    -> decltype(dyn_cast<T>(inline_(ty, name, arg_names)))
  { return dyn_cast<T>(inline_(ty, name, arg_names)); }
  //@}
}}

