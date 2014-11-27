#include "stdio.h"
#include "stdint.h"
#include "stdlib.h"
#include <cassert>

#define SUCC_0(root) reinterpret_cast<sprite::compiler::node*&>(root->slot0)
#define SUCC_1(root) reinterpret_cast<sprite::compiler::node*&>(root->slot1)
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
    labelfun_t * label;
    arityfun_t * arity;
    rangefun_t * succ;
    stepfun_t * N;
    stepfun_t * H;
  };

  struct node
  {
    vtable * vptr;
    int64_t tag;
    void * slot0;
    void * slot1;
  };
}}

extern "C"
{
  // These are defined in the "LLVM part" of the runtime.
  extern sprite::compiler::vtable _vt_Char;
  extern sprite::compiler::vtable _vt_failed;
  extern sprite::compiler::vtable _vt_fwd;
  extern sprite::compiler::vtable _vt_Int64;
  extern sprite::compiler::vtable _vt_success;
  extern sprite::compiler::vtable _vt_PartialSpine;
  extern sprite::compiler::vtable _vt_CTOR_Prelude_True;
  extern sprite::compiler::vtable _vt_CTOR_Prelude_False;
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

  enum EqualityResult { EQ_TRUE, EQ_FALSE, EQ_FAILED };

  EqualityResult _boolequal_impl(node * lhs, node * rhs)
  {
    lhs->vptr->H(lhs);
    rhs->vptr->H(rhs);
    if(lhs->vptr == &_vt_failed || rhs->vptr == &_vt_failed)
      return EQ_FAILED;
    if(lhs->tag != rhs->tag)
      return EQ_FALSE;

    node ** lbegin;
    node ** lend;
    node ** rbegin;
    node ** rend;
    lhs->vptr->succ(lhs, &lbegin, &lend);
    rhs->vptr->succ(rhs, &rbegin, &rend);
    assert(lend - lbegin == rend - rbegin);
    for(; lbegin!=lend; ++lbegin, ++rbegin)
    {
      EqualityResult const value = _boolequal_impl(*lbegin, *rbegin);
      if(value != EQ_TRUE)
        return value;
    }
    return EQ_TRUE;
  }
}}

