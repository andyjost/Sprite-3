
#include "Prelude.hpp"
#include "Compare.hpp"

namespace _Prelude {

  Node* __0x2E::hfun() { // .
    // VAR (1,1,(IPath (Arg 1)))
    // VAR (2,1,(IPath (Arg 2)))
    // LHS variable (1,1,(IPath (Arg 1))) is argument 1
    // LHS variable (2,1,(IPath (Arg 2))) is argument 2
    return new Engine::Partial(::_Prelude::__0x2E_0x2E__0x23lambda1::make(arg1, arg2), 1);
  }

  Node* __0x2E_0x2E__0x23lambda1::hfun() { // .._#lambda1
    // VAR (1,1,(IPath (Arg 1)))
    // VAR (2,1,(IPath (Arg 2)))
    // VAR (3,1,(IPath (Arg 3)))
    // LHS variable (1,1,(IPath (Arg 1))) is argument 1
    // LHS variable (2,1,(IPath (Arg 2))) is argument 2
    // LHS variable (3,1,(IPath (Arg 3))) is argument 3
    return new ::_Prelude::_apply(arg1, ::_Prelude::_apply::make(arg2, arg3));
  }

  Node* _id::hfun() { // id
    // VAR (1,1,(IPath (Arg 1)))
    // LHS variable (1,1,(IPath (Arg 1))) is argument 1
    return *(arg1);
  }

  Node* _const::hfun() { // const
    // VAR (1,1,(IPath (Arg 1)))
    // VAR (2,0,(IPath (Arg 2)))
    // LHS variable (1,1,(IPath (Arg 1))) is argument 1
    // LHS variable (2,0,(IPath (Arg 2))) is not used
    return *(arg1);
  }

  Node* _curry::hfun() { // curry
    // VAR (1,1,(IPath (Arg 1)))
    // VAR (2,1,(IPath (Arg 2)))
    // VAR (3,1,(IPath (Arg 3)))
    // LHS variable (1,1,(IPath (Arg 1))) is argument 1
    // LHS variable (2,1,(IPath (Arg 2))) is argument 2
    // LHS variable (3,1,(IPath (Arg 3))) is argument 3
    return new ::_Prelude::_apply(arg1, ::_Prelude::__0x28_0x2C_0x29::make(arg2, arg3));
  }

  Node* _uncurry::hfun() { // uncurry
    // VAR (1,1,(IPath (Arg 1)))
    // VAR (2,5,(IPath (Arg 2)))
    // VAR (3,1,(IPath (Rel 2 ("Prelude","(,)") 1)))
    // VAR (4,1,(IPath (Rel 2 ("Prelude","(,)") 2)))
    // LHS variable (1,1,(IPath (Arg 1))) is argument 1
    // LHS variable (2,5,(IPath (Arg 2))) is argument 2
    static void* table_5[]
      = {&&fail_5, &&var_5, &&choice_5, &&oper_5, &&__0x28_0x2C_0x29_5};
      goto *table_5[(*arg2)->get_kind()];
    fail_5:
      return DO_FAIL;
    var_5:
      // Engine::narrow(arg2, generator());
      throw "No narrowing yet";
      goto *table_5[(*arg2)->get_kind()];
    choice_5:
    oper_5:
      Engine::hfun(arg2);
      goto *table_5[(*arg2)->get_kind()];
    __0x28_0x2C_0x29_5: // "(,)"
      // LHS variable (3,1,(IPath (Rel 2 ("Prelude","(,)") 1))) is inlined as [(2,5,(IPath (Arg 2))),(3,1,(IPath (Rel 2 ("Prelude","(,)") 1)))]
      // LHS variable (4,1,(IPath (Rel 2 ("Prelude","(,)") 2))) is inlined as [(2,5,(IPath (Arg 2))),(4,1,(IPath (Rel 2 ("Prelude","(,)") 2)))]
      return new ::_Prelude::_apply(::_Prelude::_apply::make(arg1, ((::_Prelude::__0x28_0x2C_0x29*) *(arg2))->arg1), ((::_Prelude::__0x28_0x2C_0x29*) *(arg2))->arg2);
  }

  Node* _flip::hfun() { // flip
    // VAR (1,1,(IPath (Arg 1)))
    // VAR (2,1,(IPath (Arg 2)))
    // VAR (3,1,(IPath (Arg 3)))
    // LHS variable (1,1,(IPath (Arg 1))) is argument 1
    // LHS variable (2,1,(IPath (Arg 2))) is argument 2
    // LHS variable (3,1,(IPath (Arg 3))) is argument 3
    return new ::_Prelude::_apply(::_Prelude::_apply::make(arg1, arg3), arg2);
  }

  Node* _until::hfun() { // until
    // VAR (1,2,(IPath (Arg 1)))
    // VAR (2,2,(IPath (Arg 2)))
    // VAR (3,2,(IPath (Arg 3)))
    // LHS variable (1,2,(IPath (Arg 1))) is argument 1
    // LHS variable (2,2,(IPath (Arg 2))) is argument 2
    // LHS variable (3,2,(IPath (Arg 3))) is argument 3
    static void* table_4[]
      = {&&fail_4, &&var_4, &&choice_4, &&oper_4, &&_False_4, &&_True_4};
    Node** var_4 = ::_Prelude::_apply::make(arg1, arg3);
      goto *table_4[(*var_4)->get_kind()];
    fail_4:
      return DO_FAIL;
    var_4:
      // Engine::narrow(var_4, generator());
      throw "No narrowing yet";
      goto *table_4[(*var_4)->get_kind()];
    choice_4:
    oper_4:
      Engine::hfun(var_4);
      goto *table_4[(*var_4)->get_kind()];
    _False_4: // "False"
      return new ::_Prelude::_until(arg1, arg2, ::_Prelude::_apply::make(arg2, arg3));
    _True_4: // "True"
      return *(arg3);
  }

  Node* _seq::hfun() { // seq
    // VAR (1,1,(IPath (Arg 1)))
    // VAR (2,1,(IPath (Arg 2)))
    // LHS variable (1,1,(IPath (Arg 1))) is argument 1
    // LHS variable (2,1,(IPath (Arg 2))) is argument 2
    return new ::_Prelude::__0x24_0x21(Engine::Partial::make(::_Prelude::_const::make(arg2), 1), arg1);
  }

  // external Node* _ensureNotFree::hfun() { throw "External \"Prelude.ensureNotFree\" not implemented"; }

  Node* _ensureSpine::hfun() { // ensureSpine
    // VAR (1,1,(IPath (Arg 1)))
    // LHS variable (1,1,(IPath (Arg 1))) is argument 1
    return new ::_Prelude::_ensureSpine_0x2EensureList_0x2E20(::_Prelude::_ensureNotFree::make(arg1));
  }

  Node* _ensureSpine_0x2EensureList_0x2E20::hfun() { // ensureSpine.ensureList.20
    // VAR (1,5,(IPath (Arg 1)))
    // VAR (2,1,(IPath (Rel 1 ("Prelude",":") 1)))
    // VAR (3,1,(IPath (Rel 1 ("Prelude",":") 2)))
    // LHS variable (1,5,(IPath (Arg 1))) is argument 1
    static void* table_4[]
      = {&&fail_4, &&var_4, &&choice_4, &&oper_4, &&__0x5B_0x5D_4, &&__0x3A_4};
      goto *table_4[(*arg1)->get_kind()];
    fail_4:
      return DO_FAIL;
    var_4:
      // Engine::narrow(arg1, generator());
      throw "No narrowing yet";
      goto *table_4[(*arg1)->get_kind()];
    choice_4:
    oper_4:
      Engine::hfun(arg1);
      goto *table_4[(*arg1)->get_kind()];
    __0x5B_0x5D_4: // "[]"
      return new ::_Prelude::__0x5B_0x5D();
    __0x3A_4: // ":"
      // LHS variable (2,1,(IPath (Rel 1 ("Prelude",":") 1))) is inlined as [(1,5,(IPath (Arg 1))),(2,1,(IPath (Rel 1 ("Prelude",":") 1)))]
      // LHS variable (3,1,(IPath (Rel 1 ("Prelude",":") 2))) is inlined as [(1,5,(IPath (Arg 1))),(3,1,(IPath (Rel 1 ("Prelude",":") 2)))]
      return new ::_Prelude::__0x3A(((::_Prelude::__0x3A*) *(arg1))->arg1, ::_Prelude::_ensureSpine::make(((::_Prelude::__0x3A*) *(arg1))->arg2));
  }

  Node* __0x24::hfun() { // $
    // VAR (1,1,(IPath (Arg 1)))
    // VAR (2,1,(IPath (Arg 2)))
    // LHS variable (1,1,(IPath (Arg 1))) is argument 1
    // LHS variable (2,1,(IPath (Arg 2))) is argument 2
    return new ::_Prelude::_apply(arg1, arg2);
  }

  // external Node* __0x24_0x21::hfun() { throw "External \"Prelude.$!\" not implemented"; }

  // external Node* __0x24_0x21_0x21::hfun() { throw "External \"Prelude.$!!\" not implemented"; }

  Node* __0x24_0x23::hfun() { // $#
    // VAR (1,1,(IPath (Arg 1)))
    // VAR (2,1,(IPath (Arg 2)))
    // LHS variable (1,1,(IPath (Arg 1))) is argument 1
    // LHS variable (2,1,(IPath (Arg 2))) is argument 2
    return new ::_Prelude::__0x24_0x21(arg1, ::_Prelude::_ensureNotFree::make(arg2));
  }

  // external Node* __0x24_0x23_0x23::hfun() { throw "External \"Prelude.$##\" not implemented"; }

  Node* _error::hfun() { // error
    // VAR (1,1,(IPath (Arg 1)))
    // LHS variable (1,1,(IPath (Arg 1))) is argument 1
    return new ::_Prelude::__0x24_0x23_0x23(Engine::Partial::make(::_Prelude::_prim_error::make(), 1), arg1);
  }

  // external Node* _prim_error::hfun() { throw "External \"Prelude.prim_error\" not implemented"; }

  // external Node* _failed::hfun() { throw "External \"Prelude.failed\" not implemented"; }

  Node* __0x26_0x26::hfun() { // &&
    // VAR (1,3,(IPath (Arg 1)))
    // VAR (2,1,(IPath (Arg 2)))
    // LHS variable (1,3,(IPath (Arg 1))) is argument 1
    // LHS variable (2,1,(IPath (Arg 2))) is argument 2
    static void* table_3[]
      = {&&fail_3, &&var_3, &&choice_3, &&oper_3, &&_False_3, &&_True_3};
      goto *table_3[(*arg1)->get_kind()];
    fail_3:
      return DO_FAIL;
    var_3:
      // Engine::narrow(arg1, generator());
      throw "No narrowing yet";
      goto *table_3[(*arg1)->get_kind()];
    choice_3:
    oper_3:
      Engine::hfun(arg1);
      goto *table_3[(*arg1)->get_kind()];
    _False_3: // "False"
      return new ::_Prelude::_False();
    _True_3: // "True"
      return *(arg2);
  }

  Node* __0x7C_0x7C::hfun() { // ||
    // VAR (1,3,(IPath (Arg 1)))
    // VAR (2,1,(IPath (Arg 2)))
    // LHS variable (1,3,(IPath (Arg 1))) is argument 1
    // LHS variable (2,1,(IPath (Arg 2))) is argument 2
    static void* table_3[]
      = {&&fail_3, &&var_3, &&choice_3, &&oper_3, &&_False_3, &&_True_3};
      goto *table_3[(*arg1)->get_kind()];
    fail_3:
      return DO_FAIL;
    var_3:
      // Engine::narrow(arg1, generator());
      throw "No narrowing yet";
      goto *table_3[(*arg1)->get_kind()];
    choice_3:
    oper_3:
      Engine::hfun(arg1);
      goto *table_3[(*arg1)->get_kind()];
    _False_3: // "False"
      return *(arg2);
    _True_3: // "True"
      return new ::_Prelude::_True();
  }

  Node* _not::hfun() { // not
    // VAR (1,3,(IPath (Arg 1)))
    // LHS variable (1,3,(IPath (Arg 1))) is argument 1
    static void* table_2[]
      = {&&fail_2, &&var_2, &&choice_2, &&oper_2, &&_False_2, &&_True_2};
      goto *table_2[(*arg1)->get_kind()];
    fail_2:
      return DO_FAIL;
    var_2:
      // Engine::narrow(arg1, generator());
      throw "No narrowing yet";
      goto *table_2[(*arg1)->get_kind()];
    choice_2:
    oper_2:
      Engine::hfun(arg1);
      goto *table_2[(*arg1)->get_kind()];
    _False_2: // "False"
      return new ::_Prelude::_True();
    _True_2: // "True"
      return new ::_Prelude::_False();
  }

  Node* _otherwise::hfun() { // otherwise
    return new ::_Prelude::_True();
  }

  Node* _if_then_else::hfun() { // if_then_else
    // VAR (1,3,(IPath (Arg 1)))
    // VAR (2,1,(IPath (Arg 2)))
    // VAR (3,1,(IPath (Arg 3)))
    // LHS variable (1,3,(IPath (Arg 1))) is argument 1
    // LHS variable (2,1,(IPath (Arg 2))) is argument 2
    // LHS variable (3,1,(IPath (Arg 3))) is argument 3
    static void* table_4[]
      = {&&fail_4, &&var_4, &&choice_4, &&oper_4, &&_False_4, &&_True_4};
      goto *table_4[(*arg1)->get_kind()];
    fail_4:
      return DO_FAIL;
    var_4:
      // Engine::narrow(arg1, generator());
      throw "No narrowing yet";
      goto *table_4[(*arg1)->get_kind()];
    choice_4:
    oper_4:
      Engine::hfun(arg1);
      goto *table_4[(*arg1)->get_kind()];
    _False_4: // "False"
      return *(arg3);
    _True_4: // "True"
      return *(arg2);
  }

  Node* _solve::hfun() { // solve
    // VAR (1,3,(IPath (Arg 1)))
    // LHS variable (1,3,(IPath (Arg 1))) is argument 1
    static void* table_2[]
      = {&&fail_2, &&var_2, &&choice_2, &&oper_2, &&_False_2, &&_True_2};
      goto *table_2[(*arg1)->get_kind()];
    fail_2:
      return DO_FAIL;
    var_2:
      // Engine::narrow(arg1, generator());
      throw "No narrowing yet";
      goto *table_2[(*arg1)->get_kind()];
    choice_2:
    oper_2:
      Engine::hfun(arg1);
      goto *table_2[(*arg1)->get_kind()];
    _False_2: // "False"
      return DO_FAIL;
    _True_2: // "True"
      return new ::_Prelude::_True();
  }

  Node* __0x26_0x26_0x3E::hfun() { // &&>
    // VAR (1,3,(IPath (Arg 1)))
    // VAR (2,1,(IPath (Arg 2)))
    // LHS variable (1,3,(IPath (Arg 1))) is argument 1
    // LHS variable (2,1,(IPath (Arg 2))) is argument 2
    static void* table_3[]
      = {&&fail_3, &&var_3, &&choice_3, &&oper_3, &&_False_3, &&_True_3};
      goto *table_3[(*arg1)->get_kind()];
    fail_3:
      return DO_FAIL;
    var_3:
      // Engine::narrow(arg1, generator());
      throw "No narrowing yet";
      goto *table_3[(*arg1)->get_kind()];
    choice_3:
    oper_3:
      Engine::hfun(arg1);
      goto *table_3[(*arg1)->get_kind()];
    _False_3: // "False"
      return DO_FAIL;
    _True_3: // "True"
      return *(arg2);
  }

  // external Node* __0x3D_0x3D::hfun() { throw "External \"Prelude.==\" not implemented"; }

  Node* __0x2F_0x3D::hfun() { // /=
    // VAR (1,1,(IPath (Arg 1)))
    // VAR (2,1,(IPath (Arg 2)))
    // LHS variable (1,1,(IPath (Arg 1))) is argument 1
    // LHS variable (2,1,(IPath (Arg 2))) is argument 2
    return new ::_Prelude::_not(::_Prelude::__0x3D_0x3D::make(arg1, arg2));
  }

  // external Node* _compare::hfun() { throw "External \"Prelude.compare\" not implemented"; }

  Node* __0x3C::hfun() { // <
    // VAR (1,1,(IPath (Arg 1)))
    // VAR (2,1,(IPath (Arg 2)))
    // LHS variable (1,1,(IPath (Arg 1))) is argument 1
    // LHS variable (2,1,(IPath (Arg 2))) is argument 2
    static void* table_3[]
      = {&&fail_3, &&var_3, &&choice_3, &&oper_3, &&_LT_3, &&_EQ_3, &&_GT_3};
    Node** var_3 = ::_Prelude::_compare::make(arg1, arg2);
      goto *table_3[(*var_3)->get_kind()];
    fail_3:
      return DO_FAIL;
    var_3:
      // Engine::narrow(var_3, generator());
      throw "No narrowing yet";
      goto *table_3[(*var_3)->get_kind()];
    choice_3:
    oper_3:
      Engine::hfun(var_3);
      goto *table_3[(*var_3)->get_kind()];
    _LT_3: // "LT"
      return new ::_Prelude::_True();
    _EQ_3: // "EQ"
      return new ::_Prelude::_False();
    _GT_3: // "GT"
      return new ::_Prelude::_False();
  }

  Node* __0x3E::hfun() { // >
    // VAR (1,1,(IPath (Arg 1)))
    // VAR (2,1,(IPath (Arg 2)))
    // LHS variable (1,1,(IPath (Arg 1))) is argument 1
    // LHS variable (2,1,(IPath (Arg 2))) is argument 2
    static void* table_3[]
      = {&&fail_3, &&var_3, &&choice_3, &&oper_3, &&_LT_3, &&_EQ_3, &&_GT_3};
    Node** var_3 = ::_Prelude::_compare::make(arg1, arg2);
      goto *table_3[(*var_3)->get_kind()];
    fail_3:
      return DO_FAIL;
    var_3:
      // Engine::narrow(var_3, generator());
      throw "No narrowing yet";
      goto *table_3[(*var_3)->get_kind()];
    choice_3:
    oper_3:
      Engine::hfun(var_3);
      goto *table_3[(*var_3)->get_kind()];
    _LT_3: // "LT"
      return new ::_Prelude::_False();
    _EQ_3: // "EQ"
      return new ::_Prelude::_False();
    _GT_3: // "GT"
      return new ::_Prelude::_True();
  }

  Node* __0x3C_0x3D::hfun() { // <=
    // VAR (1,1,(IPath (Arg 1)))
    // VAR (2,1,(IPath (Arg 2)))
    // LHS variable (1,1,(IPath (Arg 1))) is argument 1
    // LHS variable (2,1,(IPath (Arg 2))) is argument 2
    return new ::_Prelude::_not(::_Prelude::__0x3E::make(arg1, arg2));
  }

  Node* __0x3E_0x3D::hfun() { // >=
    // VAR (1,1,(IPath (Arg 1)))
    // VAR (2,1,(IPath (Arg 2)))
    // LHS variable (1,1,(IPath (Arg 1))) is argument 1
    // LHS variable (2,1,(IPath (Arg 2))) is argument 2
    return new ::_Prelude::_not(::_Prelude::__0x3C::make(arg1, arg2));
  }

  Node* _max::hfun() { // max
    // VAR (1,2,(IPath (Arg 1)))
    // VAR (2,2,(IPath (Arg 2)))
    // LHS variable (1,2,(IPath (Arg 1))) is argument 1
    // LHS variable (2,2,(IPath (Arg 2))) is argument 2
    static void* table_3[]
      = {&&fail_3, &&var_3, &&choice_3, &&oper_3, &&_False_3, &&_True_3};
    Node** var_3 = ::_Prelude::__0x3E_0x3D::make(arg1, arg2);
      goto *table_3[(*var_3)->get_kind()];
    fail_3:
      return DO_FAIL;
    var_3:
      // Engine::narrow(var_3, generator());
      throw "No narrowing yet";
      goto *table_3[(*var_3)->get_kind()];
    choice_3:
    oper_3:
      Engine::hfun(var_3);
      goto *table_3[(*var_3)->get_kind()];
    _False_3: // "False"
      return *(arg2);
    _True_3: // "True"
      return *(arg1);
  }

  Node* _min::hfun() { // min
    // VAR (1,2,(IPath (Arg 1)))
    // VAR (2,2,(IPath (Arg 2)))
    // LHS variable (1,2,(IPath (Arg 1))) is argument 1
    // LHS variable (2,2,(IPath (Arg 2))) is argument 2
    static void* table_3[]
      = {&&fail_3, &&var_3, &&choice_3, &&oper_3, &&_False_3, &&_True_3};
    Node** var_3 = ::_Prelude::__0x3C_0x3D::make(arg1, arg2);
      goto *table_3[(*var_3)->get_kind()];
    fail_3:
      return DO_FAIL;
    var_3:
      // Engine::narrow(var_3, generator());
      throw "No narrowing yet";
      goto *table_3[(*var_3)->get_kind()];
    choice_3:
    oper_3:
      Engine::hfun(var_3);
      goto *table_3[(*var_3)->get_kind()];
    _False_3: // "False"
      return *(arg2);
    _True_3: // "True"
      return *(arg1);
  }

  Node* _fst::hfun() { // fst
    // VAR (1,4,(IPath (Arg 1)))
    // VAR (2,1,(IPath (Rel 1 ("Prelude","(,)") 1)))
    // VAR (3,0,(IPath (Rel 1 ("Prelude","(,)") 2)))
    // LHS variable (1,4,(IPath (Arg 1))) is argument 1
    static void* table_4[]
      = {&&fail_4, &&var_4, &&choice_4, &&oper_4, &&__0x28_0x2C_0x29_4};
      goto *table_4[(*arg1)->get_kind()];
    fail_4:
      return DO_FAIL;
    var_4:
      // Engine::narrow(arg1, generator());
      throw "No narrowing yet";
      goto *table_4[(*arg1)->get_kind()];
    choice_4:
    oper_4:
      Engine::hfun(arg1);
      goto *table_4[(*arg1)->get_kind()];
    __0x28_0x2C_0x29_4: // "(,)"
      // LHS variable (2,1,(IPath (Rel 1 ("Prelude","(,)") 1))) is inlined as [(1,4,(IPath (Arg 1))),(2,1,(IPath (Rel 1 ("Prelude","(,)") 1)))]
      // LHS variable (3,0,(IPath (Rel 1 ("Prelude","(,)") 2))) is not used
      return *(((::_Prelude::__0x28_0x2C_0x29*) *(arg1))->arg1);
  }

  Node* _snd::hfun() { // snd
    // VAR (1,4,(IPath (Arg 1)))
    // VAR (2,0,(IPath (Rel 1 ("Prelude","(,)") 1)))
    // VAR (3,1,(IPath (Rel 1 ("Prelude","(,)") 2)))
    // LHS variable (1,4,(IPath (Arg 1))) is argument 1
    static void* table_4[]
      = {&&fail_4, &&var_4, &&choice_4, &&oper_4, &&__0x28_0x2C_0x29_4};
      goto *table_4[(*arg1)->get_kind()];
    fail_4:
      return DO_FAIL;
    var_4:
      // Engine::narrow(arg1, generator());
      throw "No narrowing yet";
      goto *table_4[(*arg1)->get_kind()];
    choice_4:
    oper_4:
      Engine::hfun(arg1);
      goto *table_4[(*arg1)->get_kind()];
    __0x28_0x2C_0x29_4: // "(,)"
      // LHS variable (2,0,(IPath (Rel 1 ("Prelude","(,)") 1))) is not used
      // LHS variable (3,1,(IPath (Rel 1 ("Prelude","(,)") 2))) is inlined as [(1,4,(IPath (Arg 1))),(3,1,(IPath (Rel 1 ("Prelude","(,)") 2)))]
      return *(((::_Prelude::__0x28_0x2C_0x29*) *(arg1))->arg2);
  }

  Node* _head::hfun() { // head
    // VAR (1,4,(IPath (Arg 1)))
    // VAR (2,1,(IPath (Rel 1 ("Prelude",":") 1)))
    // VAR (3,0,(IPath (Rel 1 ("Prelude",":") 2)))
    // LHS variable (1,4,(IPath (Arg 1))) is argument 1
    static void* table_4[]
      = {&&fail_4, &&var_4, &&choice_4, &&oper_4, &&__0x5B_0x5D_4, &&__0x3A_4};
      goto *table_4[(*arg1)->get_kind()];
    fail_4:
      return DO_FAIL;
    var_4:
      // Engine::narrow(arg1, generator());
      throw "No narrowing yet";
      goto *table_4[(*arg1)->get_kind()];
    choice_4:
    oper_4:
      Engine::hfun(arg1);
      goto *table_4[(*arg1)->get_kind()];
    __0x5B_0x5D_4: // "[]"
      return DO_FAIL;
    __0x3A_4: // ":"
      // LHS variable (2,1,(IPath (Rel 1 ("Prelude",":") 1))) is inlined as [(1,4,(IPath (Arg 1))),(2,1,(IPath (Rel 1 ("Prelude",":") 1)))]
      // LHS variable (3,0,(IPath (Rel 1 ("Prelude",":") 2))) is not used
      return *(((::_Prelude::__0x3A*) *(arg1))->arg1);
  }

  Node* _tail::hfun() { // tail
    // VAR (1,4,(IPath (Arg 1)))
    // VAR (2,0,(IPath (Rel 1 ("Prelude",":") 1)))
    // VAR (3,1,(IPath (Rel 1 ("Prelude",":") 2)))
    // LHS variable (1,4,(IPath (Arg 1))) is argument 1
    static void* table_4[]
      = {&&fail_4, &&var_4, &&choice_4, &&oper_4, &&__0x5B_0x5D_4, &&__0x3A_4};
      goto *table_4[(*arg1)->get_kind()];
    fail_4:
      return DO_FAIL;
    var_4:
      // Engine::narrow(arg1, generator());
      throw "No narrowing yet";
      goto *table_4[(*arg1)->get_kind()];
    choice_4:
    oper_4:
      Engine::hfun(arg1);
      goto *table_4[(*arg1)->get_kind()];
    __0x5B_0x5D_4: // "[]"
      return DO_FAIL;
    __0x3A_4: // ":"
      // LHS variable (2,0,(IPath (Rel 1 ("Prelude",":") 1))) is not used
      // LHS variable (3,1,(IPath (Rel 1 ("Prelude",":") 2))) is inlined as [(1,4,(IPath (Arg 1))),(3,1,(IPath (Rel 1 ("Prelude",":") 2)))]
      return *(((::_Prelude::__0x3A*) *(arg1))->arg2);
  }

