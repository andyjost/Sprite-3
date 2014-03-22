/**
 * @file
 * @brief Implements class scope.
 */

#include "sprite/backend/core/function.hpp"
#include "sprite/backend/core/label.hpp"
#include "sprite/backend/core/module.hpp"
#include "sprite/backend/core/scope.hpp"
#include "sprite/backend/support/exceptions.hpp"
#include <utility>
#include "llvm/IR/IRBuilder.h"

namespace sprite { namespace backend
{
  struct scope::frame { virtual ~frame() {} };
}}

namespace
{
  using namespace sprite::backend;
  using builder_type = llvm::IRBuilder<>;

  module g_current_module(nullptr);
  function g_current_function(nullptr);
  label g_current_label(nullptr);
  builder_type * g_current_builder = nullptr;

  struct label_frame : scope::frame
  {
    // When a new function is pushed onto the function stack, its entry label
    // is pushed onto the label stack.  In that case, @p check=false is set
    // when called from @p function_frame, otherwise, this constructor would
    // check against the previous function.
    label_frame(label && l = label(nullptr), bool check = true)
      : m_prev_label(l), m_prev_builder(nullptr)
    {
      if(check) check_function();

      if(m_prev_label)
      {
        new(&m_builder_loc) builder_type(m_prev_label.ptr());
        m_prev_builder = reinterpret_cast<builder_type *>(&m_builder_loc);
      }

      std::swap(m_prev_label, g_current_label);
      std::swap(m_prev_builder, g_current_builder);
    }

    ~label_frame() override
    {
      std::swap(m_prev_builder, g_current_builder);
      std::swap(m_prev_label, g_current_label);
      
      if(m_prev_label)
        reinterpret_cast<builder_type *>(&m_builder_loc)->~builder_type();
    }

    void check_function()
    {
      if(!g_current_function)
      {
        throw scope_error(
            "No current function while setting label scope."
          );
      }

      llvm::Function * new_func =
          m_prev_label ? m_prev_label->getParent() : nullptr;
      if(g_current_function.ptr() != new_func)
      {
        throw scope_error(
            "New label scope does not belong to the current function."
          );
      }
    }

  private:

    label m_prev_label;
    builder_type * m_prev_builder;
    std::aligned_storage<sizeof(builder_type)>::type m_builder_loc;
  };

  struct function_frame : label_frame
  {
    function_frame(function && f = function(nullptr), bool check=true)
      : label_frame(f ? f.entry() : label(nullptr), false), m_prev_function(f)
    {
      if(check) check_module();
      std::swap(m_prev_function, g_current_function);
    }

    ~function_frame() override
      { std::swap(m_prev_function, g_current_function); }

    void check_module()
    {
      if(!g_current_module)
      {
        throw scope_error(
            "No current module while setting function scope."
          );
      }

      llvm::Module * new_module =
          m_prev_function ? m_prev_function->getParent() : nullptr;
      if(g_current_module.ptr() != new_module)
      {
        throw scope_error(
            "New function scope does not belong to the current module."
          );
      }
    }

  private:

    function m_prev_function;
  };

  struct module_frame : function_frame
  {
    module_frame(module && m = module(nullptr))
      : function_frame(function(nullptr), false), m_prev_module(m)
      { std::swap(m_prev_module, g_current_module); }
    ~module_frame() override
    {
      assert(!g_current_function);
      std::swap(m_prev_module, g_current_module);
    }
  private:
    module m_prev_module;
  };
}

namespace sprite { namespace backend
{
  scope::scope(scope && arg)
    : m_frame(std::move(arg.m_frame))
  {}

  scope::~scope() {}

  scope::scope(module m)
    : m_frame(new module_frame(std::move(m)))
  {}

  scope::scope(function f)
    : m_frame(new function_frame(std::move(f)))
  {}

  scope::scope(label l)
    : m_frame(new label_frame(std::move(l)))
  {}

  module scope::current_module() { return g_current_module; }

  function scope::current_function() { return g_current_function; }

  label scope::current_label() { return g_current_label; }

  llvm::LLVMContext & scope::current_context()
    { return g_current_module.context(); }

  llvm::IRBuilder<> & current_builder()
  {
    if(g_current_builder)
      return *g_current_builder;
    throw scope_error("No current context for building instructions.");
  }
}}

