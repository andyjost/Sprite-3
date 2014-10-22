#include "Litint.hpp"

namespace _Prelude {
  using namespace Engine;

  inline Node* Litint::boolequal(Node** right) { 
    static void* table[] = {&&fail, &&var, &&choice, &&oper, &&ctor};
    goto *table[(*right)->get_kind()];
  fail:
    return DO_FAIL;
  var:
    throw "Program flounders";
  choice:
  oper:
    Engine::hfun(right);
    goto *table[(*right)->get_kind()];
  ctor:
    if (arg1 == ((Litint*) (*right))->arg1)
      return new _Prelude::_True();
    else
      return new _Prelude::_False();
  }

  inline Node* Litint::compare(Node** right) { 
    static void* table[] = {&&fail, &&var, &&choice, &&oper, &&ctor};
    goto *table[(*right)->get_kind()];
  fail:
    return DO_FAIL;
  var:
    throw "Program flounders";
  choice:
  oper:
    Engine::hfun(right);
    goto *table[(*right)->get_kind()];
  ctor:
    int arg2 = ((Litint*) (*right))->arg1;
    if (arg1 < arg2)
      return new _Prelude::_LT();
    else if (arg1 > arg2)
      return new _Prelude::_GT();
    else
      return new _Prelude::_EQ();
  }

#define IMPL(INTNAME,EXTNAME,CODE) \
  Node* INTNAME::hfun() {				      \
    static void* table[] = {&&fail, &&var, &&choice, &&oper, &&ctor}; \
    goto *table[(*arg1)->get_kind()]; \
  fail: \
    return DO_FAIL; \
  var: \
    throw "Program floundes, variable left operand of \" ## EXTNAME ## \""; \
  choice: \
  oper: \
    Engine::hfun(arg1); \
    goto *table[(*arg1)->get_kind()]; \
  ctor: \
    static void* table_0[] = {&&fail_0, &&var_0, &&choice_0, &&oper_0, &&ctor_0}; \
    goto *table_0[(*arg2)->get_kind()]; \
  fail_0: \
    return DO_FAIL; \
  var_0: \
    throw "Program floundes, variable right operand of \" ## EXTNAME ## \""; \
  choice_0: \
  oper_0: \
    Engine::hfun(arg2); \
    goto *table_0[(*arg2)->get_kind()]; \
  ctor_0: \
    CODE \
  } // end

#define CODEADD \
  int left = ((Litint*) (*arg1))->arg1;		\
  int right = ((Litint*) (*arg2))->arg1;	\
  return new Litint(left + right);

#define CODESUB \
  int left = ((Litint*) (*arg1))->arg1;		\
  int right = ((Litint*) (*arg2))->arg1;	\
  return new Litint(left - right);

#define CODEMUL \
  int left = ((Litint*) (*arg1))->arg1;		\
  int right = ((Litint*) (*arg2))->arg1;	\
  return new Litint(left * right);
    
#define CODEDIV \
  int right = ((Litint*) (*arg2))->arg1;	\
  if (right == 0)				\
    return DO_FAIL;				\
  else {					\
  int left = ((Litint*) (*arg1))->arg1;		\
  return new Litint(left / right);		\
  }

#define CODEMOD \
  int right = ((Litint*) (*arg2))->arg1;	\
  if (right == 0)				\
    return DO_FAIL;				\
  else {					\
  int left = ((Litint*) (*arg1))->arg1;		\
  return new Litint(left % right);		\
  }

IMPL(__0x2B, "+", CODEADD)
IMPL(__0x2D, "-", CODESUB)
IMPL(__0x2A, "*", CODEMUL)
IMPL(_div, "div", CODEDIV)
IMPL(_mod, "mod", CODEMOD)

}