  Node* _null::hfun() { // null
    // VAR (1,3,(IPath (Arg 1)))
    // VAR (2,0,(IPath (Rel 1 ("Prelude",":") 1)))
    // VAR (3,0,(IPath (Rel 1 ("Prelude",":") 2)))
    // LHS variable (1,3,(IPath (Arg 1))) is argument 1
    static void* table_4[]
      = {&&fail_4, &&var_4, &&choice_4, &&oper_4, &&__0x5B_0x5D_4, &&__0x3A_4};
      goto *table_4[(*arg1)->get_kind()];
    fail_4:
      return DO_FAIL;
    var_4:
      // Engine::narrow(arg1, generator());
      throw "No narrowing yet";
      goto *table_4[(*arg1)->get_kind()];
    choice_4:
    oper_4:
      Engine::hfun(arg1);
      goto *table_4[(*arg1)->get_kind()];
    __0x5B_0x5D_4: // "[]"
      return new ::_Prelude::_True();
    __0x3A_4: // ":"
      // LHS variable (2,0,(IPath (Rel 1 ("Prelude",":") 1))) is not used
      // LHS variable (3,0,(IPath (Rel 1 ("Prelude",":") 2))) is not used
      return new ::_Prelude::_False();
  }

  Node* __0x2B_0x2B::hfun() { // ++
    // VAR (1,5,(IPath (Arg 1)))
    // VAR (2,1,(IPath (Arg 2)))
    // VAR (3,1,(IPath (Rel 1 ("Prelude",":") 1)))
    // VAR (4,1,(IPath (Rel 1 ("Prelude",":") 2)))
    // LHS variable (1,5,(IPath (Arg 1))) is argument 1
    // LHS variable (2,1,(IPath (Arg 2))) is argument 2
    static void* table_5[]
      = {&&fail_5, &&var_5, &&choice_5, &&oper_5, &&__0x5B_0x5D_5, &&__0x3A_5};
      goto *table_5[(*arg1)->get_kind()];
    fail_5:
      return DO_FAIL;
    var_5:
      // Engine::narrow(arg1, generator());
      throw "No narrowing yet";
      goto *table_5[(*arg1)->get_kind()];
    choice_5:
    oper_5:
      Engine::hfun(arg1);
      goto *table_5[(*arg1)->get_kind()];
    __0x5B_0x5D_5: // "[]"
      return *(arg2);
    __0x3A_5: // ":"
      // LHS variable (3,1,(IPath (Rel 1 ("Prelude",":") 1))) is inlined as [(1,5,(IPath (Arg 1))),(3,1,(IPath (Rel 1 ("Prelude",":") 1)))]
      // LHS variable (4,1,(IPath (Rel 1 ("Prelude",":") 2))) is inlined as [(1,5,(IPath (Arg 1))),(4,1,(IPath (Rel 1 ("Prelude",":") 2)))]
      return new ::_Prelude::__0x3A(((::_Prelude::__0x3A*) *(arg1))->arg1, ::_Prelude::__0x2B_0x2B::make(((::_Prelude::__0x3A*) *(arg1))->arg2, arg2));
  }

  Node* _length::hfun() { // length
    // VAR (1,4,(IPath (Arg 1)))
    // VAR (2,0,(IPath (Rel 1 ("Prelude",":") 1)))
    // VAR (3,1,(IPath (Rel 1 ("Prelude",":") 2)))
    // LHS variable (1,4,(IPath (Arg 1))) is argument 1
    static void* table_4[]
      = {&&fail_4, &&var_4, &&choice_4, &&oper_4, &&__0x5B_0x5D_4, &&__0x3A_4};
      goto *table_4[(*arg1)->get_kind()];
    fail_4:
      return DO_FAIL;
    var_4:
      // Engine::narrow(arg1, generator());
      throw "No narrowing yet";
      goto *table_4[(*arg1)->get_kind()];
    choice_4:
    oper_4:
      Engine::hfun(arg1);
      goto *table_4[(*arg1)->get_kind()];
    __0x5B_0x5D_4: // "[]"
      return new Litint(0);
    __0x3A_4: // ":"
      // LHS variable (2,0,(IPath (Rel 1 ("Prelude",":") 1))) is not used
      // LHS variable (3,1,(IPath (Rel 1 ("Prelude",":") 2))) is inlined as [(1,4,(IPath (Arg 1))),(3,1,(IPath (Rel 1 ("Prelude",":") 2)))]
      return new ::_Prelude::__0x2B(Litint::make(1), ::_Prelude::_length::make(((::_Prelude::__0x3A*) *(arg1))->arg2));
  }

  Node* __0x21_0x21::hfun() { // !!
    // VAR (1,5,(IPath (Arg 1)))
    // VAR (2,3,(IPath (Arg 2)))
    // VAR (3,1,(IPath (Rel 1 ("Prelude",":") 1)))
    // VAR (4,1,(IPath (Rel 1 ("Prelude",":") 2)))
    // LHS variable (1,5,(IPath (Arg 1))) is argument 1
    // LHS variable (2,3,(IPath (Arg 2))) is argument 2
    static void* table_5[]
      = {&&fail_5, &&var_5, &&choice_5, &&oper_5, &&__0x5B_0x5D_5, &&__0x3A_5};
      goto *table_5[(*arg1)->get_kind()];
    fail_5:
      return DO_FAIL;
    var_5:
      // Engine::narrow(arg1, generator());
      throw "No narrowing yet";
      goto *table_5[(*arg1)->get_kind()];
    choice_5:
    oper_5:
      Engine::hfun(arg1);
      goto *table_5[(*arg1)->get_kind()];
    __0x5B_0x5D_5: // "[]"
      return DO_FAIL;
    __0x3A_5: // ":"
      // LHS variable (3,1,(IPath (Rel 1 ("Prelude",":") 1))) is inlined as [(1,5,(IPath (Arg 1))),(3,1,(IPath (Rel 1 ("Prelude",":") 1)))]
      // LHS variable (4,1,(IPath (Rel 1 ("Prelude",":") 2))) is inlined as [(1,5,(IPath (Arg 1))),(4,1,(IPath (Rel 1 ("Prelude",":") 2)))]
      static void* table_6[]
        = {&&fail_6, &&var_6, &&choice_6, &&oper_6, &&_False_6, &&_True_6};
      Node** var_6 = ::_Prelude::__0x3D_0x3D::make(arg2, Litint::make(0));
        goto *table_6[(*var_6)->get_kind()];
      fail_6:
        return DO_FAIL;
      var_6:
        // Engine::narrow(var_6, generator());
        throw "No narrowing yet";
        goto *table_6[(*var_6)->get_kind()];
      choice_6:
      oper_6:
        Engine::hfun(var_6);
        goto *table_6[(*var_6)->get_kind()];
      _False_6: // "False"
        static void* table_7[]
          = {&&fail_7, &&var_7, &&choice_7, &&oper_7, &&_False_7, &&_True_7};
        Node** var_7 = ::_Prelude::__0x3E::make(arg2, Litint::make(0));
          goto *table_7[(*var_7)->get_kind()];
        fail_7:
          return DO_FAIL;
        var_7:
          // Engine::narrow(var_7, generator());
          throw "No narrowing yet";
          goto *table_7[(*var_7)->get_kind()];
        choice_7:
        oper_7:
          Engine::hfun(var_7);
          goto *table_7[(*var_7)->get_kind()];
        _False_7: // "False"
          return new ::_Prelude::_failed();
        _True_7: // "True"
          return new ::_Prelude::__0x21_0x21(((::_Prelude::__0x3A*) *(arg1))->arg2, ::_Prelude::__0x2D::make(arg2, Litint::make(1)));
      _True_6: // "True"
        return *(((::_Prelude::__0x3A*) *(arg1))->arg1);
  }

  Node* _map::hfun() { // map
    // VAR (1,2,(IPath (Arg 1)))
    // VAR (2,5,(IPath (Arg 2)))
    // VAR (3,1,(IPath (Rel 2 ("Prelude",":") 1)))
    // VAR (4,1,(IPath (Rel 2 ("Prelude",":") 2)))
    // LHS variable (1,2,(IPath (Arg 1))) is argument 1
    // LHS variable (2,5,(IPath (Arg 2))) is argument 2
    static void* table_5[]
      = {&&fail_5, &&var_5, &&choice_5, &&oper_5, &&__0x5B_0x5D_5, &&__0x3A_5};
      goto *table_5[(*arg2)->get_kind()];
    fail_5:
      return DO_FAIL;
    var_5:
      // Engine::narrow(arg2, generator());
      throw "No narrowing yet";
      goto *table_5[(*arg2)->get_kind()];
    choice_5:
    oper_5:
      Engine::hfun(arg2);
      goto *table_5[(*arg2)->get_kind()];
    __0x5B_0x5D_5: // "[]"
      return new ::_Prelude::__0x5B_0x5D();
    __0x3A_5: // ":"
      // LHS variable (3,1,(IPath (Rel 2 ("Prelude",":") 1))) is inlined as [(2,5,(IPath (Arg 2))),(3,1,(IPath (Rel 2 ("Prelude",":") 1)))]
      // LHS variable (4,1,(IPath (Rel 2 ("Prelude",":") 2))) is inlined as [(2,5,(IPath (Arg 2))),(4,1,(IPath (Rel 2 ("Prelude",":") 2)))]
      return new ::_Prelude::__0x3A(::_Prelude::_apply::make(arg1, ((::_Prelude::__0x3A*) *(arg2))->arg1), ::_Prelude::_map::make(arg1, ((::_Prelude::__0x3A*) *(arg2))->arg2));
  }

  Node* _foldl::hfun() { // foldl
    // VAR (1,2,(IPath (Arg 1)))
    // VAR (2,1,(IPath (Arg 2)))
    // VAR (3,5,(IPath (Arg 3)))
    // VAR (4,1,(IPath (Rel 3 ("Prelude",":") 1)))
    // VAR (5,1,(IPath (Rel 3 ("Prelude",":") 2)))
    // LHS variable (1,2,(IPath (Arg 1))) is argument 1
    // LHS variable (2,1,(IPath (Arg 2))) is argument 2
    // LHS variable (3,5,(IPath (Arg 3))) is argument 3
    static void* table_6[]
      = {&&fail_6, &&var_6, &&choice_6, &&oper_6, &&__0x5B_0x5D_6, &&__0x3A_6};
      goto *table_6[(*arg3)->get_kind()];
    fail_6:
      return DO_FAIL;
    var_6:
      // Engine::narrow(arg3, generator());
      throw "No narrowing yet";
      goto *table_6[(*arg3)->get_kind()];
    choice_6:
    oper_6:
      Engine::hfun(arg3);
      goto *table_6[(*arg3)->get_kind()];
    __0x5B_0x5D_6: // "[]"
      return *(arg2);
    __0x3A_6: // ":"
      // LHS variable (4,1,(IPath (Rel 3 ("Prelude",":") 1))) is inlined as [(3,5,(IPath (Arg 3))),(4,1,(IPath (Rel 3 ("Prelude",":") 1)))]
      // LHS variable (5,1,(IPath (Rel 3 ("Prelude",":") 2))) is inlined as [(3,5,(IPath (Arg 3))),(5,1,(IPath (Rel 3 ("Prelude",":") 2)))]
      return new ::_Prelude::_foldl(arg1, ::_Prelude::_apply::make(::_Prelude::_apply::make(arg1, arg2), ((::_Prelude::__0x3A*) *(arg3))->arg1), ((::_Prelude::__0x3A*) *(arg3))->arg2);
  }

  Node* _foldl1::hfun() { // foldl1
    // VAR (1,1,(IPath (Arg 1)))
    // VAR (2,5,(IPath (Arg 2)))
    // VAR (3,1,(IPath (Rel 2 ("Prelude",":") 1)))
    // VAR (4,1,(IPath (Rel 2 ("Prelude",":") 2)))
    // LHS variable (1,1,(IPath (Arg 1))) is argument 1
    // LHS variable (2,5,(IPath (Arg 2))) is argument 2
    static void* table_5[]
      = {&&fail_5, &&var_5, &&choice_5, &&oper_5, &&__0x5B_0x5D_5, &&__0x3A_5};
      goto *table_5[(*arg2)->get_kind()];
    fail_5:
      return DO_FAIL;
    var_5:
      // Engine::narrow(arg2, generator());
      throw "No narrowing yet";
      goto *table_5[(*arg2)->get_kind()];
    choice_5:
    oper_5:
      Engine::hfun(arg2);
      goto *table_5[(*arg2)->get_kind()];
    __0x5B_0x5D_5: // "[]"
      return DO_FAIL;
    __0x3A_5: // ":"
      // LHS variable (3,1,(IPath (Rel 2 ("Prelude",":") 1))) is inlined as [(2,5,(IPath (Arg 2))),(3,1,(IPath (Rel 2 ("Prelude",":") 1)))]
      // LHS variable (4,1,(IPath (Rel 2 ("Prelude",":") 2))) is inlined as [(2,5,(IPath (Arg 2))),(4,1,(IPath (Rel 2 ("Prelude",":") 2)))]
      return new ::_Prelude::_foldl(arg1, ((::_Prelude::__0x3A*) *(arg2))->arg1, ((::_Prelude::__0x3A*) *(arg2))->arg2);
  }

  Node* _foldr::hfun() { // foldr
    // VAR (1,2,(IPath (Arg 1)))
    // VAR (2,1,(IPath (Arg 2)))
    // VAR (3,5,(IPath (Arg 3)))
    // VAR (4,1,(IPath (Rel 3 ("Prelude",":") 1)))
    // VAR (5,1,(IPath (Rel 3 ("Prelude",":") 2)))
    // LHS variable (1,2,(IPath (Arg 1))) is argument 1
    // LHS variable (2,1,(IPath (Arg 2))) is argument 2
    // LHS variable (3,5,(IPath (Arg 3))) is argument 3
    static void* table_6[]
      = {&&fail_6, &&var_6, &&choice_6, &&oper_6, &&__0x5B_0x5D_6, &&__0x3A_6};
      goto *table_6[(*arg3)->get_kind()];
    fail_6:
      return DO_FAIL;
    var_6:
      // Engine::narrow(arg3, generator());
      throw "No narrowing yet";
      goto *table_6[(*arg3)->get_kind()];
    choice_6:
    oper_6:
      Engine::hfun(arg3);
      goto *table_6[(*arg3)->get_kind()];
    __0x5B_0x5D_6: // "[]"
      return *(arg2);
    __0x3A_6: // ":"
      // LHS variable (4,1,(IPath (Rel 3 ("Prelude",":") 1))) is inlined as [(3,5,(IPath (Arg 3))),(4,1,(IPath (Rel 3 ("Prelude",":") 1)))]
      // LHS variable (5,1,(IPath (Rel 3 ("Prelude",":") 2))) is inlined as [(3,5,(IPath (Arg 3))),(5,1,(IPath (Rel 3 ("Prelude",":") 2)))]
      return new ::_Prelude::_apply(::_Prelude::_apply::make(arg1, ((::_Prelude::__0x3A*) *(arg3))->arg1), ::_Prelude::_foldr::make(arg1, arg2, ((::_Prelude::__0x3A*) *(arg3))->arg2));
  }

  Node* _foldr1::hfun() { // foldr1
    // VAR (1,2,(IPath (Arg 1)))
    // VAR (2,5,(IPath (Arg 2)))
    // VAR (5,1,(IPath (Rel 4 ("Prelude",":") 1)))
    // VAR (6,1,(IPath (Rel 4 ("Prelude",":") 2)))
    // VAR (3,1,(IPath (Rel 2 ("Prelude",":") 1)))
    // VAR (4,5,(IPath (Rel 2 ("Prelude",":") 2)))
    // LHS variable (1,2,(IPath (Arg 1))) is argument 1
    // LHS variable (2,5,(IPath (Arg 2))) is argument 2
    static void* table_7[]
      = {&&fail_7, &&var_7, &&choice_7, &&oper_7, &&__0x5B_0x5D_7, &&__0x3A_7};
      goto *table_7[(*arg2)->get_kind()];
    fail_7:
      return DO_FAIL;
    var_7:
      // Engine::narrow(arg2, generator());
      throw "No narrowing yet";
      goto *table_7[(*arg2)->get_kind()];
    choice_7:
    oper_7:
      Engine::hfun(arg2);
      goto *table_7[(*arg2)->get_kind()];
    __0x5B_0x5D_7: // "[]"
      return DO_FAIL;
    __0x3A_7: // ":"
      // LHS variable (3,1,(IPath (Rel 2 ("Prelude",":") 1))) is inlined as [(2,5,(IPath (Arg 2))),(3,1,(IPath (Rel 2 ("Prelude",":") 1)))]
      Node** var_4 = ((::_Prelude::__0x3A*) *(arg2))->arg2; // [(4,5,(IPath (Rel 2 ("Prelude",":") 2)))] 
      static void* table_8[]
        = {&&fail_8, &&var_8, &&choice_8, &&oper_8, &&__0x5B_0x5D_8, &&__0x3A_8};
        goto *table_8[(*var_4)->get_kind()];
      fail_8:
        return DO_FAIL;
      var_8:
        // Engine::narrow(var_4, generator());
        throw "No narrowing yet";
        goto *table_8[(*var_4)->get_kind()];
      choice_8:
      oper_8:
        Engine::hfun(var_4);
        goto *table_8[(*var_4)->get_kind()];
      __0x5B_0x5D_8: // "[]"
        return *(((::_Prelude::__0x3A*) *(arg2))->arg1);
      __0x3A_8: // ":"
        // LHS variable (5,1,(IPath (Rel 4 ("Prelude",":") 1))) is inlined as [(4,5,(IPath (Rel 2 ("Prelude",":") 2))),(5,1,(IPath (Rel 4 ("Prelude",":") 1)))]
        // LHS variable (6,1,(IPath (Rel 4 ("Prelude",":") 2))) is inlined as [(4,5,(IPath (Rel 2 ("Prelude",":") 2))),(6,1,(IPath (Rel 4 ("Prelude",":") 2)))]
        return new ::_Prelude::_apply(::_Prelude::_apply::make(arg1, ((::_Prelude::__0x3A*) *(arg2))->arg1), ::_Prelude::_foldr1::make(arg1, ::_Prelude::__0x3A::make(((::_Prelude::__0x3A*) *(((::_Prelude::__0x3A*) *(arg2))->arg2))->arg1, ((::_Prelude::__0x3A*) *(((::_Prelude::__0x3A*) *(arg2))->arg2))->arg2)));
  }

  Node* _filter::hfun() { // filter
    // VAR (1,2,(IPath (Arg 1)))
    // VAR (2,5,(IPath (Arg 2)))
    // VAR (3,2,(IPath (Rel 2 ("Prelude",":") 1)))
    // VAR (4,1,(IPath (Rel 2 ("Prelude",":") 2)))
    // LHS variable (1,2,(IPath (Arg 1))) is argument 1
    // LHS variable (2,5,(IPath (Arg 2))) is argument 2
    static void* table_5[]
      = {&&fail_5, &&var_5, &&choice_5, &&oper_5, &&__0x5B_0x5D_5, &&__0x3A_5};
      goto *table_5[(*arg2)->get_kind()];
    fail_5:
      return DO_FAIL;
    var_5:
      // Engine::narrow(arg2, generator());
      throw "No narrowing yet";
      goto *table_5[(*arg2)->get_kind()];
    choice_5:
    oper_5:
      Engine::hfun(arg2);
      goto *table_5[(*arg2)->get_kind()];
    __0x5B_0x5D_5: // "[]"
      return new ::_Prelude::__0x5B_0x5D();
    __0x3A_5: // ":"
      Node** var_3 = ((::_Prelude::__0x3A*) *(arg2))->arg1; // [(3,2,(IPath (Rel 2 ("Prelude",":") 1)))] 
      // LHS variable (4,1,(IPath (Rel 2 ("Prelude",":") 2))) is inlined as [(2,5,(IPath (Arg 2))),(4,1,(IPath (Rel 2 ("Prelude",":") 2)))]
      static void* table_6[]
        = {&&fail_6, &&var_6, &&choice_6, &&oper_6, &&_False_6, &&_True_6};
      Node** var_6 = ::_Prelude::_apply::make(arg1, var_3);
        goto *table_6[(*var_6)->get_kind()];
      fail_6:
        return DO_FAIL;
      var_6:
        // Engine::narrow(var_6, generator());
        throw "No narrowing yet";
        goto *table_6[(*var_6)->get_kind()];
      choice_6:
      oper_6:
        Engine::hfun(var_6);
        goto *table_6[(*var_6)->get_kind()];
      _False_6: // "False"
        return new ::_Prelude::_filter(arg1, ((::_Prelude::__0x3A*) *(arg2))->arg2);
      _True_6: // "True"
        return new ::_Prelude::__0x3A(var_3, ::_Prelude::_filter::make(arg1, ((::_Prelude::__0x3A*) *(arg2))->arg2));
  }

  Node* _zip::hfun() { // zip
    // VAR (1,5,(IPath (Arg 1)))
    // VAR (2,5,(IPath (Arg 2)))
    // VAR (5,1,(IPath (Rel 2 ("Prelude",":") 1)))
    // VAR (6,1,(IPath (Rel 2 ("Prelude",":") 2)))
    // VAR (3,1,(IPath (Rel 1 ("Prelude",":") 1)))
    // VAR (4,1,(IPath (Rel 1 ("Prelude",":") 2)))
    // LHS variable (1,5,(IPath (Arg 1))) is argument 1
    // LHS variable (2,5,(IPath (Arg 2))) is argument 2
    static void* table_7[]
      = {&&fail_7, &&var_7, &&choice_7, &&oper_7, &&__0x5B_0x5D_7, &&__0x3A_7};
      goto *table_7[(*arg1)->get_kind()];
    fail_7:
      return DO_FAIL;
    var_7:
      // Engine::narrow(arg1, generator());
      throw "No narrowing yet";
      goto *table_7[(*arg1)->get_kind()];
    choice_7:
    oper_7:
      Engine::hfun(arg1);
      goto *table_7[(*arg1)->get_kind()];
    __0x5B_0x5D_7: // "[]"
      return new ::_Prelude::__0x5B_0x5D();
    __0x3A_7: // ":"
      // LHS variable (3,1,(IPath (Rel 1 ("Prelude",":") 1))) is inlined as [(1,5,(IPath (Arg 1))),(3,1,(IPath (Rel 1 ("Prelude",":") 1)))]
      // LHS variable (4,1,(IPath (Rel 1 ("Prelude",":") 2))) is inlined as [(1,5,(IPath (Arg 1))),(4,1,(IPath (Rel 1 ("Prelude",":") 2)))]
      static void* table_8[]
        = {&&fail_8, &&var_8, &&choice_8, &&oper_8, &&__0x5B_0x5D_8, &&__0x3A_8};
        goto *table_8[(*arg2)->get_kind()];
      fail_8:
        return DO_FAIL;
      var_8:
        // Engine::narrow(arg2, generator());
        throw "No narrowing yet";
        goto *table_8[(*arg2)->get_kind()];
      choice_8:
      oper_8:
        Engine::hfun(arg2);
        goto *table_8[(*arg2)->get_kind()];
      __0x5B_0x5D_8: // "[]"
        return new ::_Prelude::__0x5B_0x5D();
      __0x3A_8: // ":"
        // LHS variable (5,1,(IPath (Rel 2 ("Prelude",":") 1))) is inlined as [(2,5,(IPath (Arg 2))),(5,1,(IPath (Rel 2 ("Prelude",":") 1)))]
        // LHS variable (6,1,(IPath (Rel 2 ("Prelude",":") 2))) is inlined as [(2,5,(IPath (Arg 2))),(6,1,(IPath (Rel 2 ("Prelude",":") 2)))]
        return new ::_Prelude::__0x3A(::_Prelude::__0x28_0x2C_0x29::make(((::_Prelude::__0x3A*) *(arg1))->arg1, ((::_Prelude::__0x3A*) *(arg2))->arg1), ::_Prelude::_zip::make(((::_Prelude::__0x3A*) *(arg1))->arg2, ((::_Prelude::__0x3A*) *(arg2))->arg2));
  }

