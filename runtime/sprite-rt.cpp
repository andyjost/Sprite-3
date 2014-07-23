#include "stdio.h"
#include "stdint.h"

namespace sprite { namespace compiler
{
  struct node;

  typedef char * labelfun_t(node *);
  typedef uint64_t arityfun_t(node *);
  typedef void stepfun_t(node *);

  struct vtable
  {
    labelfun_t * label;
    arityfun_t * arity;
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
    size_t const N = root->vptr->arity(root);
    if(!is_outer && N > 0) fputs("(", stdout);
    fputs(root->vptr->label(root), stdout);
    node ** p = (node **)(&root->slot0);
    node ** e = p + N;
    for(; p != e; ++p)
    {
      fputs(" ", stdout);
      _printexpr(*p, false);
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
  }

  void sprite_normalize(sprite::compiler::node * root)
    { root->vptr->N(root); }
}
