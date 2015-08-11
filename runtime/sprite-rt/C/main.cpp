#include "basic_runtime.hpp"
#include "computation_frame.hpp"
#include <algorithm>
#include <cassert>
#include <iostream>
#include <limits>
#include <list>
#include "llvm/ADT/SmallVector.h"
#include <unordered_set>
#include <sstream>
#include <stack>
#include <vector>
#include "stdio.h"
#include "stdlib.h"
#include "cymemory.hpp"
#include "fingerprint.hpp"
#include <boost/scope_exit.hpp>

#define SUCC_0(root) reinterpret_cast<node*&>(root->slot0)
#define SUCC_1(root) reinterpret_cast<node*&>(root->slot1)
#define DATA(root, type) (*reinterpret_cast<type *>(&root->slot0))

#define NODE_ALLOC(variable, label)            \
    NODE_ALLOC_WITH_ACTIONS(variable, label, ) \
  /**/

#define NODE_ALLOC_WITH_ACTIONS(variable, label, actions)           \
    do {                                                            \
      if(CyMem_FreeList)                                            \
      {                                                             \
        variable = reinterpret_cast<node*>(CyMem_FreeList);         \
        CyMem_FreeList = *reinterpret_cast<void**>(CyMem_FreeList); \
      }                                                             \
      else                                                          \
        { {actions}; CyMem_NodePool->collect(); goto label; }       \
    } while(0)                                                      \
  /**/


// The maximum arity of any node, plus one.
#ifndef SPRITE_ARITY_BOUND
#define SPRITE_ARITY_BOUND 50
#endif

// The maximum number of children stored within a node, plus one.
#ifndef SPRITE_INPLACE_BOUND
#define SPRITE_INPLACE_BOUND 3
#endif

#ifdef DIAGNOSTICS
#define DPRINTF(...) printf(__VA_ARGS__)
#else
#define DPRINTF(...) (void) nullptr
#endif

using namespace sprite::compiler;

extern "C"
{
  bool Cy_TestChoiceIsMade(aux_t id);
  bool Cy_TestChoiceIsLeft(aux_t id);
}

namespace sprite { namespace compiler
{
  /// The pool for allocating new nodes.
  typedef NodePool<GlobalAllocator> node_pool_type;
  node_pool_type * CyMem_NodePool = new node_pool_type(NODE_BYTES, 256);

  /// An array of pools for allocating successor arrays of length 3,4,...,n.
  boost::pool<> * Cy_ArrayPool = []
  {
    boost::pool<> * p = reinterpret_cast<boost::pool<> *>(
        new char[sizeof(boost::pool<>) * SPRITE_ARITY_BOUND]
      );
    for(size_t i=SPRITE_INPLACE_BOUND; i<SPRITE_ARITY_BOUND; ++i)
      new(&p[i]) boost::pool<>(i * sizeof(void *));
    return p;
  }();

  // The memory roots, used for gc.
  std::deque<node*> CyMem_Roots;
}}

