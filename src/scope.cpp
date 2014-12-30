/**
 * @file
 * @brief Implements class scope.
 */

#include "sprite/backend/core/global.hpp"
#include "sprite/backend/core/label.hpp"
#include "sprite/backend/core/metadata.hpp"
#include "sprite/backend/core/module.hpp"
#include "sprite/backend/core/scope.hpp"
#include "sprite/backend/support/exceptions.hpp"
#include <exception>
#include <unordered_map>
#include <utility>
#include <vector>
#include "llvm/IR/IRBuilder.h"
#include "llvm/Support/CFG.h"

// DEBUG
#include "sprite/backend/core/valueof.hpp"

namespace sprite { namespace backend
{
  struct scope::frame { virtual ~frame() {} };
}}

namespace
{
  using namespace sprite::backend;
  using builder_type = llvm::IRBuilder<>;

  // std::swap is in scope, but more specific versions will still match first.
  using std::swap;

  /**
   * @brief Used to bring a function into scope.
   *
   * There are hidden indexes to improve the speed of argument lookup.
   */
  struct function_data : function
  {
    using function::function;
    std::vector<Value *> args_by_position;
    std::unordered_map<std::string, Value *> args_by_name;

    friend void swap(function_data & lhs, function_data & rhs)
    {
      swap(static_cast<function&>(lhs), static_cast<function&>(rhs));
      swap(lhs.args_by_position, rhs.args_by_position);
      swap(lhs.args_by_name, rhs.args_by_name);
    }
  };

  module g_current_module(nullptr);
  function_data g_current_function(nullptr);
  label g_current_label(nullptr);
  builder_type * g_current_builder = nullptr;

  // Finalizes the label state when done with it.
  void finalize(label const & l)
  {
    // The LLVM clean-up functions do not always behave well for malformed
    // programs (e.g., hangs or SEGVs are possible).  It's better to just leave
    // the broken program alone.
    if(std::uncaught_exception())
      return;
    auto const ptr = l.ptr();
    if(ptr)
    {
      // If the block has multiple terminators, and the last one was implicitly
      // added by Sprite, then remove it.
      auto const it = ptr->rbegin();
      auto const end = ptr->rend();
      if(it != end && instruction(&*it).has_metadata(SPRITE_IMPLIED_METADATA))
      {
        auto prev = it; ++prev;
        if(prev != end && prev->isTerminator())
          it->eraseFromParent();
      }

      // Delete this block if it is empty.
      if(ptr->empty())
      {
        // There should be no predecessors.
        assert(llvm::pred_begin(ptr) == llvm::pred_end(ptr));
        if(ptr->getParent())
          ptr->eraseFromParent();
      }
    }
  }

  /**
   * @brief Returns the insertion position for a label.
   *
   * If the block has a terminator tagged with sprite.implied metadata, then
   * the insertion position is just before that.  Otherwise, it is the end of
   * the block.
   */
  llvm::BasicBlock::iterator get_insert_pos(label const & l)
  {
    if(llvm::Instruction * term = l->getTerminator())
    {
      if(instruction(term).has_metadata(SPRITE_IMPLIED_METADATA))
        return --(l->end());
    }
    return l->end();
  }

  struct label_frame : scope::frame
  {
    // When a new function is pushed onto the function stack, its entry label
    // is pushed onto the label stack.  In that case, @p check=false is set
    // when called from @p function_frame, otherwise, this constructor would
    // check against the previous function.
    label_frame(label && l = label(nullptr), bool check = true)
      : m_prev_label(std::move(l)), m_prev_builder(nullptr)
    {
      if(check) check_function(m_prev_label);

      if(m_prev_label.ptr())
      {
        new(&m_builder_loc) builder_type(
            m_prev_label.ptr(), get_insert_pos(m_prev_label)
          );
        m_prev_builder = reinterpret_cast<builder_type *>(&m_builder_loc);
      }

      swap(m_prev_label, g_current_label);
      swap(m_prev_builder, g_current_builder);
    }

    ~label_frame() override
    {
      swap(m_prev_builder, g_current_builder);
      swap(m_prev_label, g_current_label);
      finalize(m_prev_label);
      if(m_prev_builder) m_prev_builder->~builder_type();
    }

