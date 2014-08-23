module MacroStatement where

import List

import ICurry
import Block
import Format
import Utils


-- This module generates the code of the hfun methods.
-- There are two main concepts: statements and expressions.

-- The body of an hfun method is a statement that computes and returns
-- an expression.  The return type is Node* whereas the symbols of
-- an expression are Node**.

-- We map each ICurry construct to a statement and some
-- ICurry constructs to expressions.
makeStmt _ (IExternal name)
  = error ("The body of hfun of \""++name++"\" cannot be external")
makeStmt _ (Comment string)
  = SLine ("// " ++  string)
makeStmt table (DeclareLHSVar path@((i, c, _): _))
  | c == 1 = SLine (format "// var %d is inlined as %s" [FI i, FS (show path)])
  | True   = SLine (format "Node** %s = %s; // %s " [FS (pvar_id i), FS (makeVar table path), FS (show path)])
makeStmt _ (DeclareFreeVar (i, c, IFree))
  | c==1 = SLine (format "// free variable %s is inlined" [FS (pvar_id i)])
  | True = SLine (format "Node** %s = ::Engine::Variable::make();" [FS (pvar_id i)])
makeStmt vtable (Forward v)
  = SLine (format "Node** %s;" [FS (refVar vtable [v])])
-- TODO: this gets messy. All the variables should have a different
-- number and the same name.
makeStmt vtable@(_ ++ [(i, _, _)] ++ _) (Assign i expr)
  = SLine (format "%s = %s;" [FS (pvar_id i), FS (makeArg vtable expr)])
makeStmt vtable (Initialize (i, _, _) expr)
  = SLine (format "Node** %s = %s;" [FS (pvar_id i), FS (makeArg vtable expr)])
makeStmt vtable (Fill i path j)
  = SLine (format "%s = %s;" [FS (makePath (pvar_id i) path), FS (pvar_id j)])
makeStmt vtable (Return expr)
  = SLine (format "return %s;" [FS (makeExpr vtable expr)])

------------------------------------------------------------------

