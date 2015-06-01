#include "basic_runtime.hpp"
#include <algorithm>
#include <cassert>
#include <iostream>
#include <limits>
#include <list>
#include "llvm/ADT/SmallVector.h"
#include <unordered_set>
#include <sstream>
#include <stack>
#include "stdio.h"
#include "stdlib.h"
#include "cymemory.hpp"
#include "fingerprint.hpp"

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
  using namespace sprite::compiler;

  // These are defined in the "LLVM part" of the runtime.
  extern vtable CyVt_Char __asm__(".vt.Char");
  extern vtable CyVt_Choice __asm__(".vt.choice");
  extern vtable CyVt_Failed __asm__(".vt.failed");
  extern vtable CyVt_Fwd __asm__(".vt.fwd");
  extern vtable CyVt_Int64 __asm__(".vt.Int64");
  extern vtable CyVt_Float __asm__(".vt.Float");
  extern vtable CyVt_Success __asm__(".vt.success");
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
    { Cy_ArrayPool[n].free(px); }

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
    // I'm not sure how to implement this.  For now, it's a placeholder
    // that exits the program.
    printf("[Fixme] Goal suspended!");
    std::exit(EXIT_FAILURE);
  }

  void CyPrelude_suspend(node * root)
    { Cy_Suspend(); }

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

  struct Cy_EvalFrame
  {
    node * expr;
    std::shared_ptr<Fingerprint> fingerprint;

    Cy_EvalFrame(node * e, std::shared_ptr<Fingerprint> const & f)
      : expr(e), fingerprint(f)
    {}
  };

  // Evaluates an expression.  Calls yield(x) for each result x.
  void Cy_Eval(node * root, void(*yield)(node * root))
  {
    CyMem_PushRoot(root);
    std::list<Cy_EvalFrame> computation = {
        {root, std::shared_ptr<Fingerprint>(new Fingerprint())}
      };
    while(!computation.empty())
    {
      Cy_EvalFrame & frame = computation.front();
      node * expr = frame.expr;
      expr->vptr->N(expr);
      redo: switch(expr->tag)
      {
        case FAIL:
          computation.pop_front();
          break;
        case FWD:
          computation.front().expr = expr = SUCC_0(expr);
          goto redo;
        case CHOICE:
        {
          aux_t const id = expr->aux;
          switch(frame.fingerprint->test(id))
          {
            case ChoiceState::LEFT:
              // Discard right expression.
              computation.front().expr = SUCC_0(expr);
              break;
            case ChoiceState::RIGHT:
              // Discard left expression.
              computation.front().expr = SUCC_1(expr);
              break;
            case ChoiceState::UNDETERMINED:
            {
              // Discard no expression.
              auto left = frame.fingerprint->clone();
              left->set_left(id);
              computation.emplace_back(SUCC_0(expr), left);
              frame.expr = SUCC_1(expr);
              frame.fingerprint->set_right(id);
              break;
            }
          }
          // Rotate the front frame to the back.
          computation.splice(computation.end(), computation, computation.begin());
          break;
        }
        case OPER:
          fprintf(stderr, "Normalized expression yields a function!  Discarding.\n");
          computation.pop_front();
          break;
        default:
          yield(expr);
          computation.pop_front();
      }
    }
    CyMem_PopRoot();
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
    #include "normalize1.def" // head-normalizes by default.
    root->vptr = &CyVt_Fwd;
    root->tag = FWD;
  }

  // Right-associative application with strict evaluation of its argument
  // to normal form.
  void CyPrelude_DollarBangBang(node *) __asm__("CyPrelude_$!!");
  void CyPrelude_DollarBangBang(node * root)
  {
    #define NORMALIZE(arg) arg->vptr->N(arg)
    #include "normalize1.def"
    root->vptr = &CyVt_Fwd;
    root->tag = FWD;
  }

  // Right-associative application with strict evaluation of its argument
  // to ground normal form.
  void CyPrelude_DollarHashHash(node *) __asm__("CyPrelude_$##");
  void CyPrelude_DollarHashHash(node * root)
  {
    #define NORMALIZE(arg) arg->vptr->N(arg)
    #define WHEN_FREE(arg) Cy_Suspend()
    #include "normalize1.def"
    root->vptr = &CyVt_Fwd;
    root->tag = FWD;
  }

  void CyPrelude_error(node * root)
  {
    node * arg = SUCC_0(root);
    CyPrelude_DollarHashHash(arg);
    Cy_FPrint(arg, stderr);
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
          root->tag = OPER;
          root->slot0 = root->slot1;
          break;
        case 2:
          root->vptr = DATA(SUCC_0(arg), vtable*);
          root->tag = OPER;
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
          root->tag = OPER;
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
    // FIXME: normalize sequentially. if LHS is a var, then try the RHS.
    #define WHEN_FREE(arg) Cy_Suspend()
    #include "normalize2.def"
    root->vptr = &CyVt_Success;
    root->tag = CTOR;
  }

  void CyPrelude_Eq(node *) __asm__("CyPrelude_==");
  void CyPrelude_Eq(node * root)
    { root->vptr = SUCC_0(root)->vptr->equal; }

  void CyPrelude_EqColonEq(node *) __asm__("CyPrelude_=:=");
  void CyPrelude_EqColonEq(node * root)
    { root->vptr = SUCC_0(root)->vptr->equate; }

  void CyPrelude_compare(node * root)
    { root->vptr = SUCC_0(root)->vptr->compare; }

  void CyPrelude_show(node * root)
    { root->vptr = SUCC_0(root)->vptr->show; }

  void CyPrelude_prim_label(node * root)
  {
    node * arg = SUCC_0(root);
    char const * str = arg->vptr->label(arg);
    Cy_CStringToCyString(str, root);
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
  
  static size_t CyFree_NextId = 0;
  static std::unordered_set<node*> CyFree_Seen;

  void CyFree_ResetCounter()
  {
    CyFree_NextId = 0;
    CyFree_Seen.clear();
  }

  // ==.freevar
  void Cy_Free_Eq(node *) __asm__("CyPrelude_primitive.==.freevar");
  void Cy_Free_Eq(node * root)
  {
    // "_a == _b | isVar(_a) && isVar(_b) = True"
    #define WHEN_FREE(arg) return Cy_true(root);
    #include "normalize2nd.def"
    root->vptr = arg->vptr->equal;
  }

  // =:=.freevar
  void Cy_Free_EqColonEq(node *) __asm__("CyPrelude_primitive.=:=.freevar");
  void Cy_Free_EqColonEq(node * root)
  {
    // "_a == _b | isVar(_a) && isVar(_b) = Success"
    #define WHEN_FREE(arg) return Cy_success(root);
    #include "normalize2nd.def"
    root->vptr = arg->vptr->equate;
  }

  // compare.freevar
  void Cy_Free_compare(node *) __asm__("CyPrelude_primitive.compare.freevar");
  void Cy_Free_compare(node * root)
  {
    // By convention:
    //    "_a < _b | isVar(_a) && isVar(_b) = False"
    // There seems to be no good answer.  Perhaps gen_Bool is better?  This behavior
    // matches KiCS2.  It seems to instantiate both variables to ().
    #define WHEN_FREE(arg) return Cy_false(root);
    #include "normalize2nd.def"
    root->vptr = arg->vptr->compare;
  }

  // show.freevar
  void Cy_Free_show(node *) __asm__("CyPrelude_primitive.show.freevar");
  void Cy_Free_show(node * root)
  {
    node * arg = SUCC_0(root);
    if(CyFree_Seen.insert(arg).second)
    {
      // If unseen, put the string into the free variable's data region.
      char * p = &DATA(arg, char);
      *p++ = '_';
      size_t id = CyFree_NextId++;
      if(id<26)
      {
        *p++ = 'a' + id;
        *p = '\0';
      }
      else
      {
        *p++ = 'a';
        size_t constexpr maxsize = sizeof(arg->slot0) + sizeof(arg->slot1) - 2;
        snprintf(p, maxsize, "%zu", (id-25));
      }
    }
    Cy_CStringToCyString(&DATA(arg, char), root);
  }

  // ==.fwd
  void CyPrelude_FwdEq(node *) __asm__("CyPrelude_primitive.==.fwd");
  void CyPrelude_FwdEq(node * root)
  {
    node *& arg0 = SUCC_0(root);
    arg0 = DATA(arg0, node *);
    root->vptr = arg0->vptr->equal;
  }

  // =:=.fwd
  void CyPrelude_FwdEqColonEq(node *) __asm__("CyPrelude_primitive.=:=.fwd");
  void CyPrelude_FwdEqColonEq(node * root)
  {
    node *& arg0 = SUCC_0(root);
    arg0 = DATA(arg0, node *);
    root->vptr = arg0->vptr->equate;
  }

  // compare.fwd
  void Cy_Fwd_compare(node *) __asm__("CyPrelude_primitive.compare.fwd");
  void Cy_Fwd_compare(node * root)
  {
    node *& arg0 = SUCC_0(root);
    arg0 = DATA(arg0, node *);
    root->vptr = arg0->vptr->compare;
  }

  // show.fwd
  void Cy_Fwd_show(node *) __asm__("CyPrelude_primitive.show.fwd");
  void Cy_Fwd_show(node * root)
  {
    node *& arg0 = SUCC_0(root);
    arg0 = DATA(arg0, node *);
    root->vptr = arg0->vptr->show;
  }

  void _Cy_Choice_Eq_or_EqColonEq(node * root, size_t offset)
  {
    node * arg0 = SUCC_0(root);
    node * lhs_choice, * rhs_choice;
  redo:
    NODE_ALLOC(lhs_choice, redo);
    NODE_ALLOC(rhs_choice, redo);
    lhs_choice->vptr = rhs_choice->vptr = *reinterpret_cast<vtable**>(
        reinterpret_cast<char*>(SUCC_0(arg0)->vptr) + offset
      );
    lhs_choice->tag = rhs_choice->tag = OPER;
    lhs_choice->slot0 = arg0->slot0;
    rhs_choice->slot0 = arg0->slot1;
    lhs_choice->slot1 = rhs_choice->slot1 = root->slot1;
    root->vptr = &CyVt_Choice;
    root->tag = CHOICE;
    root->aux = arg0->aux;
    root->slot0 = lhs_choice;
    root->slot1 = rhs_choice;
  }

  // ==.choice
  void Cy_Choice_Eq(node *) __asm__("CyPrelude_primitive.==.choice");
  void Cy_Choice_Eq(node * root)
  {
    _Cy_Choice_Eq_or_EqColonEq(root, offsetof(vtable, equal));
  }

  // =:=.choice
  void Cy_Choice_EqColonEq(node *) __asm__("CyPrelude_primitive.=:=.choice");
  void Cy_Choice_EqColonEq(node * root)
  {
    _Cy_Choice_Eq_or_EqColonEq(root, offsetof(vtable, equate));
  }

  // compare.choice
  void Cy_Choice_compare(node *) __asm__("CyPrelude_primitive.compare.choice");
  void Cy_Choice_compare(node * root)
  {
    node * arg0 = SUCC_0(root);
    node * lhs_choice, * rhs_choice;
  redo:
    NODE_ALLOC(lhs_choice, redo);
    NODE_ALLOC(rhs_choice, redo);
    lhs_choice->vptr = rhs_choice->vptr = SUCC_0(arg0)->vptr->compare;
    lhs_choice->tag = rhs_choice->tag = OPER;
    lhs_choice->slot0 = arg0->slot0;
    rhs_choice->slot0 = arg0->slot1;
    lhs_choice->slot1 = rhs_choice->slot1 = root->slot1;
    root->vptr = &CyVt_Choice;
    root->tag = CHOICE;
    root->aux = arg0->aux;
    root->slot0 = lhs_choice;
    root->slot1 = rhs_choice;
  }

  // show.choice
  void Cy_Choice_show(node *) __asm__("CyPrelude_primitive.show.choice");
  void Cy_Choice_show(node * root)
  {
    node * arg0 = SUCC_0(root);
    node * lhs_choice, * rhs_choice;
  redo:
    NODE_ALLOC(lhs_choice, redo);
    NODE_ALLOC(rhs_choice, redo);
    lhs_choice->vptr = rhs_choice->vptr = SUCC_0(arg0)->vptr->show;
    lhs_choice->tag = rhs_choice->tag = OPER;
    lhs_choice->slot0 = arg0->slot0;
    rhs_choice->slot0 = arg0->slot1;
    lhs_choice->slot1 = rhs_choice->slot1 = root->slot1;
    root->vptr = &CyVt_Choice;
    root->tag = CHOICE;
    root->aux = arg0->aux;
    root->slot0 = lhs_choice;
    root->slot1 = rhs_choice;
  }

  // ==.oper
  void Cy_Oper_Eq(node *) __asm__("CyPrelude_primitive.==.oper");
  void Cy_Oper_Eq(node * root)
  {
    node * arg0 = SUCC_0(root);
    arg0->vptr->H(arg0);
    root->vptr = arg0->vptr->equal;
  }

  // =:=.oper
  void Cy_Oper_EqColonEq(node *) __asm__("CyPrelude_primitive.=:=.oper");
  void Cy_Oper_EqColonEq(node * root)
  {
    node * arg0 = SUCC_0(root);
    arg0->vptr->H(arg0);
    root->vptr = arg0->vptr->equate;
  }

  // compare.oper
  void Cy_Oper_compare(node *) __asm__("CyPrelude_primitive.compare.oper");
  void Cy_Oper_compare(node * root)
  {
    node * arg0 = SUCC_0(root);
    arg0->vptr->H(arg0);
    root->vptr = arg0->vptr->compare;
  }

  // show.oper
  void Cy_Oper_show(node *) __asm__("CyPrelude_primitive.show.oper");
  void Cy_Oper_show(node * root)
  {
    node * arg0 = SUCC_0(root);
    arg0->vptr->H(arg0);
    root->vptr = arg0->vptr->show;
  }

  // ==.Success
  void Cy_Success_Eq(node *) __asm__("CyPrelude_primitive.==.Success");
  void Cy_Success_Eq(node * root)
  {
    Cy_true(root);
  }

  // =:=.Success
  void Cy_Success_EqColonEq(node *) __asm__("CyPrelude_primitive.=:=.Success");
  void Cy_Success_EqColonEq(node * root)
  {
    Cy_success(root);
  }

  // compare.Success
  void Cy_Success_compare(node *) __asm__("CyPrelude_primitive.compare.Success");
  void Cy_Success_compare(node * root)
  {
    Cy_eq(root);
  }

  // show.Success
  void Cy_Success_show(node *) __asm__("CyPrelude_primitive.show.Success");
  void Cy_Success_show(node * root)
  {
    // TODO
    printf("Showing for success is not implemented");
    std::exit(1);
  }

  // ==.IO
  void Cy_IO_Eq(node *) __asm__("CyPrelude_primitive.==.IO");
  void Cy_IO_Eq(node * root)
  {
    // TODO
    printf("Equality for IO is not implemented");
    std::exit(1);
  }

  // ==.IO
  void Cy_IO_EqColonEq(node *) __asm__("CyPrelude_primitive.=:=.IO");
  void Cy_IO_EqColonEq(node * root)
  {
    // TODO
    printf("The equational constraint for IO is not implemented");
    std::exit(1);
  }


  // compare.IO
  void Cy_IO_compare(node *) __asm__("CyPrelude_primitive.compare.IO");
  void Cy_IO_compare(node * root)
  {
    // TODO
    printf("Comparison for IO is not implemented");
    std::exit(1);
  }

  // show.IO
  void Cy_IO_show(node *) __asm__("CyPrelude_primitive.show.IO");
  void Cy_IO_show(node * root)
  {
    // TODO
    printf("Show for IO is not implemented");
    std::exit(1);
  }

  // ==.T
  // Creates the lowest-level function implementing equality for a fundamental
  // type.

  void Cy_Char_Eq(node *) __asm__("CyPrelude_primitive.==.Char");
  void Cy_Char_Eq(node * root)
  {
    #define TAG(arg) arg->tag
    #define WHEN_FREE(arg) Cy_Suspend()
    #include "normalize2.def"
    bool const result = (DATA(lhs, char) == DATA(rhs, char));
    vtable * const vptr = result ? &CyVt_True : &CyVt_False;
    tag_t const tag = result ? 1 : 0;
    root->vptr = vptr;
    root->tag = tag;
  }

  void Cy_Int_Eq(node *) __asm__("CyPrelude_primitive.==.Int");
  void Cy_Int_Eq(node * root)
  {
    #define TAG(arg) arg->tag
    #define WHEN_FREE(arg) Cy_Suspend()
    #include "normalize2.def"
    bool const result = (DATA(lhs, int64_t) == DATA(rhs, int64_t));
    vtable * const vptr = result ? &CyVt_True : &CyVt_False;
    tag_t const tag = result ? 1 : 0;
    root->vptr = vptr;
    root->tag = tag;
  }

  void Cy_Float_Eq(node *) __asm__("CyPrelude_primitive.==.Float");
  void Cy_Float_Eq(node * root)
  {
    #define TAG(arg) arg->tag
    #define WHEN_FREE(arg) Cy_Suspend()
    #include "normalize2.def"
    bool const result = (DATA(lhs, double) == DATA(rhs, double));
    vtable * const vptr = result ? &CyVt_True : &CyVt_False;
    tag_t const tag = result ? 1 : 0;
    root->vptr = vptr;
    root->tag = tag;
  }

  // =:=.T
  // Creates the lowest-level function implementing the equational constraint
  // for a fundamental type.

  void Cy_Char_EqColonEq(node *) __asm__("CyPrelude_primitive.=:=.Char");
  void Cy_Char_EqColonEq(node * root)
  {
    #define TAG(arg) arg->tag
    #define WHEN_FREE(arg) fprintf(stderr, "No generator for type Char"); Cy_Suspend()
    #include "normalize2.def"
    bool const result = (DATA(lhs, char) == DATA(rhs, char));
    vtable * const vptr = result ? &CyVt_Success : &CyVt_Failed;
    tag_t const tag = result ? CTOR : FAIL;
    root->vptr = vptr;
    root->tag = tag;
  }

  void Cy_Int_EqColonEq(node *) __asm__("CyPrelude_primitive.=:=.Int");
  void Cy_Int_EqColonEq(node * root)
  {
    #define TAG(arg) arg->tag
    #define WHEN_FREE(arg) fprintf(stderr, "No generator for type Int"); Cy_Suspend()
    #include "normalize2.def"
    bool const result = (DATA(lhs, int64_t) == DATA(rhs, int64_t));
    vtable * const vptr = result ? &CyVt_Success : &CyVt_Failed;
    tag_t const tag = result ? CTOR : FAIL;
    root->vptr = vptr;
    root->tag = tag;
  }

  void Cy_Float_EqColonEq(node *) __asm__("CyPrelude_primitive.=:=.Float");
  void Cy_Float_EqColonEq(node * root)
  {
    #define TAG(arg) arg->tag
    #define WHEN_FREE(arg) fprintf(stderr, "No generator for type Float"); Cy_Suspend()
    #include "normalize2.def"
    bool const result = (DATA(lhs, double) == DATA(rhs, double));
    vtable * const vptr = result ? &CyVt_Success : &CyVt_Failed;
    tag_t const tag = result ? CTOR : FAIL;
    root->vptr = vptr;
    root->tag = tag;
  }

  // compare.T
  // Creates the lowest-level function implementing comparison for a
  // fundamental type.
  void Cy_Char_compare(node *) __asm__("CyPrelude_primitive.compare.Char");
  void Cy_Char_compare(node * root)
  {
    #define TAG(arg) arg->tag
    #define WHEN_FREE(arg) Cy_Suspend()
    #include "normalize2.def"
    char const lhs_data = DATA(lhs, char);
    char const rhs_data = DATA(rhs, char);
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

  void Cy_Int_compare(node *) __asm__("CyPrelude_primitive.compare.Int");
  void Cy_Int_compare(node * root)
  {
    #define TAG(arg) arg->tag
    #define WHEN_FREE(arg) Cy_Suspend()
    #include "normalize2.def"
    int64_t const lhs_data = DATA(lhs, int64_t);
    int64_t const rhs_data = DATA(rhs, int64_t);
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

  void Cy_Float_compare(node *) __asm__("CyPrelude_primitive.compare.Float");
  void Cy_Float_compare(node * root)
  {
    #define TAG(arg) arg->tag
    #define WHEN_FREE(arg) Cy_Suspend()
    #include "normalize2.def"
    double const lhs_data = DATA(lhs, double);
    double const rhs_data = DATA(rhs, double);
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

  // show.T
  // Creates the lowest-level function implementing comparison for a
  // fundamental type.  The successors are guaranteed to be normalized.
  void Cy_Char_show(node *) __asm__("CyPrelude_primitive.show.Char");
  void Cy_Char_show(node * root) { CyPrelude_prim_label(root); }

  void Cy_Int_show(node *) __asm__("CyPrelude_primitive.show.Int");
  void Cy_Int_show(node * root) { CyPrelude_prim_label(root); }

  void Cy_Float_show(node *) __asm__("CyPrelude_primitive.show.Float");
  void Cy_Float_show(node * root) { CyPrelude_prim_label(root); }
}

