
#pragma once

#include <string>
#include <iostream>
#include "Engine.hpp"
#include "Litint.hpp"
#include "Litchar.hpp"


namespace _Prelude {
  using namespace Engine;

  struct __0x28_0x29 : Constructor { // ()
    __0x28_0x29() {}
    static Node** make() {
      return new Node*(new __0x28_0x29());
    }
    inline std::string name() { return "()"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      return name();
    }
    inline Node* apply(Node** _arg, int _missing) {
      throw "can't apply nullary symbol!";
      return 0;
    }
    inline int get_kind() { return (CTOR+0); }
    inline Node* nfun() {
      return this;
    }
    inline Node* afun() {
      return this;
    }
    /*inline*/ Node* boolequal(Node**);
    /*inline*/ Node* compare(Node**);
  };

  struct __0x5B_0x5D : Constructor { // []
    __0x5B_0x5D() {}
    static Node** make() {
      return new Node*(new __0x5B_0x5D());
    }
    inline std::string name() { return "[]"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      return name();
    }
    inline Node* apply(Node** _arg, int _missing) {
      throw "can't apply nullary symbol!";
      return 0;
    }
    inline int get_kind() { return (CTOR+0); }
    inline Node* nfun() {
      return this;
    }
    inline Node* afun() {
      return this;
    }
    /*inline*/ Node* boolequal(Node**);
    /*inline*/ Node* compare(Node**);
  };

