// Defines Cy_ComputationFrame and related data structures.
#pragma once
#include "basic_runtime.hpp"
// #include "context_switch.hpp"
#include "fingerprint.hpp"
#include <list>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>


namespace sprite { namespace compiler
{
  // Manages an instance of T using copy-on-write semantics.
  //
  // Read access produces the object directly.  Write access triggers a copy if
  // the object is not unique.  Follow the never-empty rule.
  template<typename T> struct Shared
  {
    // Construct a new managed object.
    template<typename...Args>
    Shared(Args&&...args)
      : data(std::make_shared<T>(std::forward<Args>(args)...))
    {}

    // Copy/assign/move.
    template<typename U> Shared(Shared<U> const & arg) : data(arg.data) {}
    template<typename U> Shared(Shared<U> & arg) : data(arg.data) {}
    template<typename U> Shared & operator=(Shared<U> const & arg)
      { data = arg.data; }
    template<typename U> Shared & operator=(Shared<U> & arg)
      { data = arg.data; }
    template<typename U> Shared(Shared<U> && arg) : data(std::move(arg.data))
      {}
    template<typename U> Shared & operator=(Shared<U> && arg)
      { data = std::move(arg.data); }

    // Data access.
    T const & read() const { return *data; }
    T & write()
    {
      if(!data.unique())
        data = std::make_shared<T>(*data);
      return *data;
    }
    bool needs_copy() const { return !data.unique(); }
    T const * operator->() const { return &read(); } // implied read

    // The GC gets special write access without triggering a copy.
    T & gc_write() const { return const_cast<T&>(*data); }

  private:

    mutable std::shared_ptr<T> data;
  };
}}

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
    using eq_choice_set_t = std::unordered_set<aux_t>;
    using eq_choice_map_t = std::unordered_map<aux_t, Shared<eq_choice_set_t>>;
    Shared<eq_choice_map_t> eq_choice;

    void add_choice_constraints(aux_t a, aux_t b)
    {
      _add_one_choice_constraint(a, b);
      _add_one_choice_constraint(b, a);
    }

  private:

    void _add_one_choice_constraint(aux_t a, aux_t b)
    {
      auto & cstore = this->eq_choice.write();
      auto p = cstore.find(a);
      if(p == cstore.end())
        this->eq_choice.write()[a].write().insert(b);
      else if(!p->second.read().count(b))
        p->second.write().insert(b);
    }

  public:

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
    // may narrow to more than one choice (i.e., if the type has more than
    // two constructors).  Moreover, bindings between variables imply
    // additional bindings between successor variables.  For example, if
    // x0=:=x1 for two lists, then after narrowing to x0=([] ?0 (x2:x3)) and
    // x1=([] ?1 (x4:x5)), we also have x2=:=x4 and x3=:=x5.
    using eq_var_vector_t = std::vector<BindData>;
    using eq_var_map_t = std::unordered_map<aux_t, Shared<eq_var_vector_t>>;
    Shared<eq_var_map_t> eq_var;

  private:

    void _add_one_var_constriant(node * from, node * to, bool is_lazy)
    {
      auto & vstore = eq_var.write();
      auto bucket = vstore.find(from->aux);
      auto & slot = (bucket == vstore.end()) ? vstore[from->aux] : bucket->second;
      slot.write().emplace_back(from, to, is_lazy);
    }

  public:

    void add_var_constraints(node * from, node * to, bool is_lazy)
    {
      this->_add_one_var_constriant(from, to, is_lazy);
      if(!is_lazy)
        this->_add_one_var_constriant(to, from, is_lazy);
    }
  };

  // A subcomputation.  One entry in a work queue from the Fair Scheme.
  struct Cy_EvalFrame
  {
    node * expr;
    Shared<Fingerprint> fingerprint;
    Shared<ConstraintStore> constraints;
    // The number of GC cycles before this is rotated to the end of the queue.
    size_t time_allotment = 1;
    // Interval time_interval;
    // Fiber fiber;

    Cy_EvalFrame(node * e) : expr(e), fingerprint(), constraints() {}

    Cy_EvalFrame(
        node * e, Shared<Fingerprint> && f, Shared<ConstraintStore> && c
      )
      : expr(e), fingerprint(std::move(f)), constraints(std::move(c))
    {}

    // Non-copyable.
    Cy_EvalFrame(Cy_EvalFrame const &) = delete;
    Cy_EvalFrame & operator=(Cy_EvalFrame const &) = delete;
  };

  // The computation of a single invocation of Cy_Eval.  Holds one instance of
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
  extern Shared<Fingerprint> * Cy_CurrentFingerprint;

  // The cucrrent constraint store.
  extern Shared<ConstraintStore> * Cy_CurrentConstraints;
}
