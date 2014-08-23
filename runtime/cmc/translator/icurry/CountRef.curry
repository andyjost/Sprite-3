module CountRef(execute) where

import List
import FlatCurry
import ICurry
-- import Unsafe

-- Function "countExpr" traverses a FlatCurry program and counts the
-- number of times a variable j is referenced.  In multibranch statements
-- the count is the maximum across the branches.
--
-- Then, it traverses the table and adds how many variables with a
-- non-zero count are defined relative to j.



-- It is executed for each variable declared in a scope.
-- It counts the number of references to that variable
-- and updates the variable table


execute table_list (Prog _ _ _ funct_list _)
  = new_table_list
  where new_table_list = map countFunct (zip table_list funct_list)

countFunct (table, Func _ _ _ _ rule) = countRule table rule

countRule table (External _) = table
countRule (qname, table) (Rule _ expr) 
  = (qname, pass_2)
  where pass_1 = [(j, countExpr expr j, x) | (j, 0, x) <- table]
        pass_2 = [(j, c1+c2, x) | (j, c1, x) <- pass_1, 
                      let c2 = length [_ | (_, d, IPath (Rel k _ _)) <- pass_1, k==j, d>0]]

------------------------------------------------------------------

-- utility
delta i j = if i==j then 1 else 0

countExpr (Var i) j = delta i j
countExpr (Lit _) _ = 0
countExpr (Comb _ _ expr_list) j = countExprList expr_list j
countExpr (Let bind_list expr) j = countExpr expr j + countExprList (map snd bind_list) j
countExpr (Free _ expr) j = countExpr expr j
countExpr (Or expr_l expr_r) j = countExpr expr_l j + countExpr expr_r j
countExpr (Case _ expr branch_list) j
  = maxlist [countExpr branch_expr j | (Branch _ branch_expr) <- branch_list]
    + case expr of
         (Var i) | i==j -> 3  -- here and in various branches (oper, choice, var)
         _              -> countExpr expr j
countExpr (Typed expr _) j = countExpr expr j

countExprList [] _ = 0
countExprList (x:xs) j = countExpr x j + countExprList xs j

maxlist [] = 0
maxlist (x:xs) = max x (maxlist xs)
