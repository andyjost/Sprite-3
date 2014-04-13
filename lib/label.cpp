#include "sprite/backend/core/function.hpp"
#include "sprite/backend/core/label.hpp"
#include "sprite/backend/core/operations.hpp"
#include "sprite/backend/core/scope.hpp"

// DEBUG
#include "llvm/Support/CFG.h"

namespace sprite { namespace backend
{
  llvm::BasicBlock * label::init(twine const & name)
  {
    function f = scope::current_function();
    llvm::LLVMContext & cxt = scope::current_context();
    return SPRITE_APICALL(BasicBlock::Create(cxt, name, f.ptr()));
  }

  void label::term()
  {
    if(this->px)
    {
      // Append the terminator, if requied.
      if(this->m_next && !this->px->getTerminator())
      {
        llvm::IRBuilder<> bldr(this->px);
        SPRITE_APICALL(bldr.CreateBr(this->m_next));
      }

      // Delete this block if it is useless.
      if(this->px->empty())
      {
        using namespace llvm;
        for(pred_iterator PI = pred_begin(this->px), E = pred_end(this->px); PI != E; ++PI)
        {
          // BasicBlock * pred = *PI;
          assert(0 && "found predecessor");
        }
        this->px->eraseFromParent();
      }
    }
  }
}}