extern "C"
{
  // Global variables (initialization).
  Cy_ComputationFrame * Cy_GlobalComputations = nullptr;
  Shared<Fingerprint> * Cy_CurrentFingerprint = nullptr;
  Shared<ConstraintStore> * Cy_CurrentConstraints = nullptr;

  // These are defined in the "LLVM part" of the runtime.
  extern vtable CyVt_Binding __asm__(".vt.binding");
  extern vtable CyVt_LazyBinding __asm__(".vt.lazybinding");
  extern vtable CyVt_Char __asm__(".vt.Char");
  extern vtable CyVt_Choice __asm__(".vt.choice");
  extern vtable CyVt_Failed __asm__(".vt.failed");
  extern vtable CyVt_Free __asm__(".vt.freevar");
  extern vtable CyVt_Fwd __asm__(".vt.fwd");
  extern vtable CyVt_Int64 __asm__(".vt.Int64");
  extern vtable CyVt_Float __asm__(".vt.Float");
  extern vtable CyVt_Success __asm__(".vt.Success");
  extern vtable CyVt_PartialSpine __asm__(".vt.PartialSpine");
  extern vtable CyVt_True __asm__(".vt.CTOR.Prelude.True");
  extern vtable CyVt_False __asm__(".vt.CTOR.Prelude.False");
  extern vtable CyVt_Cons __asm__(".vt.CTOR.Prelude.:");
  extern vtable CyVt_Nil __asm__(".vt.CTOR.Prelude.[]");
  extern vtable CyVt_LT __asm__(".vt.CTOR.Prelude.LT");
  extern vtable CyVt_EQ __asm__(".vt.CTOR.Prelude.EQ");
  extern vtable CyVt_GT __asm__(".vt.CTOR.Prelude.GT");
  extern vtable CyVt_Tuple0 __asm__(".vt.CTOR.Prelude.()");
  extern vtable CyVt_apply __asm__(".vt.OPER.Prelude.apply");
  extern vtable CyVt_pair __asm__(".vt.CTOR.Prelude.(,)");
  extern vtable CyVt_cond __asm__(".vt.OPER.Prelude.cond");
  extern vtable CyVt_amp __asm__(".vt.OPER.Prelude.&");

  aux_t Cy_NextChoiceId = 0;

  int64_t CyTrace_IndentLvl = 0;
  void CyTrace_Indent() { CyTrace_IndentLvl += 2; }
  void CyTrace_Dedent() { CyTrace_IndentLvl -= 2; }
  void CyTrace_ShowIndent()
  {
    for(int64_t i=0; i<CyTrace_IndentLvl; ++i)
      putchar(' ');
  }

  void CyMem_PushRoot(node * p) { CyMem_Roots.push_back(p); }
  void CyMem_PopRoot() { CyMem_Roots.pop_back(); }
  void CyMem_Collect() { CyMem_NodePool->collect(); }

  node ** Cy_ArrayAllocTyped(aux_t n)
    { return reinterpret_cast<node**>(Cy_ArrayPool[n].malloc()); }

  void Cy_ArrayDealloc(aux_t n, void * px)
    { if(px) Cy_ArrayPool[n].free(px); }

  node * Cy_SkipFwd(node * root)
  {
    while(root->vptr == &CyVt_Fwd)
      root = SUCC_0(root);
    return root;
  }

  void Cy_Suspend() __attribute__((__noreturn__));
  void Cy_Suspend()
  {
    // TODO
    // This is probably not needed.  KiCS2 showed how to narrow Int and Char.
    printf("[Fixme] Goal suspended!");
    std::exit(EXIT_FAILURE);
  }

  void CyPrelude_suspend(node * root)
    { Cy_Suspend(); }

  void CyErr_Undefined(char const * op, char const * type)
  {
    fprintf(stderr, "%s is undefined for type %s", op, type);
    Cy_Suspend();
  }

  void CyErr_NoGenerator(char const *) __attribute__((__noreturn__));
  void CyErr_NoGenerator(char const * type)
  {
    fprintf(stderr, "No generator for type %s", type);
    Cy_Suspend();
  }

  void Cy_failed(node * root, bool need_destroy=false)
  {
    if(need_destroy)
      root->vptr->destroy(root);
    root->vptr = &CyVt_Failed;
    root->tag = FAIL;
  }

  void CyPrelude_failed(node * root)
  {
    root->vptr->destroy(root);
    root->vptr = &CyVt_Failed;
    root->tag = FAIL;
  }

  void Cy_success(node * root, bool need_destroy=false)
  {
    if(need_destroy)
      root->vptr->destroy(root);
    root->vptr = &CyVt_Success;
    root->tag = CTOR;
  }

  void CyPrelude_success(node * root)
  {
    root->vptr->destroy(root);
    root->vptr = &CyVt_Success;
    root->tag = CTOR;
  }

  // (>>=) :: IO a -> (a -> IO b) -> IO b
  void CyPrelude_RightRightEqual(node *) __asm__("CyPrelude_>>=");
  void CyPrelude_RightRightEqual(node * root)
  {
    // Normalize the first arg to trigger its IO action.
    #include "normalize1.def"
    root->vptr = &CyVt_apply;
    // tag == OPER already.
    std::swap(SUCC_0(root), SUCC_1(root));
  }

  void CyPrelude_return(node * root)
  {
    root->vptr = &CyVt_Fwd;
    root->tag = FWD;
  }

  void CyPrelude_putChar(node * root)
  {
    #define TAG(arg) arg->tag
    #define WHEN_FREE(arg) Cy_Suspend()
    #include "normalize1.def"
    putc(DATA(arg, char), stdout);
    root->vptr = &CyVt_Tuple0;
    root->tag = CTOR;
  }

  void Cy_true(node * root, bool need_destroy=false)
  {
    if(need_destroy)
      root->vptr->destroy(root);
    root->vptr = &CyVt_True;
    root->tag = 1;
  }

  void Cy_false(node * root, bool need_destroy=false)
  {
    if(need_destroy)
      root->vptr->destroy(root);
    root->vptr = &CyVt_False;
    root->tag = 0;
  }

  void Cy_eq(node * root, bool need_destroy=false)
  {
    if(need_destroy)
      root->vptr->destroy(root);
    root->vptr = &CyVt_EQ;
    root->tag = 1;
  }

  void Cy_NoAction(node * root) {}

  void _Cy_Repr(
      std::unordered_set<node *> & seen, node * root, FILE * stream
    , bool is_outer
    )
  {
    if(!root)
    {
      fputs("(!null-node!)", stream);
      return;
    }
    node ** begin, ** end;
    if(!root->vptr)
    {
      fputs("(!null-vptr!)", stream);
      return;
    }
    root->vptr->succ(root, &begin, &end);
    size_t const N = end - begin;
    if(!is_outer && N > 0) fputs("(", stream);
    fputs(root->vptr->label(root), stream);
    seen.insert(root);
    for(; begin != end; ++begin)
    {
      fputs(" ", stream);
      if(begin && seen.count(*begin))
        fputs("...", stream);
      else
        _Cy_Repr(seen, *begin, stream, false);
    }
    if(!is_outer && N > 0) fputs(")", stream);
    seen.erase(root);
  }

  // Writes a representation of the expression to the given stream.  The
  // representation is not normalized.  If is_outer, then the root is the
  // outermost term and is never parenthesized.
  void Cy_Repr(node * root, FILE * stream, bool is_outer)
  {
    std::unordered_set<node *> seen;
    _Cy_Repr(seen, root, stream, is_outer);
  }

  // Prints a node representation to stdout (for debugging).
  void Cy_Print(node * root) 
  {
    Cy_Repr(root, stdout, true);
    fputs("\n", stdout);
    fflush(NULL);
  }

  // Converts a C-string to a Curry string (list of Char).  Rewrites root to be
  // the Curry string.  Returns the null terminator node of the Curry string.
  node * Cy_CStringToCyString(
      char const * str, node * root
    , size_t max = std::numeric_limits<size_t>::max()
    )
  {
    size_t n = 0;
    node * char_data;
    node * next;

    // Before the first character is created, the root node must be destroyed.
    if(*str && n++ != max)
    {
    redo0:
      NODE_ALLOC(char_data, redo0);
      NODE_ALLOC(next, redo0);

      root->vptr->destroy(root);

      char_data->vptr = &CyVt_Char;
      char_data->tag = CTOR;
      DATA(char_data, char) = *str++;

      root->vptr = &CyVt_Cons;
      root->tag = CTOR + 1;
      root->slot0 = char_data;
      root->slot1 = next;

      root = next;
    }

    // For subsequent characters, the string must be terminated just before
    // running garbage collection.
    #define TERMINATE_STRING root->vptr = &CyVt_Nil; root->tag = CTOR;
    while(*str && n++ != max)
    {
    redo:
      NODE_ALLOC_WITH_ACTIONS(char_data, redo, TERMINATE_STRING);
      NODE_ALLOC_WITH_ACTIONS(next, redo, TERMINATE_STRING);

      char_data->vptr = &CyVt_Char;
      char_data->tag = CTOR;
      DATA(char_data, char) = *str++;

      root->vptr = &CyVt_Cons;
      root->tag = CTOR + 1;
      root->slot0 = char_data;
      root->slot1 = next;

      root = next;
    }

    TERMINATE_STRING
    #undef TERMINATE_STRING
    return root;
  }

  void Cy_CyStringToCString(node * root, FILE * stream)
  {
    while(root->vptr != &CyVt_Nil)
    {
      if(root->vptr == &CyVt_Fwd)
        root = SUCC_0(root);
      else
      {
        fputc(DATA(SUCC_0(root), char), stream);
        root = SUCC_1(root);
      }
    }
  }

  void Cy_Normalize(node * root) { root->vptr->N(root); }

  FILE * Cy_stdin() { return stdin; }
  FILE * Cy_stdout() { return stdout; }
  FILE * Cy_stderr() { return stderr; }

  // Copy a stencil when instantiating a narrowed variable.
  node * CyFree_CopyStencil(node * head)
  {
    // Activations.
    std::vector<node *> active;
    active.reserve(8);

    // Return values.
    struct Ret { node * p; size_t n; };
    std::vector<Ret> ret;
    ret.reserve(8);

    node * copy;
    node **src_begin;
    node **src_end;
    node **dst_begin;

  redo:
    active.clear();
    active.push_back(head);
    ret.clear();

    while(!active.empty())
    {
      node * item = active.back();
      active.pop_back();
      if(item->tag == CHOICE)
      {
        active.push_back(SUCC_1(item));
        active.push_back(SUCC_0(item));
        NODE_ALLOC(copy, redo);
        copy->vptr = &CyVt_Choice;
        copy->tag = CHOICE;
        copy->aux = item->aux;
        ret.push_back(Ret{copy, 0});
      }
      else
      {
        NODE_ALLOC(copy, redo);
        copy->vptr = item->vptr;
        copy->tag = item->tag;
        
        item->vptr->succ(item, &src_begin, &src_end);
        aux_t const N = static_cast<aux_t>(src_end - src_begin);
        if(N < SPRITE_INPLACE_BOUND)
          dst_begin = &SUCC_0(copy);
        else
        {
          dst_begin = Cy_ArrayAllocTyped(N);
          DATA(copy, node**) = dst_begin;
        }
        while(src_begin!=src_end)
          *dst_begin++ = *src_begin++;

        do
        {
          auto & r = ret.back();
          if(r.n == 0)
          {
            SUCC_0(r.p) = copy;
            r.n++;
            break;
          }
          else
          {
            SUCC_1(r.p) = copy;
            copy = r.p;
            ret.pop_back();
          }
        } while(!ret.empty());
      }
    }
    return copy;
  }

  void CyBind_BindVars(node * from, node * to)
  {
    auto & constraints = Cy_CurrentConstraints->write();
    constraints.add_var_constraints(from, to, false);
    constraints.add_choice_constraints(from->aux, to->aux);
  }

  // Used to implement =:<=.  The RHS may be an unevaluated expression.
  void CyBind_LazyBindVar(node * from, node * to)
    { Cy_CurrentConstraints->write().add_var_constraints(from, to, true); }

  // Clone a stencil from one variable to be attached to another variable.
  // Fresh IDs are used for copied choices and variables, but the first choice
  // ID is given by the parameter.
  node * CyFree_CloneStencil(node * head, aux_t first_id)
  {
    // Activations.
    std::vector<node *> active;
    active.reserve(8);

    // Return values.
    struct Ret { node * p; size_t n; };
    std::vector<Ret> ret;
    ret.reserve(8);

    node * copy;
    node **src_begin;
    node **src_end;
    node **dst_begin;
    auto saved_next_choice = Cy_NextChoiceId;

  redo:
    bool first = true;
    Cy_NextChoiceId = saved_next_choice;
    active.clear();
    active.push_back(head);
    ret.clear();

    while(!active.empty())
    {
      node * item = active.back();
      active.pop_back();
      if(item->tag == CHOICE)
      {
        active.push_back(SUCC_1(item));
        active.push_back(SUCC_0(item));
        NODE_ALLOC(copy, redo);
        copy->vptr = &CyVt_Choice;
        copy->tag = CHOICE;
        if(first)
        {
          first = false;
          copy->aux = first_id;
        }
        else
          copy->aux = Cy_NextChoiceId++;
        ret.push_back(Ret{copy, 0});
      }
      else
      {
        NODE_ALLOC(copy, redo);
        copy->vptr = item->vptr;
        copy->tag = item->tag;
        
        item->vptr->succ(item, &src_begin, &src_end);
        aux_t const N = static_cast<aux_t>(src_end - src_begin);
        if(N < SPRITE_INPLACE_BOUND)
          dst_begin = &SUCC_0(copy);
        else
        {
          dst_begin = Cy_ArrayAllocTyped(N);
          DATA(copy, node**) = dst_begin;
        }
        for(; src_begin!=src_end; ++src_begin, ++dst_begin)
        {
          node * tmp;
          NODE_ALLOC(tmp, redo);
          tmp->vptr = &CyVt_Free;
          tmp->tag = FREE;
          tmp->aux = Cy_NextChoiceId++;
          tmp->slot0 = 0;
          *dst_begin = tmp;
        }

        do
        {
          auto & r = ret.back();
          if(r.n == 0)
          {
            SUCC_0(r.p) = copy;
            r.n++;
            break;
          }
          else
          {
            SUCC_1(r.p) = copy;
            copy = r.p;
            ret.pop_back();
          }
        } while(!ret.empty());
      }
    }
    return copy;
  }

  // Set the proper choice and variable constraints between corresponding stencils.
  void CyFree_NarrowConstraints(node * head0, node * head1)
  {
    ConstraintStore * constraints = nullptr;

    // Activations.
    std::vector<std::pair<node *, node *>> active;
    active.reserve(8);
    active.emplace_back(head0, head1);

    node **src_begin;
    node **src_end;
    node **dst_begin;

    while(!active.empty())
    {
      node * lhs, * rhs;
      std::tie(lhs, rhs) = active.back();
      active.pop_back();
      if(lhs->tag == CHOICE)
      {
        active.emplace_back(SUCC_1(lhs), SUCC_1(rhs));
        active.emplace_back(SUCC_0(lhs), SUCC_0(rhs));
        if(!constraints)
          constraints = &Cy_CurrentConstraints->write();
        constraints->add_choice_constraints(lhs->aux, rhs->aux);
      }
      else
      {
        lhs->vptr->succ(lhs, &src_begin, &src_end);
        aux_t const N = static_cast<aux_t>(src_end - src_begin);
        dst_begin = N < SPRITE_INPLACE_BOUND ? &SUCC_0(rhs) : DATA(rhs, node**);
        while(src_begin!=src_end)
          CyBind_BindVars(*src_begin++, *dst_begin++);
      }
    }
  }

  void Cy_ExpandLazyVar(node *& conditions, node * p, node * q)
  {
    // It is time to evaluate this, now.  The variable participating in
    // lazy binding is needed by the current computation.
    q->vptr->H(q);
  redo:
    // Create a new condition.  It will be prepended to the evaluation.
    node * newc;
    NODE_ALLOC(newc, redo);
    newc->vptr = q->vptr->ns_equate; // use the type-specific one.
    newc->tag = OPER;
    SUCC_0(newc) = p;
    SUCC_1(newc) = q;
   
    // Create (cond newc _) if conditions does not exist.  Using _ to mean "don't care."
    if(!conditions)
    {
      NODE_ALLOC(conditions, redo);
      CyMem_PushRoot(conditions);
      conditions->vptr = &CyVt_cond;
      conditions->tag = OPER;
      SUCC_0(conditions) = newc;
      conditions->slot1 = conditions; // to be filled in by the caller.  GC expects non-null.
    }
    // Otherwise, extend the conditions:
    //     (cond c _) -> (cond (newc & c) _)
    else
    {
      node * amp;
      NODE_ALLOC(amp, redo);
      amp->vptr = &CyVt_amp;
      amp->tag = OPER;
      SUCC_0(amp) = newc;
      SUCC_1(amp) = SUCC_0(conditions);

      // This & operation takes the place of the old condition.
      SUCC_0(conditions) = amp;
    }
  }

  node * Cy_ExpandVarConstraints(ConstraintStore & constraints, aux_t id)
  {
    std::unordered_set<aux_t> seen;
    seen.insert(id);
    std::vector<aux_t> todo;
    todo.push_back(id);

    // Capture additional conditions that arise as the result of expanding the
    // lazy bindings.
    node * conditions = nullptr;
    BOOST_SCOPE_EXIT((&conditions))
      { if(conditions) CyMem_PopRoot(); }
    BOOST_SCOPE_EXIT_END

    // Scratch space.  Only used in rare cases.
    std::vector<ConstraintStore::BindData> scratch;

    while(!todo.empty())
    {
      id = todo.back();
      todo.pop_back();

      auto const & vstore = constraints.eq_var.read();
      auto it = vstore.find(id);
      if(it == vstore.end())
        continue;
      DPRINTF("Processing id=%d\n", id);
      auto const & bucket = it->second.read();
      auto in = bucket.begin();
      auto end = bucket.end();
      for(; in!=end; ++in)
      {
        node * p = in->first;
        node * q = in->second;
        DPRINTF("Begin\n");
        BOOST_SCOPE_EXIT((in)) { DPRINTF("End\n"); } BOOST_SCOPE_EXIT_END
        if(in->is_lazy)
        {
          DPRINTF("Expanding lazy\n");
          Cy_ExpandLazyVar(conditions, p, q);
        }
        else
        {
          DPRINTF("Expanding non-lazy with lazy_only=%d\n", 0);
          DPRINTF("Has stencil: p=%p:%d, q=%p:%d\n", p, bool(SUCC_0(p)), q, bool(SUCC_0(q)));
          if(!seen.count(q->aux))
          {
            todo.push_back(q->aux);
            seen.insert(q->aux);
          }

          auto & sp = SUCC_0(p);
          auto & sq = SUCC_0(q);
          int const code = (sp ? 2 : 0) + (sq ? 1 : 0);
          switch(code)
          {
            case 0: // !sp && !sq
              scratch.emplace_back(*in);
              continue;
            case 1: // !sp && sq
              sp = CyFree_CloneStencil(sq, p->aux);
              break;
            case 2: // sp && !sq
              sq = CyFree_CloneStencil(sp, q->aux);
              break;
            case 3: // sp && sq
              break;
          }

          CyFree_NarrowConstraints(sp, sq);
        }
      }

      // Erase elements that were not kept, and remove the container, if empty.
      // Local variable scratch contains the retained contents.
      if(scratch.empty())
      {
        // Reuse the iterator, if possible.
        if(constraints.eq_var.needs_copy())
          constraints.eq_var.write().erase(id);
        else
          constraints.eq_var.write().erase(it);
      }
      else
      {
        constraints.eq_var.write()[id] = std::move(scratch);
        scratch.clear();
      }
    }
    return conditions;
  }

  // Validate (and clear) the constraints in the store, updating the
  // fingerprint as needed.  Return true if the computation fails.
  bool Cy_ValidateConstraints(
      Shared<Fingerprint> & fp, Shared<ConstraintStore> & constraints, aux_t id
    )
  {
    DPRINTF("Now validating constraints for %d\n", id);
    DPRINTF("The eq_choice keys are:");
    #ifdef DIAGNOSTICS
    for(auto ii: constraints->eq_choice.read())
      DPRINTF(" %d", ii.first);
    #endif
    DPRINTF("\n");

    std::vector<aux_t> todo{id};
    while(!todo.empty())
    {
      id = todo.back();
      todo.pop_back();
      DPRINTF("Checking id=%d\n", id);

      auto const & cstore = constraints->eq_choice.read();
      auto it = cstore.find(id);
      if(it == cstore.end())
      {
        DPRINTF("Not found\n");
        continue;
      }
      auto made = fp->test(id);
      for(auto id2: it->second.read())
      {
        DPRINTF("Checking id2=%d\n", id2);
        auto made2 = fp->test(id2);
        switch(made2)
        {
          case ChoiceState::LEFT:
          case ChoiceState::RIGHT:
            if(made != made2)
            {
              DPRINTF("?%d <=> ?%d FAILED\n", id, id2);
              return true;
            }
            else
              DPRINTF("?%d <=> ?%d PASSED\n", id, id2);
            break;
          case ChoiceState::UNDETERMINED:
            if(made == ChoiceState::LEFT)
            {
              DPRINTF("Setting ?%d to LEFT\n", id2);
              fp.write().set_left_no_check(id2);
            }
            else
            {
              DPRINTF("Setting ?%d to RIGHT\n", id2);
              fp.write().set_right_no_check(id2);
            }
            break;
        }

        // Process chains of bindings.
        todo.push_back(id2);
      }
      // All affected choices are committed, now.  It is safe to clear this
      // part of the constraint store.
      constraints.write().eq_choice.write().erase(id);
    }
    return false;
  }

  // Gets the representation of the current fingerprint.
  void Cy_ReprFingerprint(FILE * stream)
  {
    if(Cy_CurrentFingerprint)
    {
      auto & fp = Cy_CurrentFingerprint->read();
      size_t const n = fp.size();
      bool first = true;
      for(size_t i=0; i<n; ++i)
      {
        switch(fp.test_no_check(i))
        {
          case ChoiceState::LEFT:
            if(!first) fputc(',', stream); else first = false;
            fprintf(stream, "%zu:L", i); break;
          case ChoiceState::RIGHT:
            if(!first) fputc(',', stream); else first = false;
            fprintf(stream, "%zu:R", i); break;
          case ChoiceState::UNDETERMINED:;
        }
      }
    }
  }

  // Gets the representation of the current constraint store.
  void Cy_ReprConstraints(FILE * stream)
  {
    auto & store = *Cy_CurrentConstraints;
    bool first_out = true;
    for(auto const & kv: store->eq_choice.read())
    {
      if(!first_out) fputc(',', stream); else first_out = false;
      fprintf(stream, "%d:=:(", kv.first);
      bool first = true;
      for(auto id: kv.second.read())
      {
        if(!first) fputc(',', stream); else first = false;
        fprintf(stream, "%d", id);
      }
      fputc(')', stream);
    }
    for(auto const & kv: store->eq_var.read())
    {
      if(!first_out) fputc(',', stream); else first_out = false;
      fprintf(stream, "%d => [", kv.first);
      bool first = true;
      for(auto const & data: kv.second.read())
      {
        if(!first) fputc(',', stream); else first = false;
        if(data.is_lazy)
        {
          fprintf(stream, "_x%d:=<:", data.first->aux);
          Cy_Repr(data.second, stream, false);
        }
        else
          fprintf(stream, "_x%d:=:_x%d", data.first->aux, data.second->aux);
      }
      fputc(']', stream);
    }
  }

  // Tests the indicated choice in the current fingerprint.  Precondition:
  // Cy_Eval is on the stack, so that Cy_CurrentFingerprint is non-null.
  bool Cy_TestChoiceIsMade(aux_t id)
    { return Cy_CurrentFingerprint->read().choice_is_made(id); }

  // Indicates whether a made choice is LEFT or RIGHT.  Precondition: Cy_TestChoiceIsMade(id).
  bool Cy_TestChoiceIsLeft(aux_t id)
    { return Cy_CurrentFingerprint->read().choice_is_left_no_check(id); }

  void CyFree_GcSucc(node * root, node *** begin, node *** end)
  {
    *begin = &SUCC_0(root);
    *end = SUCC_0(root) ? &SUCC_1(root) : &SUCC_0(root);
  }

  // Evaluates an expression.  Calls yield(x) for each result x.
  void Cy_Eval(node * root, void(*yield)(node * root))
  {
    // Set up this computations.
    std::list<Cy_EvalFrame> computation;
    computation.emplace_back(root);

    // Link it into the list of global computations.
    Cy_ComputationFrame compframe = { &computation, Cy_GlobalComputations };
    Cy_GlobalComputations = &compframe;
    struct Cleanup
      { ~Cleanup() { Cy_GlobalComputations = Cy_GlobalComputations->next; } }
      _cleanup;

    while(!computation.empty())
    {
      Cy_EvalFrame & frame = computation.front();
      Cy_CurrentFingerprint = &frame.fingerprint;
      Cy_CurrentConstraints = &frame.constraints;
      node * expr = frame.expr;
      DPRINTF("Q> WORKING_ON %p\n", expr);
      expr->vptr->N(expr);
      redo: switch(expr->tag)
      {
        handle_failure:
        case FAIL:
          DPRINTF("Q> FAILED %p\n", expr);
          computation.pop_front();
          break;
        case FWD:
          DPRINTF("Q> REBASE %p -> %p\n", expr, SUCC_0(expr));
          frame.expr = expr = SUCC_0(expr);
          goto redo;
        case FREE:
        {
          // Expand lazy bindings on this variable before accepting it as data.
          if(frame.constraints->eq_var->count(expr->aux))
          {
            // Trigger a copy-on-write.
            auto & vstore = frame.constraints.write().eq_var.write();
            auto pbucket = vstore.find(expr->aux);
            auto & bucket = pbucket->second.write();

            node * conditions = nullptr; 
            BOOST_SCOPE_EXIT((conditions))
              { if(conditions) CyMem_PopRoot(); }
            BOOST_SCOPE_EXIT_END

            for(auto const & data: bucket)
            {
              if(data.is_lazy)
                Cy_ExpandLazyVar(conditions, data.first, data.second);
            }
            auto last = std::remove_if(
                bucket.begin(), bucket.end()
              , [](ConstraintStore::BindData const & data) { return data.is_lazy; }
              );
            bucket.erase(last, bucket.end());
            if(bucket.empty())
              vstore.erase(pbucket);

            if(conditions)
            {
              SUCC_1(conditions) = expr;
              frame.expr = conditions;
              DPRINTF("Q> ADD_CONDITIONS %p\n", frame.expr);
              break;
            }
          }

          if(frame.fingerprint->choice_is_made(expr->aux))
          {
            node * choice = SUCC_0(expr);
            frame.expr = expr = CyFree_CopyStencil(choice);
            goto handle_choice;
          }
          goto yield_value;
        }
        case BINDING:
        {
          node * args = SUCC_1(expr);
          node * lhs = SUCC_0(args);
          node * rhs = SUCC_1(args);
          bool const is_lazy = expr->vptr == &CyVt_LazyBinding;
          // Use a ref here to be sure the added conditions are always attached
          // to the main computation.
          node *& next_expr = SUCC_0(expr);

          DPRINTF("A binding reached the top; lazy = %s\n", (is_lazy ? "T" : "F"));

          if(is_lazy)
          {
            CyBind_LazyBindVar(lhs, rhs);

            // Check LHS of constraint.
            aux_t id = lhs->aux;
            if(frame.fingerprint->test(id) != ChoiceState::UNDETERMINED)
            {
              DPRINTF("Processing LEFT\n");
              node * conditions = Cy_ExpandVarConstraints(frame.constraints.write(), id);
              if(Cy_ValidateConstraints(frame.fingerprint, frame.constraints, id))
                goto handle_failure;
              if(conditions)
              {
                SUCC_1(conditions) = next_expr;
                next_expr = conditions;
              }
            }
          }
          else // expr->vptr == &CyVt_Binding
          {
            CyBind_BindVars(lhs, rhs);

            DPRINTF("Processing LEFT\n");
            aux_t id = lhs->aux;
            node * conditions = Cy_ExpandVarConstraints(frame.constraints.write(), id);
            if(frame.fingerprint->test(id) != ChoiceState::UNDETERMINED)
            {
              if(Cy_ValidateConstraints(frame.fingerprint, frame.constraints, id))
                goto handle_failure;
            }
            if(conditions)
            {
              SUCC_1(conditions) = next_expr;
              next_expr = conditions;
            }

            // Check RHS of constraint (only for non-lazy bindings).
            DPRINTF("Processing RIGHT\n");
            id = rhs->aux;
            conditions = Cy_ExpandVarConstraints(frame.constraints.write(), id);
            if(frame.fingerprint->test(id) != ChoiceState::UNDETERMINED)
            {
              if(Cy_ValidateConstraints(frame.fingerprint, frame.constraints, id))
                goto handle_failure;
            }
            if(conditions)
            {
              SUCC_1(conditions) = next_expr;
              next_expr = conditions;
            }
          }

          #ifdef DIAGNOSTICS
          if(next_expr != SUCC_0(expr))
            DPRINTF("Q> ADD_CONDITIONS %p\n", next_expr);
          #endif
          DPRINTF("Q> REBASE %p\n", next_expr); // DEBUG
          frame.expr = next_expr;
          break;
        }
        case CHOICE:
        handle_choice:
        {
          aux_t const id = expr->aux;
          // Convert constraints over variables into constraints over choices.
          // If new conditions are uncovered, process them before this choice.
          DPRINTF("Expanding vars\n");
          node * conditions = Cy_ExpandVarConstraints(frame.constraints.write(), id);
          if(conditions)
          {
            SUCC_1(conditions) = expr;
            frame.expr = conditions;
            DPRINTF("Q> ADD_CONDITIONS@@ %p\n", frame.expr); // DEBUG
            break;
          }
          DPRINTF("Done expanding vars\n");

          switch(frame.fingerprint->test(id))
          {
            case ChoiceState::LEFT:
              // Discard right expression.
              DPRINTF("Q> CHOOSE_LEFT %p\n", SUCC_0(expr)); // DEBUG
              frame.expr = SUCC_0(expr);
              break;
            case ChoiceState::RIGHT:
              // Discard left expression.
              DPRINTF("Q> CHOOSE_RIGHT %p\n", SUCC_1(expr)); // DEBUG
              frame.expr = SUCC_1(expr);
              break;
            case ChoiceState::UNDETERMINED:
            {
              // Keep both left and right (unless constraints are not met).
              auto head = computation.begin();

              // LEFT
              Shared<Fingerprint> left_fp(frame.fingerprint);
              left_fp.write().set_left_no_check(id); // note: call to test() dominates.
              Shared<ConstraintStore> left_cst(frame.constraints);
              if(!Cy_ValidateConstraints(left_fp, left_cst, id))
              {
                computation.emplace(
                    head
                  , SUCC_0(expr), std::move(left_fp), std::move(left_cst)
                  );
                DPRINTF("Q> FORK +> %p\n", SUCC_0(expr));
              }
              else
                DPRINTF("Q> LHS_CONSTRAINTS_FAILED\n");

              // RIGHT
              frame.expr = SUCC_1(expr);
              frame.fingerprint.write().set_right_no_check(id); // note: as above
              if(!Cy_ValidateConstraints(frame.fingerprint, frame.constraints, id))
              {
                DPRINTF("Q> REBASE %p -> %p\n", expr, SUCC_1(expr));
                frame.expr = SUCC_1(expr);
              }
              else
              {
                DPRINTF("Q> RHS CONSTRAINTS FAILED\n");
                computation.erase(head);
                break;
              }
            }
          }
          break;
        }
        case OPER:
          fprintf(stderr, "Normalized expression yields a function!  Discarding.\n");
          computation.pop_front();
          break;
        default:
        yield_value:
          yield(expr);
          computation.pop_front();
          DPRINTF("Q> DONE %p\n", expr); // DEBUG
      }
    }
  }

  // Note: root is a [Char], already normalized.
  void Cy_FPrint(node * root, FILE * stream)
  {
    std::stringstream ss;
    while(root->vptr != &CyVt_Nil)
    {
      node * arg = SUCC_0(root);
      ss << DATA(arg, char);
      root = SUCC_1(root);
    }
    fputs(ss.str().c_str(), stream);
  }

  void CyPrelude_prim_isChar(node * root)
  {
    #include "normalize1.def"
    if(arg->vptr == &CyVt_Char)
    {
      root->vptr = &CyVt_True;
      root->tag = CTOR + 1;
    }
    else
    {
      root->vptr = &CyVt_False;
      root->tag = CTOR;
    }
  }

  // Evaluates the argument to head normal form and returns it.  Suspends until
  // the result is bound to a non-variable term.
  void CyPrelude_ensureNotFree(node * root)
  {
    #define WHEN_FREE(arg) Cy_Suspend()
    #include "normalize1.def"
    return;
  }

  // Right-associative application with strict evaluation of its argument to
  // head normal form.
  void CyPrelude_DollarBang(node *) __asm__("CyPrelude_$!");
  void CyPrelude_DollarBang(node * root)
  {
    node * rhs;
    #include "normalize2nd.def" // head-normalizes by default.
    root->vptr = &CyVt_apply;
  }

  // Right-associative application with strict evaluation of its argument
  // to normal form.
  void CyPrelude_DollarBangBang(node *) __asm__("CyPrelude_$!!");
  void CyPrelude_DollarBangBang(node * root)
  {
    node * rhs;
    #define NORMALIZE(arg) arg->vptr->N(arg)
    #include "normalize2nd.def"
    root->vptr = &CyVt_apply;
  }

  // Right-associative application with strict evaluation of its argument
  // to ground normal form.
  void CyPrelude_DollarHashHash(node *) __asm__("CyPrelude_$##");
  void CyPrelude_DollarHashHash(node * root)
  {
    node * rhs;
    #define NORMALIZE(arg) arg->vptr->N(arg)
    #define WHEN_FREE(arg) Cy_Suspend() // FIXME
    #include "normalize2nd.def"
    root->vptr = &CyVt_apply;
  }

  void CyPrelude_error(node * root)
  {
    node * arg = SUCC_0(root);
    CyPrelude_DollarHashHash(arg);
    Cy_FPrint(arg, stderr);
    putc('\n', stderr);
    std::exit(EXIT_FAILURE); // FIXME: should throw or lngjump.
  }

  void CyPrelude_IntPlus(node *) __asm__("CyPrelude_+");
  void CyPrelude_IntPlus(node * root)
  {
    #define TAG(arg) arg->tag
    #define WHEN_FREE(arg) Cy_Suspend()
    #include "normalize2.def"
    int64_t const x = DATA(lhs, int64_t);
    int64_t const y = DATA(rhs, int64_t);
    root->vptr = &CyVt_Int64;
    root->tag = CTOR;
    DATA(root, int64_t) = x + y;
  }

  void CyPrelude_IntSub(node *) __asm__("CyPrelude_-");
  void CyPrelude_IntSub(node * root)
  {
    #define TAG(arg) arg->tag
    #define WHEN_FREE(arg) Cy_Suspend()
    #include "normalize2.def"
    int64_t const x = DATA(lhs, int64_t);
    int64_t const y = DATA(rhs, int64_t);
    root->vptr = &CyVt_Int64;
    root->tag = CTOR;
    DATA(root, int64_t) = x - y;
  }

  void CyPrelude_IntMul(node *) __asm__("CyPrelude_*");
  void CyPrelude_IntMul(node * root)
  {
    #define TAG(arg) arg->tag
    #define WHEN_FREE(arg) Cy_Suspend()
    #include "normalize2.def"
    int64_t const x = DATA(lhs, int64_t);
    int64_t const y = DATA(rhs, int64_t);
    root->vptr = &CyVt_Int64;
    root->tag = CTOR;
    DATA(root, int64_t) = x * y;
  }

  // Truncates towards negative infinity.
  void CyPrelude_div(node * root)
  {
    #define TAG(arg) arg->tag
    #define WHEN_FREE(arg) Cy_Suspend()
    #include "normalize2.def"
    int64_t const x = DATA(lhs, int64_t);
    int64_t const y = DATA(rhs, int64_t);
    int64_t q = x/y;
    int64_t const r = x%y;
    if ((r!=0) && ((r<0) != (y<0))) --q;
    root->vptr = &CyVt_Int64;
    root->tag = CTOR;
    DATA(root, int64_t) = q;
  }

  // Truncates towards negative infinity.
  void CyPrelude_mod(node * root)
  {
    #define TAG(arg) arg->tag
    #define WHEN_FREE(arg) Cy_Suspend()
    #include "normalize2.def"
    int64_t const x = DATA(lhs, int64_t);
    int64_t const y = DATA(rhs, int64_t);
    int64_t r = x%y;
    if ((r!=0) && ((r<0) != (y<0))) { r += y; }
    root->vptr = &CyVt_Int64;
    root->tag = CTOR;
    DATA(root, int64_t) = r;
  }

  // Truncates towards zero (standard behavior in C++11).
  void CyPrelude_quot(node * root)
  {
    #define TAG(arg) arg->tag
    #define WHEN_FREE(arg) Cy_Suspend()
    #include "normalize2.def"
    int64_t const x = DATA(lhs, int64_t);
    int64_t const y = DATA(rhs, int64_t);
    root->vptr = &CyVt_Int64;
    root->tag = CTOR;
    DATA(root, int64_t) = x/y;
  }

  // Truncates towards zero (standard behavior in C++11).
  void CyPrelude_rem(node * root)
  {
    #define TAG(arg) arg->tag
    #define WHEN_FREE(arg) Cy_Suspend()
    #include "normalize2.def"
    int64_t const x = DATA(lhs, int64_t);
    int64_t const y = DATA(rhs, int64_t);
    root->vptr = &CyVt_Int64;
    root->tag = CTOR;
    DATA(root, int64_t) = x%y;
  }

  // Unary minus on Floats. Usually written as "-e".
  void CyPrelude_negateFloat(node * root)
  {
    #define TAG(arg) arg->tag
    #define WHEN_FREE(arg) Cy_Suspend()
    #include "normalize1.def"
    double const x = DATA(arg, double);
    root->vptr = &CyVt_Float;
    root->tag = CTOR;
    DATA(root, double) = -x;
  }

  // Converts a character into its ASCII value.
  void CyPrelude_ord(node * root)
  {
    #define TAG(arg) arg->tag
    #define WHEN_FREE(arg) Cy_Suspend()
    #include "normalize1.def"
    root->vptr = &CyVt_Int64;
    root->tag = CTOR;
    DATA(root, int64_t) = DATA(arg, char);
    root->slot1 = 0;
  }

  // Converts an ASCII value into a character.
  void CyPrelude_chr(node * root)
  {
    #define TAG(arg) arg->tag
    #define WHEN_FREE(arg) Cy_Suspend()
    #include "normalize1.def"
    root->vptr = &CyVt_Char;
    root->tag = CTOR;
    DATA(root, char) = static_cast<char>(DATA(arg, int64_t));
    root->slot1 = 0;
  }

  void CyPrelude_apply(node * root)
  {
    #define WHEN_FREE(arg) Cy_Suspend()
    #include "normalize_apply.def"
    aux_t const rem = arg->aux;
    // Handle complete calls.  There is one argument, i.e., this one,
    // remaining.
    if(rem == 1)
    {
      tag_t const n = arg->tag;
      switch(n)
      {
        case 0:
          assert(0 && "applying argument to nullary function");
          break;
        case 1:
          root->vptr = DATA(arg, vtable*);
          root->tag = root->vptr->tag;
          root->slot0 = root->slot1;
          break;
        case 2:
          root->vptr = DATA(SUCC_0(arg), vtable*);
          root->tag = root->vptr->tag;
          root->slot0 = arg->slot1;
          // The second argument is already in the proper place.
          break;
        default:
        {
          node ** args = Cy_ArrayAllocTyped(n);
          node ** pos = args + n - 1;
          node * current = root;
          for(int16_t i=0; i<n; ++i)
          {
            *pos-- = SUCC_1(current);
            current = SUCC_0(current);
          }

          root->vptr = DATA(current, vtable*);
          root->tag = root->vptr->tag;
          root->slot0 = args;
          root->slot1 = 0;
        }
      }
    }
    // Handle incomplete calls.
    else
    {
      root->vptr = &CyVt_PartialSpine;
      root->tag = arg->tag;
      root->aux = rem - 1;
      // The successors do not need to be modified.
    }
  }

  // cond :: Success -> a -> a
  void CyPrelude_cond(node * root)
  {
    node * lhs;
    #define WHEN_FREE(arg) Cy_Suspend()
    #include "normalize1st.def"
    root->vptr = &CyVt_Fwd;
    root->tag = FWD;
    SUCC_0(root) = SUCC_1(root);
  }

  // Concurrent conjunction on constraints.
  // An expression like (c1 & c2) is evaluated by evaluating
  // the constraints c1 and c2 in a concurrent manner.
  // (&)     :: Success -> Success -> Success
  void CyPrelude_Amp(node * root) __asm__("CyPrelude_&");
  void CyPrelude_Amp(node * root)
  {
    node * lhs, * rhs;
    #define WHEN_FREE(lhs) goto try_rhs;
    #include "normalize1st.def"
    // If the LHS is a constructor (i.e., Success), then just return the RHS.
    root->vptr = &CyVt_Fwd;
    root->tag = FWD;
    SUCC_0(root) = SUCC_1(root);
    return;
  try_rhs:
    #define WHEN_FREE(rhs) Cy_Suspend(); // both sides are free
    #include "normalize2nd.def"
    // If the RHS is a constructor return the LHS.
    root->vptr = &CyVt_Fwd;
    root->tag = FWD;
  }

  // Creates a binding object between from and to whose value is Success.
  void CyBind_CreateAsSuccess(node * root, node * from, node * to, bool is_lazy)
  {
  redo:
    node * pair;
    node * success;
    NODE_ALLOC(pair, redo);
    NODE_ALLOC(success, redo);
    pair->vptr = &CyVt_pair;
    pair->tag = CTOR;
    SUCC_0(pair) = from;
    SUCC_1(pair) = to;
    success->vptr = &CyVt_Success;
    success->tag = CTOR;
    root->vptr = is_lazy ? &CyVt_LazyBinding : &CyVt_Binding;
    root->tag = BINDING;
    SUCC_0(root) = success;
    SUCC_1(root) = pair;
  }

  void CyPrelude_Eq(node *) __asm__("CyPrelude_==");
  void CyPrelude_Eq(node * root)
  {
    node * lhs, * rhs;
    bool freelhs = false;
    #define WHEN_FREE(lhs) freelhs = true;
    #include "normalize1st.def"
    #define WHEN_FREE(rhs) \
      if(freelhs) { printf("equality between free variables"); Cy_Suspend(); }
    #include "normalize2nd.def"
    root->vptr = (freelhs ? rhs : lhs)->vptr->equal;
  }

  void CyPrelude_EqColonEq(node *) __asm__("CyPrelude_=:=");
  void CyPrelude_EqColonEq(node * root)
  {
    node * lhs, * rhs;
    bool freelhs = false;
    #define WHEN_FREE(lhs) freelhs = true;
    #include "normalize1st.def"
    #define WHEN_FREE(rhs) if(freelhs) return CyBind_CreateAsSuccess(root, rhs, lhs, false);
    #include "normalize2nd.def"
    root->vptr = (freelhs ? rhs : lhs)->vptr->equate;
  }

  void CyPrelude_EqColonLtEq(node *) __asm__("CyPrelude_=:<=");
  void CyPrelude_EqColonLtEq(node * root)
  {
    node * lhs;
    #define WHEN_FREE(lhs) return CyBind_CreateAsSuccess(root, lhs, SUCC_1(root), true);
    #include "normalize1st.def"
    node * rhs;
    #define WHEN_FREE(rhs) { root->vptr = lhs->vptr->ns_equate; return; }
    #include "normalize2nd.def"
    root->vptr = lhs->vptr->ns_equate;
  }

  void CyPrelude_compare(node * root)
  {
    node * lhs, * rhs;
    bool freelhs = false;
    #define WHEN_FREE(lhs) freelhs = true;
    #include "normalize1st.def"
    // By convention:
    //    "_a < _b | isVar(_a) && isVar(_b) = False"
    // There seems to be no good answer.  Perhaps gen_Bool is better?  This behavior
    // matches KiCS2.  It seems to instantiate both variables to ().
    #define WHEN_FREE(rhs) if(freelhs) return Cy_false(root);
    #include "normalize2nd.def"
    root->vptr = (freelhs ? rhs : lhs)->vptr->compare;
  }
  
  void CyPrelude_prim_label(node * root)
  {
    node * arg = SUCC_0(root);
    char const * str = arg->vptr->label(arg);
    Cy_CStringToCyString(str, root);
  }

  void CyPrelude_show(node * root)
  {
    #define WHEN_FREE(lhs) { Cy_CStringToCyString(lhs->vptr->label(lhs), root); return; }
    #include "normalize1.def"
    root->vptr = arg->vptr->show;
  }

  // Gives the representation of a char in a double-quoted string.
  void CyPrelude_prim_char_repr(node * root)
  {
    #define TAG(arg) arg->tag
    #define WHEN_FREE(arg) Cy_Suspend()
    #include "normalize1.def"
    char const c = DATA(arg, char);
    // Remove escape for single quote.
    if(c == '\'')
      Cy_CStringToCyString("'", root);
    // Add escape for double quote.
    else if (c == '"')
      Cy_CStringToCyString("\\\"", root);
    // Otherwise, use the same representation as for a single-quoted char
    // (excluding the single quotes).
    else
    {
      // str is the representation as a single-quoted character.
      char const * str = arg->vptr->label(arg);
      str++; // skip the initial single quote.
      size_t len = std::strlen(str);
      Cy_CStringToCyString(str, root, len-1); // skip the trailing single quote.
    }
  }
  
  // ==.IO
  void Cy_IO_Eq(node *) __asm__("CyPrelude_primitive.==.IO");
  void Cy_IO_Eq(node * root)
  {
    // TODO
    CyErr_Undefined("(==)", "IO");
  }

  // =:=.IO
  void Cy_IO_EqColonEq(node *) __asm__("CyPrelude_primitive.=:=.IO");
  void Cy_IO_EqColonEq(node * root)
  {
    // TODO
    CyErr_Undefined("(=:=)", "IO");
  }

  // =:<=.IO
  void Cy_IO_EqColonLtEq(node *) __asm__("CyPrelude_primitive.=:<=.IO");
  void Cy_IO_EqColonLtEq(node * root)
  {
    // TODO
    CyErr_Undefined("(=:<=)", "IO");
  }

  // compare.IO
  void Cy_IO_compare(node *) __asm__("CyPrelude_primitive.compare.IO");
  void Cy_IO_compare(node * root)
  {
    // TODO
    CyErr_Undefined("compare", "IO");
  }

  // show.IO
  void Cy_IO_show(node *) __asm__("CyPrelude_primitive.show.IO");
  void Cy_IO_show(node * root)
  {
    node * io = SUCC_0(root);
    SUCC_0(root) = SUCC_0(io);
    root->vptr = io->vptr->show;
  }
}

