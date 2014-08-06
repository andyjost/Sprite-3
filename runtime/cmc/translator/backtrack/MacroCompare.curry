import ICurry
import Utils
import Block
import Format

-- This module produces the compare method of a constructor
-- This method is the same for all constructors of the same arity,
-- except for casting.  Maybe I should define classes
-- Node0, Node1, etc, for nodes of arity 0, 1, etc. and subclass.

macro_compare constr_list qname@(_,name) arity
  = Block 0 [
      NL,
      SLine (format "Node* %s::compare(Node** right) { // %s " [FS (translate name), FS name]),
      SLine (format "  static void* table[] = {&&fail, &&var, &&choice, &&oper%s};" [FS label_array]),
      SLine "  goto *table[(*right)->get_kind()];",
      SLine "fail:",
      SLine "  return DO_FAIL;",
      SLine "var:",
      SLine "  throw \"Program flounders\";",
      SLine "choice:",
      SLine "oper:",
      SLine "  Engine::hfun(right);",
      SLine "  goto *table[(*right)->get_kind()];",
      Block 0 [macro_entry constr_list qname right_name arity | (IConstructor right_name _) <- constr_list],
      SLine "}"
    ]
  where label_names = [translate n | (IConstructor (_,n) _) <- constr_list]
        label_array = concat [", &&"++n | n <- label_names]

macro_entry constr_list left_name right_name arity
  = Block 0 [
      SLine (format "%s:" [FS (translate (snd (right_name)))]),
      Block 1 [
        if left_name == right_name then
	   macro_entry_same left_name arity
        else
           SLine (format "return new %s();" [FS (loop constr_list)])
      ]
    ]
  where loop (IConstructor name _ : rest)
          | name == left_name  = qualify ("Prelude","LT")
          | name == right_name = qualify ("Prelude","GT")
          | otherwise = loop rest

{-
    descend_compare(compare(arg1,(*right)->arg1),
      descend_compare(compare(arg2,(*right)->arg2),
        ...
          compare(argn,(*right)->argn)))
-}

macro_entry_same qname arity 
  = Block 0 [
      SLine (format "return %s;" [FS (macro_string qname arity)])
    ]

compare_symbol_name = ("Prelude","compare")
descend_symbol_name = ("Prelude","descend_compare")

macro_string qname  arity
  | arity == 0 = format "new %s()" [FS (qualify ("Prelude","EQ"))]
  | arity == 1 = format "new %s(arg1,((%s*) (*right))->arg1)" [sname, FS (qualify qname)]
  | otherwise = loop qname (arity-1) 
      (format "%s::make(arg%d,((%s*) (*right))->arg%d)" [sname, x, FS (qualify qname), x])
  where x = FI arity
        sname = FS (qualify compare_symbol_name)
        

loop qname count string
  | count == 0 = string
  | count == 1 = format "new %s(%s::make(arg1,((%s*) (*right))->arg1),%s)" 
                    [dname, sname, FS (qualify qname), FS string]
  | otherwise = loop qname (count-1) 
      (format "%s::make(%s::make(arg1,((%s*) (*right))->arg1),%s)" 
                    [dname, sname, FS (qualify qname), FS string])
  where sname = FS (qualify compare_symbol_name)
        dname = FS (qualify descend_symbol_name)
