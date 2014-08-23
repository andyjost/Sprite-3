module RemoveInnerCases(execute) where

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

travExpr stuff a@(Var _) = (stuff, a)

travExpr stuff a@(Lit _) = (stuff, a)

travExpr stuff (Comb mode qname expr_list)
  = (new_stuff, Comb mode qname new_expr_list)
  where travArgs stuff' [] = (stuff', [])
        travArgs (_,b,c,d) (x:xs)
           = let (head_stuff, head_expr) = travExpr (True,b,c,d) x
                 (tail_stuff, tail_expr) = travArgs head_stuff xs
             in  (tail_stuff, head_expr : tail_expr)
        (new_stuff, new_expr_list) = travArgs stuff expr_list

travExpr stuff (Let bind_list expr) 
  = (new_stuff, Let new_bind_list new_expr)
  where -- first we do the bindings
        travBindings stuff' [] = (stuff', [])
        travBindings (_,b,c,d) ((v, x) : xs)
           = let (head_stuff, head_expr) = travExpr (True,b,c,d) x
                 (tail_stuff, tail_bind) = travBindings head_stuff xs
             in  (tail_stuff, (v, head_expr) : tail_bind)
        (bind_stuff, new_bind_list) = travBindings stuff bind_list
        -- then we do the expression
        (new_stuff, new_expr) = travExpr bind_stuff expr

travExpr stuff (Free var_list expr) 
  = (new_stuff, Free var_list new_expr) 
  where (new_stuff, new_expr) = travExpr stuff expr

travExpr (_,b,c,d) (Or expr_1 expr_2)
  = (new_stuff, Or new_expr_1 new_expr_2)
  where (stuff_1, new_expr_1) = travExpr (True,b,c,d) expr_1
        (new_stuff, new_expr_2) = travExpr stuff_1 expr_2

travExpr stuff@(a,_,_,_) (Case flex expr branch_list)
  = result
  where -- first we do the branches
        travBranches stuff' [] = (stuff', [])
        travBranches stuff' (Branch pat x : xs)
           = let (head_stuff, head_expr) = travExpr stuff' x
                 (tail_stuff, tail_branch) = travBranches head_stuff xs
             in  (tail_stuff, Branch pat head_expr : tail_branch)
        (branch_stuff, new_branch_list) = travBranches stuff branch_list
        -- then we do the expression selector
        (select_stuff, new_expr) = travExpr branch_stuff expr
        -- then either copy or create a new function
        result = if a then create_and_replace y else y
               where y = (select_stuff, Case flex new_expr new_branch_list)

travExpr stuff (Typed expr xtype)
  = (new_stuff, Typed new_expr xtype)
  where (new_stuff, new_expr) = travExpr stuff expr

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
