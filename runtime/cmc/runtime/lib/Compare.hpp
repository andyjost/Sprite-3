#include "Prelude.hpp"
#include <iostream>   // for debugging/printing

namespace _Prelude {
  using namespace Engine;
  using namespace std;

  struct _descend_compare : Operation { // descend_compare
    Node** arg1;
    Node** arg2;
    _descend_compare(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new _descend_compare(_arg1, _arg2));
    }
    inline std::string name() { return "descend_compare"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _descend_compare(arg1, _arg);
      case 2: return new Engine::Partial(_descend_compare::make(_arg), 1);
      }
    }
    Node* hfun() {
      static void* table[]
	= {&&fail, &&var, &&choice, &&oper, &&_LT, &&_EQ, &&_GT};
      goto *table[(*arg1)->get_kind()];
    fail:
      return DO_FAIL;
    var:
      // Engine::narrow(arg1, generator());
      throw "No narrowing yet";
      goto *table[(*arg1)->get_kind()];
    choice:
    oper:
      Engine::hfun(arg1);
      goto *table[(*arg1)->get_kind()];
    _LT: // "LT"
      return new ::_Prelude::_LT();
    _EQ: // "EQ"
      return *(arg2);
    _GT: // "GT"
      return new ::_Prelude::_GT();
    }
  };

}
