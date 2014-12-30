#include "stdio.h"
#include "stdint.h"
#include "stdlib.h"
#include <cassert>
#include "llvm/ADT/SmallVector.h"
#include <list>
#include <iostream>

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
    labelfun_t * label; // Gives the string repr.
    arityfun_t * arity; // Gives the arity.
    rangefun_t * succ;  // Gives the range containing the successors.
    vtable * equality;  // Type-specific equality function.
    stepfun_t * N;      // Normalization function.
    stepfun_t * H;      // Head-normalization function.
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
  extern sprite::compiler::vtable _vt_Char __asm__(".vt.Char");
  extern sprite::compiler::vtable _vt_choice __asm__(".vt.choice");
  extern sprite::compiler::vtable _vt_failed __asm__(".vt.failed");
  extern sprite::compiler::vtable _vt_fwd __asm__(".vt.fwd");
  extern sprite::compiler::vtable _vt_Int64 __asm__(".vt.Int64");
  extern sprite::compiler::vtable _vt_success __asm__(".vt.success");
  extern sprite::compiler::vtable _vt_PartialSpine __asm__(".vt.PartialSpine");
  extern sprite::compiler::vtable _vt_CTOR_Prelude_True __asm__(".vt.CTOR.Prelude.True");
  extern sprite::compiler::vtable _vt_CTOR_Prelude_False __asm__(".vt.CTOR.Prelude.False");

  int32_t next_choice_id = 0;
}

namespace sprite { namespace compiler
{
  void _printexpr(node * root, bool is_outer)
  {
    node ** begin, ** end;
    root->vptr->succ(root, &begin, &end);
    size_t const N = end - begin;
    if(!is_outer && N > 0) fputs("(", stdout);
    fputs(root->vptr->label(root), stdout);
    for(; begin != end; ++begin)
    {
      fputs(" ", stdout);
      _printexpr(*begin, false);
    }
    if(!is_outer && N > 0) fputs(")", stdout);
  }
}}

