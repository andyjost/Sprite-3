module MacroSymbols where

import List
import ICurry
import Block
import XFormat
import Utils
import MacroCore
import MacroEqual
import MacroCompare

-- common portions of data constructor and defined operation symbol
macro_symbol qname arity
  = Block 0 [
      macro_instance_var       qname arity,
      macro_constructor_declar qname arity,
      macro_make               qname arity,
      macro_name               qname arity,
      macro_to_s               qname arity,
      macro_apply              qname arity,
      NOP
    ]

macro_constructor (index, IConstructor qname@(_,name) arity)
  = Block 0 [
      SLine (format "struct %s : Constructor { // %s" [FS (translate name), FS name]),
      Block 1 [
        macro_symbol qname arity,
        SLine (format "inline int get_kind() { return (CTOR+%d); }" [FI index]),
        SLine "inline Node* nfun() {",
          Block 1 [SLine (format "Engine::nfun(arg%d);" [FI i]) | i <- [1..arity]],
          SLine "  return this;",
        SLine "}",
        SLine "inline Node* afun() {",
        Block 1 [SLine (format "if ((*arg%d)->get_kind() == FAIL) return DO_FAIL;" [FI i]) | i <- [1..arity]],
        SLine "  return this;",
        SLine "}",
        SLine "/*inline*/ Node* boolequal(Node**);",
        SLine "/*inline*/ Node* compare(Node**);"
      ],
      SLine "};",
      NL
    ]

-- For now both "internal"
-- No difference between locally defined and external 
        
macro_operation qname@(_,name) arity
  = Block 0 [
      SLine (format "struct %s : Operation { // %s" [FS (translate name), FS name]),
      Block 1 [
        macro_symbol qname arity,
        -- bodies for following are in cpp
        SLine "/*inline*/ Node* hfun();"
      ],
      SLine "};",
      NL
    ]
