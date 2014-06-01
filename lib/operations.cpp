#include "sprite/backend/core/operations.hpp"
#include "sprite/backend/core/metadata.hpp"
#include "sprite/backend/core/valueof.hpp"

namespace sprite { namespace backend
{
  constant offsetof_(type const & ty, unsigned FieldNo)
  {
    if(auto const p = dyn_cast<StructType>(ty))
    {
      return constant(
          SPRITE_APICALL(ConstantExpr::getOffsetOf(p.ptr(), FieldNo))
        );
    }
    throw type_error("Expected StructType for offsetof_.");
  }

  instruction return_()
  {
    if(!scope::current_function().return_type()->isVoidTy())
    {
      throw type_error(
          "A return value must be supplied for functions not returning void."
        );
    }
    return instruction(SPRITE_APICALL(current_builder().CreateRetVoid()));
  }

  instruction if_(
      branch_condition const & cond
    , labeldescr const & true_, labeldescr const & false_
    )
  {
    llvm::IRBuilder<> & bldr = current_builder();
    auto rv = SPRITE_APICALL(
        bldr.CreateCondBr(cond.get().ptr(), true_.ptr(), false_.ptr())
      );
    label next;
    scope::update_current_label_after_branch(next);
    scope::set_continuation(true_, next);
    scope::set_continuation(false_, next);
    // Perform codegen only after the continuations are set.
    true_.codegen();
    false_.codegen();
    return instruction(rv);
  }

  instruction if_(branch_condition const & cond, labeldescr const & true_)
  {
    label next;
    llvm::IRBuilder<> & bldr = current_builder();
    auto rv = SPRITE_APICALL(
        bldr.CreateCondBr(cond.get().ptr(), true_.ptr(), next.ptr())
      );
    scope::update_current_label_after_branch(next);
    scope::set_continuation(true_, next);
    // Perform codegen only after the continuations are set.
    true_.codegen();
    return instruction(rv);
  }

  instruction while_(loop_condition const & cond, labeldescr const & body)
  {
    label test, next;
    instruction const rv = goto_(test);
    {
      scope _ = test;
      llvm::IRBuilder<> & bldr = current_builder();
      SPRITE_APICALL(
          bldr.CreateCondBr(cond.get().ptr(), body.ptr(), next.ptr())
        );
    }
    scope::update_current_label_after_branch(next);
    scope::set_continuation(body, test, MD_LOOP);
    // Perform codegen only after the continuations are set.
    body.codegen();
    return rv;
  }

  instruction break_()
  {
    // Follow implied terminators until a loopback is encountered.
    llvm::BasicBlock * bb = scope::current_label().ptr();
    while(true)
    {
      // The Sprite branching constructs add implied continuations to every
      // successor in the CFG.  If there is no terminator, then this must
      // have been called outside of any branch (e.g., in the function entry
      // block).
      llvm::TerminatorInst * term = bb->getTerminator();
      if(!term)
        throw compile_error("break_ used outside of a loop(1).");

      metadata md = instruction(term).get_metadata(SPRITE_IMPLIED_METADATA);

      if(!md)
        throw compile_error("break_ used outside of a loop(2).");
      else
        bb = term->getSuccessor(0);

      assert(bb);

      // If this is a loopback, then we've found the loop escape.  The
      // loopback goes to the basic block that evaluates the condition
      // and the target of this break is the false branch of its
      // terminator.
      if(md.size() && valueof<int>(md[0]) == MD_LOOP)
      {
        term = bb->getTerminator();
        assert(term);
        assert(term->getNumSuccessors() == 2);
        label target(term->getSuccessor(1));
        return goto_(target);
      }
    }
  }
}}
