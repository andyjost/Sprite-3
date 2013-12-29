/**
 * @file
 * @brief Defines methods for making top-level definitions (functions and
 * global variables).
 */

#pragma once
#include "llvm/IR/GlobalVariable.h"
#include "sprite/backend/core/wrappers.hpp"
#include "sprite/backend/support/casting.hpp"

namespace sprite { namespace backend
{
  /**
   * @brief Declares a function with user-specified linkage.
   */
  inline globalobj<Function> def(
      GlobalValue::LinkageTypes linkage
    , function_type const & type
    , llvm::Twine const & name = ""
    )
  {
    return wrap(
        type.factory()
      , Function::Create(
            type.ptr(), linkage, name, type.factory().module()
          )
      );
  }

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
  inline global def(
      GlobalValue::LinkageTypes linkage
    , type const & type
    , llvm::Twine const & name = ""
    )
  {
    // Create a function for function types.
    if(auto const ftype = dyn_cast<FunctionType>(type))
      return def(linkage, ftype, name);

    // Create a global variable for other types.
    return wrap(
        type.factory()
      , new llvm::GlobalVariable(
            /* Module          */ *type.factory().module()
          , /* Type            */ type.ptr()
          , /* isConstant      */ false
          , /* Linkage         */ linkage
          , /* Initializer     */ 0
          , /* Name            */ name
          , /* InsertBefore    */ 0
          , /* ThreadLocalMode */ llvm::GlobalVariable::NotThreadLocal
          , /* AddressSpace    */ type.factory().addrSpace()
          )
      );
  }

  /**
   * @brief Declares a global variable or function with external linkage.
   *
   * @snippet defs.cpp Using extern_
   */
  inline global extern_(
      type const & type
    , llvm::Twine const & name = ""
    )
  { return def(GlobalValue::ExternalLinkage, type, name); }

  /**
   * @brief Version of extern_ that takes a return type specifier.
   */
  template<typename ReturnType>
  inline globalobj<ReturnType> extern_(
      type const & type
    , llvm::Twine const & name = ""
    )
  { return dyn_cast<ReturnType>(extern_(type, name)); }

  /**
   * @brief Declares a global variable or function with internal linkage.
   *
   * @snippet defs.cpp Using static_
   */
  inline global static_(
      type const & type
    , llvm::Twine const & name = ""
    )
  { return def(GlobalValue::InternalLinkage, type, name); }

  /**
   * @brief Version of static_ that takes a return type specifier.
   */
  template<typename ReturnType>
  inline globalobj<ReturnType> static_(
      type const & type
    , llvm::Twine const & name = ""
    )
  { return dyn_cast<ReturnType>(static_(type, name)); }

  /**
   * @brief Declares an inline function.
   *
   * This function only accepts pointer types (inline arrays are not
   * meaningful).
   *
   * @snippet defs.cpp Using inline_
   */
  inline global inline_(
      type const & type
    , llvm::Twine const & name = ""
    )
  {
    // Only functions may be inline.
    if(!llvm::dyn_cast<FunctionType>(type.ptr()))
      throw type_error();
    return def(GlobalValue::LinkOnceAnyLinkage, type, name);
  }

  /**
   * @brief Version of inline_ that takes a return type specifier.
   */
  template<typename ReturnType>
  inline globalobj<ReturnType> inline_(
      type const & type
    , llvm::Twine const & name = ""
    )
  { return dyn_cast<ReturnType>(inline_(type, name)); }
}}
