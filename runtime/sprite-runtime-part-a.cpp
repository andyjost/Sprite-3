#include "stdio.h"
#include "stdint.h"

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
  void sprite_printexpr(sprite::compiler::node * root, char * suffix)
  {
    sprite::compiler::_printexpr(root, true);
    if(suffix) fputs(suffix, stdout);
    fflush(0);
  }

  void sprite_normalize(sprite::compiler::node * root)
    { root->vptr->N(root); }

  #define SUCC_0(root) reinterpret_cast<sprite::compiler::node*&>(root->slot0)
  #define SUCC_1(root) reinterpret_cast<sprite::compiler::node*&>(root->slot1)
  #define DATA(root, type) (*reinterpret_cast<type *>(&root->slot0))

  void prim_Int_plus(sprite::compiler::node * root)
  {
    sprite::compiler::node * lhs = SUCC_0(root);
    sprite::compiler::node * rhs = SUCC_1(root);
    int64_t const x = DATA(lhs, int64_t);
    int64_t const y = DATA(rhs, int64_t);
    root->vptr = lhs->vptr;
    root->tag = sprite::compiler::CTOR;
    DATA(root, int64_t) = x + y;
  }
}
