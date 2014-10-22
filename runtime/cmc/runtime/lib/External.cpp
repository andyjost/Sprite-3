// These are the external functions of the Prelude
// Functions yet to code throw an exception.
// The code of these functions is generated as a comment in Prelude.cpp
// Updated Wed Oct 22 10:50:41 PDT 2014

#include "Prelude.hpp"
#include "Litint.hpp"
#include <iostream>   // for debugging/printing

namespace _Prelude {
  using namespace Engine;
  using namespace std;

  // external Prelude.ensureNotFree
  Node* _ensureNotFree::hfun() {
    Engine::hfun(arg1);
    if ((*arg1)->get_kind() == VAR)
      throw "Suspending on variable";
    else
      return *arg1;
  }

  Node* __0x24_0x21::hfun() { throw "External \"Prelude.$!\" not implemented"; }
  Node* __0x24_0x21_0x21::hfun() { throw "External \"Prelude.$!!\" not implemented"; }
  Node* __0x24_0x23_0x23::hfun() { throw "External \"Prelude.$##\" not implemented"; }
  Node* _prim_error::hfun() { throw "External \"Prelude.prim_error\" not implemented"; }

  // external Prelude.failed
  Node* _failed::hfun() { 
    return Engine::Fail::getInstance();
  }

  // external Prelude.==
  Node* __0x3D_0x3D::hfun() { 
    return new Engine::__0x3D_0x3D(arg1, arg2);
  }

  // external Prelude.compare
  Node* _compare::hfun() { 
    return new Engine::_compare(arg1, arg2);
  }

  /*

    The following functions are defined in Litint and Litchar

  Node* _ord::hfun() { throw "External \"Prelude.ord\" not implemented"; }
  Node* _chr::hfun() { throw "External \"Prelude.chr\" not implemented"; }
  Node* __0x2B::hfun() { throw "External \"Prelude.+\" not implemented"; }
  Node* __0x2D::hfun() { throw "External \"Prelude.-\" not implemented"; }
  Node* __0x2A::hfun() { throw "External \"Prelude.*\" not implemented"; }
  Node* _div::hfun() { throw "External \"Prelude.div\" not implemented"; }
  Node* _mod::hfun() { throw "External \"Prelude.mod\" not implemented"; }

  */

  Node* _quot::hfun() { throw "External \"Prelude.quot\" not implemented"; }
  Node* _rem::hfun() { throw "External \"Prelude.rem\" not implemented"; }
  Node* _prim_negateFloat::hfun() { throw "External \"Prelude.prim_negateFloat\" not implemented"; }
  Node* __0x3D_0x3A_0x3D::hfun() { throw "External \"Prelude.=:=\" not implemented"; }
  Node* _success::hfun() { throw "External \"Prelude.success\" not implemented"; }
  Node* __0x26::hfun() { throw "External \"Prelude.&\" not implemented"; }
  Node* __0x3E_0x3E_0x3D::hfun() { throw "External \"Prelude.>>=\" not implemented"; }
  Node* _return::hfun() { throw "External \"Prelude.return\" not implemented"; }
  Node* _prim_putChar::hfun() { throw "External \"Prelude.prim_putChar\" not implemented"; }
  Node* _getChar::hfun() { throw "External \"Prelude.getChar\" not implemented"; }
  Node* _prim_readFile::hfun() { throw "External \"Prelude.prim_readFile\" not implemented"; }
  Node* _prim_readFileContents::hfun() { throw "External \"Prelude.prim_readFileContents\" not implemented"; }
  Node* _prim_writeFile::hfun() { throw "External \"Prelude.prim_writeFile\" not implemented"; }
  Node* _prim_appendFile::hfun() { throw "External \"Prelude.prim_appendFile\" not implemented"; }
  Node* _catch::hfun() { throw "External \"Prelude.catch\" not implemented"; }
  Node* _prim_show::hfun() { throw "External \"Prelude.prim_show\" not implemented"; }

  // external Prelude.?
  Node* __0x3F::hfun() {
    return new Engine::Choice(arg1, arg2);
  }

  // external Prelude.apply
  Node* _apply::hfun() {
    // TODO: put here the body of Engine::Apply
    return new Engine::Apply(arg1, arg2);
  }

  Node* _cond::hfun() { throw "External \"Prelude.cond\" not implemented"; }
  Node* _letrec::hfun() { throw "External \"Prelude.letrec\" not implemented"; }
  Node* __0x3D_0x3A_0x3C_0x3D::hfun() { throw "External \"Prelude.=:<=\" not implemented"; }
  Node* __0x3D_0x3A_0x3C_0x3C_0x3D::hfun() { throw "External \"Prelude.=:<<=\" not implemented"; }
  Node* _ifVar::hfun() { throw "External \"Prelude.ifVar\" not implemented"; }
  Node* _failure::hfun() { throw "External \"Prelude.failure\" not implemented"; }

}
