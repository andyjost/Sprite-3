#include "sprite/backend/core/label.hpp"
#include "sprite/backend/core/context.hpp"

using namespace sprite::backend;

namespace
{
  basic_block<module> new_block(twine const & name)
  {
    auto const & cxt = active_context();
    Function * parent = cxt.builder().GetInsertBlock()->getParent();
    return wrap(
        cxt.factory()
      , llvm::BasicBlock::Create(cxt.context(), name, parent)
      );
  }
}

namespace sprite { namespace backend
{
  label::label(twine const & name)
    : m_block(new_block(name))
  {
  }
}}
