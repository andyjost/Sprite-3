#include "stdio.h"
#include "stdint.h"
#include "stdlib.h"
#include <cassert>
#include "llvm/ADT/SmallVector.h"
#include <algorithm>
#include <list>
#include <iostream>
#include <sstream>
#include <limits>

#define SUCC_0(root) reinterpret_cast<node*&>(root->slot0)
#define SUCC_1(root) reinterpret_cast<node*&>(root->slot1)
#define DATA(root, type) (*reinterpret_cast<type *>(&root->slot0))

namespace sprite { namespace compiler
{
  struct node;
  enum Tag { FAIL= -4, FWD= -3, CHOICE= -2, OPER= -1, CTOR=0 };

  typedef char * labelfun_t(node *);
  typedef uint64_t arityfun_t(node *);
  typedef void stepfun_t(node *);
  typedef void rangefun_t(node *, node ***, node ***);

  struct vtable
  {
    labelfun_t * label;   // Gives the string representation of the label.
    arityfun_t * arity;   // Gives the arity.
    rangefun_t * succ;    // Gives the range containing the successors.
    vtable     * equals;  // Type-specific equality function.
    vtable     * compare; // Type-specific comparison function.
    vtable     * show;    // Type-specific show function.
    stepfun_t  * N;       // Normalization function.
    stepfun_t  * H;       // Head-normalization function.
  };

  struct node
  {
    vtable * vptr; // Pointer to the vtable.
    int32_t tag;   // Node type tag.
    int32_t aux;   // Aux space.
    void * slot0;  // First successor or start of data area.
    void * slot1;  // Second successor.
  };
}}

extern "C"
{
  // These are defined in the "LLVM part" of the runtime.
  extern sprite::compiler::vtable CyVt_Char __asm__(".vt.Char");
  extern sprite::compiler::vtable CyVt_Choice __asm__(".vt.choice");
  extern sprite::compiler::vtable CyVt_Failed __asm__(".vt.failed");
  extern sprite::compiler::vtable CyVt_Fwd __asm__(".vt.fwd");
  extern sprite::compiler::vtable CyVt_Int64 __asm__(".vt.Int64");
  extern sprite::compiler::vtable CyVt_Float __asm__(".vt.Float");
  extern sprite::compiler::vtable CyVt_Success __asm__(".vt.success");
  extern sprite::compiler::vtable CyVt_PartialSpine __asm__(".vt.PartialSpine");
  extern sprite::compiler::vtable CyVt_True __asm__(".vt.CTOR.Prelude.True");
  extern sprite::compiler::vtable CyVt_False __asm__(".vt.CTOR.Prelude.False");
  extern sprite::compiler::vtable CyVt_Cons __asm__(".vt.CTOR.Prelude.:");
  extern sprite::compiler::vtable CyVt_Nil __asm__(".vt.CTOR.Prelude.[]");
  extern sprite::compiler::vtable CyVt_LT __asm__(".vt.CTOR.Prelude.LT");
  extern sprite::compiler::vtable CyVt_EQ __asm__(".vt.CTOR.Prelude.EQ");
  extern sprite::compiler::vtable CyVt_GT __asm__(".vt.CTOR.Prelude.GT");
  extern sprite::compiler::vtable CyVt_Tuple0 __asm__(".vt.CTOR.Prelude.()");
  extern sprite::compiler::vtable CyVt_apply __asm__(".vt.OPER.Prelude.apply");

  int32_t next_choice_id = 0;
}

