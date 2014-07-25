#include "stdio.h"
#include "stdint.h"

namespace sprite { namespace compiler
{
  struct node;

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
}
