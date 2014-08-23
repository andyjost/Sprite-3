import FlatCurry
import SetFunctions
import Unsafe

-- Transform a FlatCurry program into a semantically equivalent
-- program with the following property.
--
-- In FlatCurry a symbol can be applied to constructs such as let
-- blocks and jump tables.  The output of this tranformation is a
-- program where a symbol can only be applied to an expression made of
-- symbols.
--
-- E.g: x + let ... in y  becomes let ... in x + y

------------------------------------------------------------------

-- for unit testing
-- test = (Let [(2,Comb FuncCall ("Prelude","+") [Var 1,Let [(3,Var 1)] (Case  Rigid (Comb FuncCall ("Prelude","==") [Var 3,Lit (Intc  1)]) [Branch (Pattern ("Prelude","True") []) (Lit (Intc  7)),Branch (Pattern ("Prelude","False") []) (Lit (Intc  9))])]),(4,Comb FuncCall ("Prelude","+") [Var 2,Var 1])] (Comb FuncCall ("Prelude","+") [Var 4,Lit (Intc  1)]))

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
subexpr (Comb xtype qname (x ++ [expr] ++ _)) (length x : ps)
  = subexpr expr ps
subexpr (Let bindings expr) (-1 : ps) = subexpr expr ps
subexpr (Let (x ++ [(var, binding)] ++ _) expr) (length x : ps)
  = subexpr binding ps
subexpr (Free var_list expr) (-1 : ps) = subexpr expr ps
subexpr (Or exprl exprr) (0 : ps) = subexpr exprl ps
subexpr (Or exprl exprr) (1 : ps) = subexpr exprr ps
subexpr (Case xtype expr branch_list) (-1 : ps) = subexpr expr ps
subexpr (Case xtype expr1 (x ++ [Branch pattern expr] ++ _)) (length x : ps)
  = subexpr expr ps
subexpr (Typed expr xtype) (-1 : ps) = Typed (subexpr expr ps) xtype

-- normalization steps
-- all this work only for these few cases
reduce (Comb xtype1 qname1 (x1 ++ [Case xtype expr1 branch_list] ++ y1))
  = Case xtype expr1 [Branch pattern (Comb xtype1 qname1 (x1 ++ [expr] ++ y1)) 
        | (Branch pattern expr) <- branch_list]
reduce (Comb xtype1 qname1 (x1 ++ [Let bindings expr] ++ y1))
  = Let bindings (Comb xtype1 qname1 (x1 ++ [expr] ++ y1))
