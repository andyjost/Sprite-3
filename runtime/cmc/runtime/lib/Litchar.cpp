#include "Prelude.hpp"
#include "Litchar.hpp"
#include "Litint.hpp"

namespace _Prelude {
  using namespace Engine;

  inline Node*  Litchar::boolequal(Node** right) { 
    static void* table[] = {&&fail, &&var, &&choice, &&oper, &&ctor};
    goto *table[(*right)->get_kind()];
  fail:
    return DO_FAIL;
  var:
    throw "Program flounder, argument of Litchar::hfun";
  choice:
  oper:
    Engine::hfun(right);
    goto *table[(*right)->get_kind()];
  ctor: 
    if (arg1 == ((Litchar*) (*right))->arg1)
      return new _Prelude::_True();
    else
      return new _Prelude::_False();
  };

  inline Node*  Litchar::compare(Node** right) { 
    static void* table[] = {&&fail, &&var, &&choice, &&oper, &&ctor};
    goto *table[(*right)->get_kind()];
  fail:
    return DO_FAIL;
  var:
    throw "Program flounder, argument of Litchar::hfun";
  choice:
  oper:
    Engine::hfun(right);
    goto *table[(*right)->get_kind()];
  ctor: 
    char arg2 = ((Litchar*) (*right))->arg1;
    if (arg1 < arg2)
      return new _Prelude::_LT();
    else if (arg1 > arg2)
      return new _Prelude::_GT();
    else
      return new _Prelude::_EQ();
  };

  Node* _ord::hfun() {
    static void* table[] = {&&fail, &&var, &&choice, &&oper, &&ctor};
    goto *table[(*arg1)->get_kind()];
  fail:
    return DO_FAIL;
  var:
    throw "Program flounder, argument of Litchar::ord";
  choice:
  oper:
    Engine::hfun(arg1);
    goto *table[(*arg1)->get_kind()];
  ctor: 
    char x = ((Litchar*) (*arg1))->arg1;
    return new Litint((int) x);
  };

  Node* _chr::hfun() {
    static void* table[] = {&&fail, &&var, &&choice, &&oper, &&ctor};
    goto *table[(*arg1)->get_kind()];
  fail:
    return DO_FAIL;
  var:
    throw "Program flounder, argument of Litchar::ord";
      choice:
    oper:
    Engine::hfun(arg1);
    goto *table[(*arg1)->get_kind()];
  ctor: 
    int x = ((Litint*) (*arg1))->arg1;
    return new Litchar((char) x);
  };

}
