module FlattenStmt(execute) where

import ICurry
import List
import Unsafe

execute icurry =  travIMod icurry

-- This is intended as the last transformation of ICurry.
-- It deals with bindings of variables that are not functional expressions,
-- e.g., a let block or a jump table.
-- 
-- This modulo performs transformations such as
-- 
--       let x = let bs in y in z
-- 
-- as
-- 
--       let x = y; bs in z
--
-- or
--
--       let x = y in algtable 
--                      c1 -> e1
--                      ...
--                      cn -> en
--
--as
--
--       algtable
--         c1 -> let x = y in e1
--         ...
--         cn -> let x = y in em
--
-- and similarly when x is bound to a built-in table

travIMod (IModule name imported constr_decl funct_decl)
  = IModule name imported constr_decl (map travFunct funct_decl)

travFunct (IFunction qname arity scope_expr)
  = IFunction qname arity (travScope scope_expr)

-- Traverse expr and bindings for each variable of the binding
-- AND traverse binding and expression for variable defined there
travScope (Scope_Expr var_list expr)
  = Scope_Expr new_var_list new_expr
  where -- traverse below to fix inner scopes, if any
        new_expr = travExpr expr
        -- variables of var_list are untouched, only bindings may change
        new_bind_list = map travVariableDef var_list
        new_var_list = concatMap flatten_let new_bind_list

-- The argument is one single binding of a let-block
-- The output is a list of binding, possibly containing only the argument
flatten_let arg 
  = case arg of
      (j,0,IBind (ILet (Scope_Expr var_list expr))) -> concatMap flatten_let ((j,0,IBind expr) : var_list)
      (_,_,IBind (AlgebraicTable _ _ _ _)) -> error "AlgebraicTable case in FlattenStmt not yet implemented"
      (_,_,IBind (BuiltinTable _ _ _ _)) -> error "BuiltinTable case in FlattenStmt not yet implemented"
      _ -> [arg]

--      (j,c,IBind (AlgebraicTable suffix flex expr branch_list) ->
         
-- We know that so far the count is zero, and enforce it!
-- This is needed only if there can be a scope in
-- the binding of a variable (I don't know if FlatCurry allows it).
travVariableDef a@(_,0,IPath _) = a
travVariableDef (i,0,IBind expr)= (i,0,IBind (travExpr expr))
travVariableDef a@(_,0,IFree) = a

------------------------------------------------------------------

travExpr a@(IExternal _) = a
travExpr a@(Exempt) = a
travExpr a@(IBuiltin _) = a
travExpr a@(VariableRef _) = a
travExpr (ConstrApply qname expr_list)
  = ConstrApply qname (map travExpr expr_list)
travExpr (FunctApply qname expr_list)
  = FunctApply qname (map travExpr expr_list)
travExpr (PartApplic missing expr)
  = PartApplic missing (travExpr expr)
travExpr (ILet scope_expr) = ILet (travScope scope_expr)
travExpr (FreeVar scope_expr) = FreeVar (travScope scope_expr)
travExpr (IOr expr_l expr_r)
  = IOr (travExpr expr_l) (travExpr expr_r)
travExpr (AlgebraicTable suffix flex expr entry_list)
  = AlgebraicTable suffix flex (travExpr expr) [(x, travScope y) | (x,y) <- entry_list]
travExpr (BuiltinTable suffix flex expr entry_list)
  = BuiltinTable suffix flex (travExpr expr) [(x, travExpr y) | (x,y) <- entry_list]
travExpr a@(Ignore _) = a

------------------------------------------------------------------
