#pragma once
#include "stdint.h"

// !!! Important !!!
// Definitions in this file must be kept in sync with the corresponding ones in
// runtime.hpp.

namespace sprite { namespace compiler
{
  struct node;

  using labelfun_t = char *(node *);
  using arityfun_t = uint64_t(node *);
  using stepfun_t = void(node *);
  using rangefun_t = void(node *, node ***, node ***);

  struct vtable
  {
    stepfun_t  * H;        // Head-normalization function.
    stepfun_t  * N;        // Normalization function.
    labelfun_t * label;    // Gives the string representation of the label.
    vtable     * sentinel; // Used to determine node memory state.
    arityfun_t * arity;    // Gives the arity.
    rangefun_t * succ;     // Gives the range containing the successors.
    rangefun_t * gcsucc;   // Range function that does not skip fwd nodes.
    stepfun_t  * destroy;  // Frees the associated successor array, if any.
    vtable     * equals;   // Type-specific equality function.
    vtable     * compare;  // Type-specific comparison function.
    vtable     * show;     // Type-specific show function.
  };

  using tag_t = int16_t;
  using mark_t = int16_t;
  using aux_t = int32_t;

  struct node
  {
    vtable * vptr;  // Pointer to the vtable.
    tag_t    tag;   // Node type tag.
    mark_t   mark;  // GC mark.
    aux_t    aux;   // Aux space.
    void *   slot0; // First successor or start of data area.
    void *   slot1; // Second successor.
  };

  enum Tag : tag_t
      { FAIL= -5, FREE= -4, FWD= -3, CHOICE= -2, OPER= -1, CTOR=0, TAGOFFSET= -FAIL };
}}

