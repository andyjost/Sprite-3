/**
 * @file
 * @brief Defines class Context.
 */

#pragma once
#include "sprite/llvm/core/wrappers_fwd.hpp"
#include "llvm/IR/IRBuilder.h"
#include <list>

namespace sprite { namespace llvm
{
  template<typename Factory=TypeFactory>
  struct ContextFrame
  {
    ContextFrame(llvm_::IRBuilder<> const & b, Factory const & f)
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
   * @snippet hello_world.cpp Using Context
   */
  class Context
  {
  public:
    Context(BasicBlockWrapper<TypeFactory> const &);
    Context(Context &&);
    ~Context();
    Context(Context const &) = delete;
    Context & operator=(Context const &) = delete;

  private:
    // Points to the frame in the context list associated with this context.
    std::list<ContextFrame<>>::iterator frame;
  };

  /**
   * @brief Returns the active context (where instructions will be added).
   *
   * Throws @p RuntimeError if there is no active context.
   */
  ContextFrame<> const & activeContext();
}}
