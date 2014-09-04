/**
 * @file
 * @brief Contains data structures for representing Curry input programs.
 */
#pragma once
#include <limits>
#include <string>
#include <unordered_map>
#include <vector>

namespace sprite { namespace curry
{
  struct Definition;
  struct Rule;

  /// A placeholder used to represent a failure.
  struct Fail {};

  /// Represents a qualified name.
  struct Qname
  {
    std::string module;
    std::string name;
    std::string str() const { return module + "." + name; }
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
        Qname const & qname_ = Qname()
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
    Rule(Fail arg) : tag(FAIL), fail() {}
    Rule(int64_t const & arg) : tag(INT), int_(arg) {}
    Rule(double arg) : tag(DOUBLE), double_(arg) {}
    Rule(Ref arg) : tag(VAR), var(arg) {}

    template<
        typename...Args
      , typename = typename std::enable_if<
            std::is_constructible<Expr, Args...>::value
          >::type
      >
    Rule(Args &&...args)
      : tag(NODE), expr(std::forward<Args>(args)...)
    {}

    Rule(Qname const & qname, std::vector<Rule> && args)
      : tag(NODE), expr(qname, std::move(args))
    {}

    Rule(Rule && arg) : tag(arg.tag)
    {
      switch(tag)
      {
        case FAIL: new(&fail) Fail(); break;
        case INT: new(&int_) int64_t(arg.int_); break;
        case DOUBLE: new(&double_) double(arg.double_); break;
        case VAR: new(&var) Ref(arg.var); break;
        case NODE: new(&expr) Expr(std::move(arg.expr)); break;
      }
    }
    Rule(Rule const & arg) : tag(arg.tag)
    {
      switch(tag)
      {
        case FAIL: new(&fail) Fail(); break;
        case INT: new(&int_) int64_t(arg.int_); break;
        case DOUBLE: new(&double_) double(arg.double_); break;
        case VAR: new(&var) Ref(arg.var); break;
        case NODE: new(&expr) Expr(arg.expr); break;
      }
    }
    Rule & operator=(Rule && arg)
    {
      this->~Rule();
      new(this) Rule(std::move(arg));
      return *this;
    }
    Rule & operator=(Rule const & arg)
    {
      this->~Rule();
      new(this) Rule(arg);
      return *this;
    }
    ~Rule()
    {
      if(tag == NODE) expr.~Expr();
      else if(tag == FAIL) fail.~Fail();
    }
    template<typename Visitor>
    typename std::remove_reference<Visitor>::type::result_type
    visit(Visitor && visitor) const
    {
      switch(tag)
      {
        case FAIL:
          return visitor(this->fail);
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
    enum { FAIL, INT, DOUBLE, VAR, NODE } tag;
    union { Fail fail; int64_t int_; double double_; Ref var; Expr expr; };
  };

  /**
   * @brief Represents a function definition.
   *
   * This is (indirectly) a recursive data structure where each element is
   * either a branch or a rule.
   */
  struct Definition
  {
    Definition() : tag(RULE), rule(static_cast<int64_t>(0)) {}
    Definition(Branch const & arg) : tag(BRANCH), branch(arg) {}
    Definition(Branch && arg) : tag(BRANCH), branch(std::move(arg)) {}
    Definition(Rule const & arg) : tag(RULE), rule(arg) {}
    Definition(Rule && arg) : tag(RULE), rule(std::move(arg)) {}
    Definition(Expr const & arg) : tag(RULE), rule(arg) {}
    Definition(Expr && arg) : tag(RULE), rule(std::move(arg)) {}
    // template<typename ConvertsToRule
    //   , typename = typename std::enable_if<
    //         std::is_constructible<Rule, ConvertsToRule>::value
    //       >::type
    //   >
    // Definition(ConvertsToRule && arg) : tag(RULE), rule(std::move(arg))
    // {}
    Definition(Definition && arg) : tag(arg.tag)
    {
      switch(tag)
      {
        case BRANCH: new(&branch) Branch(std::move(arg.branch)); break;
        case RULE: new(&rule) Rule(std::move(arg.rule)); break;
      }
    }
    Definition(Definition const & arg) : tag(arg.tag)
    {
      switch(tag)
      {
        case BRANCH: new(&branch) Branch(arg.branch); break;
        case RULE: new(&rule) Rule(arg.rule); break;
      }
    }
    Definition & operator=(Definition && arg)
    {
      this->~Definition();
      new(this) Definition(std::move(arg));
      return *this;
    }
    Definition & operator=(Definition const & arg)
    {
      this->~Definition();
      new(this) Definition(arg);
      return *this;
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

  /// Used with Function::PathElem to indicate no base path (i.e., a terminus).
  enum { nobase = std::numeric_limits<size_t>::max() };

  /// Represents a Curry function.
  // Note: maintain layout compatibility with Constructor!
  struct Function
  {
    std::string name;
    size_t arity;
    struct PathElem { size_t base; size_t idx; };
    std::vector<PathElem> paths;
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
