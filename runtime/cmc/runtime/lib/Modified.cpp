// These are prelude operation that are hand modified

#include "Prelude.hpp"
#include "Litint.hpp"
#include "Litchar.hpp"
#include <iostream>   // for debugging/printing

namespace _Prelude {
  using namespace Engine;
  using namespace std;

  Node* _ord::hfun() { // ord
    return new Litchar::_ord(arg1);
  }

  Node* _chr::hfun() { // chr
    return new Litchar::_chr(arg1); 
  }

#define ARITH(NAME) \
  Node* NAME::hfun() { \
    return new Litint::NAME(arg1, arg2); \
  } // end

  ARITH(__0x2B) // +
  ARITH(__0x2D) // -
  ARITH(__0x2A) // *
  ARITH(_div)
  ARITH(_mod)

};
