#pragma once
#include "stdint.h"

// !!! Important !!!
// Definitions in this file must be kept in sync with the corresponding ones in
// runtime.hpp.

namespace sprite { namespace compiler
{
  struct node;

  using tag_t = int16_t;
  using mark_t = int16_t;
  using aux_t = int32_t;

  using labelfun_t = char *(node *);
  using arityfun_t = uint64_t(node *);
  using stepfun_t = void(node *);
  using rangefun_t = void(node *, node ***, node ***);
  using genfun_t = node *(node *, aux_t id);

  struct vtable
  {
    stepfun_t  * H;         // Head-normalization function.
    stepfun_t  * N;         // Normalization function.
    labelfun_t * label;     // Gives the string representation of the label.
    vtable     * sentinel;  // Used to determine node memory state.
    tag_t        tag;       // The node tag.
    arityfun_t * arity;     // Gives the arity.
    rangefun_t * succ;      // Gives the range containing the successors.
    rangefun_t * gcsucc;    // Range function that does not skip fwd nodes.
    stepfun_t  * destroy;   // Frees the associated successor array, if any.
    // The vtables below are used only for CTOR nodes.
    vtable     * equal;     // Type-specific equality function.
    vtable     * equate;    // Type-specific equational constraint function.
    vtable     * ns_equate; // Type-specific non-strict equational constraint.
    vtable     * compare;   // Type-specific comparison function.
    vtable     * show;      // Type-specific show function.
  };

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
      { FAIL= -6, FREE= -5, FWD= -4, BINDING= -3, CHOICE= -2, OPER= -1, CTOR=0, TAGOFFSET= -FAIL };
}}

