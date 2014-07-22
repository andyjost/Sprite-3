/**
 * @file
 * @brief Contains data structures for representing Curry input programs.
 */
#pragma once
#include <string>
#include <vector>
#include <unordered_map>

namespace sprite { namespace curry
{
  struct Definition;
  struct Rule;

  /// Represents a qualified name.
  struct Qname
  {
    std::string module;
    std::string name;
  };

  /// Represents a Curry constructor.
  // Note: maintain layout compatibility with Function!
  struct Constructor
  {
    std::string name;
    size_t arity;
  };

  /**
   * @brief Represents a Curry data type, which comprises an ordered sequence
   * of constructors.
   */
  struct DataType
  {
    std::string name;
    std::vector<Constructor> constructors;
  };

  /**
   * @brief Represents one case of a branch.
   *
   *  The template parameter breaks a cyclic dependency.
   */
  template<typename Definition_ = Definition>
  struct Case_
  {
    Qname qname;
    Definition_ action;
  };
  using Case = Case_<>;

  /// Represents a branch (decision) of a function definition.
  struct Branch
  {
    std::string prefix;
    bool isflex;
    size_t pathid;
    std::vector<Case> cases;
  };

  /**
   * @brief Represents expression construction in a RHS rule expression.
   *
   * The template parameter is needed to break a cyclic dependency.
   */
  template<typename Rule_ = Rule>
  struct Expr_
  {
    Qname qname;
    std::vector<Rule_> args;

    Expr_(
        Qname const & qname_
      , std::vector<Rule_> const & args_ = std::vector<Rule_>()
      )
      : qname(qname_), args(args_)
    {}
  };
  using Expr = Expr_<>;

  /// Represents a variable reference.
  struct Ref { size_t pathid; };

  /**
   * @brief Represents a RHS rule expression.
   *
   * This is a union type where each node may be a literal int or double, a
   * variable reference, or an expression describing a node to be constructed.
   */
  struct Rule
  {
    Rule(int arg) : tag(INT), int_(arg) {}
    Rule(double arg) : tag(DOUBLE), double_(arg) {}
    Rule(Ref arg) : tag(VAR), var(arg) {}

    template<
        typename...Args
      , typename = typename std::enable_if<
            std::is_constructible<Expr, Args...>::value
          >::type
      >
    Rule(Args &&...args)
      : tag(NODE), expr(std::forward<Args...>(args)...)
    {}

    Rule(Qname const & qname, std::vector<Rule> const & args)
      : tag(NODE), expr(qname, std::move(args))
    {}

    Rule(Rule && arg) : tag(arg.tag)
    {
      switch(tag)
      {
        case INT: new(&int_) int(arg.int_); break;
        case DOUBLE: new(&double_) double(arg.double_); break;
        case VAR: new(&var) Ref(arg.var); break;
        case NODE: new(&expr) Expr(std::move(arg.expr)); break;
      }
    }
    Rule(Rule const & arg) : tag(arg.tag)
    {
      switch(tag)
      {
        case INT: new(&int_) int(arg.int_); break;
        case DOUBLE: new(&double_) double(arg.double_); break;
        case VAR: new(&var) Ref(arg.var); break;
        case NODE: new(&expr) Expr(arg.expr); break;
      }
    }
    ~Rule() { if(tag == NODE) expr.~Expr(); }
    template<typename Visitor>
    typename std::remove_reference<Visitor>::type::result_type
    visit(Visitor && visitor) const
    {
      switch(tag)
      {
        case INT:
          return visitor(this->int_);
        case DOUBLE:
          return visitor(this->double_);
        case VAR:
          return visitor(this->var);
        case NODE:
          return visitor(this->expr);
      }
    }
    Ref const * getvar() const
    {
      if(tag==VAR)
        return &var;
      return nullptr;
    }
  private:
    enum { INT, DOUBLE, VAR, NODE } tag;
    union { int int_; double double_; Ref var; Expr expr; };
  };

  /**
   * @brief Represents a function definition.
   *
   * This is (indirectly) a recursive data structure where each element is
   * either a branch or a rule.
   */
  struct Definition
  {
    Definition(Branch && arg) : tag(BRANCH), branch(std::move(arg)) {}
    template<typename ConvertsToRule
      , typename = typename std::enable_if<
            std::is_constructible<Rule, ConvertsToRule>::value
          >::type
      >
    Definition(ConvertsToRule && arg) : tag(RULE), rule(std::move(arg))
    {}
    Definition(Definition && arg) : tag(arg.tag)
    {
      switch(tag)
      {
        case BRANCH: new(&branch) Branch(std::move(arg.branch));
        case RULE: new(&rule) Rule(std::move(arg.rule));
      }
    }
    Definition(Definition const & arg) : tag(arg.tag)
    {
      switch(tag)
      {
        case BRANCH: new(&branch) Branch(arg.branch);
        case RULE: new(&rule) Rule(arg.rule);
      }
    }
    ~Definition()
    {
      switch(tag)
      {
        case BRANCH: branch.~Branch(); break;
        case RULE: rule.~Rule(); break;
      }
    }
    template<typename Visitor>
    typename std::remove_reference<Visitor>::type::result_type
    visit(Visitor && visitor) const
    {
      switch(tag)
      {
        case BRANCH:
          return visitor(this->branch);
        case RULE:
          return visitor(this->rule);
      }
    }
  private:
    enum {BRANCH, RULE} tag;
    union {Branch branch; Rule rule;};
  };

  /// Represents a Curry function.
  // Note: maintain layout compatibility with Constructor!
  struct Function
  {
    std::string name;
    size_t arity;
    struct PathElem { Qname qname; size_t idx; };
    std::unordered_map<size_t, std::vector<PathElem>> paths;
    Definition def;
  };

  /**
   * @brief Represents a Curry module, which comprises data types and
   * functions.
   */
  struct Module
  {
    std::string name;
    std::vector<std::string> imports;
    std::vector<DataType> datatypes;
    std::vector<Function> functions;
  };


  /**
   * @brief Represents a Curry library (or program), which is just a collection
   * of modules.
   */
  struct Library
  {
    std::vector<Module> modules;
  };
}}
