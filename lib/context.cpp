/**
 * @file
 * @brief Implements class context.
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
  std::list<context_frame<>> g_context_list;
}

namespace sprite { namespace llvm
{
  context::context(basic_block<type_factory> const & block)
  {
    g_context_list.emplace_front(
        llvm_::IRBuilder<>(block.ptr()), block.factory()
      );
    frame = g_context_list.begin();
  }

  context::context(context && arg) : frame(arg.frame)
    { arg.frame = g_context_list.end(); }

  context::~context()
  {
    if(frame != g_context_list.end())
      g_context_list.erase(frame);
  }

  context_frame<> const & active_context()
  {
    if(g_context_list.empty())
      throw runtime_error("No active context.");
    return g_context_list.front();
  }
}}

