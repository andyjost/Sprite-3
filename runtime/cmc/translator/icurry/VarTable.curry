-- Build a table of all the variables of an operation definition
-- An entry in the table is (v, c, x) where:
--   v => is the index of the variable
--   c => counts how many time the variable is referenced, 0 for now
--   x => describes the kind of variable, where:
--      IFree => a free,logic, unbound variable
--      IBind => a variable bound in a let block to some expr, no expr in the table
--      IPath (Arg j) => the variable is the j-th successor of the root
--      IPath (Rel base qname j) => the variable is the j-th successor of base
--                                  qname is te label of the predecessor.

module VarTable where

import FlatCurry
import ICurry

execute (Prog _ _ _ funct_list _) = map makeFunct funct_list

makeFunct (Func qname _ _ _ rule) = (qname, makeRule rule)
makeRule (External _) = []
makeRule (Rule var_list expr) = table ++ makeExpr expr
  where table = [(v, 0, IPath (Arg j)) | (v, j) <- zip var_list [1..]]

makeExpr (Var _) = []
makeExpr (Lit _) = []
makeExpr (Comb _ _ expr_list) = concatMap makeExpr expr_list
makeExpr (Let bind_list expr) = outer_table ++ inner_tables ++ makeExpr expr
  -- get the variables of: (1) the bind list, (2) the bindings, (3) the expression
  where outer_table = [(v, 0, IBind) | (v, _) <- bind_list]
        inner_tables = concat [makeExpr bind | (_, bind) <- bind_list]
makeExpr (Free var_list expr) = [(v, 0, IFree) | v <- var_list] ++ makeExpr expr
makeExpr (Or expr_l expr_r) = makeExpr expr_l ++ makeExpr expr_r
makeExpr (Case _ expr branch_list) = outer_table ++ inner_tables
  where outer_table = makeExpr expr
        inner_tables = concatMap branch_table branch_list
        branch_table (Branch (LPattern _) branch_expr) = makeExpr branch_expr
        branch_table (Branch (Pattern qname var_list) branch_expr) = makeExpr branch_expr ++
          case expr of
             (Var base) -> [(v, 0, IPath (Rel base qname j)) | (v, j) <- zip var_list [1..]]
             _          -> []
makeExpr (Typed expr _) = makeExpr expr