extern "C"
{
  void sprite_printexpr(sprite::compiler::node * root, char * suffix)
  {
    sprite::compiler::_printexpr(root, true);
    if(suffix) fputs(suffix, stdout);
    fflush(0);
  }

  void sprite_normalize(sprite::compiler::node * root)
    { root->vptr->N(root); }

  #define DEFINE_PRIMITIVE_BINARY(name, type, op)      \
      void name(sprite::compiler::node * root)         \
      {                                                \
        sprite::compiler::node * lhs = SUCC_0(root);   \
        sprite::compiler::node * rhs = SUCC_1(root);   \
        /* FIXME: need to ensureNotFree.  And more? */ \
        lhs->vptr->H(lhs);                             \
        rhs->vptr->H(rhs);                             \
        type const x = DATA(lhs, type);                \
        type const y = DATA(rhs, type);                \
        root->vptr = lhs->vptr;                        \
        root->tag = sprite::compiler::CTOR;            \
        DATA(root, type) = x op y;                     \
      }                                                \
    /**/

  DEFINE_PRIMITIVE_BINARY(prim_Int_plus, int64_t, +)
  DEFINE_PRIMITIVE_BINARY(prim_Int_minus, int64_t, -)
  DEFINE_PRIMITIVE_BINARY(prim_Int_times, int64_t, *)

  // Converts a character into its ASCII value.
  void ord(sprite::compiler::node * root)
  {
    sprite::compiler::node * arg0 = SUCC_0(root);
    arg0->vptr->H(arg0);
    root->vptr = &_vt_Int64;
    root->tag = sprite::compiler::CTOR;
    DATA(root, int64_t) = DATA(arg0, char);
    root->slot1 = 0;
  }

  // Converts an ASCII value into a character.
  void chr(sprite::compiler::node * root)
  {
    sprite::compiler::node * arg0 = SUCC_0(root);
    arg0->vptr->H(arg0);
    root->vptr = &_vt_Char;
    root->tag = sprite::compiler::CTOR;
    DATA(root, char) = static_cast<char>(DATA(arg0, int64_t));
    root->slot1 = 0;
  }

  void failed(sprite::compiler::node * root)
  {
    root->vptr = &_vt_failed;
    root->tag = sprite::compiler::FAIL;
    root->slot0 = 0;
    root->slot1 = 0;
  }

  void success(sprite::compiler::node * root)
  {
    root->vptr = &_vt_success;
    root->tag = sprite::compiler::CTOR;
    root->slot0 = 0;
    root->slot1 = 0;
  }

  void true_(sprite::compiler::node * root)
  {
    root->vptr = &_vt_CTOR_Prelude_True;
    root->tag = 1;
    root->slot0 = 0;
    root->slot1 = 0;
  }

  void false_(sprite::compiler::node * root)
  {
    root->vptr = &_vt_CTOR_Prelude_False;
    root->tag = 0;
    root->slot0 = 0;
    root->slot1 = 0;
  }

  void apply(sprite::compiler::node * root)
  {
    sprite::compiler::node * partial = SUCC_0(root);
    partial->vptr->H(partial);
    int64_t const tag = partial->tag;
    int16_t const rem = tag & 0xFFFF;
    // Handle complete calls.  There is one argument, i.e., this one,
    // remaining.
    if(rem == 1)
    {
      int16_t const n = tag >> 16;
      switch(n)
      {
        case 0:
          assert(0 && "applying argument to nullary function");
          break;
        case 1:
          root->vptr = SUCC_0(partial)->vptr;
          root->tag = sprite::compiler::OPER;
          root->slot0 = root->slot1;
          root->slot1 = 0;
          break;
        case 2:
          root->vptr = DATA(SUCC_0(partial), sprite::compiler::vtable*);
          root->tag = sprite::compiler::OPER;
          root->slot0 = partial->slot1;
          // The second argument is already in the proper place.
          break;
        default:
        {
          // FIXME: need to alloc with the correct function.
          sprite::compiler::node ** args =
              reinterpret_cast<sprite::compiler::node**>(
                  malloc(sizeof(sprite::compiler::node*) * n)
                );
          sprite::compiler::node ** pos = args + n - 1;
          sprite::compiler::node * current = root;
          for(int16_t i=0; i<n; ++i)
          {
            *pos-- = SUCC_1(current);
            current = SUCC_0(current);
          }

          root->vptr = DATA(current, sprite::compiler::vtable*);
          root->tag = sprite::compiler::OPER;
          root->slot0 = args;
          root->slot1 = 0;
        }
      }
    }
    // Handle incomplete calls.
    else
    {
      root->vptr = &_vt_PartialSpine;
      root->tag = (tag & ~0xFFFF) | (rem - 1);
      // The successors do not need to be modified.
    }
  }

  // // Evaluates the first argument to head normal form (which could also be a free
  // // variable) and returns the second argument.
  // void seq(sprite::compiler::node * root)
  // {
  //   sprite::compiler::node * arg0 = SUCC_0(root);
  //   arg0->vptr->H(arg0);
  //   sprite::compiler::node * arg1 = SUCC_1(root);
  //   root->vptr = &_vt_fwd;
  //   root->tag = sprite::compiler::FWD;
  //   root->slot0 = arg1;
  //   root->slot1 = 0;
  // }

  // // Evaluates the argument to head normal form and returns it.  Suspends until
  // // the result is bound to a non-variable term.
  // void ensureNotFree(sprite::compiler::node * root)
  // {
  //   sprite::compiler::node * arg0 = SUCC_0(root);
  //   arg0->vptr->H(arg0);
  //   // TODO: suspend.
  //   root->vptr = &_vt_fwd;
  //   root->tag = sprite::compiler::FWD;
  // }

  void _boolequal(sprite::compiler::node * root)
  {
    sprite::compiler::node * lhs = SUCC_0(root);
    sprite::compiler::node * rhs = SUCC_1(root);
    switch(_boolequal_impl(lhs, rhs))
    {
      case sprite::compiler::EQ_TRUE:
        return true_(root);
      case sprite::compiler::EQ_FALSE:
        return false_(root);
      case sprite::compiler::EQ_FAILED:
        return failed(root);
    }
  }
}

