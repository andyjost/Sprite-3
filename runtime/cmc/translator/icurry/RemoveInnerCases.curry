module RemoveInnerCases(execute) where

import Unsafe

import List
import FlatCurry

-- Transform a case argument of a symbol application into a call to a
-- function, and define the corresponding function.

execute  (Prog name imported_list data_list funct_list op_list)
 = Prog name imported_list data_list new_funct_list op_list
 where new_funct_list = concatMap travFunct funct_list

travFunct (Func qname arity visibility xtype rule)
 = Func qname arity visibility xtype new_rule : new_functs
 where (new_functs, new_rule) = travRule qname rule

travRule _ a@(External _) = ([],a)
travRule qname (Rule var_list expr) 
  = (new_functs, Rule var_list new_expr)
  where stuff = (False,qname,0,[])
        ((_,_,_,new_functs), new_expr) = travExpr stuff expr

------------------------------------------------------------------

-- Meaning of items of stuff 
-- 1. True iff traversing the arguments of an application (Comb)
-- 2. name of function being processed
-- 3. counter for the functions to be generated
-- 4. function generated so far
-- A function is generated to replace a case statement that
-- appears as an argument of a symbol application
--
-- component 1 is passed only down
-- component 2 is constant
-- components 3 and 4 are returned and used by the caller.
--
-- TODO: should use a monad for threading 'stuff'

travExpr stuff a@(Var _) = (stuff, a)
travExpr stuff a@(Lit _) = (stuff, a)

travExpr stuff (Comb mode qname expr_list)
  = (st, Comb mode qname new_expr_list)
  where (st, new_expr_list) = travExprList stuff expr_list
        travExprList stuff' [] = (stuff', [])
        travExprList (a,b,c,d) (head:tail)
          = let ((_,_,ch,dh), head_expr) = travExpr (True,b,c,d) head
                ((_,_,ct,dt), tail_expr) = travExprList (True,b,ch,dh) tail
            in  ((a,b,ct,dt), head_expr:tail_expr)

travExpr stuff@(a,b,_,_) (Let bind_list expr) 
  = ((a,b,ce,de), Let new_bind_list new_expr)
  where -- first we do the bindings
        travBindings stuff' [] = (stuff', [])
        travBindings (_,_,c,d) ((v, x) : xs)
           = let ((_,_,ch,dh), head_expr) = travExpr (True,b,c,d) x
                 ((_,_,ct,dt), tail_bind) = travBindings (True,b,ch,dh) xs
             in  ((a,b,ct,dt), (v, head_expr) : tail_bind)
        ((_,_,cb,dd), new_bind_list) = travBindings stuff bind_list
        -- then we do the expression
        ((_,_,ce,de), new_expr) = travExpr (True,b,cb,dd) expr

travExpr stuff@(a,b,_,_) (Free var_list expr) 
  = ((a,b,c,d), Free var_list new_expr) 
  where ((_,_,c,d), new_expr) = travExpr stuff expr

travExpr (a,b,c0,d0) (Or expr_1 expr_2)
  = ((a,b,c2,d2), Or new_expr_1 new_expr_2)
  where ((_,_,c1,d1), new_expr_1) = travExpr (True,b,c0,d0) expr_1
        ((_,_,c2,d2), new_expr_2) = travExpr (True,b,c1,d1) expr_2

travExpr stuff@(a,b,_,_) (Case flex expr branch_list)
  = result
  where -- first we do the branches
        -- update last two slots of stuff, preserve first two
        travBranches stuff' [] = (stuff', [])
        travBranches stuff' (Branch pat x : xs)
           = let ((_,_,ch,dh), head_expr) = travExpr stuff' x
                 ((_,_,ct,dt), tail_branch) = travBranches (a,b,ch,dh) xs
             in  ((a,b,ct,dt), Branch pat head_expr : tail_branch)
        ((_,_,cb,db), new_branch_list) = travBranches stuff branch_list
        -- then we do the expression selector
        ((_,_,ce,de), new_expr) = travExpr (a,b,cb,db) expr
        -- then either copy or create a new function
        result = if a then create_and_replace y else y
               where y = ((a,b,ce,de), Case flex new_expr new_branch_list)

travExpr stuff@(a,b,_,_) (Typed expr xtype)
  = ((a,b,c,d), Typed new_expr xtype)
  where ((_,_,c,d), new_expr) = travExpr stuff expr

------------------------------------------------------------------

create_and_replace ((a,(q,n),b,c), xcase)
  = ((a,(q,n),b+1,new_funct : c), new_expr)
  where var_list = get_var_list xcase
        qname = (q,n++"_case_#"++show (b+1))
	arity = length var_list
        visibility = Private
        xtype = TVar 0 -- bogus, irrelevant
        rule = Rule [i | i <- var_list] xcase
        new_funct = (Func qname arity visibility xtype rule)
        new_expr = Comb FuncCall qname [Var i | i <- var_list]

get_var_list xcase = nub (toPlus xcase) \\ toMinus xcase

toPlus (Var i) = [i]
toPlus (Lit _) = []
toPlus (Comb _ _ expr_list) = concatMap toPlus expr_list
toPlus (Let bind_list expr)   = toPlus expr ++ concat [toPlus x | (_, x) <- bind_list]
toPlus (Free _ expr) = toPlus expr
toPlus (Or expr_1 expr_2) = toPlus expr_1 ++ toPlus expr_2
toPlus (Case _ expr branch_list) = toPlus expr ++ concat [toPlus x | (Branch _ x) <- branch_list]
toPlus (Typed expr _) = toPlus expr

toMinus (Var _) = []
toMinus (Lit _) = []
toMinus (Comb _ _ expr_list) = concatMap toMinus expr_list
toMinus (Let bind_list expr) = toMinus expr ++ [i | (i, _) <- bind_list]
toMinus (Free var_list expr) = toMinus expr ++ [i | i <- var_list]
toMinus (Or expr_1 expr_2) = toMinus expr_1 ++ toMinus expr_2
toMinus (Case _ expr branch_list@(Branch (Pattern _ _) _ : _)) 
  = toMinus expr ++ concat [toMinus x | (Branch _ x) <- branch_list]
      ++ [i | Branch (Pattern _ var_list) _ <- branch_list, i <- var_list]
toMinus (Case _ expr branch_list@(Branch (LPattern _) _ : _))
  = toMinus expr ++ concat [toMinus x | (Branch _ x) <- branch_list]
toMinus (Typed expr _) = toMinus expr
