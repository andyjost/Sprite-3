module MacroCore where

import List
import ICurry
import Block
import XFormat
import Utils

-- This module defines function to produce core members of a struct
-- variables, constructor and methods.

------------------------------------------------------------------
-- some utilities, return a string

-- formal arguments of constructor and make
formal_args arity
  = concat (intersperse ", " [format "Node** _arg%d = 0" [FI i] | i <- [1..arity]])
-- actual argument of calls to constructor and make
actual_args arity = concat (intersperse ", " [format "_arg%d" [FI i] | i <- [1..arity]])
-- initialization of variables
init_args arity | arity == 0 = ""
                | otherwise = " : " ++ concat (intersperse ", " 
                                [format "arg%d(_arg%d)" [FI i, FI i] | i <- [1..arity]])
-- arguments of to_s return value
print_args arity | arity == 0 = ""
                 | otherwise = " + \"(\"" ++ concat (intersperse " + \",\"" 
                                 [format " + s%d" [FI i] | i <- [1..arity]]) ++ " + \")\""

------------------------------------------------------------------
-- core members, return a Block

-- core variables
macro_instance_var _ arity
  = Block 0 [SLine (format "Node** arg%d;" [FI i]) | i <- [1..arity]]

-- core constructor
macro_constructor_declar (_,name) arity
  = SLine (format "%s(%s)%s {}" [FS (translate name), FS (formal_args arity), FS (init_args arity)])

-- core method "make"
macro_make (_,name) arity
  = Block 0 [
      SLine (format "static Node** make(%s) {" [FS (formal_args arity)]),
      SLine (format "  return new Node*(new %s(%s));" [FS (translate name), FS (actual_args arity)]),
      SLine "}"
    ]

-- core method "name"
macro_name (_,name) _
  = SLine (format "inline std::string name() { return \"%s\"; }" [FS name])

-- core method "to_s"
macro_to_s _ arity
  = Block 0 [
      SLine "inline std::string to_s(int n=0) {",
      Block 1 [
        SLine "if (n>=MAXDEPTH) return HIDE;",
        Block 0 [SLine (format "std::string s%d = arg%d == 0 ? UNDEF : (*arg%d)->to_s(n+1);"
                  [FI i, FI i, FI i]) | i <- [1..arity]],
        SLine (format "return name()%s;" [FS (print_args arity)])
      ],
      SLine "}"
    ]

-- core method apply
macro_apply (_,name) arity
  = Block 0 [
      SLine "inline Node* apply(Node** _arg, int _missing) {",
      if arity == 0 then
        Block 1 [
           SLine "throw \"can't apply nullary symbol!\";",
           SLine "return 0;"
        ]
      else
        Block 1 [
          SLine "switch (_missing) {",
          SLine (format "case 1: return new %s(%s_arg);" [FS (translate name), FS first_line_args]),
          Block 0 (map second_lines [2..arity]),
          SLine "}"
        ],
      SLine "}"
    ]
  where second_lines i 
          = SLine (format "case %d: return new Engine::Partial(%s::make(%s_arg), %d);"
                  [FI i, FS (translate name), FS (second_lines_args i), FI (i-1)])
        first_line_args = concat [format "arg%d, " [FI i] | i <- [1..arity-1]] 
        second_lines_args k = concat [format "arg%d, " [FI i] | i <- [1..arity-k]]