  Node* _zip3::hfun() { // zip3
    // VAR (1,5,(IPath (Arg 1)))
    // VAR (2,5,(IPath (Arg 2)))
    // VAR (3,5,(IPath (Arg 3)))
    // VAR (8,1,(IPath (Rel 3 ("Prelude",":") 1)))
    // VAR (9,1,(IPath (Rel 3 ("Prelude",":") 2)))
    // VAR (6,1,(IPath (Rel 2 ("Prelude",":") 1)))
    // VAR (7,1,(IPath (Rel 2 ("Prelude",":") 2)))
    // VAR (4,1,(IPath (Rel 1 ("Prelude",":") 1)))
    // VAR (5,1,(IPath (Rel 1 ("Prelude",":") 2)))
    // LHS variable (1,5,(IPath (Arg 1))) is argument 1
    // LHS variable (2,5,(IPath (Arg 2))) is argument 2
    // LHS variable (3,5,(IPath (Arg 3))) is argument 3
    static void* table_10[]
      = {&&fail_10, &&var_10, &&choice_10, &&oper_10, &&__0x5B_0x5D_10, &&__0x3A_10};
      goto *table_10[(*arg1)->get_kind()];
    fail_10:
      return DO_FAIL;
    var_10:
      // Engine::narrow(arg1, generator());
      throw "No narrowing yet";
      goto *table_10[(*arg1)->get_kind()];
    choice_10:
    oper_10:
      Engine::hfun(arg1);
      goto *table_10[(*arg1)->get_kind()];
    __0x5B_0x5D_10: // "[]"
      return new ::_Prelude::__0x5B_0x5D();
    __0x3A_10: // ":"
      // LHS variable (4,1,(IPath (Rel 1 ("Prelude",":") 1))) is inlined as [(1,5,(IPath (Arg 1))),(4,1,(IPath (Rel 1 ("Prelude",":") 1)))]
      // LHS variable (5,1,(IPath (Rel 1 ("Prelude",":") 2))) is inlined as [(1,5,(IPath (Arg 1))),(5,1,(IPath (Rel 1 ("Prelude",":") 2)))]
      static void* table_11[]
        = {&&fail_11, &&var_11, &&choice_11, &&oper_11, &&__0x5B_0x5D_11, &&__0x3A_11};
        goto *table_11[(*arg2)->get_kind()];
      fail_11:
        return DO_FAIL;
      var_11:
        // Engine::narrow(arg2, generator());
        throw "No narrowing yet";
        goto *table_11[(*arg2)->get_kind()];
      choice_11:
      oper_11:
        Engine::hfun(arg2);
        goto *table_11[(*arg2)->get_kind()];
      __0x5B_0x5D_11: // "[]"
        return new ::_Prelude::__0x5B_0x5D();
      __0x3A_11: // ":"
        // LHS variable (6,1,(IPath (Rel 2 ("Prelude",":") 1))) is inlined as [(2,5,(IPath (Arg 2))),(6,1,(IPath (Rel 2 ("Prelude",":") 1)))]
        // LHS variable (7,1,(IPath (Rel 2 ("Prelude",":") 2))) is inlined as [(2,5,(IPath (Arg 2))),(7,1,(IPath (Rel 2 ("Prelude",":") 2)))]
        static void* table_12[]
          = {&&fail_12, &&var_12, &&choice_12, &&oper_12, &&__0x5B_0x5D_12, &&__0x3A_12};
          goto *table_12[(*arg3)->get_kind()];
        fail_12:
          return DO_FAIL;
        var_12:
          // Engine::narrow(arg3, generator());
          throw "No narrowing yet";
          goto *table_12[(*arg3)->get_kind()];
        choice_12:
        oper_12:
          Engine::hfun(arg3);
          goto *table_12[(*arg3)->get_kind()];
        __0x5B_0x5D_12: // "[]"
          return new ::_Prelude::__0x5B_0x5D();
        __0x3A_12: // ":"
          // LHS variable (8,1,(IPath (Rel 3 ("Prelude",":") 1))) is inlined as [(3,5,(IPath (Arg 3))),(8,1,(IPath (Rel 3 ("Prelude",":") 1)))]
          // LHS variable (9,1,(IPath (Rel 3 ("Prelude",":") 2))) is inlined as [(3,5,(IPath (Arg 3))),(9,1,(IPath (Rel 3 ("Prelude",":") 2)))]
          return new ::_Prelude::__0x3A(::_Prelude::__0x28_0x2C_0x2C_0x29::make(((::_Prelude::__0x3A*) *(arg1))->arg1, ((::_Prelude::__0x3A*) *(arg2))->arg1, ((::_Prelude::__0x3A*) *(arg3))->arg1), ::_Prelude::_zip3::make(((::_Prelude::__0x3A*) *(arg1))->arg2, ((::_Prelude::__0x3A*) *(arg2))->arg2, ((::_Prelude::__0x3A*) *(arg3))->arg2));
  }

  Node* _zipWith::hfun() { // zipWith
    // VAR (1,2,(IPath (Arg 1)))
    // VAR (2,5,(IPath (Arg 2)))
    // VAR (3,5,(IPath (Arg 3)))
    // VAR (6,1,(IPath (Rel 3 ("Prelude",":") 1)))
    // VAR (7,1,(IPath (Rel 3 ("Prelude",":") 2)))
    // VAR (4,1,(IPath (Rel 2 ("Prelude",":") 1)))
    // VAR (5,1,(IPath (Rel 2 ("Prelude",":") 2)))
    // LHS variable (1,2,(IPath (Arg 1))) is argument 1
    // LHS variable (2,5,(IPath (Arg 2))) is argument 2
    // LHS variable (3,5,(IPath (Arg 3))) is argument 3
    static void* table_8[]
      = {&&fail_8, &&var_8, &&choice_8, &&oper_8, &&__0x5B_0x5D_8, &&__0x3A_8};
      goto *table_8[(*arg2)->get_kind()];
    fail_8:
      return DO_FAIL;
    var_8:
      // Engine::narrow(arg2, generator());
      throw "No narrowing yet";
      goto *table_8[(*arg2)->get_kind()];
    choice_8:
    oper_8:
      Engine::hfun(arg2);
      goto *table_8[(*arg2)->get_kind()];
    __0x5B_0x5D_8: // "[]"
      return new ::_Prelude::__0x5B_0x5D();
    __0x3A_8: // ":"
      // LHS variable (4,1,(IPath (Rel 2 ("Prelude",":") 1))) is inlined as [(2,5,(IPath (Arg 2))),(4,1,(IPath (Rel 2 ("Prelude",":") 1)))]
      // LHS variable (5,1,(IPath (Rel 2 ("Prelude",":") 2))) is inlined as [(2,5,(IPath (Arg 2))),(5,1,(IPath (Rel 2 ("Prelude",":") 2)))]
      static void* table_9[]
        = {&&fail_9, &&var_9, &&choice_9, &&oper_9, &&__0x5B_0x5D_9, &&__0x3A_9};
        goto *table_9[(*arg3)->get_kind()];
      fail_9:
        return DO_FAIL;
      var_9:
        // Engine::narrow(arg3, generator());
        throw "No narrowing yet";
        goto *table_9[(*arg3)->get_kind()];
      choice_9:
      oper_9:
        Engine::hfun(arg3);
        goto *table_9[(*arg3)->get_kind()];
      __0x5B_0x5D_9: // "[]"
        return new ::_Prelude::__0x5B_0x5D();
      __0x3A_9: // ":"
        // LHS variable (6,1,(IPath (Rel 3 ("Prelude",":") 1))) is inlined as [(3,5,(IPath (Arg 3))),(6,1,(IPath (Rel 3 ("Prelude",":") 1)))]
        // LHS variable (7,1,(IPath (Rel 3 ("Prelude",":") 2))) is inlined as [(3,5,(IPath (Arg 3))),(7,1,(IPath (Rel 3 ("Prelude",":") 2)))]
        return new ::_Prelude::__0x3A(::_Prelude::_apply::make(::_Prelude::_apply::make(arg1, ((::_Prelude::__0x3A*) *(arg2))->arg1), ((::_Prelude::__0x3A*) *(arg3))->arg1), ::_Prelude::_zipWith::make(arg1, ((::_Prelude::__0x3A*) *(arg2))->arg2, ((::_Prelude::__0x3A*) *(arg3))->arg2));
  }

  Node* _zipWith3::hfun() { // zipWith3
    // VAR (1,2,(IPath (Arg 1)))
    // VAR (2,5,(IPath (Arg 2)))
    // VAR (3,5,(IPath (Arg 3)))
    // VAR (4,5,(IPath (Arg 4)))
    // VAR (9,1,(IPath (Rel 4 ("Prelude",":") 1)))
    // VAR (10,1,(IPath (Rel 4 ("Prelude",":") 2)))
    // VAR (7,1,(IPath (Rel 3 ("Prelude",":") 1)))
    // VAR (8,1,(IPath (Rel 3 ("Prelude",":") 2)))
    // VAR (5,1,(IPath (Rel 2 ("Prelude",":") 1)))
    // VAR (6,1,(IPath (Rel 2 ("Prelude",":") 2)))
    // LHS variable (1,2,(IPath (Arg 1))) is argument 1
    // LHS variable (2,5,(IPath (Arg 2))) is argument 2
    // LHS variable (3,5,(IPath (Arg 3))) is argument 3
    // LHS variable (4,5,(IPath (Arg 4))) is argument 4
    static void* table_11[]
      = {&&fail_11, &&var_11, &&choice_11, &&oper_11, &&__0x5B_0x5D_11, &&__0x3A_11};
      goto *table_11[(*arg2)->get_kind()];
    fail_11:
      return DO_FAIL;
    var_11:
      // Engine::narrow(arg2, generator());
      throw "No narrowing yet";
      goto *table_11[(*arg2)->get_kind()];
    choice_11:
    oper_11:
      Engine::hfun(arg2);
      goto *table_11[(*arg2)->get_kind()];
    __0x5B_0x5D_11: // "[]"
      return new ::_Prelude::__0x5B_0x5D();
    __0x3A_11: // ":"
      // LHS variable (5,1,(IPath (Rel 2 ("Prelude",":") 1))) is inlined as [(2,5,(IPath (Arg 2))),(5,1,(IPath (Rel 2 ("Prelude",":") 1)))]
      // LHS variable (6,1,(IPath (Rel 2 ("Prelude",":") 2))) is inlined as [(2,5,(IPath (Arg 2))),(6,1,(IPath (Rel 2 ("Prelude",":") 2)))]
      static void* table_12[]
        = {&&fail_12, &&var_12, &&choice_12, &&oper_12, &&__0x5B_0x5D_12, &&__0x3A_12};
        goto *table_12[(*arg3)->get_kind()];
      fail_12:
        return DO_FAIL;
      var_12:
        // Engine::narrow(arg3, generator());
        throw "No narrowing yet";
        goto *table_12[(*arg3)->get_kind()];
      choice_12:
      oper_12:
        Engine::hfun(arg3);
        goto *table_12[(*arg3)->get_kind()];
      __0x5B_0x5D_12: // "[]"
        return new ::_Prelude::__0x5B_0x5D();
      __0x3A_12: // ":"
        // LHS variable (7,1,(IPath (Rel 3 ("Prelude",":") 1))) is inlined as [(3,5,(IPath (Arg 3))),(7,1,(IPath (Rel 3 ("Prelude",":") 1)))]
        // LHS variable (8,1,(IPath (Rel 3 ("Prelude",":") 2))) is inlined as [(3,5,(IPath (Arg 3))),(8,1,(IPath (Rel 3 ("Prelude",":") 2)))]
        static void* table_13[]
          = {&&fail_13, &&var_13, &&choice_13, &&oper_13, &&__0x5B_0x5D_13, &&__0x3A_13};
          goto *table_13[(*arg4)->get_kind()];
        fail_13:
          return DO_FAIL;
        var_13:
          // Engine::narrow(arg4, generator());
          throw "No narrowing yet";
          goto *table_13[(*arg4)->get_kind()];
        choice_13:
        oper_13:
          Engine::hfun(arg4);
          goto *table_13[(*arg4)->get_kind()];
        __0x5B_0x5D_13: // "[]"
          return new ::_Prelude::__0x5B_0x5D();
        __0x3A_13: // ":"
          // LHS variable (9,1,(IPath (Rel 4 ("Prelude",":") 1))) is inlined as [(4,5,(IPath (Arg 4))),(9,1,(IPath (Rel 4 ("Prelude",":") 1)))]
          // LHS variable (10,1,(IPath (Rel 4 ("Prelude",":") 2))) is inlined as [(4,5,(IPath (Arg 4))),(10,1,(IPath (Rel 4 ("Prelude",":") 2)))]
          return new ::_Prelude::__0x3A(::_Prelude::_apply::make(::_Prelude::_apply::make(::_Prelude::_apply::make(arg1, ((::_Prelude::__0x3A*) *(arg2))->arg1), ((::_Prelude::__0x3A*) *(arg3))->arg1), ((::_Prelude::__0x3A*) *(arg4))->arg1), ::_Prelude::_zipWith3::make(arg1, ((::_Prelude::__0x3A*) *(arg2))->arg2, ((::_Prelude::__0x3A*) *(arg3))->arg2, ((::_Prelude::__0x3A*) *(arg4))->arg2));
  }

  Node* _unzip::hfun() { // unzip
    // VAR (1,5,(IPath (Arg 1)))
    // VAR (2,1,(IPath (Rel 1 ("Prelude",":") 1)))
    // VAR (3,1,(IPath (Rel 1 ("Prelude",":") 2)))
    // LHS variable (1,5,(IPath (Arg 1))) is argument 1
    static void* table_4[]
      = {&&fail_4, &&var_4, &&choice_4, &&oper_4, &&__0x5B_0x5D_4, &&__0x3A_4};
      goto *table_4[(*arg1)->get_kind()];
    fail_4:
      return DO_FAIL;
    var_4:
      // Engine::narrow(arg1, generator());
      throw "No narrowing yet";
      goto *table_4[(*arg1)->get_kind()];
    choice_4:
    oper_4:
      Engine::hfun(arg1);
      goto *table_4[(*arg1)->get_kind()];
    __0x5B_0x5D_4: // "[]"
      return new ::_Prelude::__0x28_0x2C_0x29(::_Prelude::__0x5B_0x5D::make(), ::_Prelude::__0x5B_0x5D::make());
    __0x3A_4: // ":"
      // LHS variable (2,1,(IPath (Rel 1 ("Prelude",":") 1))) is inlined as [(1,5,(IPath (Arg 1))),(2,1,(IPath (Rel 1 ("Prelude",":") 1)))]
      // LHS variable (3,1,(IPath (Rel 1 ("Prelude",":") 2))) is inlined as [(1,5,(IPath (Arg 1))),(3,1,(IPath (Rel 1 ("Prelude",":") 2)))]
      return new ::_Prelude::_unzip_case__0x231(((::_Prelude::__0x3A*) *(arg1))->arg1, ((::_Prelude::__0x3A*) *(arg1))->arg2);
  }

  Node* _unzip_case__0x231::hfun() { // unzip_case_#1
    // VAR (2,5,(IPath (Arg 1)))
    // VAR (3,1,(IPath (Arg 2)))
    // VAR (6,2,IBind)
    // VAR (7,1,IBind)
    // VAR (8,1,IBind)
    // VAR (4,1,(IPath (Rel 2 ("Prelude","(,)") 1)))
    // VAR (5,1,(IPath (Rel 2 ("Prelude","(,)") 2)))
    // LHS variable (2,5,(IPath (Arg 1))) is argument 1
    // LHS variable (3,1,(IPath (Arg 2))) is argument 2
    static void* table_9[]
      = {&&fail_9, &&var_9, &&choice_9, &&oper_9, &&__0x28_0x2C_0x29_9};
      goto *table_9[(*arg1)->get_kind()];
    fail_9:
      return DO_FAIL;
    var_9:
      // Engine::narrow(arg1, generator());
      throw "No narrowing yet";
      goto *table_9[(*arg1)->get_kind()];
    choice_9:
    oper_9:
      Engine::hfun(arg1);
      goto *table_9[(*arg1)->get_kind()];
    __0x28_0x2C_0x29_9: // "(,)"
      // LHS variable (4,1,(IPath (Rel 2 ("Prelude","(,)") 1))) is inlined as [(2,5,(IPath (Arg 1))),(4,1,(IPath (Rel 2 ("Prelude","(,)") 1)))]
      // LHS variable (5,1,(IPath (Rel 2 ("Prelude","(,)") 2))) is inlined as [(2,5,(IPath (Arg 1))),(5,1,(IPath (Rel 2 ("Prelude","(,)") 2)))]
      // [(6,[]),(7,[]),(8,[])]
      Node** var_6 = ::_Prelude::_unzip::make(arg2);
      Node** var_7 = ::_Prelude::_unzip_0x2E__0x23selFP2_0x23xs::make(var_6);
      Node** var_8 = ::_Prelude::_unzip_0x2E__0x23selFP3_0x23ys::make(var_6);
      return new ::_Prelude::__0x28_0x2C_0x29(::_Prelude::__0x3A::make(((::_Prelude::__0x28_0x2C_0x29*) *(arg1))->arg1, var_7), ::_Prelude::__0x3A::make(((::_Prelude::__0x28_0x2C_0x29*) *(arg1))->arg2, var_8));
  }

  Node* _unzip_0x2E__0x23selFP2_0x23xs::hfun() { // unzip._#selFP2#xs
    // VAR (1,4,(IPath (Arg 1)))
    // VAR (2,1,(IPath (Rel 1 ("Prelude","(,)") 1)))
    // VAR (3,0,(IPath (Rel 1 ("Prelude","(,)") 2)))
    // LHS variable (1,4,(IPath (Arg 1))) is argument 1
    static void* table_4[]
      = {&&fail_4, &&var_4, &&choice_4, &&oper_4, &&__0x28_0x2C_0x29_4};
      goto *table_4[(*arg1)->get_kind()];
    fail_4:
      return DO_FAIL;
    var_4:
      // Engine::narrow(arg1, generator());
      throw "No narrowing yet";
      goto *table_4[(*arg1)->get_kind()];
    choice_4:
    oper_4:
      Engine::hfun(arg1);
      goto *table_4[(*arg1)->get_kind()];
    __0x28_0x2C_0x29_4: // "(,)"
      // LHS variable (2,1,(IPath (Rel 1 ("Prelude","(,)") 1))) is inlined as [(1,4,(IPath (Arg 1))),(2,1,(IPath (Rel 1 ("Prelude","(,)") 1)))]
      // LHS variable (3,0,(IPath (Rel 1 ("Prelude","(,)") 2))) is not used
      return *(((::_Prelude::__0x28_0x2C_0x29*) *(arg1))->arg1);
  }

  Node* _unzip_0x2E__0x23selFP3_0x23ys::hfun() { // unzip._#selFP3#ys
    // VAR (1,4,(IPath (Arg 1)))
    // VAR (2,0,(IPath (Rel 1 ("Prelude","(,)") 1)))
    // VAR (3,1,(IPath (Rel 1 ("Prelude","(,)") 2)))
    // LHS variable (1,4,(IPath (Arg 1))) is argument 1
    static void* table_4[]
      = {&&fail_4, &&var_4, &&choice_4, &&oper_4, &&__0x28_0x2C_0x29_4};
      goto *table_4[(*arg1)->get_kind()];
    fail_4:
      return DO_FAIL;
    var_4:
      // Engine::narrow(arg1, generator());
      throw "No narrowing yet";
      goto *table_4[(*arg1)->get_kind()];
    choice_4:
    oper_4:
      Engine::hfun(arg1);
      goto *table_4[(*arg1)->get_kind()];
    __0x28_0x2C_0x29_4: // "(,)"
      // LHS variable (2,0,(IPath (Rel 1 ("Prelude","(,)") 1))) is not used
      // LHS variable (3,1,(IPath (Rel 1 ("Prelude","(,)") 2))) is inlined as [(1,4,(IPath (Arg 1))),(3,1,(IPath (Rel 1 ("Prelude","(,)") 2)))]
      return *(((::_Prelude::__0x28_0x2C_0x29*) *(arg1))->arg2);
  }

  Node* _unzip3::hfun() { // unzip3
    // VAR (1,5,(IPath (Arg 1)))
    // VAR (2,1,(IPath (Rel 1 ("Prelude",":") 1)))
    // VAR (3,1,(IPath (Rel 1 ("Prelude",":") 2)))
    // LHS variable (1,5,(IPath (Arg 1))) is argument 1
    static void* table_4[]
      = {&&fail_4, &&var_4, &&choice_4, &&oper_4, &&__0x5B_0x5D_4, &&__0x3A_4};
      goto *table_4[(*arg1)->get_kind()];
    fail_4:
      return DO_FAIL;
    var_4:
      // Engine::narrow(arg1, generator());
      throw "No narrowing yet";
      goto *table_4[(*arg1)->get_kind()];
    choice_4:
    oper_4:
      Engine::hfun(arg1);
      goto *table_4[(*arg1)->get_kind()];
    __0x5B_0x5D_4: // "[]"
      return new ::_Prelude::__0x28_0x2C_0x2C_0x29(::_Prelude::__0x5B_0x5D::make(), ::_Prelude::__0x5B_0x5D::make(), ::_Prelude::__0x5B_0x5D::make());
    __0x3A_4: // ":"
      // LHS variable (2,1,(IPath (Rel 1 ("Prelude",":") 1))) is inlined as [(1,5,(IPath (Arg 1))),(2,1,(IPath (Rel 1 ("Prelude",":") 1)))]
      // LHS variable (3,1,(IPath (Rel 1 ("Prelude",":") 2))) is inlined as [(1,5,(IPath (Arg 1))),(3,1,(IPath (Rel 1 ("Prelude",":") 2)))]
      return new ::_Prelude::_unzip3_case__0x231(((::_Prelude::__0x3A*) *(arg1))->arg1, ((::_Prelude::__0x3A*) *(arg1))->arg2);
  }

  Node* _unzip3_case__0x231::hfun() { // unzip3_case_#1
    // VAR (2,6,(IPath (Arg 1)))
    // VAR (3,1,(IPath (Arg 2)))
    // VAR (7,3,IBind)
    // VAR (8,1,IBind)
    // VAR (9,1,IBind)
    // VAR (10,1,IBind)
    // VAR (4,1,(IPath (Rel 2 ("Prelude","(,,)") 1)))
    // VAR (5,1,(IPath (Rel 2 ("Prelude","(,,)") 2)))
    // VAR (6,1,(IPath (Rel 2 ("Prelude","(,,)") 3)))
    // LHS variable (2,6,(IPath (Arg 1))) is argument 1
    // LHS variable (3,1,(IPath (Arg 2))) is argument 2
    static void* table_11[]
      = {&&fail_11, &&var_11, &&choice_11, &&oper_11, &&__0x28_0x2C_0x2C_0x29_11};
      goto *table_11[(*arg1)->get_kind()];
    fail_11:
      return DO_FAIL;
    var_11:
      // Engine::narrow(arg1, generator());
      throw "No narrowing yet";
      goto *table_11[(*arg1)->get_kind()];
    choice_11:
    oper_11:
      Engine::hfun(arg1);
      goto *table_11[(*arg1)->get_kind()];
    __0x28_0x2C_0x2C_0x29_11: // "(,,)"
      // LHS variable (4,1,(IPath (Rel 2 ("Prelude","(,,)") 1))) is inlined as [(2,6,(IPath (Arg 1))),(4,1,(IPath (Rel 2 ("Prelude","(,,)") 1)))]
      // LHS variable (5,1,(IPath (Rel 2 ("Prelude","(,,)") 2))) is inlined as [(2,6,(IPath (Arg 1))),(5,1,(IPath (Rel 2 ("Prelude","(,,)") 2)))]
      // LHS variable (6,1,(IPath (Rel 2 ("Prelude","(,,)") 3))) is inlined as [(2,6,(IPath (Arg 1))),(6,1,(IPath (Rel 2 ("Prelude","(,,)") 3)))]
      // [(7,[]),(8,[]),(9,[]),(10,[])]
      Node** var_7 = ::_Prelude::_unzip3::make(arg2);
      Node** var_8 = ::_Prelude::_unzip3_0x2E__0x23selFP5_0x23xs::make(var_7);
      Node** var_9 = ::_Prelude::_unzip3_0x2E__0x23selFP6_0x23ys::make(var_7);
      Node** var_10 = ::_Prelude::_unzip3_0x2E__0x23selFP7_0x23zs::make(var_7);
      return new ::_Prelude::__0x28_0x2C_0x2C_0x29(::_Prelude::__0x3A::make(((::_Prelude::__0x28_0x2C_0x2C_0x29*) *(arg1))->arg1, var_8), ::_Prelude::__0x3A::make(((::_Prelude::__0x28_0x2C_0x2C_0x29*) *(arg1))->arg2, var_9), ::_Prelude::__0x3A::make(((::_Prelude::__0x28_0x2C_0x2C_0x29*) *(arg1))->arg3, var_10));
  }

  Node* _unzip3_0x2E__0x23selFP5_0x23xs::hfun() { // unzip3._#selFP5#xs
    // VAR (1,4,(IPath (Arg 1)))
    // VAR (2,1,(IPath (Rel 1 ("Prelude","(,,)") 1)))
    // VAR (3,0,(IPath (Rel 1 ("Prelude","(,,)") 2)))
    // VAR (4,0,(IPath (Rel 1 ("Prelude","(,,)") 3)))
    // LHS variable (1,4,(IPath (Arg 1))) is argument 1
    static void* table_5[]
      = {&&fail_5, &&var_5, &&choice_5, &&oper_5, &&__0x28_0x2C_0x2C_0x29_5};
      goto *table_5[(*arg1)->get_kind()];
    fail_5:
      return DO_FAIL;
    var_5:
      // Engine::narrow(arg1, generator());
      throw "No narrowing yet";
      goto *table_5[(*arg1)->get_kind()];
    choice_5:
    oper_5:
      Engine::hfun(arg1);
      goto *table_5[(*arg1)->get_kind()];
    __0x28_0x2C_0x2C_0x29_5: // "(,,)"
      // LHS variable (2,1,(IPath (Rel 1 ("Prelude","(,,)") 1))) is inlined as [(1,4,(IPath (Arg 1))),(2,1,(IPath (Rel 1 ("Prelude","(,,)") 1)))]
      // LHS variable (3,0,(IPath (Rel 1 ("Prelude","(,,)") 2))) is not used
      // LHS variable (4,0,(IPath (Rel 1 ("Prelude","(,,)") 3))) is not used
      return *(((::_Prelude::__0x28_0x2C_0x2C_0x29*) *(arg1))->arg1);
  }

  Node* _unzip3_0x2E__0x23selFP6_0x23ys::hfun() { // unzip3._#selFP6#ys
    // VAR (1,4,(IPath (Arg 1)))
    // VAR (2,0,(IPath (Rel 1 ("Prelude","(,,)") 1)))
    // VAR (3,1,(IPath (Rel 1 ("Prelude","(,,)") 2)))
    // VAR (4,0,(IPath (Rel 1 ("Prelude","(,,)") 3)))
    // LHS variable (1,4,(IPath (Arg 1))) is argument 1
    static void* table_5[]
      = {&&fail_5, &&var_5, &&choice_5, &&oper_5, &&__0x28_0x2C_0x2C_0x29_5};
      goto *table_5[(*arg1)->get_kind()];
    fail_5:
      return DO_FAIL;
    var_5:
      // Engine::narrow(arg1, generator());
      throw "No narrowing yet";
      goto *table_5[(*arg1)->get_kind()];
    choice_5:
    oper_5:
      Engine::hfun(arg1);
      goto *table_5[(*arg1)->get_kind()];
    __0x28_0x2C_0x2C_0x29_5: // "(,,)"
      // LHS variable (2,0,(IPath (Rel 1 ("Prelude","(,,)") 1))) is not used
      // LHS variable (3,1,(IPath (Rel 1 ("Prelude","(,,)") 2))) is inlined as [(1,4,(IPath (Arg 1))),(3,1,(IPath (Rel 1 ("Prelude","(,,)") 2)))]
      // LHS variable (4,0,(IPath (Rel 1 ("Prelude","(,,)") 3))) is not used
      return *(((::_Prelude::__0x28_0x2C_0x2C_0x29*) *(arg1))->arg2);
  }

