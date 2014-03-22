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
    , type const & type
    , twine const & name = ""
    );

  function def(
      GlobalValue::LinkageTypes linkage
    , function_type const &
    , twine const & name = ""
    );

  template<typename T>
  inline globalobj<T> def(
      GlobalValue::LinkageTypes linkage
    , type const & tp
    , twine const & name = ""
    )
  { return dyn_cast<T>(def(linkage, tp, name)); }
  //@}

  //@{
  /**
   * @brief Declares a global variable or function with external linkage.
   *
   * @snippet defs.cpp Using extern_
   */
  inline global extern_(type const & tp, twine const & name = "")
    { return def(GlobalValue::ExternalLinkage, tp, name); }

  inline function extern_(function_type const & tp, twine const & name = "")
    { return def(GlobalValue::ExternalLinkage, tp, name); }

  template<typename T>
  inline globalobj<T> extern_(type const & tp, twine const & name = "")
    { return dyn_cast<T>(extern_(tp, name)); }
  //@}

  //@{
  /**
   * @brief Declares a global variable or function with internal linkage.
   *
   * @snippet defs.cpp Using static_
   */
  inline global static_(type const & tp, twine const & name = "")
    { return def(GlobalValue::InternalLinkage, tp, name); }

  inline function static_(function_type const & tp, twine const & name = "")
    { return def(GlobalValue::InternalLinkage, tp, name); }

  template<typename T>
  inline globalobj<T> static_(type const & tp, twine const & name = "")
    { return dyn_cast<T>(static_(tp, name)); }
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
  inline function inline_(function_type const & tp, twine const & name = "")
    { return def(GlobalValue::LinkOnceAnyLinkage, tp, name); }

  inline function inline_(type const & tp, twine const & name = "")
  {
    // Only functions may be inline.
    if(function_type fun_type = dyn_cast<FunctionType>(tp))
      return inline_(fun_type, name);
    throw type_error("Function type required for inline definition.");
  }

  /**
   * @brief Version of inline_ that takes a return type specifier.
   */
  template<typename T>
  inline globalobj<T> inline_(type const & tp, twine const & name = "")
    { return dyn_cast<T>(inline_(tp, name)); }
  //@}
}}

