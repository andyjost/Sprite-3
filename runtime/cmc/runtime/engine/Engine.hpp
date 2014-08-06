#pragma once
#include "Macros.hpp"

#include <string>
#include <iostream> 
#include <sstream> 
#include <stack>
#include <cassert>

namespace Engine {
  using namespace std;

#define FAIL   0
#define VAR    1
#define CHOICE 2
#define OPER   3
#define CTOR   4

  struct Node {
    virtual int get_kind() = 0;
    virtual std::string name() = 0;
    virtual std::string to_s(int =0) = 0;
    virtual Node* hfun() = 0;
    virtual Node* nfun() = 0;
    virtual Node* afun() = 0;
    virtual Node* apply(Node**, int) = 0;
    // boolean equality 
    virtual Node* boolequal(Node**) = 0;
    // boolean comparison
    virtual Node* compare(Node**) = 0;
  };

  // ------------------------------------------------------------------
  // Stack stuff for backtracking

  struct Stack_Item {
    Node** indi;
    Node* before;
    Node* after;
  };

  extern std::stack<Stack_Item> mystack;

  // saved a redex being replace for backtracking
  inline void push(Node** indi, Node* after) {
    Stack_Item si = {indi, *indi, after};
    mystack.push(si);
  }

  bool backtrack();

  // ------------------------------------------------------------------
  // Global N, A, and H functions

  // execute the replacement of a rewrite step
  inline void replace(Node** nptr, Node* repl) {
    if (repl != *nptr) {
      cout << "R " << (*nptr)->to_s() << " -> " << repl->to_s() << endl;
      push(nptr, repl);
      *nptr = repl;
    } else {
      // TODO: a number has skips for N, H and A.  Try to avoid some?
      // cout << "SKIP " << (*nptr)->to_s() << " -> " << repl->to_s() << endl;
    }
  }

  // Global hfun updates its argument
  // the version commented with ++ saves a recursive call in hfun()
  // there can be a very long chain (e.g., see length of a list)
  // at the cost of test that can often be avoided in hfun() 
  inline void hfun(Node** nptr) {
    cout << "H " << (*nptr)->to_s() << endl;
    while (true) {
      int kind = (*nptr)->get_kind();
      if (kind >= CTOR || kind == FAIL || kind == VAR) break;
      Node* repl = (*nptr)->hfun();
      replace(nptr, repl);
    }
  }

  // Global afun updates its argument
  inline void afun(Node** nptr) {
    cout << "A " << (*nptr)->to_s() << endl;
    Node* repl = (*nptr)->afun();
    replace(nptr, repl);
  }

  // Global nfun updates its argument
  inline void nfun(Node** nptr) {
    cout << "N " << (*nptr)->to_s() << endl;
    hfun(nptr);
    Node* repl = (*nptr)->nfun();
    replace(nptr, repl);
    afun(nptr);
  }

  // ------------------------------------------------------------------

  struct Variable : Node {
    static int counter;
    int identifier;
    Node* binding;
    Variable() { identifier = ++counter; }
    static Node** make() { return new Node*(new Variable()); }
    inline int get_kind() { return VAR; }
    inline std::string name() { 
      stringstream ss; 
      ss << "_";
      ss.fill('0');
      ss.width(4);
      ss << identifier; 
      return ss.str();
    }
    inline std::string to_s(int n=0) { return name(); }
    inline Node* hfun() { return this; }
    inline Node* nfun() { return this; }
    inline Node* afun() { return this; }
    inline Node* apply(Node**, int) { throw "Error: Variable::apply"; }
    inline Node* boolequal(Node**) { throw "Program flounders"; }
    inline Node* compare(Node**) { throw "Program flounders"; }
  };

  // Narrow a variable
  inline void narrow(Node** nptr,  Node* generator) {
    assert((*nptr)->get_kind()==VAR);
    // TODO: the generator is fully determined by the type of the variable
    ((Variable*) (*nptr))->binding = generator;
    // exactly like a replacement, but a different header
    cout << "X " << (*nptr)->to_s() << " -> " << generator->to_s() << endl;
    push(nptr, generator);
    *nptr = generator;
  }

  struct Constructor : Node {
    inline Node* hfun() { return this; }
    virtual Node* boolequal(Node**) = 0;
  };

  struct Operation : Node {
    inline int get_kind() { return OPER; }
    inline Node* nfun() { throw "Method Operation::nfun should NEVER be called"; }
    inline Node* afun() { throw "Method Operation::afun should NEVER be called"; }
    virtual Node* boolequal(Node**) { throw "Method Operation::boolequal should NEVER be called"; }
    virtual Node* compare(Node**) { throw "Method Operation::compare should NEVER be called"; }
  };

  // TODO: the following symbols could go in the Prelude or a separate file

#define DO_FAIL Fail::getInstance()
  