  Node* _unzip3_0x2E__0x23selFP7_0x23zs::hfun() { // unzip3._#selFP7#zs
    // VAR (1,4,(IPath (Arg 1)))
    // VAR (2,0,(IPath (Rel 1 ("Prelude","(,,)") 1)))
    // VAR (3,0,(IPath (Rel 1 ("Prelude","(,,)") 2)))
    // VAR (4,1,(IPath (Rel 1 ("Prelude","(,,)") 3)))
    // LHS variable (1,4,(IPath (Arg 1))) is argument 1
    static void* table_5[]
      = {&&fail_5, &&var_5, &&choice_5, &&oper_5, &&__0x28_0x2C_0x2C_0x29_5};
      goto *table_5[(*arg1)->get_kind()];
    fail_5:
      return DO_FAIL;
    var_5:
      // Engine::narrow(arg1, generator());
      throw "No narrowing yet";
      goto *table_5[(*arg1)->get_kind()];
    choice_5:
    oper_5:
      Engine::hfun(arg1);
      goto *table_5[(*arg1)->get_kind()];
    __0x28_0x2C_0x2C_0x29_5: // "(,,)"
      // LHS variable (2,0,(IPath (Rel 1 ("Prelude","(,,)") 1))) is not used
      // LHS variable (3,0,(IPath (Rel 1 ("Prelude","(,,)") 2))) is not used
      // LHS variable (4,1,(IPath (Rel 1 ("Prelude","(,,)") 3))) is inlined as [(1,4,(IPath (Arg 1))),(4,1,(IPath (Rel 1 ("Prelude","(,,)") 3)))]
      return *(((::_Prelude::__0x28_0x2C_0x2C_0x29*) *(arg1))->arg3);
  }

  Node* _concat::hfun() { // concat
    // VAR (1,1,(IPath (Arg 1)))
    // LHS variable (1,1,(IPath (Arg 1))) is argument 1
    return new ::_Prelude::_foldr(Engine::Partial::make(::_Prelude::__0x2B_0x2B::make(), 2), ::_Prelude::__0x5B_0x5D::make(), arg1);
  }

  Node* _concatMap::hfun() { // concatMap
    // VAR (1,1,(IPath (Arg 1)))
    // LHS variable (1,1,(IPath (Arg 1))) is argument 1
    return new ::_Prelude::__0x2E(Engine::Partial::make(::_Prelude::_concat::make(), 1), Engine::Partial::make(::_Prelude::_map::make(arg1), 1));
  }

  Node* _iterate::hfun() { // iterate
    // VAR (1,2,(IPath (Arg 1)))
    // VAR (2,2,(IPath (Arg 2)))
    // LHS variable (1,2,(IPath (Arg 1))) is argument 1
    // LHS variable (2,2,(IPath (Arg 2))) is argument 2
    return new ::_Prelude::__0x3A(arg2, ::_Prelude::_iterate::make(arg1, ::_Prelude::_apply::make(arg1, arg2)));
  }

  Node* _repeat::hfun() { // repeat
    // VAR (1,2,(IPath (Arg 1)))
    // LHS variable (1,2,(IPath (Arg 1))) is argument 1
    return new ::_Prelude::__0x3A(arg1, ::_Prelude::_repeat::make(arg1));
  }

  Node* _replicate::hfun() { // replicate
    // VAR (1,1,(IPath (Arg 1)))
    // VAR (2,1,(IPath (Arg 2)))
    // LHS variable (1,1,(IPath (Arg 1))) is argument 1
    // LHS variable (2,1,(IPath (Arg 2))) is argument 2
    return new ::_Prelude::_take(arg1, ::_Prelude::_repeat::make(arg2));
  }

  Node* _take::hfun() { // take
    // VAR (1,2,(IPath (Arg 1)))
    // VAR (2,1,(IPath (Arg 2)))
    // LHS variable (1,2,(IPath (Arg 1))) is argument 1
    // LHS variable (2,1,(IPath (Arg 2))) is argument 2
    static void* table_3[]
      = {&&fail_3, &&var_3, &&choice_3, &&oper_3, &&_False_3, &&_True_3};
    Node** var_3 = ::_Prelude::__0x3C_0x3D::make(arg1, Litint::make(0));
      goto *table_3[(*var_3)->get_kind()];
    fail_3:
      return DO_FAIL;
    var_3:
      // Engine::narrow(var_3, generator());
      throw "No narrowing yet";
      goto *table_3[(*var_3)->get_kind()];
    choice_3:
    oper_3:
      Engine::hfun(var_3);
      goto *table_3[(*var_3)->get_kind()];
    _False_3: // "False"
      return new ::_Prelude::_take_0x2Etakep_0x2E220(arg1, arg2);
    _True_3: // "True"
      return new ::_Prelude::__0x5B_0x5D();
  }

  Node* _take_0x2Etakep_0x2E220::hfun() { // take.takep.220
    // VAR (1,1,(IPath (Arg 1)))
    // VAR (2,5,(IPath (Arg 2)))
    // VAR (3,1,(IPath (Rel 2 ("Prelude",":") 1)))
    // VAR (4,1,(IPath (Rel 2 ("Prelude",":") 2)))
    // LHS variable (1,1,(IPath (Arg 1))) is argument 1
    // LHS variable (2,5,(IPath (Arg 2))) is argument 2
    static void* table_5[]
      = {&&fail_5, &&var_5, &&choice_5, &&oper_5, &&__0x5B_0x5D_5, &&__0x3A_5};
      goto *table_5[(*arg2)->get_kind()];
    fail_5:
      return DO_FAIL;
    var_5:
      // Engine::narrow(arg2, generator());
      throw "No narrowing yet";
      goto *table_5[(*arg2)->get_kind()];
    choice_5:
    oper_5:
      Engine::hfun(arg2);
      goto *table_5[(*arg2)->get_kind()];
    __0x5B_0x5D_5: // "[]"
      return new ::_Prelude::__0x5B_0x5D();
    __0x3A_5: // ":"
      // LHS variable (3,1,(IPath (Rel 2 ("Prelude",":") 1))) is inlined as [(2,5,(IPath (Arg 2))),(3,1,(IPath (Rel 2 ("Prelude",":") 1)))]
      // LHS variable (4,1,(IPath (Rel 2 ("Prelude",":") 2))) is inlined as [(2,5,(IPath (Arg 2))),(4,1,(IPath (Rel 2 ("Prelude",":") 2)))]
      return new ::_Prelude::__0x3A(((::_Prelude::__0x3A*) *(arg2))->arg1, ::_Prelude::_take::make(::_Prelude::__0x2D::make(arg1, Litint::make(1)), ((::_Prelude::__0x3A*) *(arg2))->arg2));
  }

  Node* _drop::hfun() { // drop
    // VAR (1,2,(IPath (Arg 1)))
    // VAR (2,1,(IPath (Arg 2)))
    // LHS variable (1,2,(IPath (Arg 1))) is argument 1
    // LHS variable (2,1,(IPath (Arg 2))) is argument 2
    static void* table_3[]
      = {&&fail_3, &&var_3, &&choice_3, &&oper_3, &&_False_3, &&_True_3};
    Node** var_3 = ::_Prelude::__0x3C_0x3D::make(arg1, Litint::make(0));
      goto *table_3[(*var_3)->get_kind()];
    fail_3:
      return DO_FAIL;
    var_3:
      // Engine::narrow(var_3, generator());
      throw "No narrowing yet";
      goto *table_3[(*var_3)->get_kind()];
    choice_3:
    oper_3:
      Engine::hfun(var_3);
      goto *table_3[(*var_3)->get_kind()];
    _False_3: // "False"
      return new ::_Prelude::_drop_0x2Edropp_0x2E229(arg1, arg2);
    _True_3: // "True"
      return *(arg2);
  }

  Node* _drop_0x2Edropp_0x2E229::hfun() { // drop.dropp.229
    // VAR (1,1,(IPath (Arg 1)))
    // VAR (2,4,(IPath (Arg 2)))
    // VAR (3,0,(IPath (Rel 2 ("Prelude",":") 1)))
    // VAR (4,1,(IPath (Rel 2 ("Prelude",":") 2)))
    // LHS variable (1,1,(IPath (Arg 1))) is argument 1
    // LHS variable (2,4,(IPath (Arg 2))) is argument 2
    static void* table_5[]
      = {&&fail_5, &&var_5, &&choice_5, &&oper_5, &&__0x5B_0x5D_5, &&__0x3A_5};
      goto *table_5[(*arg2)->get_kind()];
    fail_5:
      return DO_FAIL;
    var_5:
      // Engine::narrow(arg2, generator());
      throw "No narrowing yet";
      goto *table_5[(*arg2)->get_kind()];
    choice_5:
    oper_5:
      Engine::hfun(arg2);
      goto *table_5[(*arg2)->get_kind()];
    __0x5B_0x5D_5: // "[]"
      return new ::_Prelude::__0x5B_0x5D();
    __0x3A_5: // ":"
      // LHS variable (3,0,(IPath (Rel 2 ("Prelude",":") 1))) is not used
      // LHS variable (4,1,(IPath (Rel 2 ("Prelude",":") 2))) is inlined as [(2,4,(IPath (Arg 2))),(4,1,(IPath (Rel 2 ("Prelude",":") 2)))]
      return new ::_Prelude::_drop(::_Prelude::__0x2D::make(arg1, Litint::make(1)), ((::_Prelude::__0x3A*) *(arg2))->arg2);
  }

  Node* _splitAt::hfun() { // splitAt
    // VAR (1,2,(IPath (Arg 1)))
    // VAR (2,1,(IPath (Arg 2)))
    // LHS variable (1,2,(IPath (Arg 1))) is argument 1
    // LHS variable (2,1,(IPath (Arg 2))) is argument 2
    static void* table_3[]
      = {&&fail_3, &&var_3, &&choice_3, &&oper_3, &&_False_3, &&_True_3};
    Node** var_3 = ::_Prelude::__0x3C_0x3D::make(arg1, Litint::make(0));
      goto *table_3[(*var_3)->get_kind()];
    fail_3:
      return DO_FAIL;
    var_3:
      // Engine::narrow(var_3, generator());
      throw "No narrowing yet";
      goto *table_3[(*var_3)->get_kind()];
    choice_3:
    oper_3:
      Engine::hfun(var_3);
      goto *table_3[(*var_3)->get_kind()];
    _False_3: // "False"
      return new ::_Prelude::_splitAt_0x2EsplitAtp_0x2E239(arg1, arg2);
    _True_3: // "True"
      return new ::_Prelude::__0x28_0x2C_0x29(::_Prelude::__0x5B_0x5D::make(), arg2);
  }

  Node* _splitAt_0x2EsplitAtp_0x2E239::hfun() { // splitAt.splitAtp.239
    // VAR (1,1,(IPath (Arg 1)))
    // VAR (2,5,(IPath (Arg 2)))
    // VAR (3,1,(IPath (Rel 2 ("Prelude",":") 1)))
    // VAR (4,1,(IPath (Rel 2 ("Prelude",":") 2)))
    // LHS variable (1,1,(IPath (Arg 1))) is argument 1
    // LHS variable (2,5,(IPath (Arg 2))) is argument 2
    static void* table_5[]
      = {&&fail_5, &&var_5, &&choice_5, &&oper_5, &&__0x5B_0x5D_5, &&__0x3A_5};
      goto *table_5[(*arg2)->get_kind()];
    fail_5:
      return DO_FAIL;
    var_5:
      // Engine::narrow(arg2, generator());
      throw "No narrowing yet";
      goto *table_5[(*arg2)->get_kind()];
    choice_5:
    oper_5:
      Engine::hfun(arg2);
      goto *table_5[(*arg2)->get_kind()];
    __0x5B_0x5D_5: // "[]"
      return new ::_Prelude::__0x28_0x2C_0x29(::_Prelude::__0x5B_0x5D::make(), ::_Prelude::__0x5B_0x5D::make());
    __0x3A_5: // ":"
      // LHS variable (3,1,(IPath (Rel 2 ("Prelude",":") 1))) is inlined as [(2,5,(IPath (Arg 2))),(3,1,(IPath (Rel 2 ("Prelude",":") 1)))]
      // LHS variable (4,1,(IPath (Rel 2 ("Prelude",":") 2))) is inlined as [(2,5,(IPath (Arg 2))),(4,1,(IPath (Rel 2 ("Prelude",":") 2)))]
      return new ::_Prelude::_splitAt_0x2EsplitAtp_0x2E239_let__0x231(((::_Prelude::__0x3A*) *(arg2))->arg1, arg1, ((::_Prelude::__0x3A*) *(arg2))->arg2);
  }

  Node* _splitAt_0x2EsplitAtp_0x2E239_let__0x231::hfun() { // splitAt.splitAtp.239_let_#1
    // VAR (3,1,(IPath (Arg 1)))
    // VAR (1,1,(IPath (Arg 2)))
    // VAR (4,1,(IPath (Arg 3)))
    // VAR (5,2,IBind)
    // VAR (6,1,IBind)
    // VAR (7,1,IBind)
    // LHS variable (3,1,(IPath (Arg 1))) is argument 1
    // LHS variable (1,1,(IPath (Arg 2))) is argument 2
    // LHS variable (4,1,(IPath (Arg 3))) is argument 3
    // [(5,[]),(6,[]),(7,[])]
    Node** var_5 = ::_Prelude::_splitAt::make(::_Prelude::__0x2D::make(arg2, Litint::make(1)), arg3);
    Node** var_6 = ::_Prelude::_splitAt_0x2EsplitAtp_0x2E239_0x2E__0x23selFP9_0x23ys::make(var_5);
    Node** var_7 = ::_Prelude::_splitAt_0x2EsplitAtp_0x2E239_0x2E__0x23selFP10_0x23zs::make(var_5);
    return new ::_Prelude::__0x28_0x2C_0x29(::_Prelude::__0x3A::make(arg1, var_6), var_7);
  }

  Node* _splitAt_0x2EsplitAtp_0x2E239_0x2E__0x23selFP9_0x23ys::hfun() { // splitAt.splitAtp.239._#selFP9#ys
    // VAR (1,4,(IPath (Arg 1)))
    // VAR (2,1,(IPath (Rel 1 ("Prelude","(,)") 1)))
    // VAR (3,0,(IPath (Rel 1 ("Prelude","(,)") 2)))
    // LHS variable (1,4,(IPath (Arg 1))) is argument 1
    static void* table_4[]
      = {&&fail_4, &&var_4, &&choice_4, &&oper_4, &&__0x28_0x2C_0x29_4};
      goto *table_4[(*arg1)->get_kind()];
    fail_4:
      return DO_FAIL;
    var_4:
      // Engine::narrow(arg1, generator());
      throw "No narrowing yet";
      goto *table_4[(*arg1)->get_kind()];
    choice_4:
    oper_4:
      Engine::hfun(arg1);
      goto *table_4[(*arg1)->get_kind()];
    __0x28_0x2C_0x29_4: // "(,)"
      // LHS variable (2,1,(IPath (Rel 1 ("Prelude","(,)") 1))) is inlined as [(1,4,(IPath (Arg 1))),(2,1,(IPath (Rel 1 ("Prelude","(,)") 1)))]
      // LHS variable (3,0,(IPath (Rel 1 ("Prelude","(,)") 2))) is not used
      return *(((::_Prelude::__0x28_0x2C_0x29*) *(arg1))->arg1);
  }

  Node* _splitAt_0x2EsplitAtp_0x2E239_0x2E__0x23selFP10_0x23zs::hfun() { // splitAt.splitAtp.239._#selFP10#zs
    // VAR (1,4,(IPath (Arg 1)))
    // VAR (2,0,(IPath (Rel 1 ("Prelude","(,)") 1)))
    // VAR (3,1,(IPath (Rel 1 ("Prelude","(,)") 2)))
    // LHS variable (1,4,(IPath (Arg 1))) is argument 1
    static void* table_4[]
      = {&&fail_4, &&var_4, &&choice_4, &&oper_4, &&__0x28_0x2C_0x29_4};
      goto *table_4[(*arg1)->get_kind()];
    fail_4:
      return DO_FAIL;
    var_4:
      // Engine::narrow(arg1, generator());
      throw "No narrowing yet";
      goto *table_4[(*arg1)->get_kind()];
    choice_4:
    oper_4:
      Engine::hfun(arg1);
      goto *table_4[(*arg1)->get_kind()];
    __0x28_0x2C_0x29_4: // "(,)"
      // LHS variable (2,0,(IPath (Rel 1 ("Prelude","(,)") 1))) is not used
      // LHS variable (3,1,(IPath (Rel 1 ("Prelude","(,)") 2))) is inlined as [(1,4,(IPath (Arg 1))),(3,1,(IPath (Rel 1 ("Prelude","(,)") 2)))]
      return *(((::_Prelude::__0x28_0x2C_0x29*) *(arg1))->arg2);
  }

  Node* _takeWhile::hfun() { // takeWhile
    // VAR (1,2,(IPath (Arg 1)))
    // VAR (2,5,(IPath (Arg 2)))
    // VAR (3,2,(IPath (Rel 2 ("Prelude",":") 1)))
    // VAR (4,1,(IPath (Rel 2 ("Prelude",":") 2)))
    // LHS variable (1,2,(IPath (Arg 1))) is argument 1
    // LHS variable (2,5,(IPath (Arg 2))) is argument 2
    static void* table_5[]
      = {&&fail_5, &&var_5, &&choice_5, &&oper_5, &&__0x5B_0x5D_5, &&__0x3A_5};
      goto *table_5[(*arg2)->get_kind()];
    fail_5:
      return DO_FAIL;
    var_5:
      // Engine::narrow(arg2, generator());
      throw "No narrowing yet";
      goto *table_5[(*arg2)->get_kind()];
    choice_5:
    oper_5:
      Engine::hfun(arg2);
      goto *table_5[(*arg2)->get_kind()];
    __0x5B_0x5D_5: // "[]"
      return new ::_Prelude::__0x5B_0x5D();
    __0x3A_5: // ":"
      Node** var_3 = ((::_Prelude::__0x3A*) *(arg2))->arg1; // [(3,2,(IPath (Rel 2 ("Prelude",":") 1)))] 
      // LHS variable (4,1,(IPath (Rel 2 ("Prelude",":") 2))) is inlined as [(2,5,(IPath (Arg 2))),(4,1,(IPath (Rel 2 ("Prelude",":") 2)))]
      static void* table_6[]
        = {&&fail_6, &&var_6, &&choice_6, &&oper_6, &&_False_6, &&_True_6};
      Node** var_6 = ::_Prelude::_apply::make(arg1, var_3);
        goto *table_6[(*var_6)->get_kind()];
      fail_6:
        return DO_FAIL;
      var_6:
        // Engine::narrow(var_6, generator());
        throw "No narrowing yet";
        goto *table_6[(*var_6)->get_kind()];
      choice_6:
      oper_6:
        Engine::hfun(var_6);
        goto *table_6[(*var_6)->get_kind()];
      _False_6: // "False"
        return new ::_Prelude::__0x5B_0x5D();
      _True_6: // "True"
        return new ::_Prelude::__0x3A(var_3, ::_Prelude::_takeWhile::make(arg1, ((::_Prelude::__0x3A*) *(arg2))->arg2));
  }

  Node* _dropWhile::hfun() { // dropWhile
    // VAR (1,2,(IPath (Arg 1)))
    // VAR (2,5,(IPath (Arg 2)))
    // VAR (3,2,(IPath (Rel 2 ("Prelude",":") 1)))
    // VAR (4,1,(IPath (Rel 2 ("Prelude",":") 2)))
    // LHS variable (1,2,(IPath (Arg 1))) is argument 1
    // LHS variable (2,5,(IPath (Arg 2))) is argument 2
    static void* table_5[]
      = {&&fail_5, &&var_5, &&choice_5, &&oper_5, &&__0x5B_0x5D_5, &&__0x3A_5};
      goto *table_5[(*arg2)->get_kind()];
    fail_5:
      return DO_FAIL;
    var_5:
      // Engine::narrow(arg2, generator());
      throw "No narrowing yet";
      goto *table_5[(*arg2)->get_kind()];
    choice_5:
    oper_5:
      Engine::hfun(arg2);
      goto *table_5[(*arg2)->get_kind()];
    __0x5B_0x5D_5: // "[]"
      return new ::_Prelude::__0x5B_0x5D();
    __0x3A_5: // ":"
      Node** var_3 = ((::_Prelude::__0x3A*) *(arg2))->arg1; // [(3,2,(IPath (Rel 2 ("Prelude",":") 1)))] 
      // LHS variable (4,1,(IPath (Rel 2 ("Prelude",":") 2))) is inlined as [(2,5,(IPath (Arg 2))),(4,1,(IPath (Rel 2 ("Prelude",":") 2)))]
      static void* table_6[]
        = {&&fail_6, &&var_6, &&choice_6, &&oper_6, &&_False_6, &&_True_6};
      Node** var_6 = ::_Prelude::_apply::make(arg1, var_3);
        goto *table_6[(*var_6)->get_kind()];
      fail_6:
        return DO_FAIL;
      var_6:
        // Engine::narrow(var_6, generator());
        throw "No narrowing yet";
        goto *table_6[(*var_6)->get_kind()];
      choice_6:
      oper_6:
        Engine::hfun(var_6);
        goto *table_6[(*var_6)->get_kind()];
      _False_6: // "False"
        return new ::_Prelude::__0x3A(var_3, ((::_Prelude::__0x3A*) *(arg2))->arg2);
      _True_6: // "True"
        return new ::_Prelude::_dropWhile(arg1, ((::_Prelude::__0x3A*) *(arg2))->arg2);
  }

  Node* _span::hfun() { // span
    // VAR (1,1,(IPath (Arg 1)))
    // VAR (2,5,(IPath (Arg 2)))
    // VAR (3,1,(IPath (Rel 2 ("Prelude",":") 1)))
    // VAR (4,1,(IPath (Rel 2 ("Prelude",":") 2)))
    // LHS variable (1,1,(IPath (Arg 1))) is argument 1
    // LHS variable (2,5,(IPath (Arg 2))) is argument 2
    static void* table_5[]
      = {&&fail_5, &&var_5, &&choice_5, &&oper_5, &&__0x5B_0x5D_5, &&__0x3A_5};
      goto *table_5[(*arg2)->get_kind()];
    fail_5:
      return DO_FAIL;
    var_5:
      // Engine::narrow(arg2, generator());
      throw "No narrowing yet";
      goto *table_5[(*arg2)->get_kind()];
    choice_5:
    oper_5:
      Engine::hfun(arg2);
      goto *table_5[(*arg2)->get_kind()];
    __0x5B_0x5D_5: // "[]"
      return new ::_Prelude::__0x28_0x2C_0x29(::_Prelude::__0x5B_0x5D::make(), ::_Prelude::__0x5B_0x5D::make());
    __0x3A_5: // ":"
      // LHS variable (3,1,(IPath (Rel 2 ("Prelude",":") 1))) is inlined as [(2,5,(IPath (Arg 2))),(3,1,(IPath (Rel 2 ("Prelude",":") 1)))]
      // LHS variable (4,1,(IPath (Rel 2 ("Prelude",":") 2))) is inlined as [(2,5,(IPath (Arg 2))),(4,1,(IPath (Rel 2 ("Prelude",":") 2)))]
      return new ::_Prelude::_span_case__0x232(arg1, ((::_Prelude::__0x3A*) *(arg2))->arg1, ((::_Prelude::__0x3A*) *(arg2))->arg2);
  }

  Node* _span_case__0x232::hfun() { // span_case_#2
    // VAR (1,2,(IPath (Arg 1)))
    // VAR (3,2,(IPath (Arg 2)))
    // VAR (4,1,(IPath (Arg 3)))
    // VAR (5,2,IBind)
    // VAR (6,1,IBind)
    // VAR (7,1,IBind)
    // LHS variable (1,2,(IPath (Arg 1))) is argument 1
    // LHS variable (3,2,(IPath (Arg 2))) is argument 2
    // LHS variable (4,1,(IPath (Arg 3))) is argument 3
    static void* table_8[]
      = {&&fail_8, &&var_8, &&choice_8, &&oper_8, &&_False_8, &&_True_8};
    Node** var_8 = ::_Prelude::_apply::make(arg1, arg2);
      goto *table_8[(*var_8)->get_kind()];
    fail_8:
      return DO_FAIL;
    var_8:
      // Engine::narrow(var_8, generator());
      throw "No narrowing yet";
      goto *table_8[(*var_8)->get_kind()];
    choice_8:
    oper_8:
      Engine::hfun(var_8);
      goto *table_8[(*var_8)->get_kind()];
    _False_8: // "False"
      return new ::_Prelude::_span_case__0x231(arg2, arg3);
    _True_8: // "True"
      // [(5,[]),(6,[]),(7,[])]
      Node** var_5 = ::_Prelude::_span::make(arg1, arg3);
      Node** var_6 = ::_Prelude::_span_0x2E__0x23selFP12_0x23ys::make(var_5);
      Node** var_7 = ::_Prelude::_span_0x2E__0x23selFP13_0x23zs::make(var_5);
      return new ::_Prelude::__0x28_0x2C_0x29(::_Prelude::__0x3A::make(arg2, var_6), var_7);
  }

  Node* _span_case__0x231::hfun() { // span_case_#1
    // VAR (3,1,(IPath (Arg 1)))
    // VAR (4,1,(IPath (Arg 2)))
    // LHS variable (3,1,(IPath (Arg 1))) is argument 1
    // LHS variable (4,1,(IPath (Arg 2))) is argument 2
    static void* table_5[]
      = {&&fail_5, &&var_5, &&choice_5, &&oper_5, &&_False_5, &&_True_5};
    Node** var_5 = ::_Prelude::_otherwise::make();
      goto *table_5[(*var_5)->get_kind()];
    fail_5:
      return DO_FAIL;
    var_5:
      // Engine::narrow(var_5, generator());
      throw "No narrowing yet";
      goto *table_5[(*var_5)->get_kind()];
    choice_5:
    oper_5:
      Engine::hfun(var_5);
      goto *table_5[(*var_5)->get_kind()];
    _False_5: // "False"
      return new ::_Prelude::_failed();
    _True_5: // "True"
      return new ::_Prelude::__0x28_0x2C_0x29(::_Prelude::__0x5B_0x5D::make(), ::_Prelude::__0x3A::make(arg1, arg2));
  }

