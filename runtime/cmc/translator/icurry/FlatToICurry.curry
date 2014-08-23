-- This file is too long, but the step from FlatCurry to ICurry is
-- large.  Introducing an intermediate representation seems a lot of
-- troubles.

-- This program makes big assumptions on Flatcurry.  These assumption
-- should be guaranteed by Norm.  In a FlatCurry expression,
-- multibranch tables and let blocks are outermost.  They can be
-- nested.
--
-- An expression is found is several contexts (see ICurry syntax).
-- In particular, the bindings of a let block and the selector of
-- a multibranch table are expressions.

module FlatToICurry(execute) where

import FlatCurry
import ICurry
import Declarations
import LetPlan

-- ttable is used to complete and reorder the branches of an ATable.

execute type_table var_table (Prog name imported type_list funct_list _)
  = IModule name imported constr_decl funct_decl
  where constr_decl = [makeType x | x@(Type _ _ _ _) <- type_list]
  	funct_decl = map (makeFunct type_table) (zip var_table funct_list)

makeType (Type qname _ _ constr_list)
  = (qname, [IConstructor cname arity | (Cons cname arity _ _) <- constr_list])

makeFunct type_table ((qname,var_table), Func qname arity _ _ rule)
  = IFunction qname arity var_table (makeRule stuff rule)
  -- TODO: I am assuming that the variables in the var_table have indices 1, 2, ... n
  where stuff = (Return, type_table, var_table)

makeRule _ (External x) = [IExternal x]
makeRule stuff (Rule var_list expr) 
  = declare_lhs  (vtable stuff) var_list
      ++ declare_free (vtable stuff)
      ++ makeStmt stuff expr
 
declare_let stuff bind_list
  = [Forward (vlookup (vtable stuff) i) | (i, _) <- bind_list] ++
       concat [makeStmt (new_stuff i) expr | (i, expr) <- bind_list]
  where new_stuff i = (Assign i, ttable stuff, vtable stuff)

mode   (x, _, _) = x
ttable (_, x, _) = x
vtable (_, _, x) = x

------------------------------------------------------------------

makeStmt stuff a@(Var _) = [(mode stuff) (makeExpr stuff a)]
makeExpr stuff (Var i) = Reference (variable (vtable stuff) i)
makeStmt stuff a@(Lit _) = [(mode stuff) (makeExpr stuff a)]
makeExpr _ (Lit (Intc x)) = BuiltinVariant (Bint x)
makeExpr _ (Lit (Charc x)) = BuiltinVariant (Bchar x)
makeExpr _ (Lit (Floatc x)) = BuiltinVariant (Bfloat x)

makeStmt stuff a@(Comb _ _ _) = [(mode stuff) (makeExpr stuff a)]
makeExpr stuff (Comb FuncCall qname expr_list)
  = Applic False qname (map (makeExpr stuff) expr_list)
makeExpr stuff (Comb ConsCall qname expr_list)
  = Applic True qname (map (makeExpr stuff) expr_list)
makeExpr stuff (Comb (FuncPartCall missing) qname expr_list)
  = PartApplic missing (Applic False qname (map (makeExpr stuff) expr_list))
makeExpr stuff (Comb (ConsPartCall missing) qname expr_list)
  = PartApplic missing (Applic True qname (map (makeExpr stuff) expr_list))

makeStmt stuff (Let bind_list expr)
  = Comment (show sorted_list) :
       execute_plan stuff bind_list plan ++
           makeStmt stuff expr
  where (sorted_list, plan) = make_plan (vtable stuff) bind_list

makeExpr _ (Let _ _) = error "FlatToICurry found a let-block while making an expression"

-- Free vars appear in the vtable.  They will be defined (declared and
-- assigned) at the beginning of an operation code or inlined (if
-- occur only once).
makeStmt stuff (Free _ expr) = makeStmt stuff expr
makeExpr stuff (Free _ expr) = makeExpr stuff expr

makeStmt stuff a@(Or _ _) = [(mode stuff) (makeExpr stuff a)]
makeExpr stuff (Or expr_1 expr_2) = IOr (makeExpr stuff expr_1) (makeExpr stuff expr_2)

makeStmt stuff (Case flex expr branch_list@(Branch (Pattern cname _) _ : _))
  = [ATable counter (flex==Flex) (makeExpr stuff expr) full_table]
  where counter = unknown  -- later replace with an int 
        -- get the complete list of constructors labeling the table
        -- and attach an index to the constructor
        -- the index is intended for creating unique labels for the entries of the table
        get (_ ++ [(_, a@(_ ++ [(qname,_)] ++ _))] ++ _) qname = zip a [0..]
        full_table = concatMap (choose_create branch_list) (get (ttable stuff) cname)
        -- if the table is missing a constructor, create an entry for it
        -- otherwise choose the corresponing entry in the table
	choose_create (Branch (Pattern pname var_list) branch_expr : z) ctor@((cname', arity), _)
	  | cname'==pname = [(IConstructor cname' arity, 
                                      declare_lhs (vtable stuff) var_list ++
                                         makeStmt stuff branch_expr)]
	  | otherwise = choose_create z ctor
	choose_create [] ((cname', arity), _) = [(IConstructor cname' arity, [(mode stuff) Exempt])]

makeStmt stuff (Case flex expr branch_list@(Branch (LPattern _) _ : _))
  = [BTable counter (flex==Flex) (makeExpr stuff expr)
      [(translate pattern, makeStmt stuff branch_expr) 
          | (Branch (LPattern pattern) branch_expr) <- branch_list]]
  where counter = unknown  -- later replace with an int
        translate (Intc x) = Bint x
        translate (Charc x) = Bchar x
        translate (Floatc x) = Bfloat x

makeExpr _ (Case _ _ _) = error "FlatToICurry found a multibranch case while making an expression"

makeStmt stuff (Typed expr _) = makeStmt stuff expr
makeExpr stuff (Typed expr _) = makeExpr stuff expr

execute_plan stuff bind_list plan
  = concatMap (execute_plan_step stuff bind_list) plan

-- No inlining for now
-- IBind in vtable is empty there is no binding
execute_plan_step stuff  _ (Pforward i) = [Forward (vlookup (vtable stuff) i)]
execute_plan_step stuff (_ ++ [(i, expr)] ++ _) (Passign i)
  = [Assign i (makeExpr stuff expr)]
execute_plan_step stuff (_ ++ [(i, expr)] ++ _) (Pinitialize i)
  = [Initialize (vlookup (vtable stuff) i) (makeExpr stuff expr)]
execute_plan_step _ (_ ++ [(i, expr)] ++ _) (Pfill i j)
  = [Fill i (reverse p) j | p <- find_path_set expr j]