    static void check_function(label const & new_label)
    {
      if(!g_current_function.ptr())
      {
        throw scope_error(
            "No current function while setting label scope."
          );
      }

      llvm::Function * new_func =
          new_label.ptr() ? new_label->getParent() : nullptr;
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
      : label_frame(f.ptr() ? f.entry() : label(nullptr), false), m_prev_function(f)
    {
      if(check) check_module(m_prev_function);
      swap(m_prev_function, g_current_function);
    }

    ~function_frame() override
      { swap(m_prev_function, g_current_function); }

    static void check_module(function const & new_function)
    {
      if(!g_current_module.ptr())
      {
        throw scope_error(
            "No current module while setting function scope."
          );
      }

      llvm::Module * new_module =
          new_function.ptr() ? new_function->getParent() : nullptr;
      if(g_current_module.ptr() != new_module)
      {
        throw scope_error(
            "New function scope does not belong to the current module."
          );
      }
    }

  private:

    function_data m_prev_function;
  };

  struct module_frame : function_frame
  {
    module_frame(module && m = module(nullptr))
      : function_frame(function(nullptr), false), m_prev_module(m)
      { swap(m_prev_module, g_current_module); }
    ~module_frame() override
    {
      assert(!g_current_function.ptr());
      swap(m_prev_module, g_current_module);
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

  void scope::update_current_label_after_branch(label const & cont)
  {
    label_frame::check_function(cont);

    // The old basic block should have a terminator.  If it is implicit,
    // then move it to the new block.
    llvm::TerminatorInst * term = g_current_label->getTerminator();
    if(!term)
      throw compile_error("Expected a terminated basic block.");
    if(instruction(term).has_metadata(SPRITE_IMPLIED_METADATA))
    {
      term->removeFromParent();
      cont->getInstList().push_back(term);
      assert(g_current_label->getTerminator());
      assert(cont->getTerminator());
    }

    // Replace the current builder and basic block.
    if(g_current_builder)
    {
      g_current_builder->~builder_type();
      new(g_current_builder) builder_type(cont.ptr(), get_insert_pos(cont));
    }
    g_current_label = cont; // nothrow
  }

  void scope::set_continuation(
      label const & src, label const & tgt, MdBranchType tag
    )
  {
    if(src.ptr() && !src->getTerminator())
    {
      assert(scope::current_label().ptr() != src.ptr());
      llvm::IRBuilder<> bldr(src.ptr());
      Instruction * term = SPRITE_APICALL(bldr.CreateBr(tgt.ptr()));
      instruction(term).set_metadata(
          SPRITE_IMPLIED_METADATA, static_cast<int>(tag)
        );
    }
  }

  llvm::LLVMContext & scope::current_context()
    { return g_current_module.context(); }

  llvm::IRBuilder<> & current_builder()
  {
    if(g_current_builder)
      return *g_current_builder;
    throw scope_error("No current context for building instructions.");
  }

  // Declared in function.hpp, but needs to access the data in this file.
  value arg(size_t i)
  {
    // The first time a positional lookup occurs, this index is filled.
    auto & lookup = g_current_function.args_by_position;
    if(lookup.empty())
    {
      size_t const n = SPRITE_APICALL(
          g_current_function->getFunctionType()->getNumParams()
        );
      lookup.reserve(n);
      auto & args = SPRITE_APICALL(g_current_function->getArgumentList());
      for(auto & arg : args)
        lookup.push_back(&arg);
    }
    if(i >= lookup.size())
      throw value_error("Argument index out of range.");
    return value(lookup[i]);
  }

  // Declared in function.hpp, but needs to access the data in this file.
  value arg(string_ref const & name)
  {
    // This index remembers arguments previously looked up by name in the
    // current scope.  If the initial lookup fails, then the full argument list
    // is searched.  During that search any arguments passed over are also
    // placed into the index.
    auto & lookup = g_current_function.args_by_name;
    auto it = lookup.find(name);
    if(it == lookup.end())
    {
      auto & args = SPRITE_APICALL(g_current_function->getArgumentList());
      for(auto & arg : args)
      {
        string_ref argname = arg.getName();
        lookup[argname] = &arg;
        if(argname == name)
          return value(&arg);
      }
      throw value_error("Argument \"" + name.str() + "\" not found.");
    }
    return value(it->second);
  }
}}