  struct __0x3A : Constructor { // :
    Node** arg1;
    Node** arg2;
    __0x3A(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new __0x3A(_arg1, _arg2));
    }
    inline std::string name() { return ":"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new __0x3A(arg1, _arg);
      case 2: return new Engine::Partial(__0x3A::make(_arg), 1);
      }
    }
    inline int get_kind() { return (CTOR+1); }
    inline Node* nfun() {
      Engine::nfun(arg1);
      Engine::nfun(arg2);
      return this;
    }
    inline Node* afun() {
      if ((*arg1)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg2)->get_kind() == FAIL) return DO_FAIL;
      return this;
    }
    /*inline*/ Node* boolequal(Node**);
    /*inline*/ Node* compare(Node**);
  };

  struct __0x28_0x2C_0x29 : Constructor { // (,)
    Node** arg1;
    Node** arg2;
    __0x28_0x2C_0x29(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new __0x28_0x2C_0x29(_arg1, _arg2));
    }
    inline std::string name() { return "(,)"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new __0x28_0x2C_0x29(arg1, _arg);
      case 2: return new Engine::Partial(__0x28_0x2C_0x29::make(_arg), 1);
      }
    }
    inline int get_kind() { return (CTOR+0); }
    inline Node* nfun() {
      Engine::nfun(arg1);
      Engine::nfun(arg2);
      return this;
    }
    inline Node* afun() {
      if ((*arg1)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg2)->get_kind() == FAIL) return DO_FAIL;
      return this;
    }
    /*inline*/ Node* boolequal(Node**);
    /*inline*/ Node* compare(Node**);
  };

  struct __0x28_0x2C_0x2C_0x29 : Constructor { // (,,)
    Node** arg1;
    Node** arg2;
    Node** arg3;
    __0x28_0x2C_0x2C_0x29(Node** _arg1 = 0, Node** _arg2 = 0, Node** _arg3 = 0) : arg1(_arg1), arg2(_arg2), arg3(_arg3) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0, Node** _arg3 = 0) {
      return new Node*(new __0x28_0x2C_0x2C_0x29(_arg1, _arg2, _arg3));
    }
    inline std::string name() { return "(,,)"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      std::string s3 = arg3 == 0 ? UNDEF : (*arg3)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + "," + s3 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new __0x28_0x2C_0x2C_0x29(arg1, arg2, _arg);
      case 2: return new Engine::Partial(__0x28_0x2C_0x2C_0x29::make(arg1, _arg), 1);
      case 3: return new Engine::Partial(__0x28_0x2C_0x2C_0x29::make(_arg), 2);
      }
    }
    inline int get_kind() { return (CTOR+0); }
    inline Node* nfun() {
      Engine::nfun(arg1);
      Engine::nfun(arg2);
      Engine::nfun(arg3);
      return this;
    }
    inline Node* afun() {
      if ((*arg1)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg2)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg3)->get_kind() == FAIL) return DO_FAIL;
      return this;
    }
    /*inline*/ Node* boolequal(Node**);
    /*inline*/ Node* compare(Node**);
  };

  struct __0x28_0x2C_0x2C_0x2C_0x29 : Constructor { // (,,,)
    Node** arg1;
    Node** arg2;
    Node** arg3;
    Node** arg4;
    __0x28_0x2C_0x2C_0x2C_0x29(Node** _arg1 = 0, Node** _arg2 = 0, Node** _arg3 = 0, Node** _arg4 = 0) : arg1(_arg1), arg2(_arg2), arg3(_arg3), arg4(_arg4) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0, Node** _arg3 = 0, Node** _arg4 = 0) {
      return new Node*(new __0x28_0x2C_0x2C_0x2C_0x29(_arg1, _arg2, _arg3, _arg4));
    }
    inline std::string name() { return "(,,,)"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      std::string s3 = arg3 == 0 ? UNDEF : (*arg3)->to_s(n+1);
      std::string s4 = arg4 == 0 ? UNDEF : (*arg4)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + "," + s3 + "," + s4 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new __0x28_0x2C_0x2C_0x2C_0x29(arg1, arg2, arg3, _arg);
      case 2: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x29::make(arg1, arg2, _arg), 1);
      case 3: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x29::make(arg1, _arg), 2);
      case 4: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x29::make(_arg), 3);
      }
    }
    inline int get_kind() { return (CTOR+0); }
    inline Node* nfun() {
      Engine::nfun(arg1);
      Engine::nfun(arg2);
      Engine::nfun(arg3);
      Engine::nfun(arg4);
      return this;
    }
    inline Node* afun() {
      if ((*arg1)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg2)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg3)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg4)->get_kind() == FAIL) return DO_FAIL;
      return this;
    }
    /*inline*/ Node* boolequal(Node**);
    /*inline*/ Node* compare(Node**);
  };

  struct __0x28_0x2C_0x2C_0x2C_0x2C_0x29 : Constructor { // (,,,,)
    Node** arg1;
    Node** arg2;
    Node** arg3;
    Node** arg4;
    Node** arg5;
    __0x28_0x2C_0x2C_0x2C_0x2C_0x29(Node** _arg1 = 0, Node** _arg2 = 0, Node** _arg3 = 0, Node** _arg4 = 0, Node** _arg5 = 0) : arg1(_arg1), arg2(_arg2), arg3(_arg3), arg4(_arg4), arg5(_arg5) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0, Node** _arg3 = 0, Node** _arg4 = 0, Node** _arg5 = 0) {
      return new Node*(new __0x28_0x2C_0x2C_0x2C_0x2C_0x29(_arg1, _arg2, _arg3, _arg4, _arg5));
    }
    inline std::string name() { return "(,,,,)"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      std::string s3 = arg3 == 0 ? UNDEF : (*arg3)->to_s(n+1);
      std::string s4 = arg4 == 0 ? UNDEF : (*arg4)->to_s(n+1);
      std::string s5 = arg5 == 0 ? UNDEF : (*arg5)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + "," + s3 + "," + s4 + "," + s5 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new __0x28_0x2C_0x2C_0x2C_0x2C_0x29(arg1, arg2, arg3, arg4, _arg);
      case 2: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, arg2, arg3, _arg), 1);
      case 3: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, arg2, _arg), 2);
      case 4: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, _arg), 3);
      case 5: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x29::make(_arg), 4);
      }
    }
    inline int get_kind() { return (CTOR+0); }
    inline Node* nfun() {
      Engine::nfun(arg1);
      Engine::nfun(arg2);
      Engine::nfun(arg3);
      Engine::nfun(arg4);
      Engine::nfun(arg5);
      return this;
    }
    inline Node* afun() {
      if ((*arg1)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg2)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg3)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg4)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg5)->get_kind() == FAIL) return DO_FAIL;
      return this;
    }
    /*inline*/ Node* boolequal(Node**);
    /*inline*/ Node* compare(Node**);
  };

  struct __0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x29 : Constructor { // (,,,,,)
    Node** arg1;
    Node** arg2;
    Node** arg3;
    Node** arg4;
    Node** arg5;
    Node** arg6;
    __0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x29(Node** _arg1 = 0, Node** _arg2 = 0, Node** _arg3 = 0, Node** _arg4 = 0, Node** _arg5 = 0, Node** _arg6 = 0) : arg1(_arg1), arg2(_arg2), arg3(_arg3), arg4(_arg4), arg5(_arg5), arg6(_arg6) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0, Node** _arg3 = 0, Node** _arg4 = 0, Node** _arg5 = 0, Node** _arg6 = 0) {
      return new Node*(new __0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x29(_arg1, _arg2, _arg3, _arg4, _arg5, _arg6));
    }
    inline std::string name() { return "(,,,,,)"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      std::string s3 = arg3 == 0 ? UNDEF : (*arg3)->to_s(n+1);
      std::string s4 = arg4 == 0 ? UNDEF : (*arg4)->to_s(n+1);
      std::string s5 = arg5 == 0 ? UNDEF : (*arg5)->to_s(n+1);
      std::string s6 = arg6 == 0 ? UNDEF : (*arg6)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + "," + s3 + "," + s4 + "," + s5 + "," + s6 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new __0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x29(arg1, arg2, arg3, arg4, arg5, _arg);
      case 2: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, arg2, arg3, arg4, _arg), 1);
      case 3: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, arg2, arg3, _arg), 2);
      case 4: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, arg2, _arg), 3);
      case 5: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, _arg), 4);
      case 6: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(_arg), 5);
      }
    }
    inline int get_kind() { return (CTOR+0); }
    inline Node* nfun() {
      Engine::nfun(arg1);
      Engine::nfun(arg2);
      Engine::nfun(arg3);
      Engine::nfun(arg4);
      Engine::nfun(arg5);
      Engine::nfun(arg6);
      return this;
    }
    inline Node* afun() {
      if ((*arg1)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg2)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg3)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg4)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg5)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg6)->get_kind() == FAIL) return DO_FAIL;
      return this;
    }
    /*inline*/ Node* boolequal(Node**);
    /*inline*/ Node* compare(Node**);
  };

  struct __0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29 : Constructor { // (,,,,,,)
    Node** arg1;
    Node** arg2;
    Node** arg3;
    Node** arg4;
    Node** arg5;
    Node** arg6;
    Node** arg7;
    __0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29(Node** _arg1 = 0, Node** _arg2 = 0, Node** _arg3 = 0, Node** _arg4 = 0, Node** _arg5 = 0, Node** _arg6 = 0, Node** _arg7 = 0) : arg1(_arg1), arg2(_arg2), arg3(_arg3), arg4(_arg4), arg5(_arg5), arg6(_arg6), arg7(_arg7) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0, Node** _arg3 = 0, Node** _arg4 = 0, Node** _arg5 = 0, Node** _arg6 = 0, Node** _arg7 = 0) {
      return new Node*(new __0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29(_arg1, _arg2, _arg3, _arg4, _arg5, _arg6, _arg7));
    }
    inline std::string name() { return "(,,,,,,)"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      std::string s3 = arg3 == 0 ? UNDEF : (*arg3)->to_s(n+1);
      std::string s4 = arg4 == 0 ? UNDEF : (*arg4)->to_s(n+1);
      std::string s5 = arg5 == 0 ? UNDEF : (*arg5)->to_s(n+1);
      std::string s6 = arg6 == 0 ? UNDEF : (*arg6)->to_s(n+1);
      std::string s7 = arg7 == 0 ? UNDEF : (*arg7)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + "," + s3 + "," + s4 + "," + s5 + "," + s6 + "," + s7 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new __0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29(arg1, arg2, arg3, arg4, arg5, arg6, _arg);
      case 2: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, arg2, arg3, arg4, arg5, _arg), 1);
      case 3: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, arg2, arg3, arg4, _arg), 2);
      case 4: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, arg2, arg3, _arg), 3);
      case 5: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, arg2, _arg), 4);
      case 6: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, _arg), 5);
      case 7: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(_arg), 6);
      }
    }
    inline int get_kind() { return (CTOR+0); }
    inline Node* nfun() {
      Engine::nfun(arg1);
      Engine::nfun(arg2);
      Engine::nfun(arg3);
      Engine::nfun(arg4);
      Engine::nfun(arg5);
      Engine::nfun(arg6);
      Engine::nfun(arg7);
      return this;
    }
    inline Node* afun() {
      if ((*arg1)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg2)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg3)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg4)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg5)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg6)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg7)->get_kind() == FAIL) return DO_FAIL;
      return this;
    }
    /*inline*/ Node* boolequal(Node**);
    /*inline*/ Node* compare(Node**);
  };

  struct __0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29 : Constructor { // (,,,,,,,)
    Node** arg1;
    Node** arg2;
    Node** arg3;
    Node** arg4;
    Node** arg5;
    Node** arg6;
    Node** arg7;
    Node** arg8;
    __0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29(Node** _arg1 = 0, Node** _arg2 = 0, Node** _arg3 = 0, Node** _arg4 = 0, Node** _arg5 = 0, Node** _arg6 = 0, Node** _arg7 = 0, Node** _arg8 = 0) : arg1(_arg1), arg2(_arg2), arg3(_arg3), arg4(_arg4), arg5(_arg5), arg6(_arg6), arg7(_arg7), arg8(_arg8) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0, Node** _arg3 = 0, Node** _arg4 = 0, Node** _arg5 = 0, Node** _arg6 = 0, Node** _arg7 = 0, Node** _arg8 = 0) {
      return new Node*(new __0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29(_arg1, _arg2, _arg3, _arg4, _arg5, _arg6, _arg7, _arg8));
    }
    inline std::string name() { return "(,,,,,,,)"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      std::string s3 = arg3 == 0 ? UNDEF : (*arg3)->to_s(n+1);
      std::string s4 = arg4 == 0 ? UNDEF : (*arg4)->to_s(n+1);
      std::string s5 = arg5 == 0 ? UNDEF : (*arg5)->to_s(n+1);
      std::string s6 = arg6 == 0 ? UNDEF : (*arg6)->to_s(n+1);
      std::string s7 = arg7 == 0 ? UNDEF : (*arg7)->to_s(n+1);
      std::string s8 = arg8 == 0 ? UNDEF : (*arg8)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + "," + s3 + "," + s4 + "," + s5 + "," + s6 + "," + s7 + "," + s8 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new __0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29(arg1, arg2, arg3, arg4, arg5, arg6, arg7, _arg);
      case 2: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, arg2, arg3, arg4, arg5, arg6, _arg), 1);
      case 3: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, arg2, arg3, arg4, arg5, _arg), 2);
      case 4: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, arg2, arg3, arg4, _arg), 3);
      case 5: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, arg2, arg3, _arg), 4);
      case 6: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, arg2, _arg), 5);
      case 7: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, _arg), 6);
      case 8: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(_arg), 7);
      }
    }
    inline int get_kind() { return (CTOR+0); }
    inline Node* nfun() {
      Engine::nfun(arg1);
      Engine::nfun(arg2);
      Engine::nfun(arg3);
      Engine::nfun(arg4);
      Engine::nfun(arg5);
      Engine::nfun(arg6);
      Engine::nfun(arg7);
      Engine::nfun(arg8);
      return this;
    }
    inline Node* afun() {
      if ((*arg1)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg2)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg3)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg4)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg5)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg6)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg7)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg8)->get_kind() == FAIL) return DO_FAIL;
      return this;
    }
    /*inline*/ Node* boolequal(Node**);
    /*inline*/ Node* compare(Node**);
  };

  struct __0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29 : Constructor { // (,,,,,,,,)
    Node** arg1;
    Node** arg2;
    Node** arg3;
    Node** arg4;
    Node** arg5;
    Node** arg6;
    Node** arg7;
    Node** arg8;
    Node** arg9;
    __0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29(Node** _arg1 = 0, Node** _arg2 = 0, Node** _arg3 = 0, Node** _arg4 = 0, Node** _arg5 = 0, Node** _arg6 = 0, Node** _arg7 = 0, Node** _arg8 = 0, Node** _arg9 = 0) : arg1(_arg1), arg2(_arg2), arg3(_arg3), arg4(_arg4), arg5(_arg5), arg6(_arg6), arg7(_arg7), arg8(_arg8), arg9(_arg9) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0, Node** _arg3 = 0, Node** _arg4 = 0, Node** _arg5 = 0, Node** _arg6 = 0, Node** _arg7 = 0, Node** _arg8 = 0, Node** _arg9 = 0) {
      return new Node*(new __0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29(_arg1, _arg2, _arg3, _arg4, _arg5, _arg6, _arg7, _arg8, _arg9));
    }
    inline std::string name() { return "(,,,,,,,,)"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      std::string s3 = arg3 == 0 ? UNDEF : (*arg3)->to_s(n+1);
      std::string s4 = arg4 == 0 ? UNDEF : (*arg4)->to_s(n+1);
      std::string s5 = arg5 == 0 ? UNDEF : (*arg5)->to_s(n+1);
      std::string s6 = arg6 == 0 ? UNDEF : (*arg6)->to_s(n+1);
      std::string s7 = arg7 == 0 ? UNDEF : (*arg7)->to_s(n+1);
      std::string s8 = arg8 == 0 ? UNDEF : (*arg8)->to_s(n+1);
      std::string s9 = arg9 == 0 ? UNDEF : (*arg9)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + "," + s3 + "," + s4 + "," + s5 + "," + s6 + "," + s7 + "," + s8 + "," + s9 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new __0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, _arg);
      case 2: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, arg2, arg3, arg4, arg5, arg6, arg7, _arg), 1);
      case 3: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, arg2, arg3, arg4, arg5, arg6, _arg), 2);
      case 4: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, arg2, arg3, arg4, arg5, _arg), 3);
      case 5: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, arg2, arg3, arg4, _arg), 4);
      case 6: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, arg2, arg3, _arg), 5);
      case 7: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, arg2, _arg), 6);
      case 8: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, _arg), 7);
      case 9: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(_arg), 8);
      }
    }
    inline int get_kind() { return (CTOR+0); }
    inline Node* nfun() {
      Engine::nfun(arg1);
      Engine::nfun(arg2);
      Engine::nfun(arg3);
      Engine::nfun(arg4);
      Engine::nfun(arg5);
      Engine::nfun(arg6);
      Engine::nfun(arg7);
      Engine::nfun(arg8);
      Engine::nfun(arg9);
      return this;
    }
    inline Node* afun() {
      if ((*arg1)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg2)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg3)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg4)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg5)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg6)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg7)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg8)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg9)->get_kind() == FAIL) return DO_FAIL;
      return this;
    }
    /*inline*/ Node* boolequal(Node**);
    /*inline*/ Node* compare(Node**);
  };

  struct __0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29 : Constructor { // (,,,,,,,,,)
    Node** arg1;
    Node** arg2;
    Node** arg3;
    Node** arg4;
    Node** arg5;
    Node** arg6;
    Node** arg7;
    Node** arg8;
    Node** arg9;
    Node** arg10;
    __0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29(Node** _arg1 = 0, Node** _arg2 = 0, Node** _arg3 = 0, Node** _arg4 = 0, Node** _arg5 = 0, Node** _arg6 = 0, Node** _arg7 = 0, Node** _arg8 = 0, Node** _arg9 = 0, Node** _arg10 = 0) : arg1(_arg1), arg2(_arg2), arg3(_arg3), arg4(_arg4), arg5(_arg5), arg6(_arg6), arg7(_arg7), arg8(_arg8), arg9(_arg9), arg10(_arg10) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0, Node** _arg3 = 0, Node** _arg4 = 0, Node** _arg5 = 0, Node** _arg6 = 0, Node** _arg7 = 0, Node** _arg8 = 0, Node** _arg9 = 0, Node** _arg10 = 0) {
      return new Node*(new __0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29(_arg1, _arg2, _arg3, _arg4, _arg5, _arg6, _arg7, _arg8, _arg9, _arg10));
    }
    inline std::string name() { return "(,,,,,,,,,)"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      std::string s3 = arg3 == 0 ? UNDEF : (*arg3)->to_s(n+1);
      std::string s4 = arg4 == 0 ? UNDEF : (*arg4)->to_s(n+1);
      std::string s5 = arg5 == 0 ? UNDEF : (*arg5)->to_s(n+1);
      std::string s6 = arg6 == 0 ? UNDEF : (*arg6)->to_s(n+1);
      std::string s7 = arg7 == 0 ? UNDEF : (*arg7)->to_s(n+1);
      std::string s8 = arg8 == 0 ? UNDEF : (*arg8)->to_s(n+1);
      std::string s9 = arg9 == 0 ? UNDEF : (*arg9)->to_s(n+1);
      std::string s10 = arg10 == 0 ? UNDEF : (*arg10)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + "," + s3 + "," + s4 + "," + s5 + "," + s6 + "," + s7 + "," + s8 + "," + s9 + "," + s10 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new __0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, _arg);
      case 2: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, _arg), 1);
      case 3: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, arg2, arg3, arg4, arg5, arg6, arg7, _arg), 2);
      case 4: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, arg2, arg3, arg4, arg5, arg6, _arg), 3);
      case 5: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, arg2, arg3, arg4, arg5, _arg), 4);
      case 6: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, arg2, arg3, arg4, _arg), 5);
      case 7: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, arg2, arg3, _arg), 6);
      case 8: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, arg2, _arg), 7);
      case 9: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, _arg), 8);
      case 10: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(_arg), 9);
      }
    }
    inline int get_kind() { return (CTOR+0); }
    inline Node* nfun() {
      Engine::nfun(arg1);
      Engine::nfun(arg2);
      Engine::nfun(arg3);
      Engine::nfun(arg4);
      Engine::nfun(arg5);
      Engine::nfun(arg6);
      Engine::nfun(arg7);
      Engine::nfun(arg8);
      Engine::nfun(arg9);
      Engine::nfun(arg10);
      return this;
    }
    inline Node* afun() {
      if ((*arg1)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg2)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg3)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg4)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg5)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg6)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg7)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg8)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg9)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg10)->get_kind() == FAIL) return DO_FAIL;
      return this;
    }
    /*inline*/ Node* boolequal(Node**);
    /*inline*/ Node* compare(Node**);
  };

  struct __0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29 : Constructor { // (,,,,,,,,,,)
    Node** arg1;
    Node** arg2;
    Node** arg3;
    Node** arg4;
    Node** arg5;
    Node** arg6;
    Node** arg7;
    Node** arg8;
    Node** arg9;
    Node** arg10;
    Node** arg11;
    __0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29(Node** _arg1 = 0, Node** _arg2 = 0, Node** _arg3 = 0, Node** _arg4 = 0, Node** _arg5 = 0, Node** _arg6 = 0, Node** _arg7 = 0, Node** _arg8 = 0, Node** _arg9 = 0, Node** _arg10 = 0, Node** _arg11 = 0) : arg1(_arg1), arg2(_arg2), arg3(_arg3), arg4(_arg4), arg5(_arg5), arg6(_arg6), arg7(_arg7), arg8(_arg8), arg9(_arg9), arg10(_arg10), arg11(_arg11) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0, Node** _arg3 = 0, Node** _arg4 = 0, Node** _arg5 = 0, Node** _arg6 = 0, Node** _arg7 = 0, Node** _arg8 = 0, Node** _arg9 = 0, Node** _arg10 = 0, Node** _arg11 = 0) {
      return new Node*(new __0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29(_arg1, _arg2, _arg3, _arg4, _arg5, _arg6, _arg7, _arg8, _arg9, _arg10, _arg11));
    }
    inline std::string name() { return "(,,,,,,,,,,)"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      std::string s3 = arg3 == 0 ? UNDEF : (*arg3)->to_s(n+1);
      std::string s4 = arg4 == 0 ? UNDEF : (*arg4)->to_s(n+1);
      std::string s5 = arg5 == 0 ? UNDEF : (*arg5)->to_s(n+1);
      std::string s6 = arg6 == 0 ? UNDEF : (*arg6)->to_s(n+1);
      std::string s7 = arg7 == 0 ? UNDEF : (*arg7)->to_s(n+1);
      std::string s8 = arg8 == 0 ? UNDEF : (*arg8)->to_s(n+1);
      std::string s9 = arg9 == 0 ? UNDEF : (*arg9)->to_s(n+1);
      std::string s10 = arg10 == 0 ? UNDEF : (*arg10)->to_s(n+1);
      std::string s11 = arg11 == 0 ? UNDEF : (*arg11)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + "," + s3 + "," + s4 + "," + s5 + "," + s6 + "," + s7 + "," + s8 + "," + s9 + "," + s10 + "," + s11 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new __0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, _arg);
      case 2: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, _arg), 1);
      case 3: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, _arg), 2);
      case 4: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, arg2, arg3, arg4, arg5, arg6, arg7, _arg), 3);
      case 5: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, arg2, arg3, arg4, arg5, arg6, _arg), 4);
      case 6: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, arg2, arg3, arg4, arg5, _arg), 5);
      case 7: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, arg2, arg3, arg4, _arg), 6);
      case 8: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, arg2, arg3, _arg), 7);
      case 9: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, arg2, _arg), 8);
      case 10: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, _arg), 9);
      case 11: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(_arg), 10);
      }
    }
    inline int get_kind() { return (CTOR+0); }
    inline Node* nfun() {
      Engine::nfun(arg1);
      Engine::nfun(arg2);
      Engine::nfun(arg3);
      Engine::nfun(arg4);
      Engine::nfun(arg5);
      Engine::nfun(arg6);
      Engine::nfun(arg7);
      Engine::nfun(arg8);
      Engine::nfun(arg9);
      Engine::nfun(arg10);
      Engine::nfun(arg11);
      return this;
    }
    inline Node* afun() {
      if ((*arg1)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg2)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg3)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg4)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg5)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg6)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg7)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg8)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg9)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg10)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg11)->get_kind() == FAIL) return DO_FAIL;
      return this;
    }
    /*inline*/ Node* boolequal(Node**);
    /*inline*/ Node* compare(Node**);
  };

  struct __0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29 : Constructor { // (,,,,,,,,,,,)
    Node** arg1;
    Node** arg2;
    Node** arg3;
    Node** arg4;
    Node** arg5;
    Node** arg6;
    Node** arg7;
    Node** arg8;
    Node** arg9;
    Node** arg10;
    Node** arg11;
    Node** arg12;
    __0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29(Node** _arg1 = 0, Node** _arg2 = 0, Node** _arg3 = 0, Node** _arg4 = 0, Node** _arg5 = 0, Node** _arg6 = 0, Node** _arg7 = 0, Node** _arg8 = 0, Node** _arg9 = 0, Node** _arg10 = 0, Node** _arg11 = 0, Node** _arg12 = 0) : arg1(_arg1), arg2(_arg2), arg3(_arg3), arg4(_arg4), arg5(_arg5), arg6(_arg6), arg7(_arg7), arg8(_arg8), arg9(_arg9), arg10(_arg10), arg11(_arg11), arg12(_arg12) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0, Node** _arg3 = 0, Node** _arg4 = 0, Node** _arg5 = 0, Node** _arg6 = 0, Node** _arg7 = 0, Node** _arg8 = 0, Node** _arg9 = 0, Node** _arg10 = 0, Node** _arg11 = 0, Node** _arg12 = 0) {
      return new Node*(new __0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29(_arg1, _arg2, _arg3, _arg4, _arg5, _arg6, _arg7, _arg8, _arg9, _arg10, _arg11, _arg12));
    }
    inline std::string name() { return "(,,,,,,,,,,,)"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      std::string s3 = arg3 == 0 ? UNDEF : (*arg3)->to_s(n+1);
      std::string s4 = arg4 == 0 ? UNDEF : (*arg4)->to_s(n+1);
      std::string s5 = arg5 == 0 ? UNDEF : (*arg5)->to_s(n+1);
      std::string s6 = arg6 == 0 ? UNDEF : (*arg6)->to_s(n+1);
      std::string s7 = arg7 == 0 ? UNDEF : (*arg7)->to_s(n+1);
      std::string s8 = arg8 == 0 ? UNDEF : (*arg8)->to_s(n+1);
      std::string s9 = arg9 == 0 ? UNDEF : (*arg9)->to_s(n+1);
      std::string s10 = arg10 == 0 ? UNDEF : (*arg10)->to_s(n+1);
      std::string s11 = arg11 == 0 ? UNDEF : (*arg11)->to_s(n+1);
      std::string s12 = arg12 == 0 ? UNDEF : (*arg12)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + "," + s3 + "," + s4 + "," + s5 + "," + s6 + "," + s7 + "," + s8 + "," + s9 + "," + s10 + "," + s11 + "," + s12 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new __0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, _arg);
      case 2: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, _arg), 1);
      case 3: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, _arg), 2);
      case 4: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, _arg), 3);
      case 5: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, arg2, arg3, arg4, arg5, arg6, arg7, _arg), 4);
      case 6: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, arg2, arg3, arg4, arg5, arg6, _arg), 5);
      case 7: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, arg2, arg3, arg4, arg5, _arg), 6);
      case 8: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, arg2, arg3, arg4, _arg), 7);
      case 9: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, arg2, arg3, _arg), 8);
      case 10: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, arg2, _arg), 9);
      case 11: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, _arg), 10);
      case 12: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(_arg), 11);
      }
    }
    inline int get_kind() { return (CTOR+0); }
    inline Node* nfun() {
      Engine::nfun(arg1);
      Engine::nfun(arg2);
      Engine::nfun(arg3);
      Engine::nfun(arg4);
      Engine::nfun(arg5);
      Engine::nfun(arg6);
      Engine::nfun(arg7);
      Engine::nfun(arg8);
      Engine::nfun(arg9);
      Engine::nfun(arg10);
      Engine::nfun(arg11);
      Engine::nfun(arg12);
      return this;
    }
    inline Node* afun() {
      if ((*arg1)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg2)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg3)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg4)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg5)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg6)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg7)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg8)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg9)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg10)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg11)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg12)->get_kind() == FAIL) return DO_FAIL;
      return this;
    }
    /*inline*/ Node* boolequal(Node**);
    /*inline*/ Node* compare(Node**);
  };

  struct __0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29 : Constructor { // (,,,,,,,,,,,,)
    Node** arg1;
    Node** arg2;
    Node** arg3;
    Node** arg4;
    Node** arg5;
    Node** arg6;
    Node** arg7;
    Node** arg8;
    Node** arg9;
    Node** arg10;
    Node** arg11;
    Node** arg12;
    Node** arg13;
    __0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29(Node** _arg1 = 0, Node** _arg2 = 0, Node** _arg3 = 0, Node** _arg4 = 0, Node** _arg5 = 0, Node** _arg6 = 0, Node** _arg7 = 0, Node** _arg8 = 0, Node** _arg9 = 0, Node** _arg10 = 0, Node** _arg11 = 0, Node** _arg12 = 0, Node** _arg13 = 0) : arg1(_arg1), arg2(_arg2), arg3(_arg3), arg4(_arg4), arg5(_arg5), arg6(_arg6), arg7(_arg7), arg8(_arg8), arg9(_arg9), arg10(_arg10), arg11(_arg11), arg12(_arg12), arg13(_arg13) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0, Node** _arg3 = 0, Node** _arg4 = 0, Node** _arg5 = 0, Node** _arg6 = 0, Node** _arg7 = 0, Node** _arg8 = 0, Node** _arg9 = 0, Node** _arg10 = 0, Node** _arg11 = 0, Node** _arg12 = 0, Node** _arg13 = 0) {
      return new Node*(new __0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29(_arg1, _arg2, _arg3, _arg4, _arg5, _arg6, _arg7, _arg8, _arg9, _arg10, _arg11, _arg12, _arg13));
    }
    inline std::string name() { return "(,,,,,,,,,,,,)"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      std::string s3 = arg3 == 0 ? UNDEF : (*arg3)->to_s(n+1);
      std::string s4 = arg4 == 0 ? UNDEF : (*arg4)->to_s(n+1);
      std::string s5 = arg5 == 0 ? UNDEF : (*arg5)->to_s(n+1);
      std::string s6 = arg6 == 0 ? UNDEF : (*arg6)->to_s(n+1);
      std::string s7 = arg7 == 0 ? UNDEF : (*arg7)->to_s(n+1);
      std::string s8 = arg8 == 0 ? UNDEF : (*arg8)->to_s(n+1);
      std::string s9 = arg9 == 0 ? UNDEF : (*arg9)->to_s(n+1);
      std::string s10 = arg10 == 0 ? UNDEF : (*arg10)->to_s(n+1);
      std::string s11 = arg11 == 0 ? UNDEF : (*arg11)->to_s(n+1);
      std::string s12 = arg12 == 0 ? UNDEF : (*arg12)->to_s(n+1);
      std::string s13 = arg13 == 0 ? UNDEF : (*arg13)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + "," + s3 + "," + s4 + "," + s5 + "," + s6 + "," + s7 + "," + s8 + "," + s9 + "," + s10 + "," + s11 + "," + s12 + "," + s13 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new __0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, _arg);
      case 2: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, _arg), 1);
      case 3: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, _arg), 2);
      case 4: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, _arg), 3);
      case 5: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, _arg), 4);
      case 6: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, arg2, arg3, arg4, arg5, arg6, arg7, _arg), 5);
      case 7: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, arg2, arg3, arg4, arg5, arg6, _arg), 6);
      case 8: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, arg2, arg3, arg4, arg5, _arg), 7);
      case 9: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, arg2, arg3, arg4, _arg), 8);
      case 10: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, arg2, arg3, _arg), 9);
      case 11: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, arg2, _arg), 10);
      case 12: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, _arg), 11);
      case 13: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(_arg), 12);
      }
    }
    inline int get_kind() { return (CTOR+0); }
    inline Node* nfun() {
      Engine::nfun(arg1);
      Engine::nfun(arg2);
      Engine::nfun(arg3);
      Engine::nfun(arg4);
      Engine::nfun(arg5);
      Engine::nfun(arg6);
      Engine::nfun(arg7);
      Engine::nfun(arg8);
      Engine::nfun(arg9);
      Engine::nfun(arg10);
      Engine::nfun(arg11);
      Engine::nfun(arg12);
      Engine::nfun(arg13);
      return this;
    }
    inline Node* afun() {
      if ((*arg1)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg2)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg3)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg4)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg5)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg6)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg7)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg8)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg9)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg10)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg11)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg12)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg13)->get_kind() == FAIL) return DO_FAIL;
      return this;
    }
    /*inline*/ Node* boolequal(Node**);
    /*inline*/ Node* compare(Node**);
  };

  struct __0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29 : Constructor { // (,,,,,,,,,,,,,)
    Node** arg1;
    Node** arg2;
    Node** arg3;
    Node** arg4;
    Node** arg5;
    Node** arg6;
    Node** arg7;
    Node** arg8;
    Node** arg9;
    Node** arg10;
    Node** arg11;
    Node** arg12;
    Node** arg13;
    Node** arg14;
    __0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29(Node** _arg1 = 0, Node** _arg2 = 0, Node** _arg3 = 0, Node** _arg4 = 0, Node** _arg5 = 0, Node** _arg6 = 0, Node** _arg7 = 0, Node** _arg8 = 0, Node** _arg9 = 0, Node** _arg10 = 0, Node** _arg11 = 0, Node** _arg12 = 0, Node** _arg13 = 0, Node** _arg14 = 0) : arg1(_arg1), arg2(_arg2), arg3(_arg3), arg4(_arg4), arg5(_arg5), arg6(_arg6), arg7(_arg7), arg8(_arg8), arg9(_arg9), arg10(_arg10), arg11(_arg11), arg12(_arg12), arg13(_arg13), arg14(_arg14) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0, Node** _arg3 = 0, Node** _arg4 = 0, Node** _arg5 = 0, Node** _arg6 = 0, Node** _arg7 = 0, Node** _arg8 = 0, Node** _arg9 = 0, Node** _arg10 = 0, Node** _arg11 = 0, Node** _arg12 = 0, Node** _arg13 = 0, Node** _arg14 = 0) {
      return new Node*(new __0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29(_arg1, _arg2, _arg3, _arg4, _arg5, _arg6, _arg7, _arg8, _arg9, _arg10, _arg11, _arg12, _arg13, _arg14));
    }
    inline std::string name() { return "(,,,,,,,,,,,,,)"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      std::string s3 = arg3 == 0 ? UNDEF : (*arg3)->to_s(n+1);
      std::string s4 = arg4 == 0 ? UNDEF : (*arg4)->to_s(n+1);
      std::string s5 = arg5 == 0 ? UNDEF : (*arg5)->to_s(n+1);
      std::string s6 = arg6 == 0 ? UNDEF : (*arg6)->to_s(n+1);
      std::string s7 = arg7 == 0 ? UNDEF : (*arg7)->to_s(n+1);
      std::string s8 = arg8 == 0 ? UNDEF : (*arg8)->to_s(n+1);
      std::string s9 = arg9 == 0 ? UNDEF : (*arg9)->to_s(n+1);
      std::string s10 = arg10 == 0 ? UNDEF : (*arg10)->to_s(n+1);
      std::string s11 = arg11 == 0 ? UNDEF : (*arg11)->to_s(n+1);
      std::string s12 = arg12 == 0 ? UNDEF : (*arg12)->to_s(n+1);
      std::string s13 = arg13 == 0 ? UNDEF : (*arg13)->to_s(n+1);
      std::string s14 = arg14 == 0 ? UNDEF : (*arg14)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + "," + s3 + "," + s4 + "," + s5 + "," + s6 + "," + s7 + "," + s8 + "," + s9 + "," + s10 + "," + s11 + "," + s12 + "," + s13 + "," + s14 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new __0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, _arg);
      case 2: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, _arg), 1);
      case 3: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, _arg), 2);
      case 4: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, _arg), 3);
      case 5: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, _arg), 4);
      case 6: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, _arg), 5);
      case 7: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, arg2, arg3, arg4, arg5, arg6, arg7, _arg), 6);
      case 8: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, arg2, arg3, arg4, arg5, arg6, _arg), 7);
      case 9: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, arg2, arg3, arg4, arg5, _arg), 8);
      case 10: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, arg2, arg3, arg4, _arg), 9);
      case 11: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, arg2, arg3, _arg), 10);
      case 12: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, arg2, _arg), 11);
      case 13: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, _arg), 12);
      case 14: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(_arg), 13);
      }
    }
    inline int get_kind() { return (CTOR+0); }
    inline Node* nfun() {
      Engine::nfun(arg1);
      Engine::nfun(arg2);
      Engine::nfun(arg3);
      Engine::nfun(arg4);
      Engine::nfun(arg5);
      Engine::nfun(arg6);
      Engine::nfun(arg7);
      Engine::nfun(arg8);
      Engine::nfun(arg9);
      Engine::nfun(arg10);
      Engine::nfun(arg11);
      Engine::nfun(arg12);
      Engine::nfun(arg13);
      Engine::nfun(arg14);
      return this;
    }
    inline Node* afun() {
      if ((*arg1)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg2)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg3)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg4)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg5)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg6)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg7)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg8)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg9)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg10)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg11)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg12)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg13)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg14)->get_kind() == FAIL) return DO_FAIL;
      return this;
    }
    /*inline*/ Node* boolequal(Node**);
    /*inline*/ Node* compare(Node**);
  };

  struct __0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29 : Constructor { // (,,,,,,,,,,,,,,)
    Node** arg1;
    Node** arg2;
    Node** arg3;
    Node** arg4;
    Node** arg5;
    Node** arg6;
    Node** arg7;
    Node** arg8;
    Node** arg9;
    Node** arg10;
    Node** arg11;
    Node** arg12;
    Node** arg13;
    Node** arg14;
    Node** arg15;
    __0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29(Node** _arg1 = 0, Node** _arg2 = 0, Node** _arg3 = 0, Node** _arg4 = 0, Node** _arg5 = 0, Node** _arg6 = 0, Node** _arg7 = 0, Node** _arg8 = 0, Node** _arg9 = 0, Node** _arg10 = 0, Node** _arg11 = 0, Node** _arg12 = 0, Node** _arg13 = 0, Node** _arg14 = 0, Node** _arg15 = 0) : arg1(_arg1), arg2(_arg2), arg3(_arg3), arg4(_arg4), arg5(_arg5), arg6(_arg6), arg7(_arg7), arg8(_arg8), arg9(_arg9), arg10(_arg10), arg11(_arg11), arg12(_arg12), arg13(_arg13), arg14(_arg14), arg15(_arg15) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0, Node** _arg3 = 0, Node** _arg4 = 0, Node** _arg5 = 0, Node** _arg6 = 0, Node** _arg7 = 0, Node** _arg8 = 0, Node** _arg9 = 0, Node** _arg10 = 0, Node** _arg11 = 0, Node** _arg12 = 0, Node** _arg13 = 0, Node** _arg14 = 0, Node** _arg15 = 0) {
      return new Node*(new __0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29(_arg1, _arg2, _arg3, _arg4, _arg5, _arg6, _arg7, _arg8, _arg9, _arg10, _arg11, _arg12, _arg13, _arg14, _arg15));
    }
    inline std::string name() { return "(,,,,,,,,,,,,,,)"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      std::string s3 = arg3 == 0 ? UNDEF : (*arg3)->to_s(n+1);
      std::string s4 = arg4 == 0 ? UNDEF : (*arg4)->to_s(n+1);
      std::string s5 = arg5 == 0 ? UNDEF : (*arg5)->to_s(n+1);
      std::string s6 = arg6 == 0 ? UNDEF : (*arg6)->to_s(n+1);
      std::string s7 = arg7 == 0 ? UNDEF : (*arg7)->to_s(n+1);
      std::string s8 = arg8 == 0 ? UNDEF : (*arg8)->to_s(n+1);
      std::string s9 = arg9 == 0 ? UNDEF : (*arg9)->to_s(n+1);
      std::string s10 = arg10 == 0 ? UNDEF : (*arg10)->to_s(n+1);
      std::string s11 = arg11 == 0 ? UNDEF : (*arg11)->to_s(n+1);
      std::string s12 = arg12 == 0 ? UNDEF : (*arg12)->to_s(n+1);
      std::string s13 = arg13 == 0 ? UNDEF : (*arg13)->to_s(n+1);
      std::string s14 = arg14 == 0 ? UNDEF : (*arg14)->to_s(n+1);
      std::string s15 = arg15 == 0 ? UNDEF : (*arg15)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + "," + s3 + "," + s4 + "," + s5 + "," + s6 + "," + s7 + "," + s8 + "," + s9 + "," + s10 + "," + s11 + "," + s12 + "," + s13 + "," + s14 + "," + s15 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new __0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, _arg);
      case 2: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, _arg), 1);
      case 3: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, _arg), 2);
      case 4: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, _arg), 3);
      case 5: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, _arg), 4);
      case 6: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, _arg), 5);
      case 7: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, _arg), 6);
      case 8: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, arg2, arg3, arg4, arg5, arg6, arg7, _arg), 7);
      case 9: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, arg2, arg3, arg4, arg5, arg6, _arg), 8);
      case 10: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, arg2, arg3, arg4, arg5, _arg), 9);
      case 11: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, arg2, arg3, arg4, _arg), 10);
      case 12: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, arg2, arg3, _arg), 11);
      case 13: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, arg2, _arg), 12);
      case 14: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(arg1, _arg), 13);
      case 15: return new Engine::Partial(__0x28_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x2C_0x29::make(_arg), 14);
      }
    }
    inline int get_kind() { return (CTOR+0); }
    inline Node* nfun() {
      Engine::nfun(arg1);
      Engine::nfun(arg2);
      Engine::nfun(arg3);
      Engine::nfun(arg4);
      Engine::nfun(arg5);
      Engine::nfun(arg6);
      Engine::nfun(arg7);
      Engine::nfun(arg8);
      Engine::nfun(arg9);
      Engine::nfun(arg10);
      Engine::nfun(arg11);
      Engine::nfun(arg12);
      Engine::nfun(arg13);
      Engine::nfun(arg14);
      Engine::nfun(arg15);
      return this;
    }
    inline Node* afun() {
      if ((*arg1)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg2)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg3)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg4)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg5)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg6)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg7)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg8)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg9)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg10)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg11)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg12)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg13)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg14)->get_kind() == FAIL) return DO_FAIL;
      if ((*arg15)->get_kind() == FAIL) return DO_FAIL;
      return this;
    }
    /*inline*/ Node* boolequal(Node**);
    /*inline*/ Node* compare(Node**);
  };

  struct _False : Constructor { // False
    _False() {}
    static Node** make() {
      return new Node*(new _False());
    }
    inline std::string name() { return "False"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      return name();
    }
    inline Node* apply(Node** _arg, int _missing) {
      throw "can't apply nullary symbol!";
      return 0;
    }
    inline int get_kind() { return (CTOR+0); }
    inline Node* nfun() {
      return this;
    }
    inline Node* afun() {
      return this;
    }
    /*inline*/ Node* boolequal(Node**);
    /*inline*/ Node* compare(Node**);
  };

  struct _True : Constructor { // True
    _True() {}
    static Node** make() {
      return new Node*(new _True());
    }
    inline std::string name() { return "True"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      return name();
    }
    inline Node* apply(Node** _arg, int _missing) {
      throw "can't apply nullary symbol!";
      return 0;
    }
    inline int get_kind() { return (CTOR+1); }
    inline Node* nfun() {
      return this;
    }
    inline Node* afun() {
      return this;
    }
    /*inline*/ Node* boolequal(Node**);
    /*inline*/ Node* compare(Node**);
  };

  struct _LT : Constructor { // LT
    _LT() {}
    static Node** make() {
      return new Node*(new _LT());
    }
    inline std::string name() { return "LT"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      return name();
    }
    inline Node* apply(Node** _arg, int _missing) {
      throw "can't apply nullary symbol!";
      return 0;
    }
    inline int get_kind() { return (CTOR+0); }
    inline Node* nfun() {
      return this;
    }
    inline Node* afun() {
      return this;
    }
    /*inline*/ Node* boolequal(Node**);
    /*inline*/ Node* compare(Node**);
  };

  struct _EQ : Constructor { // EQ
    _EQ() {}
    static Node** make() {
      return new Node*(new _EQ());
    }
    inline std::string name() { return "EQ"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      return name();
    }
    inline Node* apply(Node** _arg, int _missing) {
      throw "can't apply nullary symbol!";
      return 0;
    }
    inline int get_kind() { return (CTOR+1); }
    inline Node* nfun() {
      return this;
    }
    inline Node* afun() {
      return this;
    }
    /*inline*/ Node* boolequal(Node**);
    /*inline*/ Node* compare(Node**);
  };

  struct _GT : Constructor { // GT
    _GT() {}
    static Node** make() {
      return new Node*(new _GT());
    }
    inline std::string name() { return "GT"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      return name();
    }
    inline Node* apply(Node** _arg, int _missing) {
      throw "can't apply nullary symbol!";
      return 0;
    }
    inline int get_kind() { return (CTOR+2); }
    inline Node* nfun() {
      return this;
    }
    inline Node* afun() {
      return this;
    }
    /*inline*/ Node* boolequal(Node**);
    /*inline*/ Node* compare(Node**);
  };

  struct _Nothing : Constructor { // Nothing
    _Nothing() {}
    static Node** make() {
      return new Node*(new _Nothing());
    }
    inline std::string name() { return "Nothing"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      return name();
    }
    inline Node* apply(Node** _arg, int _missing) {
      throw "can't apply nullary symbol!";
      return 0;
    }
    inline int get_kind() { return (CTOR+0); }
    inline Node* nfun() {
      return this;
    }
    inline Node* afun() {
      return this;
    }
    /*inline*/ Node* boolequal(Node**);
    /*inline*/ Node* compare(Node**);
  };

  struct _Just : Constructor { // Just
    Node** arg1;
    _Just(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _Just(_arg1));
    }
    inline std::string name() { return "Just"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _Just(_arg);
      }
    }
    inline int get_kind() { return (CTOR+1); }
    inline Node* nfun() {
      Engine::nfun(arg1);
      return this;
    }
    inline Node* afun() {
      if ((*arg1)->get_kind() == FAIL) return DO_FAIL;
      return this;
    }
    /*inline*/ Node* boolequal(Node**);
    /*inline*/ Node* compare(Node**);
  };

  struct _Left : Constructor { // Left
    Node** arg1;
    _Left(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _Left(_arg1));
    }
    inline std::string name() { return "Left"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _Left(_arg);
      }
    }
    inline int get_kind() { return (CTOR+0); }
    inline Node* nfun() {
      Engine::nfun(arg1);
      return this;
    }
    inline Node* afun() {
      if ((*arg1)->get_kind() == FAIL) return DO_FAIL;
      return this;
    }
    /*inline*/ Node* boolequal(Node**);
    /*inline*/ Node* compare(Node**);
  };

  struct _Right : Constructor { // Right
    Node** arg1;
    _Right(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _Right(_arg1));
    }
    inline std::string name() { return "Right"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _Right(_arg);
      }
    }
    inline int get_kind() { return (CTOR+1); }
    inline Node* nfun() {
      Engine::nfun(arg1);
      return this;
    }
    inline Node* afun() {
      if ((*arg1)->get_kind() == FAIL) return DO_FAIL;
      return this;
    }
    /*inline*/ Node* boolequal(Node**);
    /*inline*/ Node* compare(Node**);
  };

  struct _IOError : Constructor { // IOError
    Node** arg1;
    _IOError(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _IOError(_arg1));
    }
    inline std::string name() { return "IOError"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _IOError(_arg);
      }
    }
    inline int get_kind() { return (CTOR+0); }
    inline Node* nfun() {
      Engine::nfun(arg1);
      return this;
    }
    inline Node* afun() {
      if ((*arg1)->get_kind() == FAIL) return DO_FAIL;
      return this;
    }
    /*inline*/ Node* boolequal(Node**);
    /*inline*/ Node* compare(Node**);
  };

  struct __0x2E : Operation { // .
    Node** arg1;
    Node** arg2;
    __0x2E(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new __0x2E(_arg1, _arg2));
    }
    inline std::string name() { return "."; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new __0x2E(arg1, _arg);
      case 2: return new Engine::Partial(__0x2E::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct __0x2E_0x2E__0x23lambda1 : Operation { // .._#lambda1
    Node** arg1;
    Node** arg2;
    Node** arg3;
    __0x2E_0x2E__0x23lambda1(Node** _arg1 = 0, Node** _arg2 = 0, Node** _arg3 = 0) : arg1(_arg1), arg2(_arg2), arg3(_arg3) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0, Node** _arg3 = 0) {
      return new Node*(new __0x2E_0x2E__0x23lambda1(_arg1, _arg2, _arg3));
    }
    inline std::string name() { return ".._#lambda1"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      std::string s3 = arg3 == 0 ? UNDEF : (*arg3)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + "," + s3 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new __0x2E_0x2E__0x23lambda1(arg1, arg2, _arg);
      case 2: return new Engine::Partial(__0x2E_0x2E__0x23lambda1::make(arg1, _arg), 1);
      case 3: return new Engine::Partial(__0x2E_0x2E__0x23lambda1::make(_arg), 2);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _id : Operation { // id
    Node** arg1;
    _id(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _id(_arg1));
    }
    inline std::string name() { return "id"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _id(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _const : Operation { // const
    Node** arg1;
    Node** arg2;
    _const(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new _const(_arg1, _arg2));
    }
    inline std::string name() { return "const"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _const(arg1, _arg);
      case 2: return new Engine::Partial(_const::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _curry : Operation { // curry
    Node** arg1;
    Node** arg2;
    Node** arg3;
    _curry(Node** _arg1 = 0, Node** _arg2 = 0, Node** _arg3 = 0) : arg1(_arg1), arg2(_arg2), arg3(_arg3) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0, Node** _arg3 = 0) {
      return new Node*(new _curry(_arg1, _arg2, _arg3));
    }
    inline std::string name() { return "curry"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      std::string s3 = arg3 == 0 ? UNDEF : (*arg3)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + "," + s3 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _curry(arg1, arg2, _arg);
      case 2: return new Engine::Partial(_curry::make(arg1, _arg), 1);
      case 3: return new Engine::Partial(_curry::make(_arg), 2);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _uncurry : Operation { // uncurry
    Node** arg1;
    Node** arg2;
    _uncurry(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new _uncurry(_arg1, _arg2));
    }
    inline std::string name() { return "uncurry"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _uncurry(arg1, _arg);
      case 2: return new Engine::Partial(_uncurry::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _flip : Operation { // flip
    Node** arg1;
    Node** arg2;
    Node** arg3;
    _flip(Node** _arg1 = 0, Node** _arg2 = 0, Node** _arg3 = 0) : arg1(_arg1), arg2(_arg2), arg3(_arg3) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0, Node** _arg3 = 0) {
      return new Node*(new _flip(_arg1, _arg2, _arg3));
    }
    inline std::string name() { return "flip"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      std::string s3 = arg3 == 0 ? UNDEF : (*arg3)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + "," + s3 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _flip(arg1, arg2, _arg);
      case 2: return new Engine::Partial(_flip::make(arg1, _arg), 1);
      case 3: return new Engine::Partial(_flip::make(_arg), 2);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _until : Operation { // until
    Node** arg1;
    Node** arg2;
    Node** arg3;
    _until(Node** _arg1 = 0, Node** _arg2 = 0, Node** _arg3 = 0) : arg1(_arg1), arg2(_arg2), arg3(_arg3) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0, Node** _arg3 = 0) {
      return new Node*(new _until(_arg1, _arg2, _arg3));
    }
    inline std::string name() { return "until"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      std::string s3 = arg3 == 0 ? UNDEF : (*arg3)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + "," + s3 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _until(arg1, arg2, _arg);
      case 2: return new Engine::Partial(_until::make(arg1, _arg), 1);
      case 3: return new Engine::Partial(_until::make(_arg), 2);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _seq : Operation { // seq
    Node** arg1;
    Node** arg2;
    _seq(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new _seq(_arg1, _arg2));
    }
    inline std::string name() { return "seq"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _seq(arg1, _arg);
      case 2: return new Engine::Partial(_seq::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _ensureNotFree : Operation { // ensureNotFree
    Node** arg1;
    _ensureNotFree(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _ensureNotFree(_arg1));
    }
    inline std::string name() { return "ensureNotFree"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _ensureNotFree(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _ensureSpine : Operation { // ensureSpine
    Node** arg1;
    _ensureSpine(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _ensureSpine(_arg1));
    }
    inline std::string name() { return "ensureSpine"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _ensureSpine(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _ensureSpine_0x2EensureList_0x2E18 : Operation { // ensureSpine.ensureList.18
    Node** arg1;
    _ensureSpine_0x2EensureList_0x2E18(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _ensureSpine_0x2EensureList_0x2E18(_arg1));
    }
    inline std::string name() { return "ensureSpine.ensureList.18"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _ensureSpine_0x2EensureList_0x2E18(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct __0x24 : Operation { // $
    Node** arg1;
    Node** arg2;
    __0x24(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new __0x24(_arg1, _arg2));
    }
    inline std::string name() { return "$"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new __0x24(arg1, _arg);
      case 2: return new Engine::Partial(__0x24::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct __0x24_0x21 : Operation { // $!
    Node** arg1;
    Node** arg2;
    __0x24_0x21(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new __0x24_0x21(_arg1, _arg2));
    }
    inline std::string name() { return "$!"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new __0x24_0x21(arg1, _arg);
      case 2: return new Engine::Partial(__0x24_0x21::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct __0x24_0x21_0x21 : Operation { // $!!
    Node** arg1;
    Node** arg2;
    __0x24_0x21_0x21(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new __0x24_0x21_0x21(_arg1, _arg2));
    }
    inline std::string name() { return "$!!"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new __0x24_0x21_0x21(arg1, _arg);
      case 2: return new Engine::Partial(__0x24_0x21_0x21::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct __0x24_0x23 : Operation { // $#
    Node** arg1;
    Node** arg2;
    __0x24_0x23(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new __0x24_0x23(_arg1, _arg2));
    }
    inline std::string name() { return "$#"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new __0x24_0x23(arg1, _arg);
      case 2: return new Engine::Partial(__0x24_0x23::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct __0x24_0x23_0x23 : Operation { // $##
    Node** arg1;
    Node** arg2;
    __0x24_0x23_0x23(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new __0x24_0x23_0x23(_arg1, _arg2));
    }
    inline std::string name() { return "$##"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new __0x24_0x23_0x23(arg1, _arg);
      case 2: return new Engine::Partial(__0x24_0x23_0x23::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _error : Operation { // error
    Node** arg1;
    _error(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _error(_arg1));
    }
    inline std::string name() { return "error"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _error(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _prim_error : Operation { // prim_error
    Node** arg1;
    _prim_error(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _prim_error(_arg1));
    }
    inline std::string name() { return "prim_error"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _prim_error(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _failed : Operation { // failed
    _failed() {}
    static Node** make() {
      return new Node*(new _failed());
    }
    inline std::string name() { return "failed"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      return name();
    }
    inline Node* apply(Node** _arg, int _missing) {
      throw "can't apply nullary symbol!";
      return 0;
    }
    /*inline*/ Node* hfun();
  };

  struct __0x26_0x26 : Operation { // &&
    Node** arg1;
    Node** arg2;
    __0x26_0x26(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new __0x26_0x26(_arg1, _arg2));
    }
    inline std::string name() { return "&&"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new __0x26_0x26(arg1, _arg);
      case 2: return new Engine::Partial(__0x26_0x26::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct __0x7C_0x7C : Operation { // ||
    Node** arg1;
    Node** arg2;
    __0x7C_0x7C(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new __0x7C_0x7C(_arg1, _arg2));
    }
    inline std::string name() { return "||"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new __0x7C_0x7C(arg1, _arg);
      case 2: return new Engine::Partial(__0x7C_0x7C::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _not : Operation { // not
    Node** arg1;
    _not(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _not(_arg1));
    }
    inline std::string name() { return "not"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _not(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _otherwise : Operation { // otherwise
    _otherwise() {}
    static Node** make() {
      return new Node*(new _otherwise());
    }
    inline std::string name() { return "otherwise"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      return name();
    }
    inline Node* apply(Node** _arg, int _missing) {
      throw "can't apply nullary symbol!";
      return 0;
    }
    /*inline*/ Node* hfun();
  };

  struct _if_then_else : Operation { // if_then_else
    Node** arg1;
    Node** arg2;
    Node** arg3;
    _if_then_else(Node** _arg1 = 0, Node** _arg2 = 0, Node** _arg3 = 0) : arg1(_arg1), arg2(_arg2), arg3(_arg3) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0, Node** _arg3 = 0) {
      return new Node*(new _if_then_else(_arg1, _arg2, _arg3));
    }
    inline std::string name() { return "if_then_else"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      std::string s3 = arg3 == 0 ? UNDEF : (*arg3)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + "," + s3 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _if_then_else(arg1, arg2, _arg);
      case 2: return new Engine::Partial(_if_then_else::make(arg1, _arg), 1);
      case 3: return new Engine::Partial(_if_then_else::make(_arg), 2);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct __0x3D_0x3D : Operation { // ==
    Node** arg1;
    Node** arg2;
    __0x3D_0x3D(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new __0x3D_0x3D(_arg1, _arg2));
    }
    inline std::string name() { return "=="; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new __0x3D_0x3D(arg1, _arg);
      case 2: return new Engine::Partial(__0x3D_0x3D::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct __0x2F_0x3D : Operation { // /=
    Node** arg1;
    Node** arg2;
    __0x2F_0x3D(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new __0x2F_0x3D(_arg1, _arg2));
    }
    inline std::string name() { return "/="; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new __0x2F_0x3D(arg1, _arg);
      case 2: return new Engine::Partial(__0x2F_0x3D::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _compare : Operation { // compare
    Node** arg1;
    Node** arg2;
    _compare(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new _compare(_arg1, _arg2));
    }
    inline std::string name() { return "compare"; }
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
    /*inline*/ Node* hfun();
  };

  struct __0x3C : Operation { // <
    Node** arg1;
    Node** arg2;
    __0x3C(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new __0x3C(_arg1, _arg2));
    }
    inline std::string name() { return "<"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new __0x3C(arg1, _arg);
      case 2: return new Engine::Partial(__0x3C::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct __0x3E : Operation { // >
    Node** arg1;
    Node** arg2;
    __0x3E(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new __0x3E(_arg1, _arg2));
    }
    inline std::string name() { return ">"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new __0x3E(arg1, _arg);
      case 2: return new Engine::Partial(__0x3E::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct __0x3C_0x3D : Operation { // <=
    Node** arg1;
    Node** arg2;
    __0x3C_0x3D(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new __0x3C_0x3D(_arg1, _arg2));
    }
    inline std::string name() { return "<="; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new __0x3C_0x3D(arg1, _arg);
      case 2: return new Engine::Partial(__0x3C_0x3D::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct __0x3E_0x3D : Operation { // >=
    Node** arg1;
    Node** arg2;
    __0x3E_0x3D(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new __0x3E_0x3D(_arg1, _arg2));
    }
    inline std::string name() { return ">="; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new __0x3E_0x3D(arg1, _arg);
      case 2: return new Engine::Partial(__0x3E_0x3D::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _max : Operation { // max
    Node** arg1;
    Node** arg2;
    _max(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new _max(_arg1, _arg2));
    }
    inline std::string name() { return "max"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _max(arg1, _arg);
      case 2: return new Engine::Partial(_max::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _min : Operation { // min
    Node** arg1;
    Node** arg2;
    _min(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new _min(_arg1, _arg2));
    }
    inline std::string name() { return "min"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _min(arg1, _arg);
      case 2: return new Engine::Partial(_min::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _fst : Operation { // fst
    Node** arg1;
    _fst(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _fst(_arg1));
    }
    inline std::string name() { return "fst"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _fst(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _snd : Operation { // snd
    Node** arg1;
    _snd(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _snd(_arg1));
    }
    inline std::string name() { return "snd"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _snd(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _head : Operation { // head
    Node** arg1;
    _head(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _head(_arg1));
    }
    inline std::string name() { return "head"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _head(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _tail : Operation { // tail
    Node** arg1;
    _tail(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _tail(_arg1));
    }
    inline std::string name() { return "tail"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _tail(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _null : Operation { // null
    Node** arg1;
    _null(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _null(_arg1));
    }
    inline std::string name() { return "null"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _null(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct __0x2B_0x2B : Operation { // ++
    Node** arg1;
    Node** arg2;
    __0x2B_0x2B(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new __0x2B_0x2B(_arg1, _arg2));
    }
    inline std::string name() { return "++"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new __0x2B_0x2B(arg1, _arg);
      case 2: return new Engine::Partial(__0x2B_0x2B::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _length : Operation { // length
    Node** arg1;
    _length(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _length(_arg1));
    }
    inline std::string name() { return "length"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _length(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct __0x21_0x21 : Operation { // !!
    Node** arg1;
    Node** arg2;
    __0x21_0x21(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new __0x21_0x21(_arg1, _arg2));
    }
    inline std::string name() { return "!!"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new __0x21_0x21(arg1, _arg);
      case 2: return new Engine::Partial(__0x21_0x21::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _map : Operation { // map
    Node** arg1;
    Node** arg2;
    _map(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new _map(_arg1, _arg2));
    }
    inline std::string name() { return "map"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _map(arg1, _arg);
      case 2: return new Engine::Partial(_map::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _foldl : Operation { // foldl
    Node** arg1;
    Node** arg2;
    Node** arg3;
    _foldl(Node** _arg1 = 0, Node** _arg2 = 0, Node** _arg3 = 0) : arg1(_arg1), arg2(_arg2), arg3(_arg3) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0, Node** _arg3 = 0) {
      return new Node*(new _foldl(_arg1, _arg2, _arg3));
    }
    inline std::string name() { return "foldl"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      std::string s3 = arg3 == 0 ? UNDEF : (*arg3)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + "," + s3 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _foldl(arg1, arg2, _arg);
      case 2: return new Engine::Partial(_foldl::make(arg1, _arg), 1);
      case 3: return new Engine::Partial(_foldl::make(_arg), 2);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _foldl1 : Operation { // foldl1
    Node** arg1;
    Node** arg2;
    _foldl1(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new _foldl1(_arg1, _arg2));
    }
    inline std::string name() { return "foldl1"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _foldl1(arg1, _arg);
      case 2: return new Engine::Partial(_foldl1::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _foldr : Operation { // foldr
    Node** arg1;
    Node** arg2;
    Node** arg3;
    _foldr(Node** _arg1 = 0, Node** _arg2 = 0, Node** _arg3 = 0) : arg1(_arg1), arg2(_arg2), arg3(_arg3) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0, Node** _arg3 = 0) {
      return new Node*(new _foldr(_arg1, _arg2, _arg3));
    }
    inline std::string name() { return "foldr"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      std::string s3 = arg3 == 0 ? UNDEF : (*arg3)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + "," + s3 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _foldr(arg1, arg2, _arg);
      case 2: return new Engine::Partial(_foldr::make(arg1, _arg), 1);
      case 3: return new Engine::Partial(_foldr::make(_arg), 2);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _foldr1 : Operation { // foldr1
    Node** arg1;
    Node** arg2;
    _foldr1(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new _foldr1(_arg1, _arg2));
    }
    inline std::string name() { return "foldr1"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _foldr1(arg1, _arg);
      case 2: return new Engine::Partial(_foldr1::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _filter : Operation { // filter
    Node** arg1;
    Node** arg2;
    _filter(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new _filter(_arg1, _arg2));
    }
    inline std::string name() { return "filter"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _filter(arg1, _arg);
      case 2: return new Engine::Partial(_filter::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _zip : Operation { // zip
    Node** arg1;
    Node** arg2;
    _zip(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new _zip(_arg1, _arg2));
    }
    inline std::string name() { return "zip"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _zip(arg1, _arg);
      case 2: return new Engine::Partial(_zip::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _zip3 : Operation { // zip3
    Node** arg1;
    Node** arg2;
    Node** arg3;
    _zip3(Node** _arg1 = 0, Node** _arg2 = 0, Node** _arg3 = 0) : arg1(_arg1), arg2(_arg2), arg3(_arg3) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0, Node** _arg3 = 0) {
      return new Node*(new _zip3(_arg1, _arg2, _arg3));
    }
    inline std::string name() { return "zip3"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      std::string s3 = arg3 == 0 ? UNDEF : (*arg3)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + "," + s3 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _zip3(arg1, arg2, _arg);
      case 2: return new Engine::Partial(_zip3::make(arg1, _arg), 1);
      case 3: return new Engine::Partial(_zip3::make(_arg), 2);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _zipWith : Operation { // zipWith
    Node** arg1;
    Node** arg2;
    Node** arg3;
    _zipWith(Node** _arg1 = 0, Node** _arg2 = 0, Node** _arg3 = 0) : arg1(_arg1), arg2(_arg2), arg3(_arg3) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0, Node** _arg3 = 0) {
      return new Node*(new _zipWith(_arg1, _arg2, _arg3));
    }
    inline std::string name() { return "zipWith"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      std::string s3 = arg3 == 0 ? UNDEF : (*arg3)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + "," + s3 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _zipWith(arg1, arg2, _arg);
      case 2: return new Engine::Partial(_zipWith::make(arg1, _arg), 1);
      case 3: return new Engine::Partial(_zipWith::make(_arg), 2);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _zipWith3 : Operation { // zipWith3
    Node** arg1;
    Node** arg2;
    Node** arg3;
    Node** arg4;
    _zipWith3(Node** _arg1 = 0, Node** _arg2 = 0, Node** _arg3 = 0, Node** _arg4 = 0) : arg1(_arg1), arg2(_arg2), arg3(_arg3), arg4(_arg4) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0, Node** _arg3 = 0, Node** _arg4 = 0) {
      return new Node*(new _zipWith3(_arg1, _arg2, _arg3, _arg4));
    }
    inline std::string name() { return "zipWith3"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      std::string s3 = arg3 == 0 ? UNDEF : (*arg3)->to_s(n+1);
      std::string s4 = arg4 == 0 ? UNDEF : (*arg4)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + "," + s3 + "," + s4 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _zipWith3(arg1, arg2, arg3, _arg);
      case 2: return new Engine::Partial(_zipWith3::make(arg1, arg2, _arg), 1);
      case 3: return new Engine::Partial(_zipWith3::make(arg1, _arg), 2);
      case 4: return new Engine::Partial(_zipWith3::make(_arg), 3);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _unzip : Operation { // unzip
    Node** arg1;
    _unzip(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _unzip(_arg1));
    }
    inline std::string name() { return "unzip"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _unzip(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _unzip_case__0x231 : Operation { // unzip_case_#1
    Node** arg1;
    Node** arg2;
    _unzip_case__0x231(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new _unzip_case__0x231(_arg1, _arg2));
    }
    inline std::string name() { return "unzip_case_#1"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _unzip_case__0x231(arg1, _arg);
      case 2: return new Engine::Partial(_unzip_case__0x231::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _unzip_0x2E__0x23selFP2_0x23xs : Operation { // unzip._#selFP2#xs
    Node** arg1;
    _unzip_0x2E__0x23selFP2_0x23xs(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _unzip_0x2E__0x23selFP2_0x23xs(_arg1));
    }
    inline std::string name() { return "unzip._#selFP2#xs"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _unzip_0x2E__0x23selFP2_0x23xs(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _unzip_0x2E__0x23selFP3_0x23ys : Operation { // unzip._#selFP3#ys
    Node** arg1;
    _unzip_0x2E__0x23selFP3_0x23ys(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _unzip_0x2E__0x23selFP3_0x23ys(_arg1));
    }
    inline std::string name() { return "unzip._#selFP3#ys"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _unzip_0x2E__0x23selFP3_0x23ys(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _unzip3 : Operation { // unzip3
    Node** arg1;
    _unzip3(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _unzip3(_arg1));
    }
    inline std::string name() { return "unzip3"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _unzip3(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _unzip3_case__0x231 : Operation { // unzip3_case_#1
    Node** arg1;
    Node** arg2;
    _unzip3_case__0x231(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new _unzip3_case__0x231(_arg1, _arg2));
    }
    inline std::string name() { return "unzip3_case_#1"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _unzip3_case__0x231(arg1, _arg);
      case 2: return new Engine::Partial(_unzip3_case__0x231::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _unzip3_0x2E__0x23selFP5_0x23xs : Operation { // unzip3._#selFP5#xs
    Node** arg1;
    _unzip3_0x2E__0x23selFP5_0x23xs(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _unzip3_0x2E__0x23selFP5_0x23xs(_arg1));
    }
    inline std::string name() { return "unzip3._#selFP5#xs"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _unzip3_0x2E__0x23selFP5_0x23xs(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _unzip3_0x2E__0x23selFP6_0x23ys : Operation { // unzip3._#selFP6#ys
    Node** arg1;
    _unzip3_0x2E__0x23selFP6_0x23ys(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _unzip3_0x2E__0x23selFP6_0x23ys(_arg1));
    }
    inline std::string name() { return "unzip3._#selFP6#ys"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _unzip3_0x2E__0x23selFP6_0x23ys(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _unzip3_0x2E__0x23selFP7_0x23zs : Operation { // unzip3._#selFP7#zs
    Node** arg1;
    _unzip3_0x2E__0x23selFP7_0x23zs(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _unzip3_0x2E__0x23selFP7_0x23zs(_arg1));
    }
    inline std::string name() { return "unzip3._#selFP7#zs"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _unzip3_0x2E__0x23selFP7_0x23zs(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _concat : Operation { // concat
    Node** arg1;
    _concat(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _concat(_arg1));
    }
    inline std::string name() { return "concat"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _concat(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _concatMap : Operation { // concatMap
    Node** arg1;
    _concatMap(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _concatMap(_arg1));
    }
    inline std::string name() { return "concatMap"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _concatMap(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _iterate : Operation { // iterate
    Node** arg1;
    Node** arg2;
    _iterate(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new _iterate(_arg1, _arg2));
    }
    inline std::string name() { return "iterate"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _iterate(arg1, _arg);
      case 2: return new Engine::Partial(_iterate::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _repeat : Operation { // repeat
    Node** arg1;
    _repeat(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _repeat(_arg1));
    }
    inline std::string name() { return "repeat"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _repeat(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _replicate : Operation { // replicate
    Node** arg1;
    Node** arg2;
    _replicate(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new _replicate(_arg1, _arg2));
    }
    inline std::string name() { return "replicate"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _replicate(arg1, _arg);
      case 2: return new Engine::Partial(_replicate::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _take : Operation { // take
    Node** arg1;
    Node** arg2;
    _take(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new _take(_arg1, _arg2));
    }
    inline std::string name() { return "take"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _take(arg1, _arg);
      case 2: return new Engine::Partial(_take::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _take_0x2Etakep_0x2E220 : Operation { // take.takep.220
    Node** arg1;
    Node** arg2;
    _take_0x2Etakep_0x2E220(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new _take_0x2Etakep_0x2E220(_arg1, _arg2));
    }
    inline std::string name() { return "take.takep.220"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _take_0x2Etakep_0x2E220(arg1, _arg);
      case 2: return new Engine::Partial(_take_0x2Etakep_0x2E220::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _drop : Operation { // drop
    Node** arg1;
    Node** arg2;
    _drop(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new _drop(_arg1, _arg2));
    }
    inline std::string name() { return "drop"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _drop(arg1, _arg);
      case 2: return new Engine::Partial(_drop::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _drop_0x2Edropp_0x2E229 : Operation { // drop.dropp.229
    Node** arg1;
    Node** arg2;
    _drop_0x2Edropp_0x2E229(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new _drop_0x2Edropp_0x2E229(_arg1, _arg2));
    }
    inline std::string name() { return "drop.dropp.229"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _drop_0x2Edropp_0x2E229(arg1, _arg);
      case 2: return new Engine::Partial(_drop_0x2Edropp_0x2E229::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _splitAt : Operation { // splitAt
    Node** arg1;
    Node** arg2;
    _splitAt(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new _splitAt(_arg1, _arg2));
    }
    inline std::string name() { return "splitAt"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _splitAt(arg1, _arg);
      case 2: return new Engine::Partial(_splitAt::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _splitAt_0x2EsplitAtp_0x2E239 : Operation { // splitAt.splitAtp.239
    Node** arg1;
    Node** arg2;
    _splitAt_0x2EsplitAtp_0x2E239(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new _splitAt_0x2EsplitAtp_0x2E239(_arg1, _arg2));
    }
    inline std::string name() { return "splitAt.splitAtp.239"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _splitAt_0x2EsplitAtp_0x2E239(arg1, _arg);
      case 2: return new Engine::Partial(_splitAt_0x2EsplitAtp_0x2E239::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _splitAt_0x2EsplitAtp_0x2E239_let__0x231 : Operation { // splitAt.splitAtp.239_let_#1
    Node** arg1;
    Node** arg2;
    Node** arg3;
    _splitAt_0x2EsplitAtp_0x2E239_let__0x231(Node** _arg1 = 0, Node** _arg2 = 0, Node** _arg3 = 0) : arg1(_arg1), arg2(_arg2), arg3(_arg3) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0, Node** _arg3 = 0) {
      return new Node*(new _splitAt_0x2EsplitAtp_0x2E239_let__0x231(_arg1, _arg2, _arg3));
    }
    inline std::string name() { return "splitAt.splitAtp.239_let_#1"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      std::string s3 = arg3 == 0 ? UNDEF : (*arg3)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + "," + s3 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _splitAt_0x2EsplitAtp_0x2E239_let__0x231(arg1, arg2, _arg);
      case 2: return new Engine::Partial(_splitAt_0x2EsplitAtp_0x2E239_let__0x231::make(arg1, _arg), 1);
      case 3: return new Engine::Partial(_splitAt_0x2EsplitAtp_0x2E239_let__0x231::make(_arg), 2);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _splitAt_0x2EsplitAtp_0x2E239_0x2E__0x23selFP9_0x23ys : Operation { // splitAt.splitAtp.239._#selFP9#ys
    Node** arg1;
    _splitAt_0x2EsplitAtp_0x2E239_0x2E__0x23selFP9_0x23ys(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _splitAt_0x2EsplitAtp_0x2E239_0x2E__0x23selFP9_0x23ys(_arg1));
    }
    inline std::string name() { return "splitAt.splitAtp.239._#selFP9#ys"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _splitAt_0x2EsplitAtp_0x2E239_0x2E__0x23selFP9_0x23ys(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _splitAt_0x2EsplitAtp_0x2E239_0x2E__0x23selFP10_0x23zs : Operation { // splitAt.splitAtp.239._#selFP10#zs
    Node** arg1;
    _splitAt_0x2EsplitAtp_0x2E239_0x2E__0x23selFP10_0x23zs(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _splitAt_0x2EsplitAtp_0x2E239_0x2E__0x23selFP10_0x23zs(_arg1));
    }
    inline std::string name() { return "splitAt.splitAtp.239._#selFP10#zs"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _splitAt_0x2EsplitAtp_0x2E239_0x2E__0x23selFP10_0x23zs(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _takeWhile : Operation { // takeWhile
    Node** arg1;
    Node** arg2;
    _takeWhile(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new _takeWhile(_arg1, _arg2));
    }
    inline std::string name() { return "takeWhile"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _takeWhile(arg1, _arg);
      case 2: return new Engine::Partial(_takeWhile::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _dropWhile : Operation { // dropWhile
    Node** arg1;
    Node** arg2;
    _dropWhile(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new _dropWhile(_arg1, _arg2));
    }
    inline std::string name() { return "dropWhile"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _dropWhile(arg1, _arg);
      case 2: return new Engine::Partial(_dropWhile::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _span : Operation { // span
    Node** arg1;
    Node** arg2;
    _span(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new _span(_arg1, _arg2));
    }
    inline std::string name() { return "span"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _span(arg1, _arg);
      case 2: return new Engine::Partial(_span::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _span_case__0x232 : Operation { // span_case_#2
    Node** arg1;
    Node** arg2;
    Node** arg3;
    _span_case__0x232(Node** _arg1 = 0, Node** _arg2 = 0, Node** _arg3 = 0) : arg1(_arg1), arg2(_arg2), arg3(_arg3) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0, Node** _arg3 = 0) {
      return new Node*(new _span_case__0x232(_arg1, _arg2, _arg3));
    }
    inline std::string name() { return "span_case_#2"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      std::string s3 = arg3 == 0 ? UNDEF : (*arg3)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + "," + s3 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _span_case__0x232(arg1, arg2, _arg);
      case 2: return new Engine::Partial(_span_case__0x232::make(arg1, _arg), 1);
      case 3: return new Engine::Partial(_span_case__0x232::make(_arg), 2);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _span_case__0x231 : Operation { // span_case_#1
    Node** arg1;
    Node** arg2;
    _span_case__0x231(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new _span_case__0x231(_arg1, _arg2));
    }
    inline std::string name() { return "span_case_#1"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _span_case__0x231(arg1, _arg);
      case 2: return new Engine::Partial(_span_case__0x231::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _span_0x2E__0x23selFP12_0x23ys : Operation { // span._#selFP12#ys
    Node** arg1;
    _span_0x2E__0x23selFP12_0x23ys(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _span_0x2E__0x23selFP12_0x23ys(_arg1));
    }
    inline std::string name() { return "span._#selFP12#ys"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _span_0x2E__0x23selFP12_0x23ys(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _span_0x2E__0x23selFP13_0x23zs : Operation { // span._#selFP13#zs
    Node** arg1;
    _span_0x2E__0x23selFP13_0x23zs(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _span_0x2E__0x23selFP13_0x23zs(_arg1));
    }
    inline std::string name() { return "span._#selFP13#zs"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _span_0x2E__0x23selFP13_0x23zs(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _break : Operation { // break
    Node** arg1;
    _break(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _break(_arg1));
    }
    inline std::string name() { return "break"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _break(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _lines : Operation { // lines
    Node** arg1;
    _lines(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _lines(_arg1));
    }
    inline std::string name() { return "lines"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _lines(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _lines_0x2Esplitline_0x2E271 : Operation { // lines.splitline.271
    Node** arg1;
    _lines_0x2Esplitline_0x2E271(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _lines_0x2Esplitline_0x2E271(_arg1));
    }
    inline std::string name() { return "lines.splitline.271"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _lines_0x2Esplitline_0x2E271(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _lines_0x2Esplitline_0x2E271_case__0x231 : Operation { // lines.splitline.271_case_#1
    Node** arg1;
    Node** arg2;
    _lines_0x2Esplitline_0x2E271_case__0x231(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new _lines_0x2Esplitline_0x2E271_case__0x231(_arg1, _arg2));
    }
    inline std::string name() { return "lines.splitline.271_case_#1"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _lines_0x2Esplitline_0x2E271_case__0x231(arg1, _arg);
      case 2: return new Engine::Partial(_lines_0x2Esplitline_0x2E271_case__0x231::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _lines_0x2Esplitline_0x2E271_case__0x231_let__0x231 : Operation { // lines.splitline.271_case_#1_let_#1
    Node** arg1;
    Node** arg2;
    _lines_0x2Esplitline_0x2E271_case__0x231_let__0x231(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new _lines_0x2Esplitline_0x2E271_case__0x231_let__0x231(_arg1, _arg2));
    }
    inline std::string name() { return "lines.splitline.271_case_#1_let_#1"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _lines_0x2Esplitline_0x2E271_case__0x231_let__0x231(arg1, _arg);
      case 2: return new Engine::Partial(_lines_0x2Esplitline_0x2E271_case__0x231_let__0x231::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _lines_0x2Esplitline_0x2E271_0x2E__0x23selFP15_0x23ds : Operation { // lines.splitline.271._#selFP15#ds
    Node** arg1;
    _lines_0x2Esplitline_0x2E271_0x2E__0x23selFP15_0x23ds(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _lines_0x2Esplitline_0x2E271_0x2E__0x23selFP15_0x23ds(_arg1));
    }
    inline std::string name() { return "lines.splitline.271._#selFP15#ds"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _lines_0x2Esplitline_0x2E271_0x2E__0x23selFP15_0x23ds(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _lines_0x2Esplitline_0x2E271_0x2E__0x23selFP16_0x23es : Operation { // lines.splitline.271._#selFP16#es
    Node** arg1;
    _lines_0x2Esplitline_0x2E271_0x2E__0x23selFP16_0x23es(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _lines_0x2Esplitline_0x2E271_0x2E__0x23selFP16_0x23es(_arg1));
    }
    inline std::string name() { return "lines.splitline.271._#selFP16#es"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _lines_0x2Esplitline_0x2E271_0x2E__0x23selFP16_0x23es(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _lines_0x2E__0x23selFP18_0x23l : Operation { // lines._#selFP18#l
    Node** arg1;
    _lines_0x2E__0x23selFP18_0x23l(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _lines_0x2E__0x23selFP18_0x23l(_arg1));
    }
    inline std::string name() { return "lines._#selFP18#l"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _lines_0x2E__0x23selFP18_0x23l(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _lines_0x2E__0x23selFP19_0x23xs_l : Operation { // lines._#selFP19#xs_l
    Node** arg1;
    _lines_0x2E__0x23selFP19_0x23xs_l(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _lines_0x2E__0x23selFP19_0x23xs_l(_arg1));
    }
    inline std::string name() { return "lines._#selFP19#xs_l"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _lines_0x2E__0x23selFP19_0x23xs_l(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _unlines : Operation { // unlines
    Node** arg1;
    _unlines(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _unlines(_arg1));
    }
    inline std::string name() { return "unlines"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _unlines(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _words : Operation { // words
    Node** arg1;
    _words(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _words(_arg1));
    }
    inline std::string name() { return "words"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _words(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _words_case__0x231 : Operation { // words_case_#1
    Node** arg1;
    _words_case__0x231(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _words_case__0x231(_arg1));
    }
    inline std::string name() { return "words_case_#1"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _words_case__0x231(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _words_0x2EisSpace_0x2E283 : Operation { // words.isSpace.283
    Node** arg1;
    _words_0x2EisSpace_0x2E283(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _words_0x2EisSpace_0x2E283(_arg1));
    }
    inline std::string name() { return "words.isSpace.283"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _words_0x2EisSpace_0x2E283(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _words_0x2E__0x23selFP21_0x23w : Operation { // words._#selFP21#w
    Node** arg1;
    _words_0x2E__0x23selFP21_0x23w(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _words_0x2E__0x23selFP21_0x23w(_arg1));
    }
    inline std::string name() { return "words._#selFP21#w"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _words_0x2E__0x23selFP21_0x23w(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _words_0x2E__0x23selFP22_0x23s2 : Operation { // words._#selFP22#s2
    Node** arg1;
    _words_0x2E__0x23selFP22_0x23s2(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _words_0x2E__0x23selFP22_0x23s2(_arg1));
    }
    inline std::string name() { return "words._#selFP22#s2"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _words_0x2E__0x23selFP22_0x23s2(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _unwords : Operation { // unwords
    Node** arg1;
    _unwords(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _unwords(_arg1));
    }
    inline std::string name() { return "unwords"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _unwords(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _unwords_0x2E__0x23lambda5 : Operation { // unwords._#lambda5
    Node** arg1;
    Node** arg2;
    _unwords_0x2E__0x23lambda5(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new _unwords_0x2E__0x23lambda5(_arg1, _arg2));
    }
    inline std::string name() { return "unwords._#lambda5"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _unwords_0x2E__0x23lambda5(arg1, _arg);
      case 2: return new Engine::Partial(_unwords_0x2E__0x23lambda5::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _reverse : Operation { // reverse
    _reverse() {}
    static Node** make() {
      return new Node*(new _reverse());
    }
    inline std::string name() { return "reverse"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      return name();
    }
    inline Node* apply(Node** _arg, int _missing) {
      throw "can't apply nullary symbol!";
      return 0;
    }
    /*inline*/ Node* hfun();
  };

  struct _and : Operation { // and
    _and() {}
    static Node** make() {
      return new Node*(new _and());
    }
    inline std::string name() { return "and"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      return name();
    }
    inline Node* apply(Node** _arg, int _missing) {
      throw "can't apply nullary symbol!";
      return 0;
    }
    /*inline*/ Node* hfun();
  };

  struct _or : Operation { // or
    _or() {}
    static Node** make() {
      return new Node*(new _or());
    }
    inline std::string name() { return "or"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      return name();
    }
    inline Node* apply(Node** _arg, int _missing) {
      throw "can't apply nullary symbol!";
      return 0;
    }
    /*inline*/ Node* hfun();
  };

  struct _any : Operation { // any
    Node** arg1;
    _any(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _any(_arg1));
    }
    inline std::string name() { return "any"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _any(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _all : Operation { // all
    Node** arg1;
    _all(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _all(_arg1));
    }
    inline std::string name() { return "all"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _all(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _elem : Operation { // elem
    Node** arg1;
    _elem(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _elem(_arg1));
    }
    inline std::string name() { return "elem"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _elem(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _notElem : Operation { // notElem
    Node** arg1;
    _notElem(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _notElem(_arg1));
    }
    inline std::string name() { return "notElem"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _notElem(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _lookup : Operation { // lookup
    Node** arg1;
    Node** arg2;
    _lookup(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new _lookup(_arg1, _arg2));
    }
    inline std::string name() { return "lookup"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _lookup(arg1, _arg);
      case 2: return new Engine::Partial(_lookup::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _lookup_case__0x231 : Operation { // lookup_case_#1
    Node** arg1;
    Node** arg2;
    _lookup_case__0x231(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new _lookup_case__0x231(_arg1, _arg2));
    }
    inline std::string name() { return "lookup_case_#1"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _lookup_case__0x231(arg1, _arg);
      case 2: return new Engine::Partial(_lookup_case__0x231::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _enumFrom : Operation { // enumFrom
    Node** arg1;
    _enumFrom(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _enumFrom(_arg1));
    }
    inline std::string name() { return "enumFrom"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _enumFrom(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _enumFromThen : Operation { // enumFromThen
    Node** arg1;
    Node** arg2;
    _enumFromThen(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new _enumFromThen(_arg1, _arg2));
    }
    inline std::string name() { return "enumFromThen"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _enumFromThen(arg1, _arg);
      case 2: return new Engine::Partial(_enumFromThen::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _enumFromTo : Operation { // enumFromTo
    Node** arg1;
    Node** arg2;
    _enumFromTo(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new _enumFromTo(_arg1, _arg2));
    }
    inline std::string name() { return "enumFromTo"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _enumFromTo(arg1, _arg);
      case 2: return new Engine::Partial(_enumFromTo::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _enumFromThenTo : Operation { // enumFromThenTo
    Node** arg1;
    Node** arg2;
    Node** arg3;
    _enumFromThenTo(Node** _arg1 = 0, Node** _arg2 = 0, Node** _arg3 = 0) : arg1(_arg1), arg2(_arg2), arg3(_arg3) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0, Node** _arg3 = 0) {
      return new Node*(new _enumFromThenTo(_arg1, _arg2, _arg3));
    }
    inline std::string name() { return "enumFromThenTo"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      std::string s3 = arg3 == 0 ? UNDEF : (*arg3)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + "," + s3 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _enumFromThenTo(arg1, arg2, _arg);
      case 2: return new Engine::Partial(_enumFromThenTo::make(arg1, _arg), 1);
      case 3: return new Engine::Partial(_enumFromThenTo::make(_arg), 2);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _enumFromThenTo_0x2Ep_0x2E321 : Operation { // enumFromThenTo.p.321
    Node** arg1;
    Node** arg2;
    Node** arg3;
    Node** arg4;
    _enumFromThenTo_0x2Ep_0x2E321(Node** _arg1 = 0, Node** _arg2 = 0, Node** _arg3 = 0, Node** _arg4 = 0) : arg1(_arg1), arg2(_arg2), arg3(_arg3), arg4(_arg4) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0, Node** _arg3 = 0, Node** _arg4 = 0) {
      return new Node*(new _enumFromThenTo_0x2Ep_0x2E321(_arg1, _arg2, _arg3, _arg4));
    }
    inline std::string name() { return "enumFromThenTo.p.321"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      std::string s3 = arg3 == 0 ? UNDEF : (*arg3)->to_s(n+1);
      std::string s4 = arg4 == 0 ? UNDEF : (*arg4)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + "," + s3 + "," + s4 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _enumFromThenTo_0x2Ep_0x2E321(arg1, arg2, arg3, _arg);
      case 2: return new Engine::Partial(_enumFromThenTo_0x2Ep_0x2E321::make(arg1, arg2, _arg), 1);
      case 3: return new Engine::Partial(_enumFromThenTo_0x2Ep_0x2E321::make(arg1, _arg), 2);
      case 4: return new Engine::Partial(_enumFromThenTo_0x2Ep_0x2E321::make(_arg), 3);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _enumFromThenTo_0x2Ep_0x2E321_case__0x231 : Operation { // enumFromThenTo.p.321_case_#1
    Node** arg1;
    Node** arg2;
    _enumFromThenTo_0x2Ep_0x2E321_case__0x231(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new _enumFromThenTo_0x2Ep_0x2E321_case__0x231(_arg1, _arg2));
    }
    inline std::string name() { return "enumFromThenTo.p.321_case_#1"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _enumFromThenTo_0x2Ep_0x2E321_case__0x231(arg1, _arg);
      case 2: return new Engine::Partial(_enumFromThenTo_0x2Ep_0x2E321_case__0x231::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _ord : Operation { // ord
    Node** arg1;
    _ord(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _ord(_arg1));
    }
    inline std::string name() { return "ord"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _ord(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _prim_ord : Operation { // prim_ord
    Node** arg1;
    _prim_ord(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _prim_ord(_arg1));
    }
    inline std::string name() { return "prim_ord"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _prim_ord(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _chr : Operation { // chr
    Node** arg1;
    _chr(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _chr(_arg1));
    }
    inline std::string name() { return "chr"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _chr(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _prim_chr : Operation { // prim_chr
    Node** arg1;
    _prim_chr(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _prim_chr(_arg1));
    }
    inline std::string name() { return "prim_chr"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _prim_chr(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct __0x2B : Operation { // +
    Node** arg1;
    Node** arg2;
    __0x2B(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new __0x2B(_arg1, _arg2));
    }
    inline std::string name() { return "+"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new __0x2B(arg1, _arg);
      case 2: return new Engine::Partial(__0x2B::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _prim_Int_plus : Operation { // prim_Int_plus
    Node** arg1;
    Node** arg2;
    _prim_Int_plus(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new _prim_Int_plus(_arg1, _arg2));
    }
    inline std::string name() { return "prim_Int_plus"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _prim_Int_plus(arg1, _arg);
      case 2: return new Engine::Partial(_prim_Int_plus::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct __0x2D : Operation { // -
    Node** arg1;
    Node** arg2;
    __0x2D(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new __0x2D(_arg1, _arg2));
    }
    inline std::string name() { return "-"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new __0x2D(arg1, _arg);
      case 2: return new Engine::Partial(__0x2D::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _prim_Int_minus : Operation { // prim_Int_minus
    Node** arg1;
    Node** arg2;
    _prim_Int_minus(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new _prim_Int_minus(_arg1, _arg2));
    }
    inline std::string name() { return "prim_Int_minus"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _prim_Int_minus(arg1, _arg);
      case 2: return new Engine::Partial(_prim_Int_minus::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct __0x2A : Operation { // *
    Node** arg1;
    Node** arg2;
    __0x2A(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new __0x2A(_arg1, _arg2));
    }
    inline std::string name() { return "*"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new __0x2A(arg1, _arg);
      case 2: return new Engine::Partial(__0x2A::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _prim_Int_times : Operation { // prim_Int_times
    Node** arg1;
    Node** arg2;
    _prim_Int_times(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new _prim_Int_times(_arg1, _arg2));
    }
    inline std::string name() { return "prim_Int_times"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _prim_Int_times(arg1, _arg);
      case 2: return new Engine::Partial(_prim_Int_times::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _div : Operation { // div
    Node** arg1;
    Node** arg2;
    _div(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new _div(_arg1, _arg2));
    }
    inline std::string name() { return "div"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _div(arg1, _arg);
      case 2: return new Engine::Partial(_div::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _prim_Int_div : Operation { // prim_Int_div
    Node** arg1;
    Node** arg2;
    _prim_Int_div(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new _prim_Int_div(_arg1, _arg2));
    }
    inline std::string name() { return "prim_Int_div"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _prim_Int_div(arg1, _arg);
      case 2: return new Engine::Partial(_prim_Int_div::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _mod : Operation { // mod
    Node** arg1;
    Node** arg2;
    _mod(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new _mod(_arg1, _arg2));
    }
    inline std::string name() { return "mod"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _mod(arg1, _arg);
      case 2: return new Engine::Partial(_mod::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _prim_Int_mod : Operation { // prim_Int_mod
    Node** arg1;
    Node** arg2;
    _prim_Int_mod(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new _prim_Int_mod(_arg1, _arg2));
    }
    inline std::string name() { return "prim_Int_mod"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _prim_Int_mod(arg1, _arg);
      case 2: return new Engine::Partial(_prim_Int_mod::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _negate : Operation { // negate
    Node** arg1;
    _negate(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _negate(_arg1));
    }
    inline std::string name() { return "negate"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _negate(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _negateFloat : Operation { // negateFloat
    Node** arg1;
    _negateFloat(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _negateFloat(_arg1));
    }
    inline std::string name() { return "negateFloat"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _negateFloat(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _prim_negateFloat : Operation { // prim_negateFloat
    Node** arg1;
    _prim_negateFloat(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _prim_negateFloat(_arg1));
    }
    inline std::string name() { return "prim_negateFloat"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _prim_negateFloat(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct __0x3D_0x3A_0x3D : Operation { // =:=
    Node** arg1;
    Node** arg2;
    __0x3D_0x3A_0x3D(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new __0x3D_0x3A_0x3D(_arg1, _arg2));
    }
    inline std::string name() { return "=:="; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new __0x3D_0x3A_0x3D(arg1, _arg);
      case 2: return new Engine::Partial(__0x3D_0x3A_0x3D::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _success : Operation { // success
    _success() {}
    static Node** make() {
      return new Node*(new _success());
    }
    inline std::string name() { return "success"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      return name();
    }
    inline Node* apply(Node** _arg, int _missing) {
      throw "can't apply nullary symbol!";
      return 0;
    }
    /*inline*/ Node* hfun();
  };

  struct __0x26 : Operation { // &
    Node** arg1;
    Node** arg2;
    __0x26(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new __0x26(_arg1, _arg2));
    }
    inline std::string name() { return "&"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new __0x26(arg1, _arg);
      case 2: return new Engine::Partial(__0x26::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct __0x26_0x3E : Operation { // &>
    Node** arg1;
    Node** arg2;
    __0x26_0x3E(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new __0x26_0x3E(_arg1, _arg2));
    }
    inline std::string name() { return "&>"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new __0x26_0x3E(arg1, _arg);
      case 2: return new Engine::Partial(__0x26_0x3E::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _maybe : Operation { // maybe
    Node** arg1;
    Node** arg2;
    Node** arg3;
    _maybe(Node** _arg1 = 0, Node** _arg2 = 0, Node** _arg3 = 0) : arg1(_arg1), arg2(_arg2), arg3(_arg3) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0, Node** _arg3 = 0) {
      return new Node*(new _maybe(_arg1, _arg2, _arg3));
    }
    inline std::string name() { return "maybe"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      std::string s3 = arg3 == 0 ? UNDEF : (*arg3)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + "," + s3 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _maybe(arg1, arg2, _arg);
      case 2: return new Engine::Partial(_maybe::make(arg1, _arg), 1);
      case 3: return new Engine::Partial(_maybe::make(_arg), 2);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _either : Operation { // either
    Node** arg1;
    Node** arg2;
    Node** arg3;
    _either(Node** _arg1 = 0, Node** _arg2 = 0, Node** _arg3 = 0) : arg1(_arg1), arg2(_arg2), arg3(_arg3) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0, Node** _arg3 = 0) {
      return new Node*(new _either(_arg1, _arg2, _arg3));
    }
    inline std::string name() { return "either"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      std::string s3 = arg3 == 0 ? UNDEF : (*arg3)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + "," + s3 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _either(arg1, arg2, _arg);
      case 2: return new Engine::Partial(_either::make(arg1, _arg), 1);
      case 3: return new Engine::Partial(_either::make(_arg), 2);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct __0x3E_0x3E_0x3D : Operation { // >>=
    Node** arg1;
    Node** arg2;
    __0x3E_0x3E_0x3D(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new __0x3E_0x3E_0x3D(_arg1, _arg2));
    }
    inline std::string name() { return ">>="; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new __0x3E_0x3E_0x3D(arg1, _arg);
      case 2: return new Engine::Partial(__0x3E_0x3E_0x3D::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _return : Operation { // return
    Node** arg1;
    _return(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _return(_arg1));
    }
    inline std::string name() { return "return"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _return(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct __0x3E_0x3E : Operation { // >>
    Node** arg1;
    Node** arg2;
    __0x3E_0x3E(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new __0x3E_0x3E(_arg1, _arg2));
    }
    inline std::string name() { return ">>"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new __0x3E_0x3E(arg1, _arg);
      case 2: return new Engine::Partial(__0x3E_0x3E::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct __0x3E_0x3E_0x2E__0x23lambda6 : Operation { // >>._#lambda6
    Node** arg1;
    Node** arg2;
    __0x3E_0x3E_0x2E__0x23lambda6(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new __0x3E_0x3E_0x2E__0x23lambda6(_arg1, _arg2));
    }
    inline std::string name() { return ">>._#lambda6"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new __0x3E_0x3E_0x2E__0x23lambda6(arg1, _arg);
      case 2: return new Engine::Partial(__0x3E_0x3E_0x2E__0x23lambda6::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _done : Operation { // done
    _done() {}
    static Node** make() {
      return new Node*(new _done());
    }
    inline std::string name() { return "done"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      return name();
    }
    inline Node* apply(Node** _arg, int _missing) {
      throw "can't apply nullary symbol!";
      return 0;
    }
    /*inline*/ Node* hfun();
  };

  struct _putChar : Operation { // putChar
    Node** arg1;
    _putChar(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _putChar(_arg1));
    }
    inline std::string name() { return "putChar"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _putChar(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _prim_putChar : Operation { // prim_putChar
    Node** arg1;
    _prim_putChar(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _prim_putChar(_arg1));
    }
    inline std::string name() { return "prim_putChar"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _prim_putChar(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _getChar : Operation { // getChar
    _getChar() {}
    static Node** make() {
      return new Node*(new _getChar());
    }
    inline std::string name() { return "getChar"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      return name();
    }
    inline Node* apply(Node** _arg, int _missing) {
      throw "can't apply nullary symbol!";
      return 0;
    }
    /*inline*/ Node* hfun();
  };

  struct _readFile : Operation { // readFile
    Node** arg1;
    _readFile(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _readFile(_arg1));
    }
    inline std::string name() { return "readFile"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _readFile(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _prim_readFile : Operation { // prim_readFile
    Node** arg1;
    _prim_readFile(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _prim_readFile(_arg1));
    }
    inline std::string name() { return "prim_readFile"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _prim_readFile(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _prim_readFileContents : Operation { // prim_readFileContents
    Node** arg1;
    _prim_readFileContents(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _prim_readFileContents(_arg1));
    }
    inline std::string name() { return "prim_readFileContents"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _prim_readFileContents(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _writeFile : Operation { // writeFile
    Node** arg1;
    Node** arg2;
    _writeFile(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new _writeFile(_arg1, _arg2));
    }
    inline std::string name() { return "writeFile"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _writeFile(arg1, _arg);
      case 2: return new Engine::Partial(_writeFile::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _prim_writeFile : Operation { // prim_writeFile
    Node** arg1;
    Node** arg2;
    _prim_writeFile(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new _prim_writeFile(_arg1, _arg2));
    }
    inline std::string name() { return "prim_writeFile"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _prim_writeFile(arg1, _arg);
      case 2: return new Engine::Partial(_prim_writeFile::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _appendFile : Operation { // appendFile
    Node** arg1;
    Node** arg2;
    _appendFile(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new _appendFile(_arg1, _arg2));
    }
    inline std::string name() { return "appendFile"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _appendFile(arg1, _arg);
      case 2: return new Engine::Partial(_appendFile::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _prim_appendFile : Operation { // prim_appendFile
    Node** arg1;
    Node** arg2;
    _prim_appendFile(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new _prim_appendFile(_arg1, _arg2));
    }
    inline std::string name() { return "prim_appendFile"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _prim_appendFile(arg1, _arg);
      case 2: return new Engine::Partial(_prim_appendFile::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _putStr : Operation { // putStr
    Node** arg1;
    _putStr(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _putStr(_arg1));
    }
    inline std::string name() { return "putStr"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _putStr(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _putStrLn : Operation { // putStrLn
    Node** arg1;
    _putStrLn(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _putStrLn(_arg1));
    }
    inline std::string name() { return "putStrLn"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _putStrLn(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _getLine : Operation { // getLine
    _getLine() {}
    static Node** make() {
      return new Node*(new _getLine());
    }
    inline std::string name() { return "getLine"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      return name();
    }
    inline Node* apply(Node** _arg, int _missing) {
      throw "can't apply nullary symbol!";
      return 0;
    }
    /*inline*/ Node* hfun();
  };

  struct _getLine_0x2E__0x23lambda7 : Operation { // getLine._#lambda7
    Node** arg1;
    _getLine_0x2E__0x23lambda7(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _getLine_0x2E__0x23lambda7(_arg1));
    }
    inline std::string name() { return "getLine._#lambda7"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _getLine_0x2E__0x23lambda7(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _getLine_0x2E__0x23lambda7_0x2E__0x23lambda8 : Operation { // getLine._#lambda7._#lambda8
    Node** arg1;
    Node** arg2;
    _getLine_0x2E__0x23lambda7_0x2E__0x23lambda8(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new _getLine_0x2E__0x23lambda7_0x2E__0x23lambda8(_arg1, _arg2));
    }
    inline std::string name() { return "getLine._#lambda7._#lambda8"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _getLine_0x2E__0x23lambda7_0x2E__0x23lambda8(arg1, _arg);
      case 2: return new Engine::Partial(_getLine_0x2E__0x23lambda7_0x2E__0x23lambda8::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _userError : Operation { // userError
    Node** arg1;
    _userError(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _userError(_arg1));
    }
    inline std::string name() { return "userError"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _userError(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _ioError : Operation { // ioError
    Node** arg1;
    _ioError(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _ioError(_arg1));
    }
    inline std::string name() { return "ioError"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _ioError(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _showError : Operation { // showError
    Node** arg1;
    _showError(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _showError(_arg1));
    }
    inline std::string name() { return "showError"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _showError(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _catch : Operation { // catch
    Node** arg1;
    Node** arg2;
    _catch(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new _catch(_arg1, _arg2));
    }
    inline std::string name() { return "catch"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _catch(arg1, _arg);
      case 2: return new Engine::Partial(_catch::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _catchFail : Operation { // catchFail
    Node** arg1;
    Node** arg2;
    _catchFail(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new _catchFail(_arg1, _arg2));
    }
    inline std::string name() { return "catchFail"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _catchFail(arg1, _arg);
      case 2: return new Engine::Partial(_catchFail::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _show : Operation { // show
    Node** arg1;
    _show(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _show(_arg1));
    }
    inline std::string name() { return "show"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _show(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _prim_show : Operation { // prim_show
    Node** arg1;
    _prim_show(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _prim_show(_arg1));
    }
    inline std::string name() { return "prim_show"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _prim_show(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _print : Operation { // print
    Node** arg1;
    _print(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _print(_arg1));
    }
    inline std::string name() { return "print"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _print(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _doSolve : Operation { // doSolve
    Node** arg1;
    _doSolve(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _doSolve(_arg1));
    }
    inline std::string name() { return "doSolve"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _doSolve(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _sequenceIO : Operation { // sequenceIO
    Node** arg1;
    _sequenceIO(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _sequenceIO(_arg1));
    }
    inline std::string name() { return "sequenceIO"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _sequenceIO(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _sequenceIO_0x2E__0x23lambda9 : Operation { // sequenceIO._#lambda9
    Node** arg1;
    Node** arg2;
    _sequenceIO_0x2E__0x23lambda9(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new _sequenceIO_0x2E__0x23lambda9(_arg1, _arg2));
    }
    inline std::string name() { return "sequenceIO._#lambda9"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _sequenceIO_0x2E__0x23lambda9(arg1, _arg);
      case 2: return new Engine::Partial(_sequenceIO_0x2E__0x23lambda9::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _sequenceIO_0x2E__0x23lambda9_0x2E__0x23lambda10 : Operation { // sequenceIO._#lambda9._#lambda10
    Node** arg1;
    Node** arg2;
    _sequenceIO_0x2E__0x23lambda9_0x2E__0x23lambda10(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new _sequenceIO_0x2E__0x23lambda9_0x2E__0x23lambda10(_arg1, _arg2));
    }
    inline std::string name() { return "sequenceIO._#lambda9._#lambda10"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _sequenceIO_0x2E__0x23lambda9_0x2E__0x23lambda10(arg1, _arg);
      case 2: return new Engine::Partial(_sequenceIO_0x2E__0x23lambda9_0x2E__0x23lambda10::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _sequenceIO_ : Operation { // sequenceIO_
    _sequenceIO_() {}
    static Node** make() {
      return new Node*(new _sequenceIO_());
    }
    inline std::string name() { return "sequenceIO_"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      return name();
    }
    inline Node* apply(Node** _arg, int _missing) {
      throw "can't apply nullary symbol!";
      return 0;
    }
    /*inline*/ Node* hfun();
  };

  struct _mapIO : Operation { // mapIO
    Node** arg1;
    _mapIO(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _mapIO(_arg1));
    }
    inline std::string name() { return "mapIO"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _mapIO(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _mapIO_ : Operation { // mapIO_
    Node** arg1;
    _mapIO_(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _mapIO_(_arg1));
    }
    inline std::string name() { return "mapIO_"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _mapIO_(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _foldIO : Operation { // foldIO
    Node** arg1;
    Node** arg2;
    Node** arg3;
    _foldIO(Node** _arg1 = 0, Node** _arg2 = 0, Node** _arg3 = 0) : arg1(_arg1), arg2(_arg2), arg3(_arg3) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0, Node** _arg3 = 0) {
      return new Node*(new _foldIO(_arg1, _arg2, _arg3));
    }
    inline std::string name() { return "foldIO"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      std::string s3 = arg3 == 0 ? UNDEF : (*arg3)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + "," + s3 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _foldIO(arg1, arg2, _arg);
      case 2: return new Engine::Partial(_foldIO::make(arg1, _arg), 1);
      case 3: return new Engine::Partial(_foldIO::make(_arg), 2);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _foldIO_0x2E__0x23lambda11 : Operation { // foldIO._#lambda11
    Node** arg1;
    Node** arg2;
    Node** arg3;
    _foldIO_0x2E__0x23lambda11(Node** _arg1 = 0, Node** _arg2 = 0, Node** _arg3 = 0) : arg1(_arg1), arg2(_arg2), arg3(_arg3) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0, Node** _arg3 = 0) {
      return new Node*(new _foldIO_0x2E__0x23lambda11(_arg1, _arg2, _arg3));
    }
    inline std::string name() { return "foldIO._#lambda11"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      std::string s3 = arg3 == 0 ? UNDEF : (*arg3)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + "," + s3 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _foldIO_0x2E__0x23lambda11(arg1, arg2, _arg);
      case 2: return new Engine::Partial(_foldIO_0x2E__0x23lambda11::make(arg1, _arg), 1);
      case 3: return new Engine::Partial(_foldIO_0x2E__0x23lambda11::make(_arg), 2);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _liftIO : Operation { // liftIO
    Node** arg1;
    Node** arg2;
    _liftIO(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new _liftIO(_arg1, _arg2));
    }
    inline std::string name() { return "liftIO"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _liftIO(arg1, _arg);
      case 2: return new Engine::Partial(_liftIO::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _forIO : Operation { // forIO
    Node** arg1;
    Node** arg2;
    _forIO(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new _forIO(_arg1, _arg2));
    }
    inline std::string name() { return "forIO"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _forIO(arg1, _arg);
      case 2: return new Engine::Partial(_forIO::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _forIO_ : Operation { // forIO_
    Node** arg1;
    Node** arg2;
    _forIO_(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new _forIO_(_arg1, _arg2));
    }
    inline std::string name() { return "forIO_"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _forIO_(arg1, _arg);
      case 2: return new Engine::Partial(_forIO_::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _unless : Operation { // unless
    Node** arg1;
    Node** arg2;
    _unless(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new _unless(_arg1, _arg2));
    }
    inline std::string name() { return "unless"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _unless(arg1, _arg);
      case 2: return new Engine::Partial(_unless::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _when : Operation { // when
    Node** arg1;
    Node** arg2;
    _when(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new _when(_arg1, _arg2));
    }
    inline std::string name() { return "when"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _when(arg1, _arg);
      case 2: return new Engine::Partial(_when::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct __0x3F : Operation { // ?
    Node** arg1;
    Node** arg2;
    __0x3F(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new __0x3F(_arg1, _arg2));
    }
    inline std::string name() { return "?"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new __0x3F(arg1, _arg);
      case 2: return new Engine::Partial(__0x3F::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _unknown : Operation { // unknown
    _unknown() {}
    static Node** make() {
      return new Node*(new _unknown());
    }
    inline std::string name() { return "unknown"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      return name();
    }
    inline Node* apply(Node** _arg, int _missing) {
      throw "can't apply nullary symbol!";
      return 0;
    }
    /*inline*/ Node* hfun();
  };

  struct _getAllValues : Operation { // getAllValues
    Node** arg1;
    _getAllValues(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _getAllValues(_arg1));
    }
    inline std::string name() { return "getAllValues"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _getAllValues(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _getSomeValue : Operation { // getSomeValue
    Node** arg1;
    _getSomeValue(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _getSomeValue(_arg1));
    }
    inline std::string name() { return "getSomeValue"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _getSomeValue(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _try : Operation { // try
    Node** arg1;
    _try(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _try(_arg1));
    }
    inline std::string name() { return "try"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _try(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _inject : Operation { // inject
    Node** arg1;
    Node** arg2;
    _inject(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new _inject(_arg1, _arg2));
    }
    inline std::string name() { return "inject"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _inject(arg1, _arg);
      case 2: return new Engine::Partial(_inject::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _inject_0x2E__0x23lambda12 : Operation { // inject._#lambda12
    Node** arg1;
    Node** arg2;
    Node** arg3;
    _inject_0x2E__0x23lambda12(Node** _arg1 = 0, Node** _arg2 = 0, Node** _arg3 = 0) : arg1(_arg1), arg2(_arg2), arg3(_arg3) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0, Node** _arg3 = 0) {
      return new Node*(new _inject_0x2E__0x23lambda12(_arg1, _arg2, _arg3));
    }
    inline std::string name() { return "inject._#lambda12"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      std::string s3 = arg3 == 0 ? UNDEF : (*arg3)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + "," + s3 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _inject_0x2E__0x23lambda12(arg1, arg2, _arg);
      case 2: return new Engine::Partial(_inject_0x2E__0x23lambda12::make(arg1, _arg), 1);
      case 3: return new Engine::Partial(_inject_0x2E__0x23lambda12::make(_arg), 2);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _solveAll : Operation { // solveAll
    Node** arg1;
    _solveAll(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _solveAll(_arg1));
    }
    inline std::string name() { return "solveAll"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _solveAll(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _solveAll_0x2Eevalall3_0x2E438 : Operation { // solveAll.evalall3.438
    Node** arg1;
    Node** arg2;
    _solveAll_0x2Eevalall3_0x2E438(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new _solveAll_0x2Eevalall3_0x2E438(_arg1, _arg2));
    }
    inline std::string name() { return "solveAll.evalall3.438"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _solveAll_0x2Eevalall3_0x2E438(arg1, _arg);
      case 2: return new Engine::Partial(_solveAll_0x2Eevalall3_0x2E438::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _solveAll_0x2Eevalall3_0x2E438_case__0x231 : Operation { // solveAll.evalall3.438_case_#1
    Node** arg1;
    Node** arg2;
    Node** arg3;
    _solveAll_0x2Eevalall3_0x2E438_case__0x231(Node** _arg1 = 0, Node** _arg2 = 0, Node** _arg3 = 0) : arg1(_arg1), arg2(_arg2), arg3(_arg3) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0, Node** _arg3 = 0) {
      return new Node*(new _solveAll_0x2Eevalall3_0x2E438_case__0x231(_arg1, _arg2, _arg3));
    }
    inline std::string name() { return "solveAll.evalall3.438_case_#1"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      std::string s3 = arg3 == 0 ? UNDEF : (*arg3)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + "," + s3 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _solveAll_0x2Eevalall3_0x2E438_case__0x231(arg1, arg2, _arg);
      case 2: return new Engine::Partial(_solveAll_0x2Eevalall3_0x2E438_case__0x231::make(arg1, _arg), 1);
      case 3: return new Engine::Partial(_solveAll_0x2Eevalall3_0x2E438_case__0x231::make(_arg), 2);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _solveAll_0x2Eevalall2_0x2E438 : Operation { // solveAll.evalall2.438
    Node** arg1;
    _solveAll_0x2Eevalall2_0x2E438(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _solveAll_0x2Eevalall2_0x2E438(_arg1));
    }
    inline std::string name() { return "solveAll.evalall2.438"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _solveAll_0x2Eevalall2_0x2E438(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _solveAll_0x2Eevalall_0x2E438 : Operation { // solveAll.evalall.438
    Node** arg1;
    _solveAll_0x2Eevalall_0x2E438(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _solveAll_0x2Eevalall_0x2E438(_arg1));
    }
    inline std::string name() { return "solveAll.evalall.438"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _solveAll_0x2Eevalall_0x2E438(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _solveAll2 : Operation { // solveAll2
    Node** arg1;
    _solveAll2(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _solveAll2(_arg1));
    }
    inline std::string name() { return "solveAll2"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _solveAll2(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _solveAll2_0x2EevalResult_0x2E456 : Operation { // solveAll2.evalResult.456
    Node** arg1;
    _solveAll2_0x2EevalResult_0x2E456(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _solveAll2_0x2EevalResult_0x2E456(_arg1));
    }
    inline std::string name() { return "solveAll2.evalResult.456"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _solveAll2_0x2EevalResult_0x2E456(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _once : Operation { // once
    Node** arg1;
    _once(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _once(_arg1));
    }
    inline std::string name() { return "once"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _once(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _best : Operation { // best
    Node** arg1;
    Node** arg2;
    _best(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new _best(_arg1, _arg2));
    }
    inline std::string name() { return "best"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _best(arg1, _arg);
      case 2: return new Engine::Partial(_best::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _best_0x2Econstrain_0x2E466 : Operation { // best.constrain.466
    Node** arg1;
    Node** arg2;
    Node** arg3;
    _best_0x2Econstrain_0x2E466(Node** _arg1 = 0, Node** _arg2 = 0, Node** _arg3 = 0) : arg1(_arg1), arg2(_arg2), arg3(_arg3) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0, Node** _arg3 = 0) {
      return new Node*(new _best_0x2Econstrain_0x2E466(_arg1, _arg2, _arg3));
    }
    inline std::string name() { return "best.constrain.466"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      std::string s3 = arg3 == 0 ? UNDEF : (*arg3)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + "," + s3 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _best_0x2Econstrain_0x2E466(arg1, arg2, _arg);
      case 2: return new Engine::Partial(_best_0x2Econstrain_0x2E466::make(arg1, _arg), 1);
      case 3: return new Engine::Partial(_best_0x2Econstrain_0x2E466::make(_arg), 2);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _best_0x2Econstrain_0x2E466_0x2E__0x23lambda13 : Operation { // best.constrain.466._#lambda13
    Node** arg1;
    Node** arg2;
    Node** arg3;
    _best_0x2Econstrain_0x2E466_0x2E__0x23lambda13(Node** _arg1 = 0, Node** _arg2 = 0, Node** _arg3 = 0) : arg1(_arg1), arg2(_arg2), arg3(_arg3) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0, Node** _arg3 = 0) {
      return new Node*(new _best_0x2Econstrain_0x2E466_0x2E__0x23lambda13(_arg1, _arg2, _arg3));
    }
    inline std::string name() { return "best.constrain.466._#lambda13"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      std::string s3 = arg3 == 0 ? UNDEF : (*arg3)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + "," + s3 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _best_0x2Econstrain_0x2E466_0x2E__0x23lambda13(arg1, arg2, _arg);
      case 2: return new Engine::Partial(_best_0x2Econstrain_0x2E466_0x2E__0x23lambda13::make(arg1, _arg), 1);
      case 3: return new Engine::Partial(_best_0x2Econstrain_0x2E466_0x2E__0x23lambda13::make(_arg), 2);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _best_0x2EbestHelp_0x2E466 : Operation { // best.bestHelp.466
    Node** arg1;
    Node** arg2;
    Node** arg3;
    Node** arg4;
    _best_0x2EbestHelp_0x2E466(Node** _arg1 = 0, Node** _arg2 = 0, Node** _arg3 = 0, Node** _arg4 = 0) : arg1(_arg1), arg2(_arg2), arg3(_arg3), arg4(_arg4) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0, Node** _arg3 = 0, Node** _arg4 = 0) {
      return new Node*(new _best_0x2EbestHelp_0x2E466(_arg1, _arg2, _arg3, _arg4));
    }
    inline std::string name() { return "best.bestHelp.466"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      std::string s3 = arg3 == 0 ? UNDEF : (*arg3)->to_s(n+1);
      std::string s4 = arg4 == 0 ? UNDEF : (*arg4)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + "," + s3 + "," + s4 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _best_0x2EbestHelp_0x2E466(arg1, arg2, arg3, _arg);
      case 2: return new Engine::Partial(_best_0x2EbestHelp_0x2E466::make(arg1, arg2, _arg), 1);
      case 3: return new Engine::Partial(_best_0x2EbestHelp_0x2E466::make(arg1, _arg), 2);
      case 4: return new Engine::Partial(_best_0x2EbestHelp_0x2E466::make(_arg), 3);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _best_0x2EevalY_0x2E466 : Operation { // best.evalY.466
    Node** arg1;
    Node** arg2;
    Node** arg3;
    Node** arg4;
    _best_0x2EevalY_0x2E466(Node** _arg1 = 0, Node** _arg2 = 0, Node** _arg3 = 0, Node** _arg4 = 0) : arg1(_arg1), arg2(_arg2), arg3(_arg3), arg4(_arg4) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0, Node** _arg3 = 0, Node** _arg4 = 0) {
      return new Node*(new _best_0x2EevalY_0x2E466(_arg1, _arg2, _arg3, _arg4));
    }
    inline std::string name() { return "best.evalY.466"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      std::string s3 = arg3 == 0 ? UNDEF : (*arg3)->to_s(n+1);
      std::string s4 = arg4 == 0 ? UNDEF : (*arg4)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + "," + s3 + "," + s4 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _best_0x2EevalY_0x2E466(arg1, arg2, arg3, _arg);
      case 2: return new Engine::Partial(_best_0x2EevalY_0x2E466::make(arg1, arg2, _arg), 1);
      case 3: return new Engine::Partial(_best_0x2EevalY_0x2E466::make(arg1, _arg), 2);
      case 4: return new Engine::Partial(_best_0x2EevalY_0x2E466::make(_arg), 3);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _best_0x2EevalY_0x2E466_case__0x231 : Operation { // best.evalY.466_case_#1
    Node** arg1;
    Node** arg2;
    Node** arg3;
    Node** arg4;
    Node** arg5;
    _best_0x2EevalY_0x2E466_case__0x231(Node** _arg1 = 0, Node** _arg2 = 0, Node** _arg3 = 0, Node** _arg4 = 0, Node** _arg5 = 0) : arg1(_arg1), arg2(_arg2), arg3(_arg3), arg4(_arg4), arg5(_arg5) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0, Node** _arg3 = 0, Node** _arg4 = 0, Node** _arg5 = 0) {
      return new Node*(new _best_0x2EevalY_0x2E466_case__0x231(_arg1, _arg2, _arg3, _arg4, _arg5));
    }
    inline std::string name() { return "best.evalY.466_case_#1"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      std::string s3 = arg3 == 0 ? UNDEF : (*arg3)->to_s(n+1);
      std::string s4 = arg4 == 0 ? UNDEF : (*arg4)->to_s(n+1);
      std::string s5 = arg5 == 0 ? UNDEF : (*arg5)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + "," + s3 + "," + s4 + "," + s5 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _best_0x2EevalY_0x2E466_case__0x231(arg1, arg2, arg3, arg4, _arg);
      case 2: return new Engine::Partial(_best_0x2EevalY_0x2E466_case__0x231::make(arg1, arg2, arg3, _arg), 1);
      case 3: return new Engine::Partial(_best_0x2EevalY_0x2E466_case__0x231::make(arg1, arg2, _arg), 2);
      case 4: return new Engine::Partial(_best_0x2EevalY_0x2E466_case__0x231::make(arg1, _arg), 3);
      case 5: return new Engine::Partial(_best_0x2EevalY_0x2E466_case__0x231::make(_arg), 4);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _best_0x2EevalX_0x2E466 : Operation { // best.evalX.466
    Node** arg1;
    Node** arg2;
    Node** arg3;
    Node** arg4;
    Node** arg5;
    _best_0x2EevalX_0x2E466(Node** _arg1 = 0, Node** _arg2 = 0, Node** _arg3 = 0, Node** _arg4 = 0, Node** _arg5 = 0) : arg1(_arg1), arg2(_arg2), arg3(_arg3), arg4(_arg4), arg5(_arg5) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0, Node** _arg3 = 0, Node** _arg4 = 0, Node** _arg5 = 0) {
      return new Node*(new _best_0x2EevalX_0x2E466(_arg1, _arg2, _arg3, _arg4, _arg5));
    }
    inline std::string name() { return "best.evalX.466"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      std::string s3 = arg3 == 0 ? UNDEF : (*arg3)->to_s(n+1);
      std::string s4 = arg4 == 0 ? UNDEF : (*arg4)->to_s(n+1);
      std::string s5 = arg5 == 0 ? UNDEF : (*arg5)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + "," + s3 + "," + s4 + "," + s5 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _best_0x2EevalX_0x2E466(arg1, arg2, arg3, arg4, _arg);
      case 2: return new Engine::Partial(_best_0x2EevalX_0x2E466::make(arg1, arg2, arg3, _arg), 1);
      case 3: return new Engine::Partial(_best_0x2EevalX_0x2E466::make(arg1, arg2, _arg), 2);
      case 4: return new Engine::Partial(_best_0x2EevalX_0x2E466::make(arg1, _arg), 3);
      case 5: return new Engine::Partial(_best_0x2EevalX_0x2E466::make(_arg), 4);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _best_0x2EevalX_0x2E466_case__0x231 : Operation { // best.evalX.466_case_#1
    Node** arg1;
    Node** arg2;
    Node** arg3;
    Node** arg4;
    Node** arg5;
    Node** arg6;
    _best_0x2EevalX_0x2E466_case__0x231(Node** _arg1 = 0, Node** _arg2 = 0, Node** _arg3 = 0, Node** _arg4 = 0, Node** _arg5 = 0, Node** _arg6 = 0) : arg1(_arg1), arg2(_arg2), arg3(_arg3), arg4(_arg4), arg5(_arg5), arg6(_arg6) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0, Node** _arg3 = 0, Node** _arg4 = 0, Node** _arg5 = 0, Node** _arg6 = 0) {
      return new Node*(new _best_0x2EevalX_0x2E466_case__0x231(_arg1, _arg2, _arg3, _arg4, _arg5, _arg6));
    }
    inline std::string name() { return "best.evalX.466_case_#1"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      std::string s3 = arg3 == 0 ? UNDEF : (*arg3)->to_s(n+1);
      std::string s4 = arg4 == 0 ? UNDEF : (*arg4)->to_s(n+1);
      std::string s5 = arg5 == 0 ? UNDEF : (*arg5)->to_s(n+1);
      std::string s6 = arg6 == 0 ? UNDEF : (*arg6)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + "," + s3 + "," + s4 + "," + s5 + "," + s6 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _best_0x2EevalX_0x2E466_case__0x231(arg1, arg2, arg3, arg4, arg5, _arg);
      case 2: return new Engine::Partial(_best_0x2EevalX_0x2E466_case__0x231::make(arg1, arg2, arg3, arg4, _arg), 1);
      case 3: return new Engine::Partial(_best_0x2EevalX_0x2E466_case__0x231::make(arg1, arg2, arg3, _arg), 2);
      case 4: return new Engine::Partial(_best_0x2EevalX_0x2E466_case__0x231::make(arg1, arg2, _arg), 3);
      case 5: return new Engine::Partial(_best_0x2EevalX_0x2E466_case__0x231::make(arg1, _arg), 4);
      case 6: return new Engine::Partial(_best_0x2EevalX_0x2E466_case__0x231::make(_arg), 5);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _findall : Operation { // findall
    Node** arg1;
    _findall(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _findall(_arg1));
    }
    inline std::string name() { return "findall"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _findall(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _findfirst : Operation { // findfirst
    Node** arg1;
    _findfirst(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _findfirst(_arg1));
    }
    inline std::string name() { return "findfirst"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _findfirst(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _browse : Operation { // browse
    Node** arg1;
    _browse(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _browse(_arg1));
    }
    inline std::string name() { return "browse"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _browse(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _browseList : Operation { // browseList
    Node** arg1;
    _browseList(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _browseList(_arg1));
    }
    inline std::string name() { return "browseList"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _browseList(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _unpack : Operation { // unpack
    Node** arg1;
    _unpack(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _unpack(_arg1));
    }
    inline std::string name() { return "unpack"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _unpack(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _PEVAL : Operation { // PEVAL
    Node** arg1;
    _PEVAL(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _PEVAL(_arg1));
    }
    inline std::string name() { return "PEVAL"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _PEVAL(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _normalForm : Operation { // normalForm
    Node** arg1;
    _normalForm(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _normalForm(_arg1));
    }
    inline std::string name() { return "normalForm"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _normalForm(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _groundNormalForm : Operation { // groundNormalForm
    Node** arg1;
    _groundNormalForm(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _groundNormalForm(_arg1));
    }
    inline std::string name() { return "groundNormalForm"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _groundNormalForm(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _groundNormalForm_case__0x231 : Operation { // groundNormalForm_case_#1
    Node** arg1;
    _groundNormalForm_case__0x231(Node** _arg1 = 0) : arg1(_arg1) {}
    static Node** make(Node** _arg1 = 0) {
      return new Node*(new _groundNormalForm_case__0x231(_arg1));
    }
    inline std::string name() { return "groundNormalForm_case_#1"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      return name() + "(" + s1 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _groundNormalForm_case__0x231(_arg);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _apply : Operation { // apply
    Node** arg1;
    Node** arg2;
    _apply(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new _apply(_arg1, _arg2));
    }
    inline std::string name() { return "apply"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _apply(arg1, _arg);
      case 2: return new Engine::Partial(_apply::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _cond : Operation { // cond
    Node** arg1;
    Node** arg2;
    _cond(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new _cond(_arg1, _arg2));
    }
    inline std::string name() { return "cond"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _cond(arg1, _arg);
      case 2: return new Engine::Partial(_cond::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _letrec : Operation { // letrec
    Node** arg1;
    Node** arg2;
    _letrec(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new _letrec(_arg1, _arg2));
    }
    inline std::string name() { return "letrec"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _letrec(arg1, _arg);
      case 2: return new Engine::Partial(_letrec::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct __0x3D_0x3A_0x3C_0x3D : Operation { // =:<=
    Node** arg1;
    Node** arg2;
    __0x3D_0x3A_0x3C_0x3D(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new __0x3D_0x3A_0x3C_0x3D(_arg1, _arg2));
    }
    inline std::string name() { return "=:<="; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new __0x3D_0x3A_0x3C_0x3D(arg1, _arg);
      case 2: return new Engine::Partial(__0x3D_0x3A_0x3C_0x3D::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct __0x3D_0x3A_0x3C_0x3C_0x3D : Operation { // =:<<=
    Node** arg1;
    Node** arg2;
    __0x3D_0x3A_0x3C_0x3C_0x3D(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new __0x3D_0x3A_0x3C_0x3C_0x3D(_arg1, _arg2));
    }
    inline std::string name() { return "=:<<="; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new __0x3D_0x3A_0x3C_0x3C_0x3D(arg1, _arg);
      case 2: return new Engine::Partial(__0x3D_0x3A_0x3C_0x3C_0x3D::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _ifVar : Operation { // ifVar
    Node** arg1;
    Node** arg2;
    Node** arg3;
    _ifVar(Node** _arg1 = 0, Node** _arg2 = 0, Node** _arg3 = 0) : arg1(_arg1), arg2(_arg2), arg3(_arg3) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0, Node** _arg3 = 0) {
      return new Node*(new _ifVar(_arg1, _arg2, _arg3));
    }
    inline std::string name() { return "ifVar"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      std::string s3 = arg3 == 0 ? UNDEF : (*arg3)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + "," + s3 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _ifVar(arg1, arg2, _arg);
      case 2: return new Engine::Partial(_ifVar::make(arg1, _arg), 1);
      case 3: return new Engine::Partial(_ifVar::make(_arg), 2);
      }
    }
    /*inline*/ Node* hfun();
  };

  struct _failure : Operation { // failure
    Node** arg1;
    Node** arg2;
    _failure(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {}
    static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) {
      return new Node*(new _failure(_arg1, _arg2));
    }
    inline std::string name() { return "failure"; }
    inline std::string to_s(int n=0) {
      if (n>=MAXDEPTH) return HIDE;
      std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1);
      std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1);
      return name() + "(" + s1 + "," + s2 + ")";
    }
    inline Node* apply(Node** _arg, int _missing) {
      switch (_missing) {
      case 1: return new _failure(arg1, _arg);
      case 2: return new Engine::Partial(_failure::make(_arg), 1);
      }
    }
    /*inline*/ Node* hfun();
  };

}