namespace
{
  // Implements the lowest-level equality (==, =:=, or =:<=) function for a
  // fundamental data type.
  template<typename T>
  void Cy_Data_Eq(
      node * root
    , vtable * vpos, vtable * vneg
    , tag_t tpos, tag_t tneg
    , char const * typename_
    )
  {
    // FIXME: this can be optimized (maybe?).  The arguments are already
    // head-normalized from another function such as CyPrelude_Eq.
    #define TAG(arg) arg->tag
    #define WHEN_FREE(arg) return CyErr_NoGenerator(typename_);
    #include "normalize2.def"
    bool const result = (DATA(lhs, T) == DATA(rhs, T));
    vtable * const vptr = result ? &CyVt_True : &CyVt_False;
    tag_t const tag = result ? 1 : 0;
    root->vptr = vptr;
    root->tag = tag;
  }
}

extern "C"
{
  // ==.T
  void Cy_Char_Eq(node *) __asm__("CyPrelude_primitive.==.Char");
  void Cy_Char_Eq(node * root)
    { Cy_Data_Eq<char>(root, &CyVt_True, &CyVt_False, 1, 0, "Char"); }

  void Cy_Int_Eq(node *) __asm__("CyPrelude_primitive.==.Int");
  void Cy_Int_Eq(node * root)
    { Cy_Data_Eq<int64_t>(root, &CyVt_True, &CyVt_False, 1, 0, "Int"); }

  void Cy_Float_Eq(node *) __asm__("CyPrelude_primitive.==.Float");
  void Cy_Float_Eq(node * root)
    { Cy_Data_Eq<double>(root, &CyVt_True, &CyVt_False, 1, 0, "Float"); }

  // =:=.T
  void Cy_Char_EqColonEq(node *) __asm__("CyPrelude_primitive.=:=.Char");
  void Cy_Char_EqColonEq(node * root)
    { Cy_Data_Eq<char>(root, &CyVt_Success, &CyVt_Failed, CTOR, FAIL, "Char"); }

  void Cy_Int_EqColonEq(node *) __asm__("CyPrelude_primitive.=:=.Int");
  void Cy_Int_EqColonEq(node * root)
    { Cy_Data_Eq<int64_t>(root, &CyVt_Success, &CyVt_Failed, CTOR, FAIL, "Int"); }

  void Cy_Float_EqColonEq(node *) __asm__("CyPrelude_primitive.=:=.Float");
  void Cy_Float_EqColonEq(node * root)
    { Cy_Data_Eq<double>(root, &CyVt_Success, &CyVt_Failed, CTOR, FAIL, "Float"); }

  // =:<=.T
  void Cy_Char_EqColonLtEq(node *) __asm__("CyPrelude_primitive.=:<=.Char");
  void Cy_Char_EqColonLtEq(node * root)
    { Cy_Data_Eq<char>(root, &CyVt_Success, &CyVt_Failed, CTOR, FAIL, "Char"); }

  void Cy_Int_EqColonLtEq(node *) __asm__("CyPrelude_primitive.=:<=.Int");
  void Cy_Int_EqColonLtEq(node * root)
    { Cy_Data_Eq<int64_t>(root, &CyVt_Success, &CyVt_Failed, CTOR, FAIL, "Int"); }

  void Cy_Float_EqColonLtEq(node *) __asm__("CyPrelude_primitive.=:<=.Float");
  void Cy_Float_EqColonLtEq(node * root)
    { Cy_Data_Eq<double>(root, &CyVt_Success, &CyVt_Failed, CTOR, FAIL, "Float"); }
}

