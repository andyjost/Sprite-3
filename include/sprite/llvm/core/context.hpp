/**
 * @file
 * @brief Defines class context.
 */

#pragma once
#include "sprite/llvm/core/wrappers_fwd.hpp"
#include "llvm/IR/IRBuilder.h"
#include <list>

namespace sprite { namespace llvm
{
  template<typename Factory=type_factory>
  struct context_frame
  {
    context_frame(llvm_::IRBuilder<> const & b, Factory const & f)
      : m_builder(b), m_factory(f)
    {}

    llvm_::IRBuilder<> & builder() const { return m_builder; }

    Factory const & factory() const { return m_factory; }

    llvm_::LLVMContext & context() const { return m_factory.context(); }

  private:

    // Non-const IRBuilder is always needed by LLVM API.
    mutable llvm_::IRBuilder<> m_builder;
    Factory m_factory;
  };

  /**
   * @brief Used during function creation to specify which basic block is
   * currenty under construction.
   *
   * A basic block is created by defining a new lexical block, placing an
   * instance of Scope inside it, and, finally, specifying the commands as a
   * series of simple statements.
   *
   * @snippet hello_world.cpp Using context
   */
  class context
  {
  public:
    context(basic_block<type_factory> const &);
    context(context &&);
    ~context();
    context(context const &) = delete;
    context & operator=(context const &) = delete;

  private:
    // Points to the frame in the context list associated with this context.
    std::list<context_frame<>>::iterator frame;
  };

  /**
   * @brief Returns the active context (where instructions will be added).
   *
   * Throws @p runtime_error if there is no active context.
   */
  context_frame<> const & active_context();
}}
