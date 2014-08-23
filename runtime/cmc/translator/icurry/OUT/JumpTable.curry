-- This module reorder the entries of an algebraic jump
-- table in standard order and add any missing entry

module JumpTable(execute) where

import ICurry

execute type_table flat = makeProg type_table flat

makeProg tt (IModule name imported constr_decl funct_decl)
  = IModule name imported constr_decl (map (makeFunct tt) funct_decl)
makeConstr a@(IConstructor _ _, _) = a
makeFunct tt (IFunction qname arity scope_expr)
  = IFunction qname arity (makeScope tt scope_expr)
makeScope tt (Scope_Expr var_list expr)
  = Scope_Expr new_var_list (makeExpr tt expr)
  where new_var_list = map (makeVarDef tt) var_list
makeVarDef _ a@(_, _, IPath _) = a
makeVarDef tt (i, c, IBind expr) = (i, c, IBind (makeExpr tt expr))
makeVarDef _ a@(_, _, IFree) = a

makeExpr _ a@(IExternal _) = a
makeExpr _ a@(Exempt) = a 
makeExpr _ a@(IBuiltin _) = a
makeExpr _ a@(VariableRef _) = a
makeExpr tt (ConstrApply qname expr_list)
  = ConstrApply qname (map (makeExpr tt) expr_list)
makeExpr tt (FunctApply qname expr_list)
  = FunctApply qname (map (makeExpr tt) expr_list)
makeExpr tt (PartApplic missing expr)
  = PartApplic missing (makeExpr tt expr)
makeExpr tt (ILet scope_expr)
  = ILet (makeScope tt scope_expr)
makeExpr tt (FreeVar scope_expr)
  = FreeVar (makeScope tt scope_expr)
makeExpr tt (IOr expr_l expr_r)
  = IOr (makeExpr tt expr_l) (makeExpr tt expr_r)
makeExpr tt (AlgebraicTable suffix flex expr entry_list)
  = AlgebraicTable suffix flex (makeExpr tt expr) new_entry_list
  -- The whole traversal is just for this
  where new_entry_list = extend tt entry_list
makeExpr tt (BuiltinTable suffix flex expr entry_list)
  = BuiltinTable suffix flex (makeExpr tt expr) new_entry_list
  where new_entry_list = [(x, makeExpr tt y) | (x,y) <- entry_list]
makeExpr _ a@(Ignore _) = a

------------------------------------------------------------------

extend tt entry_list = [(x, makeScope tt y) | (x, y) <- complete]
  where -- list of constructors making the labels of the jump table
        constr_list = get tt entry_list
        -- find the list of constructors labeling the table
        get (_ ++ [(_, a@(_ ++ [(cname,_)] ++ _))] ++ _)
	    ((IConstructor cname _, _) : _) = a
        complete = map choose_create constr_list 
        -- if the table is missing a constructor, create an entry for it
        -- otherwise choose the corresponing entry in the table
        choose_create (cname,arity) =
          case mylookup cname arity entry_list of
             Nothing -> (IConstructor cname arity, Scope_Expr [] Exempt)
             Just a  -> a
	mylookup cname1 arity (a@(IConstructor cname2 _, _) : z)
	  | cname1==cname2 = Just a
	  | otherwise = mylookup cname1 arity z
	mylookup _ _ [] = Nothing

