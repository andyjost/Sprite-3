#include "sprite/llvm/core/label.hpp"
#include "sprite/llvm/core/context.hpp"

using namespace sprite::llvm;

namespace
{
  basic_block<type_factory> new_block(llvm_::Twine const & name)
  {
    auto const & cxt = active_context();
    Function * parent = cxt.builder().GetInsertBlock()->getParent();
    return wrap(
        cxt.factory()
      , llvm_::BasicBlock::Create(cxt.context(), name, parent)
      );
  }
}

namespace sprite { namespace llvm
{
  label::label(llvm_::Twine const & name)
    : m_block(new_block(name))
  {
  }
}}
