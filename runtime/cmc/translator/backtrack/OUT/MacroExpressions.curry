module MacroExpressions where

import List
import Sort
import SetFunctions

import ICurry
import Block
import Format
import Utils

-- This module generates C++ expressions corresponding to
-- ICurry expressions.

applicExpr table (VariableRef i) 
  = get_var table i
applicExpr _ (IBuiltin (Bint i))
  = format "Litint::Litint::make(%d)" [FI i]
applicExpr _ (IBuiltin (Bfloat f))
  = error ("no literal floats yet \"" ++ show f ++ "\"")
applicExpr _ (IBuiltin (Bchar c))
  = format "Litchar::Litchar::make('%c')" [FC c]
applicExpr table (ConstrApply qname expr_list)
  = format "%s::make(%s)" [FS (qualify qname), FS (applicArgs table expr_list)]
applicExpr table (FunctApply qname expr_list)
  = format "%s::make(%s)" [FS (qualify qname), FS (applicArgs table expr_list)]
applicExpr table (PartApplic missing expr)
  = format "Engine::Partial::make(%s, %d)" [FS (applicExpr table expr), FI missing]
applicExpr table (IOr expr_L expr_R)
 = format "Engine::Choice::make(%s, %s)" [FS (applicExpr table expr_L), FS (applicExpr table expr_R)]

applicExpr _ a@(IExternal _) = error ("Unexpected application of " ++ show a)
applicExpr _ a@(FreeVar _) = error ("Unexpected application of " ++ show a)
applicExpr _ a@(ILet _) = error ("Unexpected application of " ++ show a)
applicExpr _ a@(AlgebraicTable _ _ _ _) = error ("Unexpected application of " ++ show a)
applicExpr _ a@(BuiltinTable _ _ _ _) = error ("Unexpected application of " ++ show a)
applicExpr _ a@(Ignore _) = error ("Unexpected application of " ++ show a)

-- recursive construction of functional expressions
-- the boolean is True iff top-level
applicArgs table expr_list
  = concat (intersperse ", " (map (applicExpr table) expr_list))

------------------------------------------------------------------
-- VARIABLES

-- Variables are both declared and referenced and the two actions are
-- interdependent.  Thus, I build a table with the information needed
-- to coordinate the two.

------------------------------------------------------------------
-- LHS

-- A left-hand side variable is either an argument of the operation it
-- belongs to or an argument of a variable.  A variable may have a
-- name or it may be a nameless expression.  In this case, we say that
-- the variable is inlined.

-- The table tells whether a variable is an argument of the function
-- or an argument of a variable.  When a variable is an argument of
-- the function, the argument is used every where the variable is
-- referenced.

-- When a variable is an argument of a variable, its reference is its
-- name or when inlined is an expression that produces the argument of
-- another variable.  In this case, the "other variable" is similarly
-- referenced.  variables are identifier by an integer thogether with
-- a table gives all the information about the variable.

-- LHS variables rules applied in order: 
--
-- 1. If the count of a variable is zero, don't declare and there are
--    no references.
--
-- 2. If the variable is an argument (instance variable), don't
--    declare and replace any reference with the argument itself.
--
-- 3. If the count of a variable is one, don't declare and inline its
--    access at the place of reference.
--
-- 4. Otherwise, declare and initialize the variable, and use the
--    variable for the reference.


-- get the C++ expression that evaluates to the variable
get_var (_ ++ [(i, _, IPath (Arg k))] ++ _) i 
  = format "arg%d" [FI k]
get_var a@(_ ++ [(i, c, IPath (Rel k qname p))] ++ _) i
  | c==1 = format "((%s*) *(%s))->arg%d" [FS (qualify qname), FS (get_var a k), FI p]
  | True = local_id i
get_var (_ ++ [(i, c, IFree)] ++ _) i
  | c==1 = "::Engine::Variable::make()"
  | True = local_id i
-- TODO: consider inlining ???
get_var (_ ++ [(i, _, IBind _)] ++ _) i  = format "%s" [FS (local_id i)]

-- Declare all the variable of a scope, we know the scope is LHS
-- The table has been extended with var_list already
macro_lhs_declare :: [(Prelude.Int, Prelude.Int, ICurry.IKind)] -> [(Prelude.Int, Prelude.Int, ICurry.IKind)] -> Block
macro_lhs_declare table var_list
  = Block 0 (map (variant table) [i+0 | (i, c, _) <- var_list, c>0])
  where variant :: [(Prelude.Int, Prelude.Int, ICurry.IKind)] -> Int -> Block
        variant (_ ++ [(i, _, IPath (Arg k))] ++ _) i
           = SLine (format "// variable %s is inlined as arg%d"  [FS (local_id i), FI k])
        variant (_ ++ [(i, c, (IPath (Rel v qname p)))] ++ _) i
           | c==1 = SLine (format "// variable %d is inlined as ((%s*) (*%s))->arg%d"
                          [FI i, FS (qualify qname), FS (get_var table v), FI p])
           | True = SLine (format "Node** %s = ((%s*) (*%s))->arg%d;" 
                          [FS (local_id i), FS (qualify qname), FS (get_var table v), FI p])
        -- variant table i = error ("variant failure " ++ show table ++ "  " ++ show i)

------------------------------------------------------------------

