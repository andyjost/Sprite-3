#include "sprite/tree_utils.hpp"

namespace
{
  using namespace sprite::curry;

  struct ContainsVariableImpl
  {
    using result_type = bool;

    ContainsVariableImpl(size_t pathid_) : pathid(pathid_) {}
    size_t pathid;

    bool operator()(Branch const & branch) const
    {
      if(branch.condition.visit(*this))
        return true;
      for(auto const & case_: branch.cases)
      {
        if(case_->action.visit(*this))
          return true;
      }
      return false;
    }

    bool operator()(Rule const & rule) const
      { return rule.visit(*this); }

    bool operator()(Ref const & ref) const
      { return ref.pathid == this->pathid; }

    // Note: handles Term and Partial.
    bool operator()(Term const & term) const
    {
      for(auto const & arg: term.args)
      {
        if(arg.visit(*this))
          return true;
      }
      return false;
    }

    bool operator()(NLTerm const & nlterm) const
    {
      for(auto const & step: nlterm.steps)
      {
        if(this->operator()(step.term))
          return true;
      }
      return nlterm.result->visit(*this);
    }

    // default.
    template<typename T>
    bool operator()(T const &) const
      { return false; }
  };
}

namespace sprite { namespace curry
{
  bool contains_variable(Definition const & def, size_t pathid)
  {
    ContainsVariableImpl impl(pathid);
    return def.visit(impl);
  }


  std::vector<size_t> find_pathids_that_are_aux_arguments(
      Function const & fun, Branch const & b
    )
  {
    std::vector<size_t> out;
    size_t const N = fun.paths.size();
    for(size_t i=0; i<N; ++i)
    {
      if(fun.paths[i].base==freevar && contains_variable(b.condition, i))
      {
        for(auto const & case_: b.cases)
        {
          if(contains_variable(case_->action, i))
          {
            out.push_back(i);
            break;
          }
        }
      }
      
    }
    return out;
  }
}}
