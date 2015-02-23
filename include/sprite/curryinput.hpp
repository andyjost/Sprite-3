/**
 * @file
 * @brief Contains data structures for representing Curry input programs.
 */
#pragma once
#include <iterator>
#include <limits>
#include <list>
#include <memory>
#include <stdexcept>
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
    // Note: std::hash<Qname> defined below.
    friend bool operator==(Qname const & lhs, Qname const & rhs)
      { return lhs.module == rhs.module && lhs.name == rhs.name; }
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
   * LHS of an ATable or BTable rule.  Either a constructor name (Qname), or
   * some built-in data of a non-enumerable type.
   */
  struct CaseLhs
  {
    CaseLhs(Qname const & arg=Qname()) : tag(QNAME), qname(arg) {}
    CaseLhs(char arg) : tag(CHAR), char_(arg) {}
    CaseLhs(int64_t arg) : tag(INT), int_(arg) {}
    CaseLhs(double arg) : tag(DOUBLE), double_(arg) {}

    template<typename Visitor>
    typename std::remove_reference<Visitor>::type::result_type
    visit(Visitor && visitor) const
    {
      switch(tag)
      {
        case QNAME:
          return visitor(this->qname);
        case CHAR:
          return visitor(this->char_);
        case INT:
          return visitor(this->int_);
        case DOUBLE:
          return visitor(this->double_);
      }
      throw std::logic_error("unreachable");
    }

    CaseLhs(CaseLhs const & arg) : tag(arg.tag)
    {
      switch(tag)
      {
        case QNAME: new(&qname) Qname(arg.qname); break;
        case CHAR: new(&char_) char(arg.char_); break;
        case INT: new(&int_) int64_t(arg.int_); break;
        case DOUBLE: new(&double_) double(arg.double_); break;
      }
    }

    CaseLhs & operator=(CaseLhs const & arg)
    {
      this->~CaseLhs();
      new(this) CaseLhs(arg);
      return *this;
    }

    ~CaseLhs()
    {
      switch(tag)
      {
        case QNAME: qname.~Qname(); break;
        case CHAR: case INT: case DOUBLE: break;
      }
    }

  private:
    enum { QNAME, CHAR, INT, DOUBLE } tag;
    union { Qname qname; char char_; int64_t int_; double double_; };
  };

  /**
   * @brief Represents one case of a branch.
   *
   *  The template parameter breaks a cyclic dependency.
   */
  template<typename Definition_ = Definition>
  struct Case_
  {
    CaseLhs lhs;
    Definition_ action;
  };
  using Case = Case_<>;
  
  /**
   * @brief Represents a linear term appearing in a RHS rule.
   *
   * The template parameter is needed to break a cyclic dependency.
   */
  template<typename Rule_ = Rule>
  struct Term_
  {
    Qname qname;
    std::vector<Rule_> args;

    Term_(
        Qname const & qname_ = Qname()
      , std::vector<Rule_> const & args_ = std::vector<Rule_>()
      )
      : qname(qname_), args(args_)
    {}

    Term_(Term_ && term)
      : qname(std::move(term.qname)), args(std::move(term.args))
    {}

    Term_(Term_ const &) = default;
    Term_ & operator=(Term_ const &) = default;
  };
  using Term = Term_<>;

  /**
   * @brief Represents a non-linear term appearing in a RHS rule.
   *
   * This is associated with IBind variables in ICurry.  Non-linear terms are
   * specified as a series of steps that define intermediate values, and a
   * final step that produces the result.
   */
  template<typename Rule_ = Rule>
  struct NLTerm_
  {
    // Each step creates a new variable binding associated with varid.
    struct Step { size_t varid; Term_<Rule_> term; };
    std::list<Step> steps;
    // The result is the value of the term.  The pointer is used to break an
    // instantiation cycle with Rule.
    std::shared_ptr<Rule_> result;
  };
  using NLTerm = NLTerm_<>;

  /// Represents a variable reference.
  struct Ref { size_t pathid; };

  /// Represents a call to an external function.
  struct ExternalCall
  {
    Qname qname;
  };

  /// Represents a partial application.
  struct Partial : Term
  {
    using Term::Term;
    Partial(Term const & term) : Term(term) {}
    Partial(Term && term) : Term(std::move(term)) {}
  };

  /**
   * @brief Represents a RHS rule expression.
   *
   * This is a union type where each node may be a literal value, a variable
   * reference, an expression describing a node to be constructed, or a call to
   * an external function.
   */
  struct Rule
  {
    Rule(Fail = Fail()) : tag(FAIL), fail() {}
    Rule(char arg) : tag(CHAR), char_(arg) {}
    Rule(int64_t arg) : tag(INT), int_(arg) {}
    Rule(double arg) : tag(DOUBLE), double_(arg) {}
    Rule(Ref arg) : tag(VAR), var(arg) {}
    Rule(ExternalCall const & arg) : tag(EXTERNAL), external(arg) {}
    Rule(Partial const & arg) : tag(PARTIAL), partial(arg) {}
    Rule(NLTerm const & arg) : tag(NLTERM), nlterm(arg) {} 
    Rule(Term const & arg) : tag(TERM), term(arg) {}

    Rule(Qname const & qname, std::vector<Rule> && args)
      : tag(TERM), term(qname, std::move(args))
    {}

    Rule(Rule && arg) : tag(arg.tag)
    {
      switch(tag)
      {
        case FAIL: new(&fail) Fail(); break;
        case CHAR: new(&char_) char(arg.char_); break;
        case INT: new(&int_) int64_t(arg.int_); break;
        case DOUBLE: new(&double_) double(arg.double_); break;
        case VAR: new(&var) Ref(arg.var); break;
        case TERM: new(&term) Term(std::move(arg.term)); break;
        case EXTERNAL: new(&external) ExternalCall(std::move(arg.external)); break;
        case PARTIAL: new(&partial) Partial(std::move(arg.partial)); break;
        case NLTERM: new(&nlterm) NLTerm(std::move(arg.nlterm)); break;
      }
    }
    Rule(Rule const & arg) : tag(arg.tag)
    {
      switch(tag)
      {
        case FAIL: new(&fail) Fail(); break;
        case CHAR: new(&char_) char(arg.char_); break;
        case INT: new(&int_) int64_t(arg.int_); break;
        case DOUBLE: new(&double_) double(arg.double_); break;
        case VAR: new(&var) Ref(arg.var); break;
        case TERM: new(&term) Term(arg.term); break;
        case EXTERNAL: new(&external) ExternalCall(arg.external); break;
        case PARTIAL: new(&partial) Partial(arg.partial); break;
        case NLTERM: new(&nlterm) NLTerm(arg.nlterm); break;
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
      switch(tag)
      {
        case TERM: term.~Term(); break;
        case FAIL: fail.~Fail(); break;
        case EXTERNAL: external.~ExternalCall(); break;
        case PARTIAL: partial.~Partial(); break;
        case NLTERM: nlterm.~NLTerm(); break;
        default:;
      }
    }
    template<typename Visitor>
    typename std::remove_reference<Visitor>::type::result_type
    visit(Visitor && visitor) const
    {
      switch(tag)
      {
        case FAIL:
          return visitor(this->fail);
        case CHAR:
          return visitor(this->char_);
        case INT:
          return visitor(this->int_);
        case DOUBLE:
          return visitor(this->double_);
        case VAR:
          return visitor(this->var);
        case TERM:
          return visitor(this->term);
        case EXTERNAL:
          return visitor(this->external);
        case PARTIAL:
          return visitor(this->partial);
        case NLTERM:
          return visitor(this->nlterm);
      }
      throw std::logic_error("unreachable");
    }
    Ref const * getvar() const
    {
      if(tag==VAR)
        return &var;
      return nullptr;
    }
    Term const * getterm() const
    {
      if(tag==TERM)
        return &term;
      return nullptr;
    }
    char const * getchar() const
    {
      if(tag==CHAR)
        return &char_;
      return nullptr;
    }
    int64_t const * getint() const
    {
      if(tag==INT)
        return &int_;
      return nullptr;
    }
    double const * getdouble() const
    {
      if(tag==DOUBLE)
        return &double_;
      return nullptr;
    }
  private:
    enum { FAIL, CHAR, INT, DOUBLE, VAR, TERM, EXTERNAL, PARTIAL, NLTERM } tag;
    union {
      Fail fail; char char_; int64_t int_; double double_; Ref var; Term term;
      ExternalCall external; Partial partial; NLTerm nlterm;
    };
  };

  /// Represents a branch (decision) of a function definition.
  struct Branch
  {
    std::string prefix;
    bool isflex;
    bool iscomplete;
    Rule condition;
    std::vector<Case> cases;

    // For a complete branch, each case has its own tag.  For an incomplete
    // branch, all cases share tag CTOR.
    size_t num_tag_cases() const { return iscomplete ? cases.size() : 1; }
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
    Definition(Term const & arg) : tag(RULE), rule(arg) {}
    Definition(Term && arg) : tag(RULE), rule(std::move(arg)) {}
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
      throw std::logic_error("unreachable");
    }
  private:
    enum {BRANCH, RULE} tag;
    union {Branch branch; Rule rule;};
  };

  /**
   * @brief Special values used with Function::PathElem.
   *
   * nobase indicates there is no base path (i.e., a terminus).
   * freevar indicates the path is to a free variable.
   * bind is used for bound variables, used when building non-linear terms.
   */
  enum {
      nobase = std::numeric_limits<size_t>::max()
    , freevar = nobase - 1
    , bind = freevar - 1
    , local = bind - 1
    };

  /// Represents a Curry function.
  // Note: maintain layout compatibility with Constructor!
  struct Function
  {
    std::string name;
    size_t arity;
    struct PathElem { size_t base; size_t idx; Qname typename_; };
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
    // Note: the module addresses need to remain stable.
    std::list<Module> modules;

    /// Move the contents of another Library into this one.
    Library & merge_from(Library & arg)
    {
      if(modules.empty())
        modules = std::move(arg.modules);
      else
      {
        std::move(
            std::begin(arg.modules), std::end(arg.modules)
          , std::back_inserter(modules)
          );
        arg.modules.clear();
      }
      return *this;
    }
  };
}}

namespace std
{
  template<>
  struct hash<sprite::curry::Qname>
  {
    typedef sprite::curry::Qname argument_type;
    typedef size_t result_type;

    result_type operator()(argument_type const & arg) const
    {
      result_type const h0(std::hash<std::string>()(arg.module));
      result_type const h1(std::hash<std::string>()(arg.name));
      return h0 ^ (h1 << 1);
    }
  };
}