makeStmt vtable (ATable suffix flex expr branch_list)  --[(IConstructor,[Statement])]
  = Block 0 [
      -- SLine (format "// TABLE %s" [FS (show vtable)]),
      SLine (format "static void* table%s[]" [FS string_suffix]),
      SLine (format "  = {%s};" [FS label_array]),
      if introduce_var 
         then SLine (format "Node** %s = %s;" [FS selector, FS (makeArg vtable expr)])
         else NOP,
      -- SLine (format "start%s:" [FS string_suffix]),
      SLine (format "  goto *table%s[(*%s)->get_kind()];" [FS string_suffix, FS selector]),
      SLine (format "fail%s:" [FS string_suffix]),
      SLine (format "  return DO_FAIL;" []),
      SLine (format "var%s:" [FS string_suffix]),
      -- if selector is not a variable, next statement is never executed
      -- TODO: next depends on flex`
      SLine (format "  // Engine::narrow(%s, generator());" [FS selector]),
      SLine (format "  throw \"No narrowing yet\";" []),
      -- SLine (format "  goto start%s;" [FS string_suffix]),
      SLine (format "  goto *table%s[(*%s)->get_kind()];" [FS string_suffix, FS selector]),
      SLine (format "choice%s:" [FS string_suffix]),
      SLine (format "oper%s:" [FS string_suffix]),
      SLine (format "  Engine::hfun(%s);"  [FS selector]),
      -- SLine (format "  goto start%s;" [FS string_suffix]),
      SLine (format "  goto *table%s[(*%s)->get_kind()];" [FS string_suffix, FS selector]),
      Block 0 (map (makeAEntry vtable string_suffix) branch_list),
      NOP
    ]

  where label_names = ["fail","var","choice","oper"]
           ++ (map ( \ (IConstructor (_,n) _,_) -> translate n) branch_list)
        label_array = concat (intersperse ", " (map ( \x -> "&&"++x++string_suffix) label_names))
        string_suffix = "_" ++ show suffix
        introduce_var = case expr of
                          (Reference _) -> False
                          _             -> True
        selector = if introduce_var then pvar_id suffix else makeArg vtable expr

makeAEntry vtable string_suffix (IConstructor (_,n) _, expr)
  = Block 0 [
      SLine (format "%s%s: // \"%s\"" [FS (translate n), FS string_suffix, FS n]),
      -- SLine (format "// Extending %s %s " [FS (show table), FS (show var_list)]),
      -- SLine (format "// result %s " [FS (show new_table)]),
      Block 1 (map (makeStmt vtable) expr)
    ]

------------------------------------------------------------------

makeStmt vtable (BTable suffix flex expr branch_list) -- [(BuiltinVariant,[Statement])]  
  = Block 0 [
      if introduce_var
         then SLine (format "Node** %s = %s;" [FS selector, FS (makeArg vtable expr)])
         else NOP,
      SLine (format "Engine::nfun(%s);" [FS selector]),
      -- use the brach list to find the type
      SLine (format "switch (%s) {" [FS (convert_to_prim (fst (head branch_list)))]),
      Block 1 (map (makeBEntry vtable) branch_list),
      SLine "}"
    ]
  where --
        introduce_var = case expr of
                          (BuiltinVariant _) -> False
                          (Reference _)      -> False
                          _                  -> True
        selector = if introduce_var then pvar_id suffix else makeArg vtable expr
        convert_to_prim (Bint _) = format "((Litint::Litint*) (*%s))->arg1"
                                          [FS selector]
        convert_to_prim (Bchar _) = format "((Litchar::Litchar*) (*%s))->arg1"
                                          [FS selector]
        convert_to_prim (Bfloat _) = error "BuiltinTable indexed by float"
        
makeBEntry table (label, stmt_list) 
  = Block 0 [
      SLine (format "case %s: {" [FS (convert_to_prim label)]),
      Block 1 (map (makeStmt table) stmt_list),
      SLine "  break; }"
    ]
  where convert_to_prim (Bint i) = show i
  	convert_to_prim (Bchar c) = show c
	convert_to_prim (Bfloat _) = error "BuiltinTable indexed by float"

------------------------------------------------------------------

choice_qname = ("Prelude","?")

makeExpr _ Exempt
  = "DO_FAIL"
makeExpr table (Reference path) 
  = format "*(%s)" [FS (refVar table path)]
makeExpr _ (BuiltinVariant (Bint i))
  = format "new Litint::Litint(%d)" [FI i]
-- TODO: Float is not yet implemented
makeExpr _ (BuiltinVariant (Bfloat f))
  = error ("no literal floats yet \"" ++ show f ++ "\"")
makeExpr _ (BuiltinVariant (Bchar c))
  = format "new Litchar::Litchar('%c')" [FC c]
makeExpr table (Applic _ qname expr_list)
  = format "new %s(%s)" [FS (qualify qname), FS (makeArgList table expr_list)]
makeExpr table (PartApplic missing expr)
  = format "new Engine::Partial(%s, %d)" [FS (makeArg table  expr), FI missing]
makeExpr table (IOr expr_1 expr_2)
  = format "new %s(%s)" [FS (qualify choice_qname), FS (makeArgList table [expr_1, expr_2])]
makeArgList table expr_list
  = concat (intersperse ", " (map (makeArg table) expr_list))

------------------------------------------------------------------

-- This is a bit messy probably because of poor initial design of IPath
-- The path is reversed,which is good: the first item is outermost.

makeVar (_ ++ [(base, _, IPath (Rel _ _ _))] ++ _) [(_, _, IPath (Rel base qname k))] 
  = format "((%s*) *(%s))->arg%d" [FS (qualify qname), FS (pvar_id base), FI k]
makeVar (_ ++ [(base, _, IPath (Arg j))] ++ _) [(_, _, IPath (Rel base qname k))] 
  = format "((%s*) *(arg%d))->arg%d" [FS (qualify qname), FI j, FI k]
-- whichever variable is the k-th argument
makeVar _ [(_, _, IPath (Arg k))] = "arg" ++ show k
-- whichever variable is the k-th argument of variable base
-- the constrain on "base" is only to catch inconsistencies
-- since this is not the last, it can't be an argument
makeVar table ((_, _, IPath (Rel base qname k)) : a@((base, _, _) : _))
  = format "((%s*) *(%s))->arg%d" [FS (qualify qname), FS (makeVar table a), FI k]

makeVar _ [(i, c, IFree)]
  | c==1 = "::Engine::Variable::make()"
  | True = pvar_id i
-- TODO: should bound variables be inlined?  Probably yes!
makeVar _ [(i, _, IBind)]
  = pvar_id i

------------------------------------------------------------------

refVar (_ ++ [(i, _, IPath (Arg j))] ++ _) ((i, _, _) : _)
  = format "arg%d" [FI j]
refVar table@(_ ++ [(i, _, IPath (Rel _ _ _))] ++ _) p@((i, c, _) : _)
  | c==1 = makeVar table p
  | True = format "%s" [FS (pvar_id i)]
refVar _ [(i, c, IFree)]
  | c==1 = "::Engine::Variable::make()"
  | True = pvar_id i
-- TODO: should bound variables be inlined?  Probably yes!
refVar _ [(i, _, IBind)]
  = pvar_id i

------------------------------------------------------------------

makeArg _ Exempt
  = "DO_FAIL"
makeArg table (Reference path) 
  = refVar table path
makeArg _ (BuiltinVariant (Bint i))
  = format "Litint::Litint::make(%d)" [FI i]
-- TODO: Float is not yet implemented
makeArg _ (BuiltinVariant (Bfloat f))
  = error ("no literal floats yet \"" ++ show f ++ "\"")
makeArg _ (BuiltinVariant (Bchar c))
  = format "Litchar::Litchar::make('%c')" [FC c]
makeArg table (Applic _ qname expr_list)
  = format "%s::make(%s)" [FS (qualify qname), FS (makeArgList table expr_list)]
makeArg table (PartApplic missing expr)
  = format "Engine::Partial::make(%s, %d)" [FS (makeArg table  expr), FI missing]
makeArg table (IOr expr_1 expr_2)
  = format "%s::make(%s)" [FS (qualify choice_qname), FS (makeArgList table [expr_1, expr_2])]

makePath seed [] = seed
makePath seed ((symb,arg) : ps) 
  = makePath new_seed ps 
  where new_seed = format "((%s*) *(%s))->arg%d" [FS (qualify symb), FS seed, FI arg]
