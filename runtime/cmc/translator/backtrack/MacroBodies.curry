module MacroBodies where

import List
import ICurry
import Block
import Utils
import XFormat
import MacroStatement

macro_body qname _ vtable stmt_list
  = case stmt_list of
      [IExternal string] -> macro_body_external qname string
      _                  -> macro_body_internal qname vtable stmt_list

macro_body_external (_,name) string
  = Block 0 [
      NL,
      SLine (format "// external Node* %s::hfun() { throw \"External \\\"%s\\\" not implemented\"; }" 
      [FS (translate name), FS string])
    ]

macro_body_internal (_,name) vtable stmt_list
  = Block 0 [
      NL,
      SLine (format "Node* %s::hfun() { // %s" [FS (translate name), FS name]),
      Block 1 (map showVar vtable),
      Block 1 (map (makeStmt vtable) stmt_list),
      SLine "}"
    ]

showVar vtable = SLine (format "// VAR %s" [FS (show vtable)])