extern "C"
{
  using namespace sprite::compiler;

  void failed(node * root)
  {
    root->vptr = &_vt_failed;
    root->tag = FAIL;
    root->slot0 = 0;
    root->slot1 = 0;
  }

  void success(node * root)
  {
    root->vptr = &_vt_success;
    root->tag = CTOR;
    root->slot0 = 0;
    root->slot1 = 0;
  }

  void true_(node * root)
  {
    root->vptr = &_vt_CTOR_Prelude_True;
    root->tag = 1;
    root->slot0 = 0;
    root->slot1 = 0;
  }

  void false_(node * root)
  {
    root->vptr = &_vt_CTOR_Prelude_False;
    root->tag = 0;
    root->slot0 = 0;
    root->slot1 = 0;
  }

  void sprite_printexpr(node * root, char const * suffix)
  {
    _printexpr(root, true);
    if(suffix) fputs(suffix, stdout);
    fflush(0);
  }

  void sprite_normalize(node * root)
    { root->vptr->N(root); }

  void sprite_evaluate(node * root)
  {
    std::list<node *> computation;
    computation.push_back(root);
    while(!computation.empty())
    {
      node * expr = computation.front();
      sprite_normalize(expr);
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
          sprite_printexpr(expr, "\n");
          computation.pop_front();
      }
    }
  }

  void Int_plus(node *) __asm__("+"); // Force the symbol name.
  void Int_plus(node * root)
  {
    #include "normalize2.def"
    int64_t const x = DATA(lhs, int64_t);
    int64_t const y = DATA(rhs, int64_t);
    root->vptr = &_vt_Int64;
    root->tag = CTOR;
    DATA(root, int64_t) = x + y;
  }

  void Int_minus(node *) __asm__("-"); // Force the symbol name.
  void Int_minus(node * root)
  {
    #include "normalize2.def"
    int64_t const x = DATA(lhs, int64_t);
    int64_t const y = DATA(rhs, int64_t);
    root->vptr = &_vt_Int64;
    root->tag = CTOR;
    DATA(root, int64_t) = x - y;
  }

  void Int_times(node *) __asm__("*"); // Force the symbol name.
  void Int_times(node * root)
  {
    #include "normalize2.def"
    int64_t const x = DATA(lhs, int64_t);
    int64_t const y = DATA(rhs, int64_t);
    root->vptr = &_vt_Int64;
    root->tag = CTOR;
    DATA(root, int64_t) = x * y;
  }

  // Converts a character into its ASCII value.
  void ord(node * root)
  {
    #include "normalize1.def"
    root->vptr = &_vt_Int64;
    root->tag = CTOR;
    DATA(root, int64_t) = DATA(arg, char);
    root->slot1 = 0;
  }

  // Converts an ASCII value into a character.
  void chr(node * root)
  {
    #include "normalize1.def"
    root->vptr = &_vt_Char;
    root->tag = CTOR;
    DATA(root, char) = static_cast<char>(DATA(arg, int64_t));
    root->slot1 = 0;
  }

  void apply(node * root)
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
          root->vptr = SUCC_0(arg)->vptr;
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
      root->vptr = &_vt_PartialSpine;
      root->tag = arg->tag;
      root->aux = rem - 1;
      // The successors do not need to be modified.
    }
  }

  // // Evaluates the first argument to head normal form (which could also be a free
  // // variable) and returns the second argument.
  // void seq(node * root)
  // {
  //   node * arg0 = SUCC_0(root);
  //   arg0->vptr->H(arg0);
  //   node * arg1 = SUCC_1(root);
  //   root->vptr = &_vt_fwd;
  //   root->tag = FWD;
  //   root->slot0 = arg1;
  //   root->slot1 = 0;
  // }

  // // Evaluates the argument to head normal form and returns it.  Suspends until
  // // the result is bound to a non-variable term.
  // void ensureNotFree(node * root)
  // {
  //   node * arg0 = SUCC_0(root);
  //   arg0->vptr->H(arg0);
  //   // TODO: suspend.
  //   root->vptr = &_vt_fwd;
  //   root->tag = FWD;
  // }

  void boolequal(node *) __asm__("==");
  void boolequal(node * root)
    { root->vptr = SUCC_0(root)->vptr->equality; }

  // ==.fwd
  void prim_equals_fwd(node *) __asm__("primitive.==.fwd");
  void prim_equals_fwd(node * root)
  {
    node *& arg0 = SUCC_0(root);
    arg0 = DATA(arg0, node *);
    root->vptr = arg0->vptr->equality;
  }

  // ==.choice
  void prim_equals_choice(node *) __asm__("primitive.==.choice");
  void prim_equals_choice(node * root)
  {
    node * arg0 = SUCC_0(root);
    node * lhs_choice, * rhs_choice;
    // FIXME: need to alloc with the correct function.
    lhs_choice = reinterpret_cast<node*>(malloc(sizeof(node)));
    rhs_choice = reinterpret_cast<node*>(malloc(sizeof(node)));
    lhs_choice->vptr = rhs_choice->vptr = SUCC_0(arg0)->vptr->equality;
    lhs_choice->tag = rhs_choice->tag = OPER;
    lhs_choice->slot0 = arg0->slot0;
    rhs_choice->slot0 = arg0->slot1;
    lhs_choice->slot1 = rhs_choice->slot1 = root->slot1;
    root->vptr = &_vt_choice;
    root->tag = CHOICE;
    root->aux = arg0->aux;
    root->slot0 = lhs_choice;
    root->slot1 = rhs_choice;
  }

  // ==.oper
  void prim_equals_oper(node *) __asm__("primitive.==.oper");
  void prim_equals_oper(node * root)
  {
    node * arg0 = SUCC_0(root);
    arg0->vptr->H(arg0);
    root->vptr = arg0->vptr->equality;
  }

  // ==.Success
  void prim_equals_Success(node *) __asm__("primitive.==.Success");
  void prim_equals_Success(node * root)
  {
    true_(root);
  }

  // ==.IO
  void prim_equals_IO(node *) __asm__("primitive.==.IO");
  void prim_equals_IO(node * root)
  {
    // TODO
    printf("Equality for IO is not implemented");
    std::exit(1);
  }

  // ==.T
  // Creates the lowest-level function implementing equality for a fundamental
  // type.  The successors are guaranteed to be normalized.
  #define DECLARE_PRIMITIVE_EQUALITY(curry_typename, c_type)            \
      void prim_equals_ ## curry_typename(node *)                       \
          __asm__("primitive.==." # curry_typename);                    \
      void prim_equals_ ## curry_typename(node * root)                  \
      {                                                                 \
        node * arg0 = SUCC_0(root);                                     \
        node * arg1 = SUCC_1(root);                                     \
        bool const result = (DATA(arg0, c_type) == DATA(arg1, c_type)); \
        vtable * const vptr = result                                    \
            ? &_vt_CTOR_Prelude_True : &_vt_CTOR_Prelude_False;         \
        int64_t const tag = result ? 1 : 0;                             \
        root->vptr = vptr;                                              \
        root->tag = tag;                                                \
      }                                                                 \
    /**/
  DECLARE_PRIMITIVE_EQUALITY(Char, char);
  DECLARE_PRIMITIVE_EQUALITY(Int, int64_t);
  DECLARE_PRIMITIVE_EQUALITY(Float, double);
}

