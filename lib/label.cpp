#include "sprite/backend/core/label.hpp"
#include "sprite/backend/core/context.hpp"

using namespace sprite::backend;

namespace
{
  basic_block<type_factory> new_block(llvm::Twine const & name)
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
  label::label(llvm::Twine const & name)
    : m_block(new_block(name))
  {
  }
}}