  Node* _span_0x2E__0x23selFP12_0x23ys::hfun() { // span._#selFP12#ys
    // VAR (1,4,(IPath (Arg 1)))
    // VAR (2,1,(IPath (Rel 1 ("Prelude","(,)") 1)))
    // VAR (3,0,(IPath (Rel 1 ("Prelude","(,)") 2)))
    // LHS variable (1,4,(IPath (Arg 1))) is argument 1
    static void* table_4[]
      = {&&fail_4, &&var_4, &&choice_4, &&oper_4, &&__0x28_0x2C_0x29_4};
      goto *table_4[(*arg1)->get_kind()];
    fail_4:
      return DO_FAIL;
    var_4:
      // Engine::narrow(arg1, generator());
      throw "No narrowing yet";
      goto *table_4[(*arg1)->get_kind()];
    choice_4:
    oper_4:
      Engine::hfun(arg1);
      goto *table_4[(*arg1)->get_kind()];
    __0x28_0x2C_0x29_4: // "(,)"
      // LHS variable (2,1,(IPath (Rel 1 ("Prelude","(,)") 1))) is inlined as [(1,4,(IPath (Arg 1))),(2,1,(IPath (Rel 1 ("Prelude","(,)") 1)))]
      // LHS variable (3,0,(IPath (Rel 1 ("Prelude","(,)") 2))) is not used
      return *(((::_Prelude::__0x28_0x2C_0x29*) *(arg1))->arg1);
  }

  Node* _span_0x2E__0x23selFP13_0x23zs::hfun() { // span._#selFP13#zs
    // VAR (1,4,(IPath (Arg 1)))
    // VAR (2,0,(IPath (Rel 1 ("Prelude","(,)") 1)))
    // VAR (3,1,(IPath (Rel 1 ("Prelude","(,)") 2)))
    // LHS variable (1,4,(IPath (Arg 1))) is argument 1
    static void* table_4[]
      = {&&fail_4, &&var_4, &&choice_4, &&oper_4, &&__0x28_0x2C_0x29_4};
      goto *table_4[(*arg1)->get_kind()];
    fail_4:
      return DO_FAIL;
    var_4:
      // Engine::narrow(arg1, generator());
      throw "No narrowing yet";
      goto *table_4[(*arg1)->get_kind()];
    choice_4:
    oper_4:
      Engine::hfun(arg1);
      goto *table_4[(*arg1)->get_kind()];
    __0x28_0x2C_0x29_4: // "(,)"
      // LHS variable (2,0,(IPath (Rel 1 ("Prelude","(,)") 1))) is not used
      // LHS variable (3,1,(IPath (Rel 1 ("Prelude","(,)") 2))) is inlined as [(1,4,(IPath (Arg 1))),(3,1,(IPath (Rel 1 ("Prelude","(,)") 2)))]
      return *(((::_Prelude::__0x28_0x2C_0x29*) *(arg1))->arg2);
  }

  Node* _break::hfun() { // break
    // VAR (1,1,(IPath (Arg 1)))
    // LHS variable (1,1,(IPath (Arg 1))) is argument 1
    return new Engine::Partial(::_Prelude::_span::make(::_Prelude::__0x2E::make(Engine::Partial::make(::_Prelude::_not::make(), 1), arg1)), 1);
  }

  Node* _lines::hfun() { // lines
    // VAR (1,5,(IPath (Arg 1)))
    // VAR (4,2,IBind)
    // VAR (5,1,IBind)
    // VAR (6,1,IBind)
    // VAR (2,1,(IPath (Rel 1 ("Prelude",":") 1)))
    // VAR (3,1,(IPath (Rel 1 ("Prelude",":") 2)))
    // LHS variable (1,5,(IPath (Arg 1))) is argument 1
    static void* table_7[]
      = {&&fail_7, &&var_7, &&choice_7, &&oper_7, &&__0x5B_0x5D_7, &&__0x3A_7};
      goto *table_7[(*arg1)->get_kind()];
    fail_7:
      return DO_FAIL;
    var_7:
      // Engine::narrow(arg1, generator());
      throw "No narrowing yet";
      goto *table_7[(*arg1)->get_kind()];
    choice_7:
    oper_7:
      Engine::hfun(arg1);
      goto *table_7[(*arg1)->get_kind()];
    __0x5B_0x5D_7: // "[]"
      return new ::_Prelude::__0x5B_0x5D();
    __0x3A_7: // ":"
      // LHS variable (2,1,(IPath (Rel 1 ("Prelude",":") 1))) is inlined as [(1,5,(IPath (Arg 1))),(2,1,(IPath (Rel 1 ("Prelude",":") 1)))]
      // LHS variable (3,1,(IPath (Rel 1 ("Prelude",":") 2))) is inlined as [(1,5,(IPath (Arg 1))),(3,1,(IPath (Rel 1 ("Prelude",":") 2)))]
      // [(4,[]),(5,[]),(6,[])]
      Node** var_4 = ::_Prelude::_lines_0x2Esplitline_0x2E271::make(::_Prelude::__0x3A::make(((::_Prelude::__0x3A*) *(arg1))->arg1, ((::_Prelude::__0x3A*) *(arg1))->arg2));
      Node** var_5 = ::_Prelude::_lines_0x2E__0x23selFP18_0x23l::make(var_4);
      Node** var_6 = ::_Prelude::_lines_0x2E__0x23selFP19_0x23xs_l::make(var_4);
      return new ::_Prelude::__0x3A(var_5, ::_Prelude::_lines::make(var_6));
  }

  Node* _lines_0x2Esplitline_0x2E271::hfun() { // lines.splitline.271
    // VAR (1,5,(IPath (Arg 1)))
    // VAR (2,1,(IPath (Rel 1 ("Prelude",":") 1)))
    // VAR (3,1,(IPath (Rel 1 ("Prelude",":") 2)))
    // LHS variable (1,5,(IPath (Arg 1))) is argument 1
    static void* table_4[]
      = {&&fail_4, &&var_4, &&choice_4, &&oper_4, &&__0x5B_0x5D_4, &&__0x3A_4};
      goto *table_4[(*arg1)->get_kind()];
    fail_4:
      return DO_FAIL;
    var_4:
      // Engine::narrow(arg1, generator());
      throw "No narrowing yet";
      goto *table_4[(*arg1)->get_kind()];
    choice_4:
    oper_4:
      Engine::hfun(arg1);
      goto *table_4[(*arg1)->get_kind()];
    __0x5B_0x5D_4: // "[]"
      return new ::_Prelude::__0x28_0x2C_0x29(::_Prelude::__0x5B_0x5D::make(), ::_Prelude::__0x5B_0x5D::make());
    __0x3A_4: // ":"
      // LHS variable (2,1,(IPath (Rel 1 ("Prelude",":") 1))) is inlined as [(1,5,(IPath (Arg 1))),(2,1,(IPath (Rel 1 ("Prelude",":") 1)))]
      // LHS variable (3,1,(IPath (Rel 1 ("Prelude",":") 2))) is inlined as [(1,5,(IPath (Arg 1))),(3,1,(IPath (Rel 1 ("Prelude",":") 2)))]
      return new ::_Prelude::_lines_0x2Esplitline_0x2E271_case__0x231(((::_Prelude::__0x3A*) *(arg1))->arg1, ((::_Prelude::__0x3A*) *(arg1))->arg2);
  }

  Node* _lines_0x2Esplitline_0x2E271_case__0x231::hfun() { // lines.splitline.271_case_#1
    // VAR (2,2,(IPath (Arg 1)))
    // VAR (3,1,(IPath (Arg 2)))
    // LHS variable (2,2,(IPath (Arg 1))) is argument 1
    // LHS variable (3,1,(IPath (Arg 2))) is argument 2
    static void* table_4[]
      = {&&fail_4, &&var_4, &&choice_4, &&oper_4, &&_False_4, &&_True_4};
    Node** var_4 = ::_Prelude::__0x3D_0x3D::make(arg1, Litchar::Litchar::make('\n'));
      goto *table_4[(*var_4)->get_kind()];
    fail_4:
      return DO_FAIL;
    var_4:
      // Engine::narrow(var_4, generator());
      throw "No narrowing yet";
      goto *table_4[(*var_4)->get_kind()];
    choice_4:
    oper_4:
      Engine::hfun(var_4);
      goto *table_4[(*var_4)->get_kind()];
    _False_4: // "False"
      return new ::_Prelude::_lines_0x2Esplitline_0x2E271_case__0x231_let__0x231(arg1, arg2);
    _True_4: // "True"
      return new ::_Prelude::__0x28_0x2C_0x29(::_Prelude::__0x5B_0x5D::make(), arg2);
  }

  Node* _lines_0x2Esplitline_0x2E271_case__0x231_let__0x231::hfun() { // lines.splitline.271_case_#1_let_#1
    // VAR (2,1,(IPath (Arg 1)))
    // VAR (3,1,(IPath (Arg 2)))
    // VAR (4,2,IBind)
    // VAR (5,1,IBind)
    // VAR (6,1,IBind)
    // LHS variable (2,1,(IPath (Arg 1))) is argument 1
    // LHS variable (3,1,(IPath (Arg 2))) is argument 2
    // [(4,[]),(5,[]),(6,[])]
    Node** var_4 = ::_Prelude::_lines_0x2Esplitline_0x2E271::make(arg2);
    Node** var_5 = ::_Prelude::_lines_0x2Esplitline_0x2E271_0x2E__0x23selFP15_0x23ds::make(var_4);
    Node** var_6 = ::_Prelude::_lines_0x2Esplitline_0x2E271_0x2E__0x23selFP16_0x23es::make(var_4);
    return new ::_Prelude::__0x28_0x2C_0x29(::_Prelude::__0x3A::make(arg1, var_5), var_6);
  }

  Node* _lines_0x2Esplitline_0x2E271_0x2E__0x23selFP15_0x23ds::hfun() { // lines.splitline.271._#selFP15#ds
    // VAR (1,4,(IPath (Arg 1)))
    // VAR (2,1,(IPath (Rel 1 ("Prelude","(,)") 1)))
    // VAR (3,0,(IPath (Rel 1 ("Prelude","(,)") 2)))
    // LHS variable (1,4,(IPath (Arg 1))) is argument 1
    static void* table_4[]
      = {&&fail_4, &&var_4, &&choice_4, &&oper_4, &&__0x28_0x2C_0x29_4};
      goto *table_4[(*arg1)->get_kind()];
    fail_4:
      return DO_FAIL;
    var_4:
      // Engine::narrow(arg1, generator());
      throw "No narrowing yet";
      goto *table_4[(*arg1)->get_kind()];
    choice_4:
    oper_4:
      Engine::hfun(arg1);
      goto *table_4[(*arg1)->get_kind()];
    __0x28_0x2C_0x29_4: // "(,)"
      // LHS variable (2,1,(IPath (Rel 1 ("Prelude","(,)") 1))) is inlined as [(1,4,(IPath (Arg 1))),(2,1,(IPath (Rel 1 ("Prelude","(,)") 1)))]
      // LHS variable (3,0,(IPath (Rel 1 ("Prelude","(,)") 2))) is not used
      return *(((::_Prelude::__0x28_0x2C_0x29*) *(arg1))->arg1);
  }

  Node* _lines_0x2Esplitline_0x2E271_0x2E__0x23selFP16_0x23es::hfun() { // lines.splitline.271._#selFP16#es
    // VAR (1,4,(IPath (Arg 1)))
    // VAR (2,0,(IPath (Rel 1 ("Prelude","(,)") 1)))
    // VAR (3,1,(IPath (Rel 1 ("Prelude","(,)") 2)))
    // LHS variable (1,4,(IPath (Arg 1))) is argument 1
    static void* table_4[]
      = {&&fail_4, &&var_4, &&choice_4, &&oper_4, &&__0x28_0x2C_0x29_4};
      goto *table_4[(*arg1)->get_kind()];
    fail_4:
      return DO_FAIL;
    var_4:
      // Engine::narrow(arg1, generator());
      throw "No narrowing yet";
      goto *table_4[(*arg1)->get_kind()];
    choice_4:
    oper_4:
      Engine::hfun(arg1);
      goto *table_4[(*arg1)->get_kind()];
    __0x28_0x2C_0x29_4: // "(,)"
      // LHS variable (2,0,(IPath (Rel 1 ("Prelude","(,)") 1))) is not used
      // LHS variable (3,1,(IPath (Rel 1 ("Prelude","(,)") 2))) is inlined as [(1,4,(IPath (Arg 1))),(3,1,(IPath (Rel 1 ("Prelude","(,)") 2)))]
      return *(((::_Prelude::__0x28_0x2C_0x29*) *(arg1))->arg2);
  }

  Node* _lines_0x2E__0x23selFP18_0x23l::hfun() { // lines._#selFP18#l
    // VAR (1,4,(IPath (Arg 1)))
    // VAR (2,1,(IPath (Rel 1 ("Prelude","(,)") 1)))
    // VAR (3,0,(IPath (Rel 1 ("Prelude","(,)") 2)))
    // LHS variable (1,4,(IPath (Arg 1))) is argument 1
    static void* table_4[]
      = {&&fail_4, &&var_4, &&choice_4, &&oper_4, &&__0x28_0x2C_0x29_4};
      goto *table_4[(*arg1)->get_kind()];
    fail_4:
      return DO_FAIL;
    var_4:
      // Engine::narrow(arg1, generator());
      throw "No narrowing yet";
      goto *table_4[(*arg1)->get_kind()];
    choice_4:
    oper_4:
      Engine::hfun(arg1);
      goto *table_4[(*arg1)->get_kind()];
    __0x28_0x2C_0x29_4: // "(,)"
      // LHS variable (2,1,(IPath (Rel 1 ("Prelude","(,)") 1))) is inlined as [(1,4,(IPath (Arg 1))),(2,1,(IPath (Rel 1 ("Prelude","(,)") 1)))]
      // LHS variable (3,0,(IPath (Rel 1 ("Prelude","(,)") 2))) is not used
      return *(((::_Prelude::__0x28_0x2C_0x29*) *(arg1))->arg1);
  }

  Node* _lines_0x2E__0x23selFP19_0x23xs_l::hfun() { // lines._#selFP19#xs_l
    // VAR (1,4,(IPath (Arg 1)))
    // VAR (2,0,(IPath (Rel 1 ("Prelude","(,)") 1)))
    // VAR (3,1,(IPath (Rel 1 ("Prelude","(,)") 2)))
    // LHS variable (1,4,(IPath (Arg 1))) is argument 1
    static void* table_4[]
      = {&&fail_4, &&var_4, &&choice_4, &&oper_4, &&__0x28_0x2C_0x29_4};
      goto *table_4[(*arg1)->get_kind()];
    fail_4:
      return DO_FAIL;
    var_4:
      // Engine::narrow(arg1, generator());
      throw "No narrowing yet";
      goto *table_4[(*arg1)->get_kind()];
    choice_4:
    oper_4:
      Engine::hfun(arg1);
      goto *table_4[(*arg1)->get_kind()];
    __0x28_0x2C_0x29_4: // "(,)"
      // LHS variable (2,0,(IPath (Rel 1 ("Prelude","(,)") 1))) is not used
      // LHS variable (3,1,(IPath (Rel 1 ("Prelude","(,)") 2))) is inlined as [(1,4,(IPath (Arg 1))),(3,1,(IPath (Rel 1 ("Prelude","(,)") 2)))]
      return *(((::_Prelude::__0x28_0x2C_0x29*) *(arg1))->arg2);
  }

  Node* _unlines::hfun() { // unlines
    // VAR (1,1,(IPath (Arg 1)))
    // LHS variable (1,1,(IPath (Arg 1))) is argument 1
    return new ::_Prelude::_apply(::_Prelude::_concatMap::make(Engine::Partial::make(::_Prelude::_flip::make(Engine::Partial::make(::_Prelude::__0x2B_0x2B::make(), 2), ::_Prelude::__0x3A::make(Litchar::Litchar::make('\n'), ::_Prelude::__0x5B_0x5D::make())), 1)), arg1);
  }

  Node* _words::hfun() { // words
    // VAR (1,1,(IPath (Arg 1)))
    // VAR (2,1,IBind)
    // LHS variable (1,1,(IPath (Arg 1))) is argument 1
    // [(2,[])]
    Node** var_2 = ::_Prelude::_dropWhile::make(Engine::Partial::make(::_Prelude::_words_0x2EisSpace_0x2E283::make(), 1), arg1);
    return new ::_Prelude::_words_case__0x231(var_2);
  }

  Node* _words_case__0x231::hfun() { // words_case_#1
    // VAR (2,2,(IPath (Arg 1)))
    // VAR (3,2,IBind)
    // VAR (4,1,IBind)
    // VAR (5,1,IBind)
    // LHS variable (2,2,(IPath (Arg 1))) is argument 1
    static void* table_6[]
      = {&&fail_6, &&var_6, &&choice_6, &&oper_6, &&_False_6, &&_True_6};
    Node** var_6 = ::_Prelude::__0x3D_0x3D::make(arg1, ::_Prelude::__0x5B_0x5D::make());
      goto *table_6[(*var_6)->get_kind()];
    fail_6:
      return DO_FAIL;
    var_6:
      // Engine::narrow(var_6, generator());
      throw "No narrowing yet";
      goto *table_6[(*var_6)->get_kind()];
    choice_6:
    oper_6:
      Engine::hfun(var_6);
      goto *table_6[(*var_6)->get_kind()];
    _False_6: // "False"
      // [(3,[]),(4,[]),(5,[])]
      Node** var_3 = ::_Prelude::_apply::make(::_Prelude::_break::make(Engine::Partial::make(::_Prelude::_words_0x2EisSpace_0x2E283::make(), 1)), arg1);
      Node** var_4 = ::_Prelude::_words_0x2E__0x23selFP21_0x23w::make(var_3);
      Node** var_5 = ::_Prelude::_words_0x2E__0x23selFP22_0x23s2::make(var_3);
      return new ::_Prelude::__0x3A(var_4, ::_Prelude::_words::make(var_5));
    _True_6: // "True"
      return new ::_Prelude::__0x5B_0x5D();
  }

  Node* _words_0x2EisSpace_0x2E283::hfun() { // words.isSpace.283
    // VAR (1,4,(IPath (Arg 1)))
    // LHS variable (1,4,(IPath (Arg 1))) is argument 1
    return new ::_Prelude::__0x7C_0x7C(::_Prelude::__0x3D_0x3D::make(arg1, Litchar::Litchar::make(' ')), ::_Prelude::__0x7C_0x7C::make(::_Prelude::__0x3D_0x3D::make(arg1, Litchar::Litchar::make('\t')), ::_Prelude::__0x7C_0x7C::make(::_Prelude::__0x3D_0x3D::make(arg1, Litchar::Litchar::make('\n')), ::_Prelude::__0x3D_0x3D::make(arg1, Litchar::Litchar::make('\r')))));
  }

  Node* _words_0x2E__0x23selFP21_0x23w::hfun() { // words._#selFP21#w
    // VAR (1,4,(IPath (Arg 1)))
    // VAR (2,1,(IPath (Rel 1 ("Prelude","(,)") 1)))
    // VAR (3,0,(IPath (Rel 1 ("Prelude","(,)") 2)))
    // LHS variable (1,4,(IPath (Arg 1))) is argument 1
    static void* table_4[]
      = {&&fail_4, &&var_4, &&choice_4, &&oper_4, &&__0x28_0x2C_0x29_4};
      goto *table_4[(*arg1)->get_kind()];
    fail_4:
      return DO_FAIL;
    var_4:
      // Engine::narrow(arg1, generator());
      throw "No narrowing yet";
      goto *table_4[(*arg1)->get_kind()];
    choice_4:
    oper_4:
      Engine::hfun(arg1);
      goto *table_4[(*arg1)->get_kind()];
    __0x28_0x2C_0x29_4: // "(,)"
      // LHS variable (2,1,(IPath (Rel 1 ("Prelude","(,)") 1))) is inlined as [(1,4,(IPath (Arg 1))),(2,1,(IPath (Rel 1 ("Prelude","(,)") 1)))]
      // LHS variable (3,0,(IPath (Rel 1 ("Prelude","(,)") 2))) is not used
      return *(((::_Prelude::__0x28_0x2C_0x29*) *(arg1))->arg1);
  }

  Node* _words_0x2E__0x23selFP22_0x23s2::hfun() { // words._#selFP22#s2
    // VAR (1,4,(IPath (Arg 1)))
    // VAR (2,0,(IPath (Rel 1 ("Prelude","(,)") 1)))
    // VAR (3,1,(IPath (Rel 1 ("Prelude","(,)") 2)))
    // LHS variable (1,4,(IPath (Arg 1))) is argument 1
    static void* table_4[]
      = {&&fail_4, &&var_4, &&choice_4, &&oper_4, &&__0x28_0x2C_0x29_4};
      goto *table_4[(*arg1)->get_kind()];
    fail_4:
      return DO_FAIL;
    var_4:
      // Engine::narrow(arg1, generator());
      throw "No narrowing yet";
      goto *table_4[(*arg1)->get_kind()];
    choice_4:
    oper_4:
      Engine::hfun(arg1);
      goto *table_4[(*arg1)->get_kind()];
    __0x28_0x2C_0x29_4: // "(,)"
      // LHS variable (2,0,(IPath (Rel 1 ("Prelude","(,)") 1))) is not used
      // LHS variable (3,1,(IPath (Rel 1 ("Prelude","(,)") 2))) is inlined as [(1,4,(IPath (Arg 1))),(3,1,(IPath (Rel 1 ("Prelude","(,)") 2)))]
      return *(((::_Prelude::__0x28_0x2C_0x29*) *(arg1))->arg2);
  }

  Node* _unwords::hfun() { // unwords
    // VAR (1,2,(IPath (Arg 1)))
    // LHS variable (1,2,(IPath (Arg 1))) is argument 1
    static void* table_2[]
      = {&&fail_2, &&var_2, &&choice_2, &&oper_2, &&_False_2, &&_True_2};
    Node** var_2 = ::_Prelude::__0x3D_0x3D::make(arg1, ::_Prelude::__0x5B_0x5D::make());
      goto *table_2[(*var_2)->get_kind()];
    fail_2:
      return DO_FAIL;
    var_2:
      // Engine::narrow(var_2, generator());
      throw "No narrowing yet";
      goto *table_2[(*var_2)->get_kind()];
    choice_2:
    oper_2:
      Engine::hfun(var_2);
      goto *table_2[(*var_2)->get_kind()];
    _False_2: // "False"
      return new ::_Prelude::_foldr1(Engine::Partial::make(::_Prelude::_unwords_0x2E__0x23lambda5::make(), 2), arg1);
    _True_2: // "True"
      return new ::_Prelude::__0x5B_0x5D();
  }

  Node* _unwords_0x2E__0x23lambda5::hfun() { // unwords._#lambda5
    // VAR (1,1,(IPath (Arg 1)))
    // VAR (2,1,(IPath (Arg 2)))
    // LHS variable (1,1,(IPath (Arg 1))) is argument 1
    // LHS variable (2,1,(IPath (Arg 2))) is argument 2
    return new ::_Prelude::__0x2B_0x2B(arg1, ::_Prelude::__0x3A::make(Litchar::Litchar::make(' '), arg2));
  }

  Node* _reverse::hfun() { // reverse
    return new Engine::Partial(::_Prelude::_foldl::make(Engine::Partial::make(::_Prelude::_flip::make(Engine::Partial::make(::_Prelude::__0x3A::make(), 2)), 2), ::_Prelude::__0x5B_0x5D::make()), 1);
  }

  Node* _and::hfun() { // and
    return new Engine::Partial(::_Prelude::_foldr::make(Engine::Partial::make(::_Prelude::__0x26_0x26::make(), 2), ::_Prelude::_True::make()), 1);
  }

  Node* _or::hfun() { // or
    return new Engine::Partial(::_Prelude::_foldr::make(Engine::Partial::make(::_Prelude::__0x7C_0x7C::make(), 2), ::_Prelude::_False::make()), 1);
  }

  Node* _any::hfun() { // any
    // VAR (1,1,(IPath (Arg 1)))
    // LHS variable (1,1,(IPath (Arg 1))) is argument 1
    return new ::_Prelude::__0x2E(::_Prelude::_or::make(), Engine::Partial::make(::_Prelude::_map::make(arg1), 1));
  }

  Node* _all::hfun() { // all
    // VAR (1,1,(IPath (Arg 1)))
    // LHS variable (1,1,(IPath (Arg 1))) is argument 1
    return new ::_Prelude::__0x2E(::_Prelude::_and::make(), Engine::Partial::make(::_Prelude::_map::make(arg1), 1));
  }

  Node* _elem::hfun() { // elem
    // VAR (1,1,(IPath (Arg 1)))
    // LHS variable (1,1,(IPath (Arg 1))) is argument 1
    return new ::_Prelude::_any(Engine::Partial::make(::_Prelude::__0x3D_0x3D::make(arg1), 1));
  }

  Node* _notElem::hfun() { // notElem
    // VAR (1,1,(IPath (Arg 1)))
    // LHS variable (1,1,(IPath (Arg 1))) is argument 1
    return new ::_Prelude::_all(Engine::Partial::make(::_Prelude::__0x2F_0x3D::make(arg1), 1));
  }

