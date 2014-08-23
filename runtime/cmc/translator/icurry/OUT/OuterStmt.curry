module OuterStmt(execute) where

import ICurry
import List

execute icurry =  travIMod icurry

-- Curry expressions are translated into imperative expressions.
-- In an imperative language, an expression can be only the
-- application of a (imperative) function to expressions.
-- In Curry the argument of an expression may be a let-block
-- or a case, which is a Curry expression, but is not an
-- imperative expression.  Thus, the program must be massaged.

-- Function "trav" traverses a program.  When it finds
-- an application in which an argument is not an imperative
-- expression it "swaps" the argument with the function being applied.
-- Loosely speaking:
--
--    f (let ... in x)
--
-- becomes
--
--    let ... in f x
--
-- The transformation must be inside out to take care of nested argument.
-- E.g., look at the following:
--
--  f (let ... in (g (let ... in x)))
--  f (let ... in (let ... in g x))
--  let ... in f (let ... in g x)
--  let ... in (let ... in f (g x))
--

travIMod (IModule name imported constr_decl funct_decl)
  = IModule name imported constr_decl (map travFunct funct_decl)

travFunct (IFunction qname arity scope_expr)
  = IFunction qname arity (travScope scope_expr)

travScope (Scope_Expr var_list expr)
  = Scope_Expr (map travVariableDef var_list) (travExpr expr)

travVariableDef a@(_,_,IPath _) = a
travVariableDef (i,c,IBind expr) = (i,c,IBind (travExpr expr))
travVariableDef a@(_,_,IFree) = a

------------------------------------------------------------------

travExpr a@(IExternal _) = a
travExpr a@(Exempt) = a
travExpr a@(IBuiltin _) = a
travExpr a@(VariableRef _) = a
travExpr (ConstrApply qname expr_list)
  = fix 0 (ConstrApply qname (map travExpr expr_list))
travExpr (FunctApply qname expr_list)
  = fix 0 (FunctApply qname (map travExpr expr_list))
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

-- TODO: avoid replicated code for FunctApply and ConstrApply ?
-- Maybe not.  It looks complicated enough

-- We assume that the arguments have the non-applicative constructs at the root
-- Termination is assured because n gets bigger or the height of the application
-- get smaller at each recursive call.
fix n a@(FunctApply qname arg_list)
  | n == length arg_list = a
  | True =
    case arg_list !! n of
      ILet (Scope_Expr bind_list expr)
        -> let new_arg_list = replace_nth n arg_list expr
           -- the recursive call to trav fixes also the arguments past n
	   in  ILet (Scope_Expr bind_list (travExpr (FunctApply qname new_arg_list)))
      AlgebraicTable suffix flex expr entry_list
        -> AlgebraicTable suffix flex expr (map (make_new_entry n a) entry_list)
      -- TODO: needs a case for BuiltinTable
      BuiltinTable _ _ _ _ -- suffix flex expr entry_list
        -> error "BuiltinTable case in OuterStmt not yet implemented"
      _ -> fix (n+1) a

make_new_entry n (FunctApply qname arg_list) (label, Scope_Expr var_list expr)
  -- the recursive call to travExpr is to push the application down all the way
  -- and also the arguments past n, which have not been checked.
  = (label, Scope_Expr var_list (travExpr (FunctApply qname new_arg_list)))
  where new_arg_list = replace_nth n arg_list expr

------------------------------------------------------------------

fix n a@(ConstrApply qname arg_list)
  | n == length arg_list = a
  | True =
    case arg_list !! n of
      ILet (Scope_Expr bind_list expr)
        -> let new_arg_list = replace_nth n arg_list expr
           -- the recursive call to trav fixes also the arguments past n
	   in  ILet (Scope_Expr bind_list (travExpr (ConstrApply qname new_arg_list)))
      AlgebraicTable suffix flex expr entry_list
        -> AlgebraicTable suffix flex expr (map (make_new_entry n a) entry_list)
      -- TODO: needs a case for BuiltinTable
      _ -> fix (n+1) a

make_new_entry n (ConstrApply qname arg_list) (label, Scope_Expr var_list expr)
  -- the recursive call to travExpr is to push the application down all the way
  -- and also the arguments past n, which have not been checked.
  = (label, Scope_Expr var_list (travExpr (ConstrApply qname new_arg_list)))
  where new_arg_list = replace_nth n arg_list expr

------------------------------------------------------------------

replace_nth n (x : xs) y 
  | n==0 = y : xs
  | True = x : replace_nth (n-1) xs y
