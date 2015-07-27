// Defines Cy_ComputationFrame and related data structures.
#pragma once
#include "basic_runtime.hpp"
#include "fingerprint.hpp"
#include <unordered_map>
#include <unordered_set>
#include <list>
#include <vector>

extern "C"
{
  using namespace sprite::compiler;

  // Holds bindings between choices and between free variables.
  //
  // @p equal stores bindings between choices.  @p eq_var stores bindings
  // between free variables.  Note that type info may not be available when
  // free variables are bound.  When a choice of a variable binding reaches the
  // top of a computation, then the variable must have been narrowed, at which
  // point the variable binding is converted into a binding between choices.
  struct ConstraintStore
  {
    // The list of bindings between choices.  E.g., {0 -> [1, 2]} says that
    // choices 0, 1, and 2 must all be made the same way.  This example implies
    // more entries for keys 1 and 2.  If the list is empty, then there must be
    // an entry in @p eq_var, below.  The key/value organization is motivated
    // by the need to process choices as they reach the of a computation.
    std::unordered_map<aux_t, std::unordered_set<aux_t>> eq_choice;

    struct BindData
    {
      BindData(node * f, node * s, bool l) : first(f), second(s), is_lazy(l) {}
      node * first;
      node * second;
      bool is_lazy;
    };

    // The variable bindings.  There are two types: lazy and normal.  A lazy
    // binding is between a variable and an unevaluated expression.  A normal
    // binding is between two free variables (not choices).  A free variable
    // may narrow to more than one choice (i.e., if the type has more than two
    // constructors).  Moreover, bindings between variables imply additional
    // bindings between successor variables.  For example, if x0=:=x1 for two
    // lists, then after narrowing to x0=([] ?0 (x2:x3)) and x1=([] ?1
    // (x4:x5)), we also have x2=:=x4 and x3=:=x5.
    std::unordered_map<aux_t, std::vector<BindData>> eq_var;
  };

  // A subcomputation.  One entry in a work queue from the Fair Scheme.
  struct Cy_EvalFrame
  {
    node * expr;
    Fingerprint fingerprint;
    ConstraintStore constraints;

    Cy_EvalFrame(node * e) : expr(e), fingerprint(), constraints()
    {}

    Cy_EvalFrame(node * e, Fingerprint && f, ConstraintStore && c)
      : expr(e), fingerprint(std::move(f)), constraints(std::move(c))
    {}

    // Non-copyable.
    Cy_EvalFrame(Cy_EvalFrame const &) = delete;
    Cy_EvalFrame & operator=(Cy_EvalFrame const &) = delete;
  };

  // The computation of a single involcation of Cy_Eval.  Holds one instance of
  // a work queue from the Fair Scheme.
  struct Cy_ComputationFrame
  {
    std::list<Cy_EvalFrame> * computation;
    Cy_ComputationFrame * next;
  };

  // A linked list of current computations.  Each element of the list is an
  // activation of Cy_Eval.
  extern Cy_ComputationFrame * Cy_GlobalComputations;

  // The current fingerprint.  Accessible through Cy_GlobalComputations, but duplicated
  // here for faster access.
  extern Fingerprint * Cy_CurrentFingerprint;

  // The cucrrent constraint store.
  extern ConstraintStore * Cy_CurrentConstraints;
}