  Node* _lookup::hfun() { // lookup
    // VAR (1,2,(IPath (Arg 1)))
    // VAR (2,5,(IPath (Arg 2)))
    // VAR (5,1,(IPath (Rel 3 ("Prelude","(,)") 1)))
    // VAR (6,1,(IPath (Rel 3 ("Prelude","(,)") 2)))
    // VAR (3,5,(IPath (Rel 2 ("Prelude",":") 1)))
    // VAR (4,1,(IPath (Rel 2 ("Prelude",":") 2)))
    // LHS variable (1,2,(IPath (Arg 1))) is argument 1
    // LHS variable (2,5,(IPath (Arg 2))) is argument 2
    static void* table_7[]
      = {&&fail_7, &&var_7, &&choice_7, &&oper_7, &&__0x5B_0x5D_7, &&__0x3A_7};
      goto *table_7[(*arg2)->get_kind()];
    fail_7:
      return DO_FAIL;
    var_7:
      // Engine::narrow(arg2, generator());
      throw "No narrowing yet";
      goto *table_7[(*arg2)->get_kind()];
    choice_7:
    oper_7:
      Engine::hfun(arg2);
      goto *table_7[(*arg2)->get_kind()];
    __0x5B_0x5D_7: // "[]"
      return new ::_Prelude::_Nothing();
    __0x3A_7: // ":"
      Node** var_3 = ((::_Prelude::__0x3A*) *(arg2))->arg1; // [(3,5,(IPath (Rel 2 ("Prelude",":") 1)))] 
      // LHS variable (4,1,(IPath (Rel 2 ("Prelude",":") 2))) is inlined as [(2,5,(IPath (Arg 2))),(4,1,(IPath (Rel 2 ("Prelude",":") 2)))]
      static void* table_8[]
        = {&&fail_8, &&var_8, &&choice_8, &&oper_8, &&__0x28_0x2C_0x29_8};
        goto *table_8[(*var_3)->get_kind()];
      fail_8:
        return DO_FAIL;
      var_8:
        // Engine::narrow(var_3, generator());
        throw "No narrowing yet";
        goto *table_8[(*var_3)->get_kind()];
      choice_8:
      oper_8:
        Engine::hfun(var_3);
        goto *table_8[(*var_3)->get_kind()];
      __0x28_0x2C_0x29_8: // "(,)"
        // LHS variable (5,1,(IPath (Rel 3 ("Prelude","(,)") 1))) is inlined as [(3,5,(IPath (Rel 2 ("Prelude",":") 1))),(5,1,(IPath (Rel 3 ("Prelude","(,)") 1)))]
        // LHS variable (6,1,(IPath (Rel 3 ("Prelude","(,)") 2))) is inlined as [(3,5,(IPath (Rel 2 ("Prelude",":") 1))),(6,1,(IPath (Rel 3 ("Prelude","(,)") 2)))]
        static void* table_9[]
          = {&&fail_9, &&var_9, &&choice_9, &&oper_9, &&_False_9, &&_True_9};
        Node** var_9 = ::_Prelude::__0x3D_0x3D::make(arg1, ((::_Prelude::__0x28_0x2C_0x29*) *(((::_Prelude::__0x3A*) *(arg2))->arg1))->arg1);
          goto *table_9[(*var_9)->get_kind()];
        fail_9:
          return DO_FAIL;
        var_9:
          // Engine::narrow(var_9, generator());
          throw "No narrowing yet";
          goto *table_9[(*var_9)->get_kind()];
        choice_9:
        oper_9:
          Engine::hfun(var_9);
          goto *table_9[(*var_9)->get_kind()];
        _False_9: // "False"
          return new ::_Prelude::_lookup_case__0x231(arg1, ((::_Prelude::__0x3A*) *(arg2))->arg2);
        _True_9: // "True"
          return new ::_Prelude::_Just(((::_Prelude::__0x28_0x2C_0x29*) *(((::_Prelude::__0x3A*) *(arg2))->arg1))->arg2);
  }

  Node* _lookup_case__0x231::hfun() { // lookup_case_#1
    // VAR (1,1,(IPath (Arg 1)))
    // VAR (4,1,(IPath (Arg 2)))
    // LHS variable (1,1,(IPath (Arg 1))) is argument 1
    // LHS variable (4,1,(IPath (Arg 2))) is argument 2
    static void* table_5[]
      = {&&fail_5, &&var_5, &&choice_5, &&oper_5, &&_False_5, &&_True_5};
    Node** var_5 = ::_Prelude::_otherwise::make();
      goto *table_5[(*var_5)->get_kind()];
    fail_5:
      return DO_FAIL;
    var_5:
      // Engine::narrow(var_5, generator());
      throw "No narrowing yet";
      goto *table_5[(*var_5)->get_kind()];
    choice_5:
    oper_5:
      Engine::hfun(var_5);
      goto *table_5[(*var_5)->get_kind()];
    _False_5: // "False"
      return new ::_Prelude::_failed();
    _True_5: // "True"
      return new ::_Prelude::_lookup(arg1, arg2);
  }

  Node* _enumFrom::hfun() { // enumFrom
    // VAR (1,2,(IPath (Arg 1)))
    // LHS variable (1,2,(IPath (Arg 1))) is argument 1
    return new ::_Prelude::__0x3A(arg1, ::_Prelude::_enumFrom::make(::_Prelude::__0x2B::make(arg1, Litint::make(1))));
  }

  Node* _enumFromThen::hfun() { // enumFromThen
    // VAR (1,2,(IPath (Arg 1)))
    // VAR (2,1,(IPath (Arg 2)))
    // LHS variable (1,2,(IPath (Arg 1))) is argument 1
    // LHS variable (2,1,(IPath (Arg 2))) is argument 2
    return new ::_Prelude::_iterate(Engine::Partial::make(::_Prelude::__0x2B::make(::_Prelude::__0x2D::make(arg2, arg1)), 1), arg1);
  }

  Node* _enumFromTo::hfun() { // enumFromTo
    // VAR (1,3,(IPath (Arg 1)))
    // VAR (2,2,(IPath (Arg 2)))
    // LHS variable (1,3,(IPath (Arg 1))) is argument 1
    // LHS variable (2,2,(IPath (Arg 2))) is argument 2
    static void* table_3[]
      = {&&fail_3, &&var_3, &&choice_3, &&oper_3, &&_False_3, &&_True_3};
    Node** var_3 = ::_Prelude::__0x3E::make(arg1, arg2);
      goto *table_3[(*var_3)->get_kind()];
    fail_3:
      return DO_FAIL;
    var_3:
      // Engine::narrow(var_3, generator());
      throw "No narrowing yet";
      goto *table_3[(*var_3)->get_kind()];
    choice_3:
    oper_3:
      Engine::hfun(var_3);
      goto *table_3[(*var_3)->get_kind()];
    _False_3: // "False"
      return new ::_Prelude::__0x3A(arg1, ::_Prelude::_enumFromTo::make(::_Prelude::__0x2B::make(arg1, Litint::make(1)), arg2));
    _True_3: // "True"
      return new ::_Prelude::__0x5B_0x5D();
  }

  Node* _enumFromThenTo::hfun() { // enumFromThenTo
    // VAR (1,2,(IPath (Arg 1)))
    // VAR (2,2,(IPath (Arg 2)))
    // VAR (3,1,(IPath (Arg 3)))
    // LHS variable (1,2,(IPath (Arg 1))) is argument 1
    // LHS variable (2,2,(IPath (Arg 2))) is argument 2
    // LHS variable (3,1,(IPath (Arg 3))) is argument 3
    return new ::_Prelude::_takeWhile(Engine::Partial::make(::_Prelude::_enumFromThenTo_0x2Ep_0x2E321::make(arg3, arg1, arg2), 1), ::_Prelude::_enumFromThen::make(arg1, arg2));
  }

  Node* _enumFromThenTo_0x2Ep_0x2E321::hfun() { // enumFromThenTo.p.321
    // VAR (1,1,(IPath (Arg 1)))
    // VAR (2,1,(IPath (Arg 2)))
    // VAR (3,1,(IPath (Arg 3)))
    // VAR (4,1,(IPath (Arg 4)))
    // LHS variable (1,1,(IPath (Arg 1))) is argument 1
    // LHS variable (2,1,(IPath (Arg 2))) is argument 2
    // LHS variable (3,1,(IPath (Arg 3))) is argument 3
    // LHS variable (4,1,(IPath (Arg 4))) is argument 4
    static void* table_5[]
      = {&&fail_5, &&var_5, &&choice_5, &&oper_5, &&_False_5, &&_True_5};
    Node** var_5 = ::_Prelude::__0x3E_0x3D::make(arg3, arg2);
      goto *table_5[(*var_5)->get_kind()];
    fail_5:
      return DO_FAIL;
    var_5:
      // Engine::narrow(var_5, generator());
      throw "No narrowing yet";
      goto *table_5[(*var_5)->get_kind()];
    choice_5:
    oper_5:
      Engine::hfun(var_5);
      goto *table_5[(*var_5)->get_kind()];
    _False_5: // "False"
      return new ::_Prelude::_enumFromThenTo_0x2Ep_0x2E321_case__0x231(arg4, arg1);
    _True_5: // "True"
      return new ::_Prelude::__0x3C_0x3D(arg4, arg1);
  }

  Node* _enumFromThenTo_0x2Ep_0x2E321_case__0x231::hfun() { // enumFromThenTo.p.321_case_#1
    // VAR (4,1,(IPath (Arg 1)))
    // VAR (1,1,(IPath (Arg 2)))
    // LHS variable (4,1,(IPath (Arg 1))) is argument 1
    // LHS variable (1,1,(IPath (Arg 2))) is argument 2
    static void* table_5[]
      = {&&fail_5, &&var_5, &&choice_5, &&oper_5, &&_False_5, &&_True_5};
    Node** var_5 = ::_Prelude::_otherwise::make();
      goto *table_5[(*var_5)->get_kind()];
    fail_5:
      return DO_FAIL;
    var_5:
      // Engine::narrow(var_5, generator());
      throw "No narrowing yet";
      goto *table_5[(*var_5)->get_kind()];
    choice_5:
    oper_5:
      Engine::hfun(var_5);
      goto *table_5[(*var_5)->get_kind()];
    _False_5: // "False"
      return new ::_Prelude::_failed();
    _True_5: // "True"
      return new ::_Prelude::__0x3E_0x3D(arg1, arg2);
  }

  // external Node* _ord::hfun() { throw "External \"Prelude.ord\" not implemented"; }

  // external Node* _chr::hfun() { throw "External \"Prelude.chr\" not implemented"; }

  // external Node* __0x2B::hfun() { throw "External \"Prelude.+\" not implemented"; }

  // external Node* __0x2D::hfun() { throw "External \"Prelude.-\" not implemented"; }

  // external Node* __0x2A::hfun() { throw "External \"Prelude.*\" not implemented"; }

  // external Node* _div::hfun() { throw "External \"Prelude.div\" not implemented"; }

  // external Node* _mod::hfun() { throw "External \"Prelude.mod\" not implemented"; }

  Node* _divMod::hfun() { // divMod
    // VAR (1,2,(IPath (Arg 1)))
    // VAR (2,2,(IPath (Arg 2)))
    // LHS variable (1,2,(IPath (Arg 1))) is argument 1
    // LHS variable (2,2,(IPath (Arg 2))) is argument 2
    return new ::_Prelude::__0x28_0x2C_0x29(::_Prelude::_div::make(arg1, arg2), ::_Prelude::_mod::make(arg1, arg2));
  }

  // external Node* _quot::hfun() { throw "External \"Prelude.quot\" not implemented"; }

  // external Node* _rem::hfun() { throw "External \"Prelude.rem\" not implemented"; }

  Node* _quotRem::hfun() { // quotRem
    // VAR (1,2,(IPath (Arg 1)))
    // VAR (2,2,(IPath (Arg 2)))
    // LHS variable (1,2,(IPath (Arg 1))) is argument 1
    // LHS variable (2,2,(IPath (Arg 2))) is argument 2
    return new ::_Prelude::__0x28_0x2C_0x29(::_Prelude::_quot::make(arg1, arg2), ::_Prelude::_rem::make(arg1, arg2));
  }

  Node* _negate::hfun() { // negate
    // VAR (1,1,(IPath (Arg 1)))
    // LHS variable (1,1,(IPath (Arg 1))) is argument 1
    return new ::_Prelude::__0x2D(Litint::make(0), arg1);
  }

  Node* _negateFloat::hfun() { // negateFloat
    // VAR (1,1,(IPath (Arg 1)))
    // LHS variable (1,1,(IPath (Arg 1))) is argument 1
    return new ::_Prelude::__0x24_0x23(Engine::Partial::make(::_Prelude::_prim_negateFloat::make(), 1), arg1);
  }

  // external Node* _prim_negateFloat::hfun() { throw "External \"Prelude.prim_negateFloat\" not implemented"; }

  // external Node* __0x3D_0x3A_0x3D::hfun() { throw "External \"Prelude.=:=\" not implemented"; }

  // external Node* _success::hfun() { throw "External \"Prelude.success\" not implemented"; }

  // external Node* __0x26::hfun() { throw "External \"Prelude.&\" not implemented"; }

  Node* __0x26_0x3E::hfun() { // &>
    // VAR (1,1,(IPath (Arg 1)))
    // VAR (2,1,(IPath (Arg 2)))
    // LHS variable (1,1,(IPath (Arg 1))) is argument 1
    // LHS variable (2,1,(IPath (Arg 2))) is argument 2
    return new ::_Prelude::_cond(arg1, arg2);
  }

  Node* _maybe::hfun() { // maybe
    // VAR (1,1,(IPath (Arg 1)))
    // VAR (2,1,(IPath (Arg 2)))
    // VAR (3,4,(IPath (Arg 3)))
    // VAR (4,1,(IPath (Rel 3 ("Prelude","Just") 1)))
    // LHS variable (1,1,(IPath (Arg 1))) is argument 1
    // LHS variable (2,1,(IPath (Arg 2))) is argument 2
    // LHS variable (3,4,(IPath (Arg 3))) is argument 3
    static void* table_5[]
      = {&&fail_5, &&var_5, &&choice_5, &&oper_5, &&_Nothing_5, &&_Just_5};
      goto *table_5[(*arg3)->get_kind()];
    fail_5:
      return DO_FAIL;
    var_5:
      // Engine::narrow(arg3, generator());
      throw "No narrowing yet";
      goto *table_5[(*arg3)->get_kind()];
    choice_5:
    oper_5:
      Engine::hfun(arg3);
      goto *table_5[(*arg3)->get_kind()];
    _Nothing_5: // "Nothing"
      return *(arg1);
    _Just_5: // "Just"
      // LHS variable (4,1,(IPath (Rel 3 ("Prelude","Just") 1))) is inlined as [(3,4,(IPath (Arg 3))),(4,1,(IPath (Rel 3 ("Prelude","Just") 1)))]
      return new ::_Prelude::_apply(arg2, ((::_Prelude::_Just*) *(arg3))->arg1);
  }

  Node* _either::hfun() { // either
    // VAR (1,1,(IPath (Arg 1)))
    // VAR (2,1,(IPath (Arg 2)))
    // VAR (3,5,(IPath (Arg 3)))
    // VAR (4,1,(IPath (Rel 3 ("Prelude","Left") 1)))
    // VAR (5,1,(IPath (Rel 3 ("Prelude","Right") 1)))
    // LHS variable (1,1,(IPath (Arg 1))) is argument 1
    // LHS variable (2,1,(IPath (Arg 2))) is argument 2
    // LHS variable (3,5,(IPath (Arg 3))) is argument 3
    static void* table_6[]
      = {&&fail_6, &&var_6, &&choice_6, &&oper_6, &&_Left_6, &&_Right_6};
      goto *table_6[(*arg3)->get_kind()];
    fail_6:
      return DO_FAIL;
    var_6:
      // Engine::narrow(arg3, generator());
      throw "No narrowing yet";
      goto *table_6[(*arg3)->get_kind()];
    choice_6:
    oper_6:
      Engine::hfun(arg3);
      goto *table_6[(*arg3)->get_kind()];
    _Left_6: // "Left"
      // LHS variable (4,1,(IPath (Rel 3 ("Prelude","Left") 1))) is inlined as [(3,5,(IPath (Arg 3))),(4,1,(IPath (Rel 3 ("Prelude","Left") 1)))]
      return new ::_Prelude::_apply(arg1, ((::_Prelude::_Left*) *(arg3))->arg1);
    _Right_6: // "Right"
      // LHS variable (5,1,(IPath (Rel 3 ("Prelude","Right") 1))) is inlined as [(3,5,(IPath (Arg 3))),(5,1,(IPath (Rel 3 ("Prelude","Right") 1)))]
      return new ::_Prelude::_apply(arg2, ((::_Prelude::_Right*) *(arg3))->arg1);
  }

  // external Node* __0x3E_0x3E_0x3D::hfun() { throw "External \"Prelude.>>=\" not implemented"; }

  // external Node* _return::hfun() { throw "External \"Prelude.return\" not implemented"; }

  Node* __0x3E_0x3E::hfun() { // >>
    // VAR (1,1,(IPath (Arg 1)))
    // VAR (2,1,(IPath (Arg 2)))
    // LHS variable (1,1,(IPath (Arg 1))) is argument 1
    // LHS variable (2,1,(IPath (Arg 2))) is argument 2
    return new ::_Prelude::__0x3E_0x3E_0x3D(arg1, Engine::Partial::make(::_Prelude::__0x3E_0x3E_0x2E__0x23lambda6::make(arg2), 1));
  }

  Node* __0x3E_0x3E_0x2E__0x23lambda6::hfun() { // >>._#lambda6
    // VAR (1,1,(IPath (Arg 1)))
    // VAR (2,0,(IPath (Arg 2)))
    // LHS variable (1,1,(IPath (Arg 1))) is argument 1
    // LHS variable (2,0,(IPath (Arg 2))) is not used
    return *(arg1);
  }

  Node* _done::hfun() { // done
    return new ::_Prelude::_return(::_Prelude::__0x28_0x29::make());
  }

  Node* _putChar::hfun() { // putChar
    // VAR (1,1,(IPath (Arg 1)))
    // LHS variable (1,1,(IPath (Arg 1))) is argument 1
    return new ::_Prelude::__0x24_0x23(Engine::Partial::make(::_Prelude::_prim_putChar::make(), 1), arg1);
  }

  // external Node* _prim_putChar::hfun() { throw "External \"Prelude.prim_putChar\" not implemented"; }

  // external Node* _getChar::hfun() { throw "External \"Prelude.getChar\" not implemented"; }

  Node* _readFile::hfun() { // readFile
    // VAR (1,1,(IPath (Arg 1)))
    // LHS variable (1,1,(IPath (Arg 1))) is argument 1
    return new ::_Prelude::__0x24_0x23_0x23(Engine::Partial::make(::_Prelude::_prim_readFile::make(), 1), arg1);
  }

  // external Node* _prim_readFile::hfun() { throw "External \"Prelude.prim_readFile\" not implemented"; }

  // external Node* _prim_readFileContents::hfun() { throw "External \"Prelude.prim_readFileContents\" not implemented"; }

  Node* _writeFile::hfun() { // writeFile
    // VAR (1,1,(IPath (Arg 1)))
    // VAR (2,1,(IPath (Arg 2)))
    // LHS variable (1,1,(IPath (Arg 1))) is argument 1
    // LHS variable (2,1,(IPath (Arg 2))) is argument 2
    return new ::_Prelude::_apply(::_Prelude::__0x24_0x23_0x23::make(Engine::Partial::make(::_Prelude::_prim_writeFile::make(), 2), arg1), arg2);
  }

  // external Node* _prim_writeFile::hfun() { throw "External \"Prelude.prim_writeFile\" not implemented"; }

  Node* _appendFile::hfun() { // appendFile
    // VAR (1,1,(IPath (Arg 1)))
    // VAR (2,1,(IPath (Arg 2)))
    // LHS variable (1,1,(IPath (Arg 1))) is argument 1
    // LHS variable (2,1,(IPath (Arg 2))) is argument 2
    return new ::_Prelude::_apply(::_Prelude::__0x24_0x23_0x23::make(Engine::Partial::make(::_Prelude::_prim_appendFile::make(), 2), arg1), arg2);
  }

  // external Node* _prim_appendFile::hfun() { throw "External \"Prelude.prim_appendFile\" not implemented"; }

  Node* _putStr::hfun() { // putStr
    // VAR (1,5,(IPath (Arg 1)))
    // VAR (2,1,(IPath (Rel 1 ("Prelude",":") 1)))
    // VAR (3,1,(IPath (Rel 1 ("Prelude",":") 2)))
    // LHS variable (1,5,(IPath (Arg 1))) is argument 1
    static void* table_4[]
      = {&&fail_4, &&var_4, &&choice_4, &&oper_4, &&__0x5B_0x5D_4, &&__0x3A_4};
      goto *table_4[(*arg1)->get_kind()];
    fail_4:
      return DO_FAIL;
    var_4:
      // Engine::narrow(arg1, generator());
      throw "No narrowing yet";
      goto *table_4[(*arg1)->get_kind()];
    choice_4:
    oper_4:
      Engine::hfun(arg1);
      goto *table_4[(*arg1)->get_kind()];
    __0x5B_0x5D_4: // "[]"
      return new ::_Prelude::_done();
    __0x3A_4: // ":"
      // LHS variable (2,1,(IPath (Rel 1 ("Prelude",":") 1))) is inlined as [(1,5,(IPath (Arg 1))),(2,1,(IPath (Rel 1 ("Prelude",":") 1)))]
      // LHS variable (3,1,(IPath (Rel 1 ("Prelude",":") 2))) is inlined as [(1,5,(IPath (Arg 1))),(3,1,(IPath (Rel 1 ("Prelude",":") 2)))]
      return new ::_Prelude::__0x3E_0x3E(::_Prelude::_putChar::make(((::_Prelude::__0x3A*) *(arg1))->arg1), ::_Prelude::_putStr::make(((::_Prelude::__0x3A*) *(arg1))->arg2));
  }

  Node* _putStrLn::hfun() { // putStrLn
    // VAR (1,1,(IPath (Arg 1)))
    // LHS variable (1,1,(IPath (Arg 1))) is argument 1
    return new ::_Prelude::__0x3E_0x3E(::_Prelude::_putStr::make(arg1), ::_Prelude::_putChar::make(Litchar::Litchar::make('\n')));
  }

  Node* _getLine::hfun() { // getLine
    return new ::_Prelude::__0x3E_0x3E_0x3D(::_Prelude::_getChar::make(), Engine::Partial::make(::_Prelude::_getLine_0x2E__0x23lambda7::make(), 1));
  }

  Node* _getLine_0x2E__0x23lambda7::hfun() { // getLine._#lambda7
    // VAR (1,2,(IPath (Arg 1)))
    // LHS variable (1,2,(IPath (Arg 1))) is argument 1
    static void* table_2[]
      = {&&fail_2, &&var_2, &&choice_2, &&oper_2, &&_False_2, &&_True_2};
    Node** var_2 = ::_Prelude::__0x3D_0x3D::make(arg1, Litchar::Litchar::make('\n'));
      goto *table_2[(*var_2)->get_kind()];
    fail_2:
      return DO_FAIL;
    var_2:
      // Engine::narrow(var_2, generator());
      throw "No narrowing yet";
      goto *table_2[(*var_2)->get_kind()];
    choice_2:
    oper_2:
      Engine::hfun(var_2);
      goto *table_2[(*var_2)->get_kind()];
    _False_2: // "False"
      return new ::_Prelude::__0x3E_0x3E_0x3D(::_Prelude::_getLine::make(), Engine::Partial::make(::_Prelude::_getLine_0x2E__0x23lambda7_0x2E__0x23lambda8::make(arg1), 1));
    _True_2: // "True"
      return new ::_Prelude::_return(::_Prelude::__0x5B_0x5D::make());
  }

  Node* _getLine_0x2E__0x23lambda7_0x2E__0x23lambda8::hfun() { // getLine._#lambda7._#lambda8
    // VAR (1,1,(IPath (Arg 1)))
    // VAR (2,1,(IPath (Arg 2)))
    // LHS variable (1,1,(IPath (Arg 1))) is argument 1
    // LHS variable (2,1,(IPath (Arg 2))) is argument 2
    return new ::_Prelude::_return(::_Prelude::__0x3A::make(arg1, arg2));
  }

  Node* _userError::hfun() { // userError
    // VAR (1,1,(IPath (Arg 1)))
    // LHS variable (1,1,(IPath (Arg 1))) is argument 1
    return new ::_Prelude::_UserError(arg1);
  }

  Node* _ioError::hfun() { // ioError
    // VAR (1,1,(IPath (Arg 1)))
    // LHS variable (1,1,(IPath (Arg 1))) is argument 1
    return new ::_Prelude::_error(::_Prelude::_showError::make(arg1));
  }

  Node* _showError::hfun() { // showError
    // VAR (1,7,(IPath (Arg 1)))
    // VAR (2,1,(IPath (Rel 1 ("Prelude","IOError") 1)))
    // VAR (3,1,(IPath (Rel 1 ("Prelude","UserError") 1)))
    // VAR (4,1,(IPath (Rel 1 ("Prelude","FailError") 1)))
    // VAR (5,1,(IPath (Rel 1 ("Prelude","NondetError") 1)))
    // LHS variable (1,7,(IPath (Arg 1))) is argument 1
    static void* table_6[]
      = {&&fail_6, &&var_6, &&choice_6, &&oper_6, &&_IOError_6, &&_UserError_6, &&_FailError_6, &&_NondetError_6};
      goto *table_6[(*arg1)->get_kind()];
    fail_6:
      return DO_FAIL;
    var_6:
      // Engine::narrow(arg1, generator());
      throw "No narrowing yet";
      goto *table_6[(*arg1)->get_kind()];
    choice_6:
    oper_6:
      Engine::hfun(arg1);
      goto *table_6[(*arg1)->get_kind()];
    _IOError_6: // "IOError"
      // LHS variable (2,1,(IPath (Rel 1 ("Prelude","IOError") 1))) is inlined as [(1,7,(IPath (Arg 1))),(2,1,(IPath (Rel 1 ("Prelude","IOError") 1)))]
      return new ::_Prelude::__0x2B_0x2B(::_Prelude::__0x3A::make(Litchar::Litchar::make('i'), ::_Prelude::__0x3A::make(Litchar::Litchar::make('/'), ::_Prelude::__0x3A::make(Litchar::Litchar::make('o'), ::_Prelude::__0x3A::make(Litchar::Litchar::make(' '), ::_Prelude::__0x3A::make(Litchar::Litchar::make('e'), ::_Prelude::__0x3A::make(Litchar::Litchar::make('r'), ::_Prelude::__0x3A::make(Litchar::Litchar::make('r'), ::_Prelude::__0x3A::make(Litchar::Litchar::make('o'), ::_Prelude::__0x3A::make(Litchar::Litchar::make('r'), ::_Prelude::__0x3A::make(Litchar::Litchar::make(':'), ::_Prelude::__0x3A::make(Litchar::Litchar::make(' '), ::_Prelude::__0x5B_0x5D::make()))))))))))), ((::_Prelude::_IOError*) *(arg1))->arg1);
    _UserError_6: // "UserError"
      // LHS variable (3,1,(IPath (Rel 1 ("Prelude","UserError") 1))) is inlined as [(1,7,(IPath (Arg 1))),(3,1,(IPath (Rel 1 ("Prelude","UserError") 1)))]
      return new ::_Prelude::__0x2B_0x2B(::_Prelude::__0x3A::make(Litchar::Litchar::make('u'), ::_Prelude::__0x3A::make(Litchar::Litchar::make('s'), ::_Prelude::__0x3A::make(Litchar::Litchar::make('e'), ::_Prelude::__0x3A::make(Litchar::Litchar::make('r'), ::_Prelude::__0x3A::make(Litchar::Litchar::make(' '), ::_Prelude::__0x3A::make(Litchar::Litchar::make('e'), ::_Prelude::__0x3A::make(Litchar::Litchar::make('r'), ::_Prelude::__0x3A::make(Litchar::Litchar::make('r'), ::_Prelude::__0x3A::make(Litchar::Litchar::make('o'), ::_Prelude::__0x3A::make(Litchar::Litchar::make('r'), ::_Prelude::__0x3A::make(Litchar::Litchar::make(':'), ::_Prelude::__0x3A::make(Litchar::Litchar::make(' '), ::_Prelude::__0x5B_0x5D::make())))))))))))), ((::_Prelude::_UserError*) *(arg1))->arg1);
    _FailError_6: // "FailError"
      // LHS variable (4,1,(IPath (Rel 1 ("Prelude","FailError") 1))) is inlined as [(1,7,(IPath (Arg 1))),(4,1,(IPath (Rel 1 ("Prelude","FailError") 1)))]
      return new ::_Prelude::__0x2B_0x2B(::_Prelude::__0x3A::make(Litchar::Litchar::make('f'), ::_Prelude::__0x3A::make(Litchar::Litchar::make('a'), ::_Prelude::__0x3A::make(Litchar::Litchar::make('i'), ::_Prelude::__0x3A::make(Litchar::Litchar::make('l'), ::_Prelude::__0x3A::make(Litchar::Litchar::make(' '), ::_Prelude::__0x3A::make(Litchar::Litchar::make('e'), ::_Prelude::__0x3A::make(Litchar::Litchar::make('r'), ::_Prelude::__0x3A::make(Litchar::Litchar::make('r'), ::_Prelude::__0x3A::make(Litchar::Litchar::make('o'), ::_Prelude::__0x3A::make(Litchar::Litchar::make('r'), ::_Prelude::__0x3A::make(Litchar::Litchar::make(':'), ::_Prelude::__0x3A::make(Litchar::Litchar::make(' '), ::_Prelude::__0x5B_0x5D::make())))))))))))), ((::_Prelude::_FailError*) *(arg1))->arg1);
    _NondetError_6: // "NondetError"
      // LHS variable (5,1,(IPath (Rel 1 ("Prelude","NondetError") 1))) is inlined as [(1,7,(IPath (Arg 1))),(5,1,(IPath (Rel 1 ("Prelude","NondetError") 1)))]
      return new ::_Prelude::__0x2B_0x2B(::_Prelude::__0x3A::make(Litchar::Litchar::make('n'), ::_Prelude::__0x3A::make(Litchar::Litchar::make('o'), ::_Prelude::__0x3A::make(Litchar::Litchar::make('n'), ::_Prelude::__0x3A::make(Litchar::Litchar::make('d'), ::_Prelude::__0x3A::make(Litchar::Litchar::make('e'), ::_Prelude::__0x3A::make(Litchar::Litchar::make('t'), ::_Prelude::__0x3A::make(Litchar::Litchar::make(' '), ::_Prelude::__0x3A::make(Litchar::Litchar::make('e'), ::_Prelude::__0x3A::make(Litchar::Litchar::make('r'), ::_Prelude::__0x3A::make(Litchar::Litchar::make('r'), ::_Prelude::__0x3A::make(Litchar::Litchar::make('o'), ::_Prelude::__0x3A::make(Litchar::Litchar::make('r'), ::_Prelude::__0x3A::make(Litchar::Litchar::make(':'), ::_Prelude::__0x3A::make(Litchar::Litchar::make(' '), ::_Prelude::__0x5B_0x5D::make())))))))))))))), ((::_Prelude::_NondetError*) *(arg1))->arg1);
  }

