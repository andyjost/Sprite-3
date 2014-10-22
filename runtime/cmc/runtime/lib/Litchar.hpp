#pragma once

#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include "Engine.hpp"
#include "Litint.hpp"
#include "Prelude.hpp"

namespace _Prelude {
  using namespace Engine;

  struct Litchar : Constructor {
    char arg1;
    Litchar(char _arg1) : arg1(_arg1) {}
    static Node** make(char _arg1) { return new  Node*(new Litchar(_arg1)); }
    inline int get_kind() { return CTOR; }
    inline std::string name() { return "Litchar"; }
    inline std::string to_s(int) { 
      stringstream ss;
      ss << "'";
      if (isprint(arg1))
        ss << arg1;
      else
	switch (arg1) {
	case '\n': ss << "\\n"; break;
	case '\r': ss << "\\r"; break;
	default: ss << "\0x" << std::hex <<std::setw(4) << arg1; break;
	}
      ss << "'";
      return ss.str();
    }
    inline Node* nfun() { return this; }
    inline Node* afun() { return this; }
    inline Node* apply(Node**, int) { throw "Impossible partially applied Litchar"; }
    /*inline*/ Node* boolequal(Node**);
    /*inline*/ Node* compare(Node**);
  };

}
