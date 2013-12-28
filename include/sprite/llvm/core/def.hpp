/**
 * @file
 * @brief Defines methods for making top-level definitions (functions and
 * global variables).
 */

#pragma once
#include "llvm/IR/GlobalVariable.h"
#include "sprite/llvm/core/wrappers.hpp"
#include "sprite/llvm/support/casting.hpp"

namespace sprite { namespace llvm
{
  /**
   * @brief Declares a function with user-specified linkage.
   */
  inline GlobalValueWrapper<llvm_::Function> def(
      llvm_::GlobalValue::LinkageTypes linkage
    , TypeWrapper<llvm_::FunctionType> const & type
    , llvm_::Twine const & name = ""
    )
  {
    return wrap(
        type.factory()
      , llvm_::Function::Create(
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
  inline GlobalValueWrapper<llvm_::GlobalValue> def(
      llvm_::GlobalValue::LinkageTypes linkage
    , TypeWrapper<llvm_::Type> const & type
    , llvm_::Twine const & name = ""
    )
  {
    // Create a function for function types.
    if(auto const ftype = dyn_cast<llvm_::FunctionType>(type))
      return def(linkage, ftype, name);

    // Create a global variable for other types.
    return wrap(
        type.factory()
      , new llvm_::GlobalVariable(
            /* Module          */ *type.factory().module()
          , /* Type            */ type.ptr()
          , /* isConstant      */ false
          , /* Linkage         */ linkage
          , /* Initializer     */ 0
          , /* Name            */ name
          , /* InsertBefore    */ 0
          , /* ThreadLocalMode */ llvm_::GlobalVariable::NotThreadLocal
          , /* AddressSpace    */ type.factory().addrSpace()
          )
      );
  }

  /**
   * @brief Declares a global variable or function with external linkage.
   *
   * @snippet defs.cpp Using extern_
   */
  inline GlobalValueWrapper<llvm_::GlobalValue> extern_(
      TypeWrapper<llvm_::Type> const & type
    , llvm_::Twine const & name = ""
    )
  { return def(llvm_::GlobalValue::ExternalLinkage, type, name); }

  /**
   * @brief Version of extern_ that takes a return type specifier.
   */
  template<typename ReturnType>
  inline GlobalValueWrapper<ReturnType> extern_(
      TypeWrapper<llvm_::Type> const & type
    , llvm_::Twine const & name = ""
    )
  { return dyn_cast<ReturnType>(extern_(type, name)); }

  /**
   * @brief Declares a global variable or function with internal linkage.
   *
   * @snippet defs.cpp Using static_
   */
  inline GlobalValueWrapper<llvm_::GlobalValue> static_(
      TypeWrapper<llvm_::Type> const & type
    , llvm_::Twine const & name = ""
    )
  { return def(llvm_::GlobalValue::InternalLinkage, type, name); }

  /**
   * @brief Version of static_ that takes a return type specifier.
   */
  template<typename ReturnType>
  inline GlobalValueWrapper<ReturnType> static_(
      TypeWrapper<llvm_::Type> const & type
    , llvm_::Twine const & name = ""
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
  inline GlobalValueWrapper<llvm_::GlobalValue> inline_(
      TypeWrapper<llvm_::Type> const & type
    , llvm_::Twine const & name = ""
    )
  {
    // Only functions may be inline.
    if(!llvm_::dyn_cast<llvm_::FunctionType>(type.ptr()))
      throw TypeError();
    return def(llvm_::GlobalValue::LinkOnceAnyLinkage, type, name);
  }

  /**
   * @brief Version of inline_ that takes a return type specifier.
   */
  template<typename ReturnType>
  inline GlobalValueWrapper<ReturnType> inline_(
      TypeWrapper<llvm_::Type> const & type
    , llvm_::Twine const & name = ""
    )
  { return dyn_cast<ReturnType>(inline_(type, name)); }
}}