macro_free_declare table var_list
  = Block 0 [variant table i | (i, _, _) <- var_list]
  where variant (_ ++ [(i, c, IFree)] ++ _) i 
          | c==1 = SLine (format "// free variable %s is inlined" [FS (local_id i)])
          | True = SLine (format "Node** %s = ::Engine::Variable::make();"
                                  [FS (local_id i)])

------------------------------------------------------------------
-- Declaration of let-block variables
-- this is only for variables in let-blocks
data Action
  = Forward Int                 -- declare only, assign later
  | Initialize Int              -- declare and initialize
  | Assign Int                  -- assign, already declared
  | Fill Int Int                -- fill value for mutual recursion

macro_let_declare table var_list
  = Block 0 [
      SLine ("// index " ++ show index_list),
      SLine ("// depend " ++ show depend_list),
      SLine ("// remove " ++ show removed_list),
      SLine ("// sorted " ++ show sorted_list),
      SLine ("// plan " ++ show plan),
      Block 0 [execute_plan_step table var_list i | i <- plan],
      NOP
    ]
  where (index_list, depend_list, removed_list, sorted_list, plan) = make_plan var_list

make_plan var_list
  = (index_list, depend_list, removed_list, sorted_list, plan)
  where -- the list of the let-bound variables
        index_list = [j | (j, _, _) <- var_list]
        -- for each let-bound variables find all the variables in its binding
        depend_list = [(i, occur expr) | (i,_ , IBind expr) <- var_list]
        -- remove non let-bound variables from the binings
        removed_list = [(i, myfilt dep) | (i, dep) <- depend_list]
                     where myfilt dep = filter (\x -> elem x index_list) dep
        -- sort by length of dependencies
        sorted_list = clever_sort removed_list
        plan = make_list sorted_list ([],[])
        -- make the plan for the construction of the graph
        -- (b,a) are the statements that must be executed before and after
        make_list [] (b,a) = b ++ a
        make_list (x:xs) (b,a)
          = make_list xs (b1,a1)
          where (b1,a1) = build_var x (b,a)
        -- make the plan for the construction of a variable       
        build_var (i,l) (b,a)
          = let forwards = [Forward k | k <- nub l
                                      , not (k == i)
                                      , not (Initialize k `elem` b)
                                      , not (Assign k `elem` b)]
                this = if Forward i `elem` b
                         then Assign i 
                         else Initialize i 
                fill_rest = [Fill k i | k <- nub l
                                      , not (Initialize k `elem` b)
                                      , not (Assign k `elem` b)]
            in (b++forwards++[this],a++fill_rest)
       
-- minList leq [x] = x
-- minList leq (x1:a@(x2:xs))
--   = let xa = minList leq a
--     in if leq x1 xa then x1 else xa


clever_sort x = clever_sort_1 [] x
clever_sort_1 _ [] = []
clever_sort_1 xdone a@(_:_) = min : rest_sorted
  where min@(v,_) = head (mergeSort (leq xdone) a)
        rest = delete min a
        rest_sorted = clever_sort_1 (v:xdone) rest
        leq xdone' (_, d1) (_, d2) = length (clean xdone' d1) < length (clean xdone d2)
        clean xdone' d = [x | x <- d, not (elem x xdone')]




occur (VariableRef j) = [j]
occur (IBuiltin _) = []
occur (ConstrApply _ expr_list) = concatMap occur expr_list
occur (FunctApply _ expr_list) = concatMap occur expr_list
occur (PartApplic _ expr) = occur expr
occur (IOr expr_L expr_R) =  occur expr_L ++ occur expr_R

-- No inlining for now
execute_plan_step _ _ (Forward i)
  = SLine (format "Node** %s = 0;" [FS (local_id i)])

execute_plan_step table (_ ++ [(i, _, IBind expr)] ++ _) (Initialize i)
  = SLine (format "Node** %s = %s;" [FS (local_id i), FS (applicExpr table expr)])

execute_plan_step table (_ ++ [(i, _, IBind expr)] ++ _) (Assign i)
  = SLine (format "%s = %s;" [FS (local_id i), FS (applicExpr table expr)])

execute_plan_step _ (_ ++ [(j, _, IBind expr)] ++ _) (Fill i j)
  = Block 0 [fill (local_id j) (reverse path) (local_id i) | path <- find_path_set expr i]

-- The path is build reversed (for efficiency)
find_path (VariableRef i) j path
  = if i == j then Just path else Nothing
find_path (IBuiltin _) _ _
  = Nothing
find_path (ConstrApply qname expr_list) j path
  = find_path expr j ((qname,index):path)
  where pairs = zip expr_list [1..]
        choose (x:xs) = x ? choose xs
        (expr, index) = choose pairs
find_path (FunctApply qname expr_list) j path
  = find_path expr j ((qname,index):path)
  where pairs = zip expr_list [1..]
        choose (x:xs) = x ? choose xs
        (expr, index) = choose pairs
find_path (PartApplic _ expr) j path
  = find_path expr j ((qname,2):path)
  where qname = ("Engine","Partial")

find_path_set expr j
  = good_paths
  where all_paths = sortValues (set3 find_path expr j [])
        good_paths = [path | (Just path) <- all_paths]

fill varl [] varr = SLine (format "%s = %s;" [FS varl, FS varr])
fill varl ((qname,index):rest) varr
  = fill previous rest varr
  where previous = format "((%s*) *%s)->arg%d" [FS (qualify qname), FS varl, FI index] 