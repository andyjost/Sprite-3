#pragma once
#include "sprite/backend/core/function.hpp"
#include "sprite/backend/core/global.hpp"
#include "sprite/backend/core/label.hpp"
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
   * @param[in] ty
   *   The entity type.  If this parameter holds an instance of llvm::Function
   *   then a function is declared.  Otherwise, a global varaible is declared.
   * @param[in] name
   *   The entity name.
   * @param[in] arg_names
   *   For function definitions only, an optional list of argument names.  The
   *   Nth element is the name of the Nth function argument.  The list length
   *   may be less than the function arity, in which case remaining arguments
   *   are unnamed.
   * @param[in] body
   *   For function definitions only, optionally specifies the body of the
   *   function.
   *
   * @snippet defs.cpp Global definitions with linkage
   */
  global def(
      GlobalValue::LinkageTypes linkage
    , type const & ty
    , twine const & name = ""
    , array_ref<twine> const & = {}
    , codeblock const & body = codeblock()
    );

  inline global def(
      GlobalValue::LinkageTypes linkage
    , type const & ty
    , twine const & name
    , codeblock const & body
    )
  { return def(linkage, ty, name, {}, body); }

  inline global def(
      GlobalValue::LinkageTypes linkage
    , type const & ty
    , codeblock const & body
    )
  { return def(linkage, ty, "", {}, body); }

  inline global def(
      GlobalValue::LinkageTypes linkage
    , type const & ty
    , array_ref<twine> const & arg_names
    , codeblock const & body = codeblock()
    )
  { return def(linkage, ty, "", arg_names, body); }

  function def(
      GlobalValue::LinkageTypes linkage
    , function_type const &
    , twine const & name = ""
    , array_ref<twine> const & = {}
    , codeblock const & body = codeblock()
    );

  inline function def(
      GlobalValue::LinkageTypes linkage
    , function_type const & ty
    , twine const & name
    , codeblock const & body
    )
  { return def(linkage, ty, name, {}, body); }

  inline function def(
      GlobalValue::LinkageTypes linkage
    , function_type const & ty
    , codeblock const & body
    )
  { return def(linkage, ty, "", {}, body); }

  inline function def(
      GlobalValue::LinkageTypes linkage
    , function_type const & ty
    , array_ref<twine> const & arg_names
    , codeblock const & body = codeblock()
    )
  { return def(linkage, ty, "", arg_names, body); }

  template<typename T>
  inline auto def(
      GlobalValue::LinkageTypes linkage
    , type const & ty
    , twine const & name = ""
    , array_ref<twine> const & arg_names = {}
    , codeblock const & body = codeblock()
    )
    -> decltype(dyn_cast<T>(def(linkage, ty, name, arg_names, body)))
  { return dyn_cast<T>(def(linkage, ty, name, arg_names, body)); }

  template<typename T>
  inline auto def(
      GlobalValue::LinkageTypes linkage
    , type const & ty
    , twine const & name
    , codeblock const & body
    )
    -> decltype(dyn_cast<T>(def(linkage, ty, name, {}, body)))
  { return dyn_cast<T>(def(linkage, ty, name, {}, body)); }

  template<typename T>
  inline auto def(
      GlobalValue::LinkageTypes linkage
    , type const & ty
    , codeblock const & body
    )
    -> decltype(dyn_cast<T>(def(linkage, ty, "", {}, body)))
  { return dyn_cast<T>(def(linkage, ty, "", {}, body)); }

  template<typename T>
  inline auto def(
      GlobalValue::LinkageTypes linkage
    , type const & ty
    , array_ref<twine> const & arg_names
    , codeblock const & body = codeblock()
    )
    -> decltype(dyn_cast<T>(def(linkage, ty, "", arg_names, body)))
  { return dyn_cast<T>(def(linkage, ty, "", arg_names, body)); }
  //@}


  //@{
  /**
   * @brief Declares a global variable or function with external linkage.
   *
   * @snippet defs.cpp Using extern_
   */
  inline global extern_(
      type const & ty
    , twine const & name = ""
    , array_ref<twine> const & arg_names = {}
    , codeblock const & body = codeblock()
    )
  { return def(GlobalValue::ExternalLinkage, ty, name, arg_names, body); }

  inline global extern_(
      type const & ty
    , twine const & name
    , codeblock const & body
    )
  { return def(GlobalValue::ExternalLinkage, ty, name, {}, body); }

  inline global extern_(
      type const & ty
    , codeblock const & body
    )
  { return def(GlobalValue::ExternalLinkage, ty, "", {}, body); }

  inline global extern_(
      type const & ty
    , array_ref<twine> const & arg_names
    , codeblock const & body = codeblock()
    )
  { return def(GlobalValue::ExternalLinkage, ty, "", arg_names, body); }

  inline function extern_(
      function_type const & ty
    , twine const & name = ""
    , array_ref<twine> const & arg_names = {}
    , codeblock const & body = codeblock()
    )
  { return def(GlobalValue::ExternalLinkage, ty, name, arg_names, body); }

  inline function extern_(
      function_type const & ty
    , twine const & name
    , codeblock const & body
    )
  { return def(GlobalValue::ExternalLinkage, ty, name, {}, body); }

  inline function extern_(
      function_type const & ty
    , codeblock const & body
    )
  { return def(GlobalValue::ExternalLinkage, ty, "", {}, body); }

  inline function extern_(
      function_type const & ty
    , array_ref<twine> const & arg_names
    , codeblock const & body = codeblock()
    )
  { return def(GlobalValue::ExternalLinkage, ty, "", arg_names, body); }

  template<typename T>
  inline auto extern_(
      type const & ty
    , twine const & name = ""
    , array_ref<twine> const & arg_names = {}
    , codeblock const & body = codeblock()
    )
    -> decltype(dyn_cast<T>(extern_(ty, name, arg_names, body)))
  { return dyn_cast<T>(extern_(ty, name, arg_names, body)); }

  template<typename T>
  inline auto extern_(
      type const & ty
    , twine const & name
    , codeblock const & body
    )
    -> decltype(dyn_cast<T>(extern_(ty, name, {}, body)))
  { return dyn_cast<T>(extern_(ty, name, {}, body)); }

  template<typename T>
  inline auto extern_(
      type const & ty
    , codeblock const & body
    )
    -> decltype(dyn_cast<T>(extern_(ty, "", {}, body)))
  { return dyn_cast<T>(extern_(ty, "", {}, body)); }

  template<typename T>
  inline auto extern_(
      type const & ty
    , array_ref<twine> const & arg_names
    , codeblock const & body = codeblock()
    )
    -> decltype(dyn_cast<T>(extern_(ty, "", arg_names, body)))
  { return dyn_cast<T>(extern_(ty, "", arg_names, body)); }
  //@}

  //@{
  /**
   * @brief Declares a global variable or function with internal linkage.
   *
   * @snippet defs.cpp Using static_
   */
  inline global static_(
      type const & ty
    , twine const & name = ""
    , array_ref<twine> const & arg_names = {}
    , codeblock const & body = codeblock()
    )
  { return def(GlobalValue::InternalLinkage, ty, name, arg_names, body); }

  inline global static_(
      type const & ty
    , twine const & name
    , codeblock const & body
    )
  { return def(GlobalValue::InternalLinkage, ty, name, {}, body); }

  inline global static_(
      type const & ty
    , codeblock const & body
    )
  { return def(GlobalValue::InternalLinkage, ty, "", {}, body); }

  inline global static_(
      type const & ty
    , array_ref<twine> const & arg_names
    , codeblock const & body = codeblock()
    )
  { return def(GlobalValue::InternalLinkage, ty, "", arg_names, body); }

  inline function static_(
      function_type const & ty
    , twine const & name = ""
    , array_ref<twine> const & arg_names = {}
    , codeblock const & body = codeblock()
    )
  { return def(GlobalValue::InternalLinkage, ty, name, arg_names, body); }

  inline function static_(
      function_type const & ty
    , twine const & name
    , codeblock const & body
    )
  { return def(GlobalValue::InternalLinkage, ty, name, {}, body); }

  inline function static_(
      function_type const & ty
    , codeblock const & body
    )
  { return def(GlobalValue::InternalLinkage, ty, "", {}, body); }

  inline function static_(
      function_type const & ty
    , array_ref<twine> const & arg_names
    , codeblock const & body = codeblock()
    )
  { return def(GlobalValue::InternalLinkage, ty, "", arg_names, body); }

  template<typename T>
  inline auto static_(
      type const & ty
    , twine const & name = ""
    , array_ref<twine> const & arg_names = {}
    , codeblock const & body = codeblock()
    )
    -> decltype(dyn_cast<T>(static_(ty, name, arg_names, body)))
  { return dyn_cast<T>(static_(ty, name, arg_names, body)); }

  template<typename T>
  inline auto static_(
      type const & ty
    , twine const & name
    , codeblock const & body
    )
    -> decltype(dyn_cast<T>(static_(ty, name, {}, body)))
  { return dyn_cast<T>(static_(ty, name, {}, body)); }

  template<typename T>
  inline auto static_(
      type const & ty
    , codeblock const & body
    )
    -> decltype(dyn_cast<T>(static_(ty, "", {}, body)))
  { return dyn_cast<T>(static_(ty, "", {}, body)); }

  template<typename T>
  inline auto static_(
      type const & ty
    , array_ref<twine> const & arg_names
    , codeblock const & body = codeblock()
    )
    -> decltype(dyn_cast<T>(static_(ty, "", arg_names, body)))
  { return dyn_cast<T>(static_(ty, "", arg_names, body)); }
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
      function_type const & ty
    , twine const & name = ""
    , array_ref<twine> const & arg_names = {}
    , codeblock const & body = codeblock()
    )
  { return def(GlobalValue::LinkOnceAnyLinkage, ty, name, arg_names, body); }

  inline function inline_(
      function_type const & ty
    , twine const & name
    , codeblock const & body
    )
  { return def(GlobalValue::LinkOnceAnyLinkage, ty, name, {}, body); }

  inline function inline_(
      function_type const & ty
    , codeblock const & body
    )
  { return def(GlobalValue::LinkOnceAnyLinkage, ty, "", {}, body); }

  inline function inline_(
      function_type const & ty
    , array_ref<twine> const & arg_names
    , codeblock const & body = codeblock()
    )
  { return def(GlobalValue::LinkOnceAnyLinkage, ty, "", arg_names, body); }

  inline function inline_(
      type const & ty
    , twine const & name = ""
    , array_ref<twine> const & arg_names = {}
    , codeblock const & body = codeblock()
    )
  {
    // Only functions may be inline.
    if(function_type fun_type = dyn_cast<FunctionType>(ty))
      return inline_(fun_type, name, arg_names, body);
    throw type_error("Function type required for inline definition.");
  }

  inline function inline_(
      type const & ty
    , twine const & name
    , codeblock const & body
    )
  { return inline_(ty, name, {}, body); }

  inline function inline_(
      type const & ty
    , codeblock const & body
    )
  { return inline_(ty, "", {}, body); }

  inline function inline_(
      type const & ty
    , array_ref<twine> const & arg_names
    , codeblock const & body = codeblock()
    )
  { return inline_(ty, "", arg_names, body); }

  /**
   * @brief Version of inline_ that takes a return type specifier.
   */
  template<typename T>
  inline auto inline_(
      type const & ty
    , twine const & name = ""
    , array_ref<twine> const & arg_names = {}
    , codeblock const & body = codeblock()
    )
    -> decltype(dyn_cast<T>(inline_(ty, name, arg_names, body)))
  { return dyn_cast<T>(inline_(ty, name, arg_names, body)); }

  template<typename T>
  inline auto inline_(
      type const & ty
    , twine const & name
    , codeblock const & body
    )
    -> decltype(dyn_cast<T>(inline_(ty, name, {}, body)))
  { return dyn_cast<T>(inline_(ty, name, {}, body)); }

  template<typename T>
  inline auto inline_(
      type const & ty
    , codeblock const & body
    )
    -> decltype(dyn_cast<T>(inline_(ty, "", {}, body)))
  { return dyn_cast<T>(inline_(ty, "", {}, body)); }

  template<typename T>
  inline auto inline_(
      type const & ty
    , array_ref<twine> const & arg_names
    , codeblock const & body = codeblock()
    )
    -> decltype(dyn_cast<T>(inline_(ty, "", arg_names, body)))
  { return dyn_cast<T>(inline_(ty, "", arg_names, body)); }
  //@}
}}