namespace
{
  template<typename T>
  void Cy_Data_compare(node * root, char const * typename_)
  {
    #define TAG(arg) arg->tag
    #define WHEN_FREE(arg) return CyErr_NoGenerator(typename_);
    #include "normalize2.def"
    T const & lhs_data = DATA(lhs, T);
    T const & rhs_data = DATA(rhs, T);
    if(lhs_data < rhs_data)
    {
      root->vptr = &CyVt_LT;
      root->tag = 0;
    }
    else if(rhs_data < lhs_data)
    {
      root->vptr = &CyVt_GT;
      root->tag = 2;
    }
    else
    {
      root->vptr = &CyVt_EQ;
      root->tag = 1;
    }
  }
}

extern "C"
{
  // compare.T
  void Cy_Char_compare(node *) __asm__("CyPrelude_primitive.compare.Char");
  void Cy_Char_compare(node * root)
    { Cy_Data_compare<char>(root, "Char"); }

  void Cy_Int_compare(node *) __asm__("CyPrelude_primitive.compare.Int");
  void Cy_Int_compare(node * root)
    { Cy_Data_compare<int64_t>(root, "Int"); }

  void Cy_Float_compare(node *) __asm__("CyPrelude_primitive.compare.Float");
  void Cy_Float_compare(node * root)
    { Cy_Data_compare<double>(root, "Float"); }

  // show.T
  void Cy_Char_show(node *) __asm__("CyPrelude_primitive.show.Char");
  void Cy_Char_show(node * root) { CyPrelude_prim_label(root); }

  void Cy_Int_show(node *) __asm__("CyPrelude_primitive.show.Int");
  void Cy_Int_show(node * root) { CyPrelude_prim_label(root); }

  void Cy_Float_show(node *) __asm__("CyPrelude_primitive.show.Float");
  void Cy_Float_show(node * root) { CyPrelude_prim_label(root); }
}

