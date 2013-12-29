/**
 * @file
 * @brief Implements class context.
 */

#include "sprite/backend/core/context.hpp"
#include "sprite/backend/core/type_factory.hpp"
#include "sprite/backend/core/wrappers.hpp"
#include "sprite/backend/support/exceptions.hpp"

namespace
{
  using namespace sprite::backend;

  /**
   * @brief Holds the global contexts.
   *
   * The active context is at the front of the list.  This should behave as a
   * stack, but a user could concievably deallocate them out of order, so
   * deletions in the middle of the list are possible.
   */
  std::list<context_frame<>> g_context_list;
}

namespace sprite { namespace backend
{
  context::context(basic_block<type_factory> const & block)
  {
    g_context_list.emplace_front(
        llvm::IRBuilder<>(block.ptr()), block.factory()
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