extern "C"
{
  using namespace sprite::compiler;

  node * Cy_SkipFwd(node * root)
  {
    while(root->vptr == &CyVt_Fwd)
      root = SUCC_0(root);
    return root;
  }

  void CyPrelude_failed(node * root)
  {
    root->vptr = &CyVt_Failed;
    root->tag = FAIL;
    root->slot0 = 0;
    root->slot1 = 0;
  }

  void CyPrelude_success(node * root)
  {
    root->vptr = &CyVt_Success;
    root->tag = CTOR;
    root->slot0 = 0;
    root->slot1 = 0;
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
    #include "normalize1.def"
    putc(DATA(arg, char), stdout);
    root->vptr = &CyVt_Tuple0;
    root->tag = CTOR;
  }

  void CyPrelude_true(node * root)
  {
    root->vptr = &CyVt_True;
    root->tag = 1;
    root->slot0 = 0;
    root->slot1 = 0;
  }

  void CyPrelude_false(node * root)
  {
    root->vptr = &CyVt_False;
    root->tag = 0;
    root->slot0 = 0;
    root->slot1 = 0;
  }

  void CyPrelude_eq(node * root)
  {
    root->vptr = &CyVt_EQ;
    root->tag = 1;
    root->slot0 = 0;
    root->slot1 = 0;
  }

  void Cy_NoAction(node * root) {}

  // Writes a representation of the expression to the given stream.  The
  // representation is not normalized.  If is_outer, then the root is the
  // outermost term and is never parenthesized.
  void Cy_Repr(node * root, FILE * stream, bool is_outer)
  {
    node ** begin, ** end;
    root->vptr->succ(root, &begin, &end);
    size_t const N = end - begin;
    if(!is_outer && N > 0) fputs("(", stream);
    fputs(root->vptr->label(root), stream);
    for(; begin != end; ++begin)
    {
      fputs(" ", stream);
      Cy_Repr(*begin, stream, false);
    }
    if(!is_outer && N > 0) fputs(")", stream);
  }

  // Converts a C-string to a Curry string (list of Char).  Rewrites root to be
  // the Curry string.  Returns the null terminator node of the Curry string.
  node * Cy_CStringToCyString(
      char const * str, node * root
    , size_t max = std::numeric_limits<size_t>::max()
    )
  {
    size_t n = 0;
    while(*str && n++ != max)
    {
      // FIXME: need to alloc with the correct function.
      node * char_data = reinterpret_cast<node*>(malloc(sizeof(node)));
      node * next = reinterpret_cast<node*>(malloc(sizeof(node)));
      char_data->vptr = &CyVt_Char;
      char_data->tag = CTOR;
      DATA(char_data, char) = *str++;

      root->vptr = &CyVt_Cons;
      root->tag = CTOR + 1;
      root->slot0 = char_data;
      root->slot1 = next;

      root = next;
    }
    root->vptr = &CyVt_Nil;
    root->tag = CTOR;
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

  void Cy_Normalize(node * root)
    { root->vptr->N(root); }

  FILE * Cy_stdin() { return stdin; }
  FILE * Cy_stdout() { return stdout; }
  FILE * Cy_stderr() { return stderr; }

  // Evaluates an expression.  Calls yield(x) for each result x.
  void Cy_Eval(node * root, void(*yield)(node * root))
  {
    std::list<node *> computation;
    computation.push_back(root);
    while(!computation.empty())
    {
      node * expr = computation.front();
      Cy_Normalize(expr);
      redo: switch(expr->tag)
      {
        case FAIL:
          computation.pop_front();
          break;
        case FWD:
          computation.front() = expr = SUCC_0(expr);
          goto redo;
        case CHOICE:
        {
          computation.push_back(SUCC_0(expr));
          computation.splice(computation.end(), computation, computation.begin());
          computation.back() = SUCC_1(expr);
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
    #include "normalize1.def"
    // TODO: suspend.
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
    // FIXME: this is incorrect -- detect variables.
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
    #include "normalize1.def"
    root->vptr = &CyVt_Char;
    root->tag = CTOR;
    DATA(root, char) = static_cast<char>(DATA(arg, int64_t));
    root->slot1 = 0;
  }

  void CyPrelude_apply(node * root)
  {
    #include "normalize_apply.def"
    int32_t const rem = arg->aux;
    // Handle complete calls.  There is one argument, i.e., this one,
    // remaining.
    if(rem == 1)
    {
      int32_t const n = arg->tag;
      switch(n)
      {
        case 0:
          assert(0 && "applying argument to nullary function");
          break;
        case 1:
          root->vptr = DATA(arg, vtable*);
          root->tag = OPER;
          root->slot0 = root->slot1;
          root->slot1 = 0;
          break;
        case 2:
          root->vptr = DATA(SUCC_0(arg), vtable*);
          root->tag = OPER;
          root->slot0 = arg->slot1;
          // The second argument is already in the proper place.
          break;
        default:
        {
          // FIXME: need to alloc with the correct function.
          node ** args = reinterpret_cast<node**>(malloc(sizeof(node*) * n));
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

  // // Evaluates the first argument to head normal form (which could also be a free
  // // variable) and returns the second argument.
  // void CyPrelude_seq(node * root)
  // {
  //   node * arg0 = SUCC_0(root);
  //   arg0->vptr->H(arg0);
  //   node * arg1 = SUCC_1(root);
  //   root->vptr = &CyVt_Fwd;
  //   root->tag = FWD;
  //   root->slot0 = arg1;
  //   root->slot1 = 0;
  // }

  void CyPrelude_Eq(node *) __asm__("CyPrelude_==");
  void CyPrelude_Eq(node * root)
    { root->vptr = SUCC_0(root)->vptr->equals; }

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
  // GHC
  void CyPrelude_prim_char_repr(node * root)
  {
    #define TAG(arg) arg->tag
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

  // ==.fwd
  void CyPrelude_FwdEq(node *) __asm__("CyPrelude_primitive.==.fwd");
  void CyPrelude_FwdEq(node * root)
  {
    node *& arg0 = SUCC_0(root);
    arg0 = DATA(arg0, node *);
    root->vptr = arg0->vptr->equals;
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

  // ==.choice
  void Cy_Choice_Eq(node *) __asm__("CyPrelude_primitive.==.choice");
  void Cy_Choice_Eq(node * root)
  {
    node * arg0 = SUCC_0(root);
    node * lhs_choice, * rhs_choice;
    // FIXME: need to alloc with the correct function.
    lhs_choice = reinterpret_cast<node*>(malloc(sizeof(node)));
    rhs_choice = reinterpret_cast<node*>(malloc(sizeof(node)));
    lhs_choice->vptr = rhs_choice->vptr = SUCC_0(arg0)->vptr->equals;
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

  // compare.choice
  void Cy_Choice_compare(node *) __asm__("CyPrelude_primitive.compare.choice");
  void Cy_Choice_compare(node * root)
  {
    node * arg0 = SUCC_0(root);
    node * lhs_choice, * rhs_choice;
    // FIXME: need to alloc with the correct function.
    lhs_choice = reinterpret_cast<node*>(malloc(sizeof(node)));
    rhs_choice = reinterpret_cast<node*>(malloc(sizeof(node)));
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
    // FIXME: need to alloc with the correct function.
    lhs_choice = reinterpret_cast<node*>(malloc(sizeof(node)));
    rhs_choice = reinterpret_cast<node*>(malloc(sizeof(node)));
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
    root->vptr = arg0->vptr->equals;
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
    CyPrelude_true(root);
  }

  // compare.Success
  void Cy_Success_compare(node *) __asm__("CyPrelude_primitive.compare.Success");
  void Cy_Success_compare(node * root)
  {
    CyPrelude_eq(root);
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
    #include "normalize2.def"
    bool const result = (DATA(lhs, char) == DATA(rhs, char));
    vtable * const vptr = result ? &CyVt_True : &CyVt_False;
    int64_t const tag = result ? 1 : 0;
    root->vptr = vptr;
    root->tag = tag;
  }

  void Cy_Int_Eq(node *) __asm__("CyPrelude_primitive.==.Int");
  void Cy_Int_Eq(node * root)
  {
    #define TAG(arg) arg->tag
    #include "normalize2.def"
    bool const result = (DATA(lhs, int64_t) == DATA(rhs, int64_t));
    vtable * const vptr = result ? &CyVt_True : &CyVt_False;
    int64_t const tag = result ? 1 : 0;
    root->vptr = vptr;
    root->tag = tag;
  }

  void Cy_Float_Eq(node *) __asm__("CyPrelude_primitive.==.Float");
  void Cy_Float_Eq(node * root)
  {
    #define TAG(arg) arg->tag
    #include "normalize2.def"
    bool const result = (DATA(lhs, double) == DATA(rhs, double));
    vtable * const vptr = result ? &CyVt_True : &CyVt_False;
    int64_t const tag = result ? 1 : 0;
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

