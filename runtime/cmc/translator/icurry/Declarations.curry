import ICurry

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

vlookup (_ ++ [v@(i, _, _)] ++ _) i = v

declare_lhs vtable var_list 
  = [result i  | i <- var_list]
  where result i =
           case variable vtable i of
              [v@(_, _, IPath (Arg k))]
                 ->  Comment ("LHS variable " ++ show v ++ " is argument " ++ show k)
              p@(v@(_, 1, _) : _)
                 -> Comment ("LHS variable " ++ show v ++ " is inlined as " ++ show (reverse p))
              p@(_ : _)
                 -> DeclareLHSVar p
              [] -> Comment ("LHS variable " ++ show (vlookup vtable i) ++ " is not used")

-- variables are self-contained in all cases except IPath
variable vtable i
  = case (vlookup vtable i) of
      (_, _, IPath _) -> get_path_tail (get_path vtable i)
      v -> [v]

-- Compute the path of a variable from the root of the
-- redex/lhs/application in which the variable appears.
-- Path is reversed
get_path (_ ++ [v@(i, _, IPath (Arg _))] ++ _) i = [v]
get_path a@(_ ++ [v@(i, _, IPath (Rel k _ _))] ++ _) i = v : get_path a k

-- Path is reversed

-- Compute the path starting from a variable that is declared and ending
-- in a variable that is declared.  variables with count 1 are not
-- declared.  They are inlined.
get_path_tail (x@(_, c, _) : xs) 
  | c==0 = []             -- variable is not used
  | c==1 = x : recur xs   -- variable is inlined
  | True = [x]            -- variable is enough
  where recur [] = []
        recur (v@(_, d, _) : vs)
           | d==0 = error "something wrong with variable paths"
           | d==1 = v : recur vs
           | True = [v]

declare_free vtable 
  = [result v | v@(_, _, IFree) <- vtable]
  where result w@(_, c, _)
          | c == 1 = Comment ("Free variable " ++ show w ++ " is inlined")
          | True   = DeclareFreeVar w