  // external Node* _catch::hfun() { throw "External \"Prelude.catch\" not implemented"; }

  Node* _show::hfun() { // show
    // VAR (1,1,(IPath (Arg 1)))
    // LHS variable (1,1,(IPath (Arg 1))) is argument 1
    return new ::_Prelude::__0x24_0x23_0x23(Engine::Partial::make(::_Prelude::_prim_show::make(), 1), arg1);
  }

  // external Node* _prim_show::hfun() { throw "External \"Prelude.prim_show\" not implemented"; }

  Node* _print::hfun() { // print
    // VAR (1,1,(IPath (Arg 1)))
    // LHS variable (1,1,(IPath (Arg 1))) is argument 1
    return new ::_Prelude::_putStrLn(::_Prelude::_show::make(arg1));
  }

  Node* _doSolve::hfun() { // doSolve
    // VAR (1,1,(IPath (Arg 1)))
    // LHS variable (1,1,(IPath (Arg 1))) is argument 1
    return new ::_Prelude::_cond(arg1, ::_Prelude::_done::make());
  }

  Node* _sequenceIO::hfun() { // sequenceIO
    // VAR (1,5,(IPath (Arg 1)))
    // VAR (2,1,(IPath (Rel 1 ("Prelude",":") 1)))
    // VAR (3,1,(IPath (Rel 1 ("Prelude",":") 2)))
    // LHS variable (1,5,(IPath (Arg 1))) is argument 1
    static void* table_4[]
      = {&&fail_4, &&var_4, &&choice_4, &&oper_4, &&__0x5B_0x5D_4, &&__0x3A_4};
      goto *table_4[(*arg1)->get_kind()];
    fail_4:
      return DO_FAIL;
    var_4:
      // Engine::narrow(arg1, generator());
      throw "No narrowing yet";
      goto *table_4[(*arg1)->get_kind()];
    choice_4:
    oper_4:
      Engine::hfun(arg1);
      goto *table_4[(*arg1)->get_kind()];
    __0x5B_0x5D_4: // "[]"
      return new ::_Prelude::_return(::_Prelude::__0x5B_0x5D::make());
    __0x3A_4: // ":"
      // LHS variable (2,1,(IPath (Rel 1 ("Prelude",":") 1))) is inlined as [(1,5,(IPath (Arg 1))),(2,1,(IPath (Rel 1 ("Prelude",":") 1)))]
      // LHS variable (3,1,(IPath (Rel 1 ("Prelude",":") 2))) is inlined as [(1,5,(IPath (Arg 1))),(3,1,(IPath (Rel 1 ("Prelude",":") 2)))]
      return new ::_Prelude::__0x3E_0x3E_0x3D(((::_Prelude::__0x3A*) *(arg1))->arg1, Engine::Partial::make(::_Prelude::_sequenceIO_0x2E__0x23lambda9::make(((::_Prelude::__0x3A*) *(arg1))->arg2), 1));
  }

  Node* _sequenceIO_0x2E__0x23lambda9::hfun() { // sequenceIO._#lambda9
    // VAR (1,1,(IPath (Arg 1)))
    // VAR (2,1,(IPath (Arg 2)))
    // LHS variable (1,1,(IPath (Arg 1))) is argument 1
    // LHS variable (2,1,(IPath (Arg 2))) is argument 2
    return new ::_Prelude::__0x3E_0x3E_0x3D(::_Prelude::_sequenceIO::make(arg1), Engine::Partial::make(::_Prelude::_sequenceIO_0x2E__0x23lambda9_0x2E__0x23lambda10::make(arg2), 1));
  }

  Node* _sequenceIO_0x2E__0x23lambda9_0x2E__0x23lambda10::hfun() { // sequenceIO._#lambda9._#lambda10
    // VAR (1,1,(IPath (Arg 1)))
    // VAR (2,1,(IPath (Arg 2)))
    // LHS variable (1,1,(IPath (Arg 1))) is argument 1
    // LHS variable (2,1,(IPath (Arg 2))) is argument 2
    return new ::_Prelude::_return(::_Prelude::__0x3A::make(arg1, arg2));
  }

  Node* _sequenceIO_::hfun() { // sequenceIO_
    return new Engine::Partial(::_Prelude::_foldr::make(Engine::Partial::make(::_Prelude::__0x3E_0x3E::make(), 2), ::_Prelude::_done::make()), 1);
  }

  Node* _mapIO::hfun() { // mapIO
    // VAR (1,1,(IPath (Arg 1)))
    // LHS variable (1,1,(IPath (Arg 1))) is argument 1
    return new ::_Prelude::__0x2E(Engine::Partial::make(::_Prelude::_sequenceIO::make(), 1), Engine::Partial::make(::_Prelude::_map::make(arg1), 1));
  }

  Node* _mapIO_::hfun() { // mapIO_
    // VAR (1,1,(IPath (Arg 1)))
    // LHS variable (1,1,(IPath (Arg 1))) is argument 1
    return new ::_Prelude::__0x2E(::_Prelude::_sequenceIO_::make(), Engine::Partial::make(::_Prelude::_map::make(arg1), 1));
  }

  Node* _foldIO::hfun() { // foldIO
    // VAR (1,2,(IPath (Arg 1)))
    // VAR (2,1,(IPath (Arg 2)))
    // VAR (3,5,(IPath (Arg 3)))
    // VAR (4,1,(IPath (Rel 3 ("Prelude",":") 1)))
    // VAR (5,1,(IPath (Rel 3 ("Prelude",":") 2)))
    // LHS variable (1,2,(IPath (Arg 1))) is argument 1
    // LHS variable (2,1,(IPath (Arg 2))) is argument 2
    // LHS variable (3,5,(IPath (Arg 3))) is argument 3
    static void* table_6[]
      = {&&fail_6, &&var_6, &&choice_6, &&oper_6, &&__0x5B_0x5D_6, &&__0x3A_6};
      goto *table_6[(*arg3)->get_kind()];
    fail_6:
      return DO_FAIL;
    var_6:
      // Engine::narrow(arg3, generator());
      throw "No narrowing yet";
      goto *table_6[(*arg3)->get_kind()];
    choice_6:
    oper_6:
      Engine::hfun(arg3);
      goto *table_6[(*arg3)->get_kind()];
    __0x5B_0x5D_6: // "[]"
      return new ::_Prelude::_return(arg2);
    __0x3A_6: // ":"
      // LHS variable (4,1,(IPath (Rel 3 ("Prelude",":") 1))) is inlined as [(3,5,(IPath (Arg 3))),(4,1,(IPath (Rel 3 ("Prelude",":") 1)))]
      // LHS variable (5,1,(IPath (Rel 3 ("Prelude",":") 2))) is inlined as [(3,5,(IPath (Arg 3))),(5,1,(IPath (Rel 3 ("Prelude",":") 2)))]
      return new ::_Prelude::__0x3E_0x3E_0x3D(::_Prelude::_apply::make(::_Prelude::_apply::make(arg1, arg2), ((::_Prelude::__0x3A*) *(arg3))->arg1), Engine::Partial::make(::_Prelude::_foldIO_0x2E__0x23lambda11::make(arg1, ((::_Prelude::__0x3A*) *(arg3))->arg2), 1));
  }

  Node* _foldIO_0x2E__0x23lambda11::hfun() { // foldIO._#lambda11
    // VAR (1,1,(IPath (Arg 1)))
    // VAR (2,1,(IPath (Arg 2)))
    // VAR (3,1,(IPath (Arg 3)))
    // LHS variable (1,1,(IPath (Arg 1))) is argument 1
    // LHS variable (2,1,(IPath (Arg 2))) is argument 2
    // LHS variable (3,1,(IPath (Arg 3))) is argument 3
    return new ::_Prelude::_foldIO(arg1, arg3, arg2);
  }

  Node* _liftIO::hfun() { // liftIO
    // VAR (1,1,(IPath (Arg 1)))
    // VAR (2,1,(IPath (Arg 2)))
    // LHS variable (1,1,(IPath (Arg 1))) is argument 1
    // LHS variable (2,1,(IPath (Arg 2))) is argument 2
    return new ::_Prelude::__0x3E_0x3E_0x3D(arg2, ::_Prelude::__0x2E::make(Engine::Partial::make(::_Prelude::_return::make(), 1), arg1));
  }

  Node* _forIO::hfun() { // forIO
    // VAR (1,1,(IPath (Arg 1)))
    // VAR (2,1,(IPath (Arg 2)))
    // LHS variable (1,1,(IPath (Arg 1))) is argument 1
    // LHS variable (2,1,(IPath (Arg 2))) is argument 2
    return new ::_Prelude::_apply(::_Prelude::_mapIO::make(arg2), arg1);
  }

  Node* _forIO_::hfun() { // forIO_
    // VAR (1,1,(IPath (Arg 1)))
    // VAR (2,1,(IPath (Arg 2)))
    // LHS variable (1,1,(IPath (Arg 1))) is argument 1
    // LHS variable (2,1,(IPath (Arg 2))) is argument 2
    return new ::_Prelude::_apply(::_Prelude::_mapIO_::make(arg2), arg1);
  }

  Node* _unless::hfun() { // unless
    // VAR (1,3,(IPath (Arg 1)))
    // VAR (2,1,(IPath (Arg 2)))
    // LHS variable (1,3,(IPath (Arg 1))) is argument 1
    // LHS variable (2,1,(IPath (Arg 2))) is argument 2
    static void* table_3[]
      = {&&fail_3, &&var_3, &&choice_3, &&oper_3, &&_False_3, &&_True_3};
      goto *table_3[(*arg1)->get_kind()];
    fail_3:
      return DO_FAIL;
    var_3:
      // Engine::narrow(arg1, generator());
      throw "No narrowing yet";
      goto *table_3[(*arg1)->get_kind()];
    choice_3:
    oper_3:
      Engine::hfun(arg1);
      goto *table_3[(*arg1)->get_kind()];
    _False_3: // "False"
      return *(arg2);
    _True_3: // "True"
      return new ::_Prelude::_done();
  }

  Node* _when::hfun() { // when
    // VAR (1,3,(IPath (Arg 1)))
    // VAR (2,1,(IPath (Arg 2)))
    // LHS variable (1,3,(IPath (Arg 1))) is argument 1
    // LHS variable (2,1,(IPath (Arg 2))) is argument 2
    static void* table_3[]
      = {&&fail_3, &&var_3, &&choice_3, &&oper_3, &&_False_3, &&_True_3};
      goto *table_3[(*arg1)->get_kind()];
    fail_3:
      return DO_FAIL;
    var_3:
      // Engine::narrow(arg1, generator());
      throw "No narrowing yet";
      goto *table_3[(*arg1)->get_kind()];
    choice_3:
    oper_3:
      Engine::hfun(arg1);
      goto *table_3[(*arg1)->get_kind()];
    _False_3: // "False"
      return new ::_Prelude::_done();
    _True_3: // "True"
      return *(arg2);
  }

  // external Node* __0x3F::hfun() { throw "External \"Prelude.?\" not implemented"; }

  Node* _unknown::hfun() { // unknown
    // VAR (1,1,IFree)
    // Free variable (1,1,IFree) is inlined
    return *(::Engine::Variable::make());
  }

  Node* _PEVAL::hfun() { // PEVAL
    // VAR (1,1,(IPath (Arg 1)))
    // LHS variable (1,1,(IPath (Arg 1))) is argument 1
    return *(arg1);
  }

  Node* _normalForm::hfun() { // normalForm
    // VAR (1,1,(IPath (Arg 1)))
    // LHS variable (1,1,(IPath (Arg 1))) is argument 1
    return new ::_Prelude::__0x24_0x21_0x21(Engine::Partial::make(::_Prelude::_id::make(), 1), arg1);
  }

  Node* _groundNormalForm::hfun() { // groundNormalForm
    // VAR (1,1,(IPath (Arg 1)))
    // LHS variable (1,1,(IPath (Arg 1))) is argument 1
    return new ::_Prelude::__0x24_0x23_0x23(Engine::Partial::make(::_Prelude::_id::make(), 1), arg1);
  }

  // external Node* _apply::hfun() { throw "External \"Prelude.apply\" not implemented"; }

  // external Node* _cond::hfun() { throw "External \"Prelude.cond\" not implemented"; }

  // external Node* _letrec::hfun() { throw "External \"Prelude.letrec\" not implemented"; }

  // external Node* __0x3D_0x3A_0x3C_0x3D::hfun() { throw "External \"Prelude.=:<=\" not implemented"; }

  // external Node* __0x3D_0x3A_0x3C_0x3C_0x3D::hfun() { throw "External \"Prelude.=:<<=\" not implemented"; }

  // external Node* _ifVar::hfun() { throw "External \"Prelude.ifVar\" not implemented"; }

  // external Node* _failure::hfun() { throw "External \"Prelude.failure\" not implemented"; }

  Node* __0x28_0x29::boolequal(Node** right) { // () 
    static void* table[] = {&&fail, &&var, &&choice, &&oper, &&__0x28_0x29};
  start:
    goto *table[(*right)->get_kind()];
  fail:
    return DO_FAIL;
  var:
    throw "Program flounders";
  choice:
  oper:
    Engine::hfun(right);
    goto start;
  __0x28_0x29:
    return new ::_Prelude::_True();
  }

  Node* __0x28_0x29::compare(Node** right) { // () 
    static void* table[] = {&&fail, &&var, &&choice, &&oper, &&__0x28_0x29};
    goto *table[(*right)->get_kind()];
  fail:
    return DO_FAIL;
  var:
    throw "Program flounders";
  choice:
  oper:
    Engine::hfun(right);
    goto *table[(*right)->get_kind()];
  __0x28_0x29:
    return new ::_Prelude::_EQ();
  }

  Node* __0x5B_0x5D::boolequal(Node** right) { // [] 
    static void* table[] = {&&fail, &&var, &&choice, &&oper, &&__0x5B_0x5D, &&__0x3A};
  start:
    goto *table[(*right)->get_kind()];
  fail:
    return DO_FAIL;
  var:
    throw "Program flounders";
  choice:
  oper:
    Engine::hfun(right);
    goto start;
  __0x5B_0x5D:
    return new ::_Prelude::_True();
  __0x3A:
    return new ::_Prelude::_False();
  }

  Node* __0x5B_0x5D::compare(Node** right) { // [] 
    static void* table[] = {&&fail, &&var, &&choice, &&oper, &&__0x5B_0x5D, &&__0x3A};
    goto *table[(*right)->get_kind()];
  fail:
    return DO_FAIL;
  var:
    throw "Program flounders";
  choice:
  oper:
    Engine::hfun(right);
    goto *table[(*right)->get_kind()];
  __0x5B_0x5D:
    return new ::_Prelude::_EQ();
  __0x3A:
    return new ::_Prelude::_LT();
  }