  // neither a constructor nor an operation
  struct Fail : Node {
    // A singleton
  private:
    Fail() {}
    static Fail* instance;
  public:
    static Fail* getInstance() { return instance; }
    static Node** make() { return new Node*(instance); }
    inline int get_kind() { return FAIL; }
    inline std::string name() { return "\u22a5"; }
    inline std::string to_s(int n=0) { return name(); }
    inline Node* hfun() { return this; }
    inline Node* nfun() { return this; }
    inline Node* afun() { return this; }
    inline Node* apply(Node**, int) { throw "Impossible partially applied Fail"; }
    inline Node* boolequal(Node**) { return FAIL; }
    inline Node* compare(Node**) { return FAIL; }
  };

  struct Partial : Constructor {
    Node** arg1;  // expression with the root partially applied
    int missing;  // no. of args missing from arg1 for full application
    Partial(Node** _arg1, int _missing) : arg1(_arg1), missing(_missing) {}
    static Node** make(Node** _arg1, int _missing) { 
      return new Node*(new Partial(_arg1, _missing));
    }
    inline int get_kind() { return CTOR; }
    inline std::string name() { return "Engine::partial"; }
    inline std::string to_s(int n=0) { 
      if (n>=MAXDEPTH) return HIDE;			    
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      stringstream ss;
      ss << name() << "(" << s1 << "," << missing << ")";
      return ss.str();
    }
    // TODO: cannot recur on the argument
    inline Node* nfun() { /* Engine::nfun(arg1); */ return this; }
    // TODO: Do afun make sense ???
    inline Node* afun() { 
      if ((*arg1)->get_kind() == FAIL) return DO_FAIL;
      return this;
    }
    inline Node* apply(Node**, int) { 
      throw ("Impossible partially applied Partial\n" + to_s());
    }
    inline Node* boolequal(Node**) { throw "Boolean equality of partial application"; }
    inline Node* compare(Node**) { throw "Boolean compare of partial application"; }
  };


  struct Choice : Node { // Engine::?
    Node** arg1;
    Node** arg2;
    Choice(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) { return new Node*(new Choice(_arg1, _arg2)); }
    inline std::string name() { return "Engine::?"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return "\u221e";
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline int get_kind() { return CHOICE; }
    inline Node* hfun() {
      // Always choose left, the client makes a choice point
      // so that on backtraking the right will be chosen.
      return *arg1;
    }
    inline Node* nfun() { throw "Method nfun of class Choice should NEVER be called"; }
    inline Node* afun() { throw "Method afun of class Choice should NEVER be called"; }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new Choice(arg1, _arg);
      case 2: return new Partial(Choice::make(_arg),1);
      }
    }
    inline Node* boolequal(Node** right) { throw "Choice boolequal not yet implemented"; }
    inline Node* compare(Node** right) { throw "Choice compare not yet implemented"; }
  };

  struct Apply : Operation {
    SYMBOL2(Apply,"Engine::apply")
    inline Node* hfun() {
    start:
      // TODO: this is a bad hack
      if ((*arg1)->name() == "Engine::partial") {
	// the first argument is a partial application
	// extend this application with a new argument (of the application),
	// which is the second argument (of Apply)
	Node** function = ((Partial*) (*arg1))->arg1;
	int missing = ((Partial*) (*arg1))->missing;
	Node** tmp = new Node*((*function)->apply(arg2, missing));
        return *tmp;
      } else {
	// the first argument is an expression
	// that supposedly evaluates to a partial application
	Engine::hfun(arg1);
	goto start;      
      }
    } 
  };

  struct BOOLEQ : Operation { // Engine::==
    Node** arg1;
    Node** arg2;
    BOOLEQ(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new BOOLEQ(_arg1, _arg2));
    }
    inline std::string name() { return "Engine::=="; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new BOOLEQ(arg1, _arg);
      case 2: return new Engine::Partial(BOOLEQ::make(_arg), 1);
      }
    }
    inline Node* hfun() {
      int kind = (*arg1)->get_kind();
      if (kind < CTOR) {
	static void* table[] = {&&fail, &&var, &&choice, &&oper};
	goto *table[kind];
      fail:
	return DO_FAIL;
      var:
	throw "Program flounders";
      choice:
      oper:
	Engine::hfun(arg1);
	// fall through
      }
      // arg1 is constructor rooted
      return (*arg1)->boolequal(arg2);
    }
  };

  struct _compare : Operation { 
    Node** arg1;
    Node** arg2;
    _compare(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new _compare(_arg1, _arg2));
    }
    inline std::string name() { return "Engine::compare"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _compare(arg1, _arg);
      case 2: return new Engine::Partial(_compare::make(_arg), 1);
      }
    }
    inline Node* hfun() {
      int kind = (*arg1)->get_kind();
      if (kind < CTOR) {
	static void* table[] = {&&fail, &&var, &&choice, &&oper};
	goto *table[kind];
      fail:
	return DO_FAIL;
      var:
	throw "Program flounders";
      choice:
      oper:
	Engine::hfun(arg1);
	// fall through
      }
      // arg1 is constructor rooted
      return (*arg1)->compare(arg2);
    }
  };

}
