import FlatCurry
import SetFunctions
import Unsafe

-- Transform a FlatCurry program into a semantically equivalent
-- program with the following changes:
--
-- 1. remove free blocks (free var are in the table)
-- 2. remove typed expressions (remove the type)
-- 3. replace Or choices with "?" applications (NOT YET!)

------------------------------------------------------------------

execute  (Prog name imported_list data_list funct_list op_list)
 = Prog name imported_list data_list new_funct_list op_list
 where new_funct_list = map reduceFunct funct_list

reduceFunct (Func qname arity visibility xtype rule)
 = Func qname arity visibility xtype (reduceRule rule)

reduceRule a@(External _) = a
reduceRule (Rule var_list expr) = Rule var_list (cuntil (set1 step) expr)

step expr | subexpr expr p =:= y 
          = replace expr p (reduce y)
          where p, y free

cuntil f x 
  = if isEmpty result then x
    else cuntil f (fst (select result))
  where result = f x


-- replace of a subexpression in an expression given a path
-- replace t p x = y iff t and y are equal except possibly at p, y at p is x
replace _ [] w = w
replace (Comb xtype qname (x ++ [expr] ++ y)) (length x : ps) w
  = Comb xtype qname (x ++ [replace expr ps w] ++ y)
replace (Let bindings expr) (-1 : ps) w = Let bindings (replace expr ps w)
replace (Let (x ++ [(var, binding)] ++ y) expr) (length x : ps) w
  = Let (x ++ [(var, replace binding ps w)] ++ y) expr
replace (Free var_list expr) (-1 : ps) w = Free var_list (replace expr ps w)
replace (Or exprl exprr) (0 : ps) w = Or (replace exprl ps w) exprr
replace (Or exprl exprr) (1 : ps) w = Or exprl (replace exprr ps w)
replace (Case xtype expr branch_list) (-1 : ps) w = Case xtype (replace expr ps w) branch_list
replace (Case xtype expr1 (x ++ [Branch pattern expr] ++ y)) (length x : ps) w
  = Case xtype expr1 (x ++ [Branch pattern (replace expr ps w)] ++ y)
replace (Typed expr xtype) (-1 : ps) w = Typed (replace expr ps w) xtype

-- find a subexpression at some position of an expression
-- subexpr t p = u iff u is the subexpression of t at p
subexpr x [] = x
subexpr (Comb _ _ (x ++ [expr] ++ _)) (length x : ps) = subexpr expr ps
subexpr (Let _ expr) (-1 : ps) = subexpr expr ps
subexpr (Let (x ++ [(_, binding)] ++ _) _) (length x : ps) = subexpr binding ps
subexpr (Free _ expr) (-1 : ps) = subexpr expr ps
subexpr (Or exprl _) (0 : ps) = subexpr exprl ps
subexpr (Or _ exprr) (1 : ps) = subexpr exprr ps
subexpr (Case _ expr _) (-1 : ps) = subexpr expr ps
subexpr (Case _ _ (x ++ [Branch _ expr] ++ _)) (length x : ps) = subexpr expr ps
subexpr (Typed expr _) (-1 : ps) = subexpr expr ps

-- normalization steps

-- Remove types from typed expressions
reduce (Typed expr _) = expr

-- Variables, including free variables are in the variable table
-- and have no scope, hence remove them.
reduce (Free _ expr) = expr