  Node* __0x3A::boolequal(Node** right) { // : 
    static void* table[] = {&&fail, &&var, &&choice, &&oper, &&__0x5B_0x5D, &&__0x3A};
  start:
    goto *table[(*right)->get_kind()];
  fail:
    return DO_FAIL;
  var:
    throw "Program flounders";
  choice:
  oper:
    Engine::hfun(right);
    goto start;
  __0x5B_0x5D:
    return new ::_Prelude::_False();
  __0x3A:
    return new ::_Prelude::__0x26_0x26(::_Prelude::__0x3D_0x3D::make(arg1,((::_Prelude::__0x3A*) (*right))->arg1), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg2,((::_Prelude::__0x3A*) (*right))->arg2), ::_Prelude::_True::make()));
  }

  Node* __0x3A::compare(Node** right) { // : 
    static void* table[] = {&&fail, &&var, &&choice, &&oper, &&__0x5B_0x5D, &&__0x3A};
    goto *table[(*right)->get_kind()];
  fail:
    return DO_FAIL;
  var:
    throw "Program flounders";
  choice:
  oper:
    Engine::hfun(right);
    goto *table[(*right)->get_kind()];
  __0x5B_0x5D:
    return new ::_Prelude::_GT();
  __0x3A:
    return new ::_Prelude::_descend_compare(::_Prelude::_compare::make(arg1,((::_Prelude::__0x3A*) (*right))->arg1),::_Prelude::_compare::make(arg2,((::_Prelude::__0x3A*) (*right))->arg2));
  }

  Node* __0x28_0x2C_0x29::boolequal(Node** right) { // (,) 
    static void* table[] = {&&fail, &&var, &&choice, &&oper, &&__0x28_0x2C_0x29};
  start:
    goto *table[(*right)->get_kind()];
  fail:
    return DO_FAIL;
  var:
    throw "Program flounders";
  choice:
  oper:
    Engine::hfun(right);
    goto start;
  __0x28_0x2C_0x29:
    return new ::_Prelude::__0x26_0x26(::_Prelude::__0x3D_0x3D::make(arg1,((::_Prelude::__0x28_0x2C_0x29*) (*right))->arg1), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg2,((::_Prelude::__0x28_0x2C_0x29*) (*right))->arg2), ::_Prelude::_True::make()));
  }

  Node* __0x28_0x2C_0x29::compare(Node** right) { // (,) 
    static void* table[] = {&&fail, &&var, &&choice, &&oper, &&__0x28_0x2C_0x29};
    goto *table[(*right)->get_kind()];
  fail:
    return DO_FAIL;
  var:
    throw "Program flounders";
  choice:
  oper:
    Engine::hfun(right);
    goto *table[(*right)->get_kind()];
  __0x28_0x2C_0x29:
    return new ::_Prelude::_descend_compare(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x29*) (*right))->arg1),::_Prelude::_compare::make(arg2,((::_Prelude::__0x28_0x2C_0x29*) (*right))->arg2));
  }

  Node* __0x28_0x2C_0x2C_0x29::boolequal(Node** right) { // (,,) 
    static void* table[] = {&&fail, &&var, &&choice, &&oper, &&__0x28_0x2C_0x2C_0x29};
  start:
    goto *table[(*right)->get_kind()];
  fail:
    return DO_FAIL;
  var:
    throw "Program flounders";
  choice:
  oper:
    Engine::hfun(right);
    goto start;
  __0x28_0x2C_0x2C_0x29:
    return new ::_Prelude::__0x26_0x26(::_Prelude::__0x3D_0x3D::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x29*) (*right))->arg1), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg2,((::_Prelude::__0x28_0x2C_0x2C_0x29*) (*right))->arg2), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg3,((::_Prelude::__0x28_0x2C_0x2C_0x29*) (*right))->arg3), ::_Prelude::_True::make())));
  }

  Node* __0x28_0x2C_0x2C_0x29::compare(Node** right) { // (,,) 
    static void* table[] = {&&fail, &&var, &&choice, &&oper, &&__0x28_0x2C_0x2C_0x29};
    goto *table[(*right)->get_kind()];
  fail:
    return DO_FAIL;
  var:
    throw "Program flounders";
  choice:
  oper:
    Engine::hfun(right);
    goto *table[(*right)->get_kind()];
  __0x28_0x2C_0x2C_0x29:
    return new ::_Prelude::_descend_compare(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_compare::make(arg3,((::_Prelude::__0x28_0x2C_0x2C_0x29*) (*right))->arg3)));
  }

  Node* __0x28_0x2C_0x2C_0x2C_0x29::boolequal(Node** right) { // (,,,) 
    static void* table[] = {&&fail, &&var, &&choice, &&oper, &&__0x28_0x2C_0x2C_0x2C_0x29};
  start:
    goto *table[(*right)->get_kind()];
  fail:
    return DO_FAIL;
  var:
    throw "Program flounders";
  choice:
  oper:
    Engine::hfun(right);
    goto start;
  __0x28_0x2C_0x2C_0x2C_0x29:
    return new ::_Prelude::__0x26_0x26(::_Prelude::__0x3D_0x3D::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x29*) (*right))->arg1), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg2,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x29*) (*right))->arg2), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg3,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x29*) (*right))->arg3), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg4,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x29*) (*right))->arg4), ::_Prelude::_True::make()))));
  }

  Node* __0x28_0x2C_0x2C_0x2C_0x29::compare(Node** right) { // (,,,) 
    static void* table[] = {&&fail, &&var, &&choice, &&oper, &&__0x28_0x2C_0x2C_0x2C_0x29};
    goto *table[(*right)->get_kind()];
  fail:
    return DO_FAIL;
  var:
    throw "Program flounders";
  choice:
  oper:
    Engine::hfun(right);
    goto *table[(*right)->get_kind()];
  __0x28_0x2C_0x2C_0x2C_0x29:
    return new ::_Prelude::_descend_compare(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_compare::make(arg4,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x29*) (*right))->arg4))));
  }

  Node* __0x28_0x2C_0x2C_0x2C_0x2C_0x29::boolequal(Node** right) { // (,,,,) 
    static void* table[] = {&&fail, &&var, &&choice, &&oper, &&__0x28_0x2C_0x2C_0x2C_0x2C_0x29};
  start:
    goto *table[(*right)->get_kind()];
  fail:
    return DO_FAIL;
  var:
    throw "Program flounders";
  choice:
  oper:
    Engine::hfun(right);
    goto start;
  __0x28_0x2C_0x2C_0x2C_0x2C_0x29:
    return new ::_Prelude::__0x26_0x26(::_Prelude::__0x3D_0x3D::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg2,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg2), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg3,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg3), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg4,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg4), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg5,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg5), ::_Prelude::_True::make())))));
  }

  Node* __0x28_0x2C_0x2C_0x2C_0x2C_0x29::compare(Node** right) { // (,,,,) 
    static void* table[] = {&&fail, &&var, &&choice, &&oper, &&__0x28_0x2C_0x2C_0x2C_0x2C_0x29};
    goto *table[(*right)->get_kind()];
  fail:
    return DO_FAIL;
  var:
    throw "Program flounders";
  choice:
  oper:
    Engine::hfun(right);
    goto *table[(*right)->get_kind()];
  __0x28_0x2C_0x2C_0x2C_0x2C_0x29:
    return new ::_Prelude::_descend_compare(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_compare::make(arg5,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg5)))));
  }

  Node* __0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::boolequal(Node** right) { // (,,,,,) 
    static void* table[] = {&&fail, &&var, &&choice, &&oper, &&__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x29};
  start:
    goto *table[(*right)->get_kind()];
  fail:
    return DO_FAIL;
  var:
    throw "Program flounders";
  choice:
  oper:
    Engine::hfun(right);
    goto start;
  __0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x29:
    return new ::_Prelude::__0x26_0x26(::_Prelude::__0x3D_0x3D::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg2,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg2), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg3,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg3), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg4,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg4), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg5,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg5), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg6,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg6), ::_Prelude::_True::make()))))));
  }

  Node* __0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::compare(Node** right) { // (,,,,,) 
    static void* table[] = {&&fail, &&var, &&choice, &&oper, &&__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x29};
    goto *table[(*right)->get_kind()];
  fail:
    return DO_FAIL;
  var:
    throw "Program flounders";
  choice:
  oper:
    Engine::hfun(right);
    goto *table[(*right)->get_kind()];
  __0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x29:
    return new ::_Prelude::_descend_compare(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_compare::make(arg6,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg6))))));
  }

  Node* __0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::boolequal(Node** right) { // (,,,,,,) 
    static void* table[] = {&&fail, &&var, &&choice, &&oper, &&__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29};
  start:
    goto *table[(*right)->get_kind()];
  fail:
    return DO_FAIL;
  var:
    throw "Program flounders";
  choice:
  oper:
    Engine::hfun(right);
    goto start;
  __0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29:
    return new ::_Prelude::__0x26_0x26(::_Prelude::__0x3D_0x3D::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg2,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg2), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg3,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg3), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg4,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg4), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg5,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg5), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg6,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg6), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg7,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg7), ::_Prelude::_True::make())))))));
  }

  Node* __0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::compare(Node** right) { // (,,,,,,) 
    static void* table[] = {&&fail, &&var, &&choice, &&oper, &&__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29};
    goto *table[(*right)->get_kind()];
  fail:
    return DO_FAIL;
  var:
    throw "Program flounders";
  choice:
  oper:
    Engine::hfun(right);
    goto *table[(*right)->get_kind()];
  __0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29:
    return new ::_Prelude::_descend_compare(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_compare::make(arg7,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg7)))))));
  }

  Node* __0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::boolequal(Node** right) { // (,,,,,,,) 
    static void* table[] = {&&fail, &&var, &&choice, &&oper, &&__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29};
  start:
    goto *table[(*right)->get_kind()];
  fail:
    return DO_FAIL;
  var:
    throw "Program flounders";
  choice:
  oper:
    Engine::hfun(right);
    goto start;
  __0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29:
    return new ::_Prelude::__0x26_0x26(::_Prelude::__0x3D_0x3D::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg2,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg2), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg3,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg3), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg4,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg4), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg5,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg5), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg6,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg6), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg7,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg7), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg8,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg8), ::_Prelude::_True::make()))))))));
  }

  Node* __0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::compare(Node** right) { // (,,,,,,,) 
    static void* table[] = {&&fail, &&var, &&choice, &&oper, &&__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29};
    goto *table[(*right)->get_kind()];
  fail:
    return DO_FAIL;
  var:
    throw "Program flounders";
  choice:
  oper:
    Engine::hfun(right);
    goto *table[(*right)->get_kind()];
  __0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29:
    return new ::_Prelude::_descend_compare(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_compare::make(arg8,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg8))))))));
  }

  Node* __0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::boolequal(Node** right) { // (,,,,,,,,) 
    static void* table[] = {&&fail, &&var, &&choice, &&oper, &&__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29};
  start:
    goto *table[(*right)->get_kind()];
  fail:
    return DO_FAIL;
  var:
    throw "Program flounders";
  choice:
  oper:
    Engine::hfun(right);
    goto start;
  __0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29:
    return new ::_Prelude::__0x26_0x26(::_Prelude::__0x3D_0x3D::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg2,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg2), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg3,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg3), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg4,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg4), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg5,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg5), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg6,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg6), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg7,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg7), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg8,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg8), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg9,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg9), ::_Prelude::_True::make())))))))));
  }

  Node* __0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::compare(Node** right) { // (,,,,,,,,) 
    static void* table[] = {&&fail, &&var, &&choice, &&oper, &&__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29};
    goto *table[(*right)->get_kind()];
  fail:
    return DO_FAIL;
  var:
    throw "Program flounders";
  choice:
  oper:
    Engine::hfun(right);
    goto *table[(*right)->get_kind()];
  __0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29:
    return new ::_Prelude::_descend_compare(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_compare::make(arg9,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg9)))))))));
  }

  Node* __0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::boolequal(Node** right) { // (,,,,,,,,,) 
    static void* table[] = {&&fail, &&var, &&choice, &&oper, &&__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29};
  start:
    goto *table[(*right)->get_kind()];
  fail:
    return DO_FAIL;
  var:
    throw "Program flounders";
  choice:
  oper:
    Engine::hfun(right);
    goto start;
  __0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29:
    return new ::_Prelude::__0x26_0x26(::_Prelude::__0x3D_0x3D::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg2,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg2), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg3,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg3), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg4,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg4), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg5,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg5), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg6,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg6), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg7,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg7), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg8,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg8), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg9,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg9), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg10,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg10), ::_Prelude::_True::make()))))))))));
  }

  Node* __0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::compare(Node** right) { // (,,,,,,,,,) 
    static void* table[] = {&&fail, &&var, &&choice, &&oper, &&__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29};
    goto *table[(*right)->get_kind()];
  fail:
    return DO_FAIL;
  var:
    throw "Program flounders";
  choice:
  oper:
    Engine::hfun(right);
    goto *table[(*right)->get_kind()];
  __0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29:
    return new ::_Prelude::_descend_compare(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_compare::make(arg10,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg10))))))))));
  }

  Node* __0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::boolequal(Node** right) { // (,,,,,,,,,,) 
    static void* table[] = {&&fail, &&var, &&choice, &&oper, &&__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29};
  start:
    goto *table[(*right)->get_kind()];
  fail:
    return DO_FAIL;
  var:
    throw "Program flounders";
  choice:
  oper:
    Engine::hfun(right);
    goto start;
  __0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29:
    return new ::_Prelude::__0x26_0x26(::_Prelude::__0x3D_0x3D::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg2,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg2), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg3,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg3), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg4,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg4), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg5,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg5), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg6,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg6), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg7,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg7), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg8,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg8), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg9,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg9), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg10,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg10), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg11,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg11), ::_Prelude::_True::make())))))))))));
  }

  Node* __0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::compare(Node** right) { // (,,,,,,,,,,) 
    static void* table[] = {&&fail, &&var, &&choice, &&oper, &&__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29};
    goto *table[(*right)->get_kind()];
  fail:
    return DO_FAIL;
  var:
    throw "Program flounders";
  choice:
  oper:
    Engine::hfun(right);
    goto *table[(*right)->get_kind()];
  __0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29:
    return new ::_Prelude::_descend_compare(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_compare::make(arg11,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg11)))))))))));
  }

  Node* __0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::boolequal(Node** right) { // (,,,,,,,,,,,) 
    static void* table[] = {&&fail, &&var, &&choice, &&oper, &&__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29};
  start:
    goto *table[(*right)->get_kind()];
  fail:
    return DO_FAIL;
  var:
    throw "Program flounders";
  choice:
  oper:
    Engine::hfun(right);
    goto start;
  __0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29:
    return new ::_Prelude::__0x26_0x26(::_Prelude::__0x3D_0x3D::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg2,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg2), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg3,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg3), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg4,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg4), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg5,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg5), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg6,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg6), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg7,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg7), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg8,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg8), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg9,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg9), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg10,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg10), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg11,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg11), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg12,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg12), ::_Prelude::_True::make()))))))))))));
  }

  Node* __0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::compare(Node** right) { // (,,,,,,,,,,,) 
    static void* table[] = {&&fail, &&var, &&choice, &&oper, &&__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29};
    goto *table[(*right)->get_kind()];
  fail:
    return DO_FAIL;
  var:
    throw "Program flounders";
  choice:
  oper:
    Engine::hfun(right);
    goto *table[(*right)->get_kind()];
  __0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29:
    return new ::_Prelude::_descend_compare(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_compare::make(arg12,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg12))))))))))));
  }

  Node* __0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::boolequal(Node** right) { // (,,,,,,,,,,,,) 
    static void* table[] = {&&fail, &&var, &&choice, &&oper, &&__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29};
  start:
    goto *table[(*right)->get_kind()];
  fail:
    return DO_FAIL;
  var:
    throw "Program flounders";
  choice:
  oper:
    Engine::hfun(right);
    goto start;
  __0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29:
    return new ::_Prelude::__0x26_0x26(::_Prelude::__0x3D_0x3D::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg2,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg2), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg3,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg3), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg4,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg4), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg5,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg5), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg6,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg6), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg7,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg7), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg8,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg8), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg9,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg9), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg10,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg10), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg11,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg11), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg12,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg12), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg13,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg13), ::_Prelude::_True::make())))))))))))));
  }

  Node* __0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::compare(Node** right) { // (,,,,,,,,,,,,) 
    static void* table[] = {&&fail, &&var, &&choice, &&oper, &&__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29};
    goto *table[(*right)->get_kind()];
  fail:
    return DO_FAIL;
  var:
    throw "Program flounders";
  choice:
  oper:
    Engine::hfun(right);
    goto *table[(*right)->get_kind()];
  __0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29:
    return new ::_Prelude::_descend_compare(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_compare::make(arg13,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg13)))))))))))));
  }

  Node* __0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::boolequal(Node** right) { // (,,,,,,,,,,,,,) 
    static void* table[] = {&&fail, &&var, &&choice, &&oper, &&__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29};
  start:
    goto *table[(*right)->get_kind()];
  fail:
    return DO_FAIL;
  var:
    throw "Program flounders";
  choice:
  oper:
    Engine::hfun(right);
    goto start;
  __0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29:
    return new ::_Prelude::__0x26_0x26(::_Prelude::__0x3D_0x3D::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg2,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg2), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg3,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg3), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg4,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg4), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg5,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg5), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg6,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg6), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg7,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg7), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg8,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg8), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg9,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg9), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg10,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg10), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg11,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg11), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg12,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg12), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg13,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg13), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg14,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg14), ::_Prelude::_True::make()))))))))))))));
  }

  Node* __0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::compare(Node** right) { // (,,,,,,,,,,,,,) 
    static void* table[] = {&&fail, &&var, &&choice, &&oper, &&__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29};
    goto *table[(*right)->get_kind()];
  fail:
    return DO_FAIL;
  var:
    throw "Program flounders";
  choice:
  oper:
    Engine::hfun(right);
    goto *table[(*right)->get_kind()];
  __0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29:
    return new ::_Prelude::_descend_compare(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_compare::make(arg14,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg14))))))))))))));
  }

  Node* __0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::boolequal(Node** right) { // (,,,,,,,,,,,,,,) 
    static void* table[] = {&&fail, &&var, &&choice, &&oper, &&__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29};
  start:
    goto *table[(*right)->get_kind()];
  fail:
    return DO_FAIL;
  var:
    throw "Program flounders";
  choice:
  oper:
    Engine::hfun(right);
    goto start;
  __0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29:
    return new ::_Prelude::__0x26_0x26(::_Prelude::__0x3D_0x3D::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg2,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg2), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg3,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg3), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg4,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg4), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg5,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg5), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg6,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg6), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg7,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg7), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg8,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg8), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg9,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg9), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg10,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg10), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg11,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg11), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg12,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg12), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg13,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg13), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg14,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg14), ::_Prelude::__0x26_0x26::make(::_Prelude::__0x3D_0x3D::make(arg15,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg15), ::_Prelude::_True::make())))))))))))))));
  }

  Node* __0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::compare(Node** right) { // (,,,,,,,,,,,,,,) 
    static void* table[] = {&&fail, &&var, &&choice, &&oper, &&__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29};
    goto *table[(*right)->get_kind()];
  fail:
    return DO_FAIL;
  var:
    throw "Program flounders";
  choice:
  oper:
    Engine::hfun(right);
    goto *table[(*right)->get_kind()];
  __0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29:
    return new ::_Prelude::_descend_compare(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_descend_compare::make(::_Prelude::_compare::make(arg1,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg1),::_Prelude::_compare::make(arg15,((::_Prelude::__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29*) (*right))->arg15)))))))))))))));
  }

  Node* _False::boolequal(Node** right) { // False 
    static void* table[] = {&&fail, &&var, &&choice, &&oper, &&_False, &&_True};
  start:
    goto *table[(*right)->get_kind()];
  fail:
    return DO_FAIL;
  var:
    throw "Program flounders";
  choice:
  oper:
    Engine::hfun(right);
    goto start;
  _False:
    return new ::_Prelude::_True();
  _True:
    return new ::_Prelude::_False();
  }

  Node* _False::compare(Node** right) { // False 
    static void* table[] = {&&fail, &&var, &&choice, &&oper, &&_False, &&_True};
    goto *table[(*right)->get_kind()];
  fail:
    return DO_FAIL;
  var:
    throw "Program flounders";
  choice:
  oper:
    Engine::hfun(right);
    goto *table[(*right)->get_kind()];
  _False:
    return new ::_Prelude::_EQ();
  _True:
    return new ::_Prelude::_LT();
  }

  Node* _True::boolequal(Node** right) { // True 
    static void* table[] = {&&fail, &&var, &&choice, &&oper, &&_False, &&_True};
  start:
    goto *table[(*right)->get_kind()];
  fail:
    return DO_FAIL;
  var:
    throw "Program flounders";
  choice:
  oper:
    Engine::hfun(right);
    goto start;
  _False:
    return new ::_Prelude::_False();
  _True:
    return new ::_Prelude::_True();
  }

  Node* _True::compare(Node** right) { // True 
    static void* table[] = {&&fail, &&var, &&choice, &&oper, &&_False, &&_True};
    goto *table[(*right)->get_kind()];
  fail:
    return DO_FAIL;
  var:
    throw "Program flounders";
  choice:
  oper:
    Engine::hfun(right);
    goto *table[(*right)->get_kind()];
  _False:
    return new ::_Prelude::_GT();
  _True:
    return new ::_Prelude::_EQ();
  }

  Node* _LT::boolequal(Node** right) { // LT 
    static void* table[] = {&&fail, &&var, &&choice, &&oper, &&_LT, &&_EQ, &&_GT};
  start:
    goto *table[(*right)->get_kind()];
  fail:
    return DO_FAIL;
  var:
    throw "Program flounders";
  choice:
  oper:
    Engine::hfun(right);
    goto start;
  _LT:
    return new ::_Prelude::_True();
  _EQ:
    return new ::_Prelude::_False();
  _GT:
    return new ::_Prelude::_False();
  }

  Node* _LT::compare(Node** right) { // LT 
    static void* table[] = {&&fail, &&var, &&choice, &&oper, &&_LT, &&_EQ, &&_GT};
    goto *table[(*right)->get_kind()];
  fail:
    return DO_FAIL;
  var:
    throw "Program flounders";
  choice:
  oper:
    Engine::hfun(right);
    goto *table[(*right)->get_kind()];
  _LT:
    return new ::_Prelude::_EQ();
  _EQ:
    return new ::_Prelude::_LT();
  _GT:
    return new ::_Prelude::_LT();
  }

  Node* _EQ::boolequal(Node** right) { // EQ 
    static void* table[] = {&&fail, &&var, &&choice, &&oper, &&_LT, &&_EQ, &&_GT};
  start:
    goto *table[(*right)->get_kind()];
  fail:
    return DO_FAIL;
  var:
    throw "Program flounders";
  choice:
  oper:
    Engine::hfun(right);
    goto start;
  _LT:
    return new ::_Prelude::_False();
  _EQ:
    return new ::_Prelude::_True();
  _GT:
    return new ::_Prelude::_False();
  }

  Node* _EQ::compare(Node** right) { // EQ 
    static void* table[] = {&&fail, &&var, &&choice, &&oper, &&_LT, &&_EQ, &&_GT};
    goto *table[(*right)->get_kind()];
  fail:
    return DO_FAIL;
  var:
    throw "Program flounders";
  choice:
  oper:
    Engine::hfun(right);
    goto *table[(*right)->get_kind()];
  _LT:
    return new ::_Prelude::_GT();
  _EQ:
    return new ::_Prelude::_EQ();
  _GT:
    return new ::_Prelude::_LT();
  }

  Node* _GT::boolequal(Node** right) { // GT 
    static void* table[] = {&&fail, &&var, &&choice, &&oper, &&_LT, &&_EQ, &&_GT};
  start:
    goto *table[(*right)->get_kind()];
  fail:
    return DO_FAIL;
  var:
    throw "Program flounders";
  choice:
  oper:
    Engine::hfun(right);
    goto start;
  _LT:
    return new ::_Prelude::_False();
  _EQ:
    return new ::_Prelude::_False();
  _GT:
    return new ::_Prelude::_True();
  }

  Node* _GT::compare(Node** right) { // GT 
    static void* table[] = {&&fail, &&var, &&choice, &&oper, &&_LT, &&_EQ, &&_GT};
    goto *table[(*right)->get_kind()];
  fail:
    return DO_FAIL;
  var:
    throw "Program flounders";
  choice:
  oper:
    Engine::hfun(right);
    goto *table[(*right)->get_kind()];
  _LT:
    return new ::_Prelude::_GT();
  _EQ:
    return new ::_Prelude::_GT();
  _GT:
    return new ::_Prelude::_EQ();
  }

  Node* _Nothing::boolequal(Node** right) { // Nothing 
    static void* table[] = {&&fail, &&var, &&choice, &&oper, &&_Nothing, &&_Just};
  start:
    goto *table[(*right)->get_kind()];
  fail:
    return DO_FAIL;
  var:
    throw "Program flounders";
  choice:
  oper:
    Engine::hfun(right);
    goto start;
  _Nothing:
    return new ::_Prelude::_True();
  _Just:
    return new ::_Prelude::_False();
  }

  Node* _Nothing::compare(Node** right) { // Nothing 
    static void* table[] = {&&fail, &&var, &&choice, &&oper, &&_Nothing, &&_Just};
    goto *table[(*right)->get_kind()];
  fail:
    return DO_FAIL;
  var:
    throw "Program flounders";
  choice:
  oper:
    Engine::hfun(right);
    goto *table[(*right)->get_kind()];
  _Nothing:
    return new ::_Prelude::_EQ();
  _Just:
    return new ::_Prelude::_LT();
  }

  Node* _Just::boolequal(Node** right) { // Just 
    static void* table[] = {&&fail, &&var, &&choice, &&oper, &&_Nothing, &&_Just};
  start:
    goto *table[(*right)->get_kind()];
  fail:
    return DO_FAIL;
  var:
    throw "Program flounders";
  choice:
  oper:
    Engine::hfun(right);
    goto start;
  _Nothing:
    return new ::_Prelude::_False();
  _Just:
    return new ::_Prelude::__0x26_0x26(::_Prelude::__0x3D_0x3D::make(arg1,((::_Prelude::_Just*) (*right))->arg1), ::_Prelude::_True::make());
  }

  Node* _Just::compare(Node** right) { // Just 
    static void* table[] = {&&fail, &&var, &&choice, &&oper, &&_Nothing, &&_Just};
    goto *table[(*right)->get_kind()];
  fail:
    return DO_FAIL;
  var:
    throw "Program flounders";
  choice:
  oper:
    Engine::hfun(right);
    goto *table[(*right)->get_kind()];
  _Nothing:
    return new ::_Prelude::_GT();
  _Just:
    return new ::_Prelude::_compare(arg1,((::_Prelude::_Just*) (*right))->arg1);
  }

  Node* _Left::boolequal(Node** right) { // Left 
    static void* table[] = {&&fail, &&var, &&choice, &&oper, &&_Left, &&_Right};
  start:
    goto *table[(*right)->get_kind()];
  fail:
    return DO_FAIL;
  var:
    throw "Program flounders";
  choice:
  oper:
    Engine::hfun(right);
    goto start;
  _Left:
    return new ::_Prelude::__0x26_0x26(::_Prelude::__0x3D_0x3D::make(arg1,((::_Prelude::_Left*) (*right))->arg1), ::_Prelude::_True::make());
  _Right:
    return new ::_Prelude::_False();
  }

  Node* _Left::compare(Node** right) { // Left 
    static void* table[] = {&&fail, &&var, &&choice, &&oper, &&_Left, &&_Right};
    goto *table[(*right)->get_kind()];
  fail:
    return DO_FAIL;
  var:
    throw "Program flounders";
  choice:
  oper:
    Engine::hfun(right);
    goto *table[(*right)->get_kind()];
  _Left:
    return new ::_Prelude::_compare(arg1,((::_Prelude::_Left*) (*right))->arg1);
  _Right:
    return new ::_Prelude::_LT();
  }

  Node* _Right::boolequal(Node** right) { // Right 
    static void* table[] = {&&fail, &&var, &&choice, &&oper, &&_Left, &&_Right};
  start:
    goto *table[(*right)->get_kind()];
  fail:
    return DO_FAIL;
  var:
    throw "Program flounders";
  choice:
  oper:
    Engine::hfun(right);
    goto start;
  _Left:
    return new ::_Prelude::_False();
  _Right:
    return new ::_Prelude::__0x26_0x26(::_Prelude::__0x3D_0x3D::make(arg1,((::_Prelude::_Right*) (*right))->arg1), ::_Prelude::_True::make());
  }

  Node* _Right::compare(Node** right) { // Right 
    static void* table[] = {&&fail, &&var, &&choice, &&oper, &&_Left, &&_Right};
    goto *table[(*right)->get_kind()];
  fail:
    return DO_FAIL;
  var:
    throw "Program flounders";
  choice:
  oper:
    Engine::hfun(right);
    goto *table[(*right)->get_kind()];
  _Left:
    return new ::_Prelude::_GT();
  _Right:
    return new ::_Prelude::_compare(arg1,((::_Prelude::_Right*) (*right))->arg1);
  }

  Node* _IOError::boolequal(Node** right) { // IOError 
    static void* table[] = {&&fail, &&var, &&choice, &&oper, &&_IOError, &&_UserError, &&_FailError, &&_NondetError};
  start:
    goto *table[(*right)->get_kind()];
  fail:
    return DO_FAIL;
  var:
    throw "Program flounders";
  choice:
  oper:
    Engine::hfun(right);
    goto start;
  _IOError:
    return new ::_Prelude::__0x26_0x26(::_Prelude::__0x3D_0x3D::make(arg1,((::_Prelude::_IOError*) (*right))->arg1), ::_Prelude::_True::make());
  _UserError:
    return new ::_Prelude::_False();
  _FailError:
    return new ::_Prelude::_False();
  _NondetError:
    return new ::_Prelude::_False();
  }

  Node* _IOError::compare(Node** right) { // IOError 
    static void* table[] = {&&fail, &&var, &&choice, &&oper, &&_IOError, &&_UserError, &&_FailError, &&_NondetError};
    goto *table[(*right)->get_kind()];
  fail:
    return DO_FAIL;
  var:
    throw "Program flounders";
  choice:
  oper:
    Engine::hfun(right);
    goto *table[(*right)->get_kind()];
  _IOError:
    return new ::_Prelude::_compare(arg1,((::_Prelude::_IOError*) (*right))->arg1);
  _UserError:
    return new ::_Prelude::_LT();
  _FailError:
    return new ::_Prelude::_LT();
  _NondetError:
    return new ::_Prelude::_LT();
  }

  Node* _UserError::boolequal(Node** right) { // UserError 
    static void* table[] = {&&fail, &&var, &&choice, &&oper, &&_IOError, &&_UserError, &&_FailError, &&_NondetError};
  start:
    goto *table[(*right)->get_kind()];
  fail:
    return DO_FAIL;
  var:
    throw "Program flounders";
  choice:
  oper:
    Engine::hfun(right);
    goto start;
  _IOError:
    return new ::_Prelude::_False();
  _UserError:
    return new ::_Prelude::__0x26_0x26(::_Prelude::__0x3D_0x3D::make(arg1,((::_Prelude::_UserError*) (*right))->arg1), ::_Prelude::_True::make());
  _FailError:
    return new ::_Prelude::_False();
  _NondetError:
    return new ::_Prelude::_False();
  }

  Node* _UserError::compare(Node** right) { // UserError 
    static void* table[] = {&&fail, &&var, &&choice, &&oper, &&_IOError, &&_UserError, &&_FailError, &&_NondetError};
    goto *table[(*right)->get_kind()];
  fail:
    return DO_FAIL;
  var:
    throw "Program flounders";
  choice:
  oper:
    Engine::hfun(right);
    goto *table[(*right)->get_kind()];
  _IOError:
    return new ::_Prelude::_GT();
  _UserError:
    return new ::_Prelude::_compare(arg1,((::_Prelude::_UserError*) (*right))->arg1);
  _FailError:
    return new ::_Prelude::_LT();
  _NondetError:
    return new ::_Prelude::_LT();
  }

  Node* _FailError::boolequal(Node** right) { // FailError 
    static void* table[] = {&&fail, &&var, &&choice, &&oper, &&_IOError, &&_UserError, &&_FailError, &&_NondetError};
  start:
    goto *table[(*right)->get_kind()];
  fail:
    return DO_FAIL;
  var:
    throw "Program flounders";
  choice:
  oper:
    Engine::hfun(right);
    goto start;
  _IOError:
    return new ::_Prelude::_False();
  _UserError:
    return new ::_Prelude::_False();
  _FailError:
    return new ::_Prelude::__0x26_0x26(::_Prelude::__0x3D_0x3D::make(arg1,((::_Prelude::_FailError*) (*right))->arg1), ::_Prelude::_True::make());
  _NondetError:
    return new ::_Prelude::_False();
  }

  Node* _FailError::compare(Node** right) { // FailError 
    static void* table[] = {&&fail, &&var, &&choice, &&oper, &&_IOError, &&_UserError, &&_FailError, &&_NondetError};
    goto *table[(*right)->get_kind()];
  fail:
    return DO_FAIL;
  var:
    throw "Program flounders";
  choice:
  oper:
    Engine::hfun(right);
    goto *table[(*right)->get_kind()];
  _IOError:
    return new ::_Prelude::_GT();
  _UserError:
    return new ::_Prelude::_GT();
  _FailError:
    return new ::_Prelude::_compare(arg1,((::_Prelude::_FailError*) (*right))->arg1);
  _NondetError:
    return new ::_Prelude::_LT();
  }

  Node* _NondetError::boolequal(Node** right) { // NondetError 
    static void* table[] = {&&fail, &&var, &&choice, &&oper, &&_IOError, &&_UserError, &&_FailError, &&_NondetError};
  start:
    goto *table[(*right)->get_kind()];
  fail:
    return DO_FAIL;
  var:
    throw "Program flounders";
  choice:
  oper:
    Engine::hfun(right);
    goto start;
  _IOError:
    return new ::_Prelude::_False();
  _UserError:
    return new ::_Prelude::_False();
  _FailError:
    return new ::_Prelude::_False();
  _NondetError:
    return new ::_Prelude::__0x26_0x26(::_Prelude::__0x3D_0x3D::make(arg1,((::_Prelude::_NondetError*) (*right))->arg1), ::_Prelude::_True::make());
  }

  Node* _NondetError::compare(Node** right) { // NondetError 
    static void* table[] = {&&fail, &&var, &&choice, &&oper, &&_IOError, &&_UserError, &&_FailError, &&_NondetError};
    goto *table[(*right)->get_kind()];
  fail:
    return DO_FAIL;
  var:
    throw "Program flounders";
  choice:
  oper:
    Engine::hfun(right);
    goto *table[(*right)->get_kind()];
  _IOError:
    return new ::_Prelude::_GT();
  _UserError:
    return new ::_Prelude::_GT();
  _FailError:
    return new ::_Prelude::_GT();
  _NondetError:
    return new ::_Prelude::_compare(arg1,((::_Prelude::_NondetError*) (*right))->arg1);
  }
}
