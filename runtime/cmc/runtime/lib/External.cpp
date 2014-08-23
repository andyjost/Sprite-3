
#include "Prelude.hpp"
#include "Litint.hpp"
#include <iostream>   // for debugging/printing

namespace _Prelude {
  using namespace Engine;
  using namespace std;

  // TODO:  make a file of constants ???
  Node** TRUE  = _Prelude::_True::make();
  Node** FALSE = _Prelude::_False::make();

  // external Prelude.seq
  Node* _seq::hfun() { 
    Engine::hfun(arg1);
    return *arg2;
  }

  // external Prelude.ensureNotFree
  Node* _ensureNotFree::hfun() {
    Engine::hfun(arg1);
    if ((*arg1)->get_kind() == VAR)
      throw "Suspending on variable";
    else
      return *arg1;
  }

  // external Prelude.failed
  Node* _failed::hfun() { 
    return Engine::Fail::getInstance();
  }

  // external Prelude.==
  Node* __0x3D_0x3D::hfun() { 
    return new Engine::__0x3D_0x3D(arg1, arg2);
  }

  // external Prelude.prim_ord
  Node* _prim_ord::hfun() {
    char x = ((Litchar::Litchar*) (*arg1))->arg1;
    return new Litint::Litint((int) x);
  }

  // external Prelude.prim_chr
  Node* _prim_chr::hfun() {
    int x = ((Litint::Litint*) (*arg1))->arg1;
    return new Litchar::Litchar((char) x);
  }

  // external Prelude.prim_Int_plus
  Node* _prim_Int_plus::hfun() { 
    int left = ((Litint::Litint*) (*arg1))->arg1;
    int right = ((Litint::Litint*) (*arg2))->arg1;
    return new Litint::Litint(left + right);
  }

  // external Prelude.prim_Int_minus
  Node* _prim_Int_minus::hfun() { 
    int left = ((Litint::Litint*) (*arg1))->arg1;
    int right = ((Litint::Litint*) (*arg2))->arg1;
    return new Litint::Litint(left - right);
  }

  // external Prelude.prim_Int_times
  Node* _prim_Int_times::hfun() { 
    int left = ((Litint::Litint*) (*arg1))->arg1;
    int right = ((Litint::Litint*) (*arg2))->arg1;
    return new Litint::Litint(left * right);
  }

  // external Prelude.prim_Int_div
  Node* _prim_Int_div::hfun() { 
    int right = ((Litint::Litint*) (*arg2))->arg1;
    if (right == 0) return new _failed();
    else {
      int left = ((Litint::Litint*) (*arg1))->arg1;
      return new Litint::Litint(left / right);
    }    
  }

  // external Prelude.prim_Int_mod
  Node* _prim_Int_mod::hfun() { 
    int right = ((Litint::Litint*) (*arg2))->arg1;
    if (right == 0) return new _failed();
    else {
      int left = ((Litint::Litint*) (*arg1))->arg1;
      return new Litint::Litint(left % right);
    }    
  }

  // external Prelude.apply
  Node* _apply::hfun() {
    // TODO: put here the body of Engine::Apply
    return new Engine::Apply(arg1, arg2);
  }

   Node* _prim_error::hfun() { throw "External \"Prelude.prim_error\" not implemented"; }

   Node* _compare::hfun() { 
    return new Engine::_compare(arg1, arg2);
  }

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
   Node* _catchFail::hfun() { throw "External \"Prelude.catchFail\" not implemented"; }
   Node* _prim_show::hfun() { throw "External \"Prelude.prim_show\" not implemented"; }
   Node* _try::hfun() { throw "External \"Prelude.try\" not implemented"; }
   Node* _cond::hfun() { throw "External \"Prelude.cond\" not implemented"; }
   Node* _letrec::hfun() { throw "External \"Prelude.letrec\" not implemented"; }
   Node* __0x3D_0x3A_0x3C_0x3D::hfun() { throw "External \"Prelude.=:<=\" not implemented"; }
   Node* __0x3D_0x3A_0x3C_0x3C_0x3D::hfun() { throw "External \"Prelude.=:<<=\" not implemented"; }
   Node* _ifVar::hfun() { throw "External \"Prelude.ifVar\" not implemented"; }
   Node* _failure::hfun() { throw "External \"Prelude.failure\" not implemented"; }


}
