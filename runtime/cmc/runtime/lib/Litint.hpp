#pragma once

#include <string>
#include <iostream>
#include <sstream>
#include "Engine.hpp"
#include "Prelude.hpp"

namespace Litint {
  using namespace Engine;

  struct Litint : Constructor {
    int arg1;
    Litint(int _arg1) : arg1(_arg1) {}
    static Node** make(int _arg1) { return new  Node*(new Litint(_arg1)); }
    inline int get_kind() { return CTOR; }
    inline std::string name() { return "Litint"; }
    inline std::string to_s(int) { 
      stringstream ss;
      ss << arg1;
      return ss.str();
    }
    inline Node* nfun() { return this; }
    inline Node* afun() { return this; }
    inline Node* apply(Node**, int) { throw "Impossible partially applied Litint"; }
    /*inline*/ Node* boolequal(Node**);
    /*inline*/ Node* compare(Node**);
  };

#define DECL(INTNAME,EXTNAME) \
  struct INTNAME : Operation { \
    SYMBOL2(INTNAME,EXTNAME) \
    /*inline*/ Node* hfun(); \
  }; // end 

  DECL(__0x2B, "Litint::+")
  DECL(__0x2D, "Litint::-")
  DECL(__0x2A, "Litint::*")
  DECL(_div, "Litint::div")
  DECL(_mod, "Litint::mod")

}
