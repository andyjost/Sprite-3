import ICurry
import Utils
import Block
import XFormat

-- This module produces the equality method of a constructor
-- This method is the same for all constructors of the same arity,
-- except for casting.  Maybe I should define classes
-- Node0, Node1, etc, for nodes of arity 0, 1, etc. and subclass.

macro_equal constr_list qname@(_,name) arity
  = Block 0 [
      NL,
      -- SLine (format "Node* %s::boolequal(Node** right) {" [FS (qualify qname)]),
      SLine (format "Node* %s::boolequal(Node** right) { // %s " [FS (translate name), FS name]),
      SLine (format "  static void* table[] = {&&fail, &&var, &&choice, &&oper%s};" [FS label_array]),
      SLine "start:",
      SLine "  goto *table[(*right)->get_kind()];",
      SLine "fail:",
      SLine "  return DO_FAIL;",
      SLine "var:",
      SLine "  throw \"Program flounders\";",
      SLine "choice:",
      SLine "oper:",
      SLine "  Engine::hfun(right);",
      SLine "  goto start;",
      Block 0 [macro_entry entry_name qname arity | (IConstructor entry_name _) <- constr_list],
      SLine "}"
    ]
  where label_names = [translate n | (IConstructor (_,n) _) <- constr_list]
        label_array = concat [", &&"++n | n <- label_names]

macro_entry (_,entry_name) qname@(_,name) arity
  = Block 0 [
      SLine (format "%s:" [FS (translate entry_name)]),
      Block 1 [
        if entry_name == name then
	   macro_entry_same qname arity
        else
           SLine (format "return new %s();" [FS (qualify ("Prelude","False"))])
      ]
    ]

macro_entry_same qname arity
  = SLine (format "return %s;" [FS first])
  where first 
          | arity == 0 = (format "new %s()" [FS (qualify ("Prelude","True"))])
          | otherwise = format "new %s(%s::make(arg1,((%s*) (*right))->arg1)%s)" 
                               [FS (qualify ("Prelude","&&")), FS (qualify ("Prelude","==")),
                                FS (qualify qname), FS (recur 2)]
        recur j         -- TODO: avoid extra step in innermost case
          | j > arity = format ", %s::make()" [FS (qualify ("Prelude","True"))]
          | otherwise = format ", %s::make(%s::make(arg%d,((%s*) (*right))->arg%d)%s)" 
	                     [FS (qualify ("Prelude","&&")), FS (qualify ("Prelude","==")),
                              FI j, FS (qualify qname), FI j, FS (recur (j+1))]
