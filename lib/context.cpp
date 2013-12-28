/**
 * @file
 * @brief Implements class Context.
 */

#include "sprite/llvm/core/context.hpp"
#include "sprite/llvm/core/type_factory.hpp"
#include "sprite/llvm/core/wrappers.hpp"
#include "sprite/llvm/support/exceptions.hpp"

namespace
{
  using namespace sprite::llvm;

  /**
   * @brief Holds the global contexts.
   *
   * The active context is at the front of the list.  This should behave as a
   * stack, but a user could concievably deallocate them out of order, so
   * deletions in the middle of the list are possible.
   */
  std::list<ContextFrame<>> g_context_list;
}

namespace sprite { namespace llvm
{
  Context::Context(BasicBlockWrapper<TypeFactory> const & block)
  {
    g_context_list.emplace_front(
        llvm_::IRBuilder<>(block.ptr()), block.factory()
      );
    frame = g_context_list.begin();
  }

  Context::Context(Context && arg) : frame(arg.frame)
    { arg.frame = g_context_list.end(); }

  Context::~Context()
  {
    if(frame != g_context_list.end())
      g_context_list.erase(frame);
  }

  ContextFrame<> const & activeContext()
  {
    if(g_context_list.empty())
      throw RuntimeError("No active context.");
    return g_context_list.front();
  }
}}

