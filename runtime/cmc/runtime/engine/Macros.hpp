#define UNDEF "\u2022"
#define HIDE "\u221e"
#define MAXDEPTH 6

#define BOOLEQ __0x3D_0x3D
#define SUCCEQ __0x3D_0x3A_0x3D
#define BOOLAND __0x26_0x26

// ------------------------------------------------------------------

#define SYMBOL0(c_name, flp_name) \
  c_name() {} \
  static Node** make() { return new Node*(new c_name()); } \
  inline std::string name() { return flp_name; } \
  inline std::string to_s(int =0) { return name(); } \
  Node* apply(Node**, int) { throw "Error: apply 0-arity symbol"; }
//end SYMBOL0

#define CONSTR0(c_name, flp_name, kind) \
  struct c_name : Constructor { \
    SYMBOL0(c_name, flp_name) \
    inline int get_kind() { return kind; } \
    inline Node* nfun() { return this; } \
    inline Node* afun() { return this; } \
  };
// end CONSTR0

// ------------------------------------------------------------------

#define SYMBOL1(c_name, flp_name) \
  Node** arg1; \
  c_name(Node** _arg1 = 0) : arg1(_arg1) {} \
  static Node** make(Node** _arg1 = 0) { return new Node*(new c_name(_arg1)); } \
  inline std::string name() { return flp_name; } \
  inline std::string to_s(int n=0) { \
    if (n>=MAXDEPTH) return HIDE; \
    std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1); \
    return name() + "(" + s1 + ")"; \
  } \
  Node* apply(Node** _arg, int _missing) { \
    switch (_missing) { \
    case 1: return new c_name(_arg); \
    } \
  }
//end SYMBOL1

#define CONSTR1(c_name, flp_name, kind) \
  struct c_name : Constructor { \
    SYMBOL1(c_name, flp_name) \
    inline int get_kind() { return kind; } \
    inline Node* nfun() { Engine::nfun(arg1); return this; } \
    inline Node* afun() { \
      if ((*arg1)->get_kind() == FAIL) return DO_FAIL; \
      return this; \
    } \
  };
// end CONSTR1

// ------------------------------------------------------------------

#define SYMBOL2(c_name, flp_name) \
  Node** arg1; \
  Node** arg2; \
  c_name(Node** _arg1 = 0, Node** _arg2 = 0) : arg1(_arg1), arg2(_arg2) {} \
  static Node** make(Node** _arg1 = 0, Node** _arg2 = 0) { return new Node*(new c_name(_arg1, _arg2)); } \
  inline std::string name() { return flp_name; } \
  inline std::string to_s(int n=0) { \
    if (n>=MAXDEPTH) return "\u221e"; \
    std::string s1 = arg1 == 0 ? UNDEF : (*arg1)->to_s(n+1); \
    std::string s2 = arg2 == 0 ? UNDEF : (*arg2)->to_s(n+1); \
    return name() + "(" + s1 + "," + s2 + ")"; \
  } \
  Node* apply(Node** _arg, int _missing) { \
    switch (_missing) { \
    case 1: return new c_name(arg1, _arg); \
    case 2: return new Partial(c_name::make(_arg),1); \
    } \
  }
// end SYMBOL2

#define CONSTR2(c_name, flp_name, kind) \
  struct c_name : Constructor { \
    SYMBOL2(c_name, flp_name) \
    inline int get_kind() { return kind; } \
    inline Node* nfun() { Engine::nfun(arg1); Engine::nfun(arg2); return this; } \
    inline Node* afun() { \
      if ((*arg1)->get_kind() == FAIL) return DO_FAIL; \
      if ((*arg2)->get_kind() == FAIL) return DO_FAIL; \
      return this; \
    } \
  };
// end CONSTR2
