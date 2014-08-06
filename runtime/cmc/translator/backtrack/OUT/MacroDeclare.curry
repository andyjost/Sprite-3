import ICurry
import Block
import Format
import Utils

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
  | True = pvar_id i
get_var (_ ++ [(i, c, IFree)] ++ _) i
  | c==1 = "::Engine::Variable::make()"
  | True = pvar_id i
-- TODO: consider inlining ???
get_var (_ ++ [(i, _, IBind _)] ++ _) i  = format "%s" [FS (pvar_id i)]
-- never inline
get_var (_ ++ [(i, _, ILocal)] ++ _) i  = format "%s" [FS (temp_id i)]

-- Declare a LHS variable
macro_lhs_declare (_ ++ [(i, _, IPath (Arg k))] ++ _) i
  = SLine (format "// lhs variable %s is inlined as arg%d"  [FS (pvar_id i), FI k])
macro_lhs_declare a@(_ ++ [(i, c, (IPath (Rel v qname p)))] ++ _) i
  | c==1 = SLine (format "// lhs variable %d is inlined as ((%s*) (*%s))->arg%d"
                          [FI i, FS (qualify qname), FS (get_var a v), FI p])
  | True = SLine (format "Node** %s = ((%s*) (*%s))->arg%d;" 
                          [FS (pvar_id i), FS (qualify qname), FS (get_var a v), FI p])

------------------------------------------------------------------

macro_free_declare (_ ++ [(i, c, IFree)] ++ _) i 
  | c==1 = SLine (format "// free variable %s is inlined" [FS (pvar_id i)])
  | True = SLine (format "Node** %s = ::Engine::Variable::make();" [FS (pvar_id i)])

------------------------------------------------------------------

macro_fwrd_declare (_ ++ [(i, c, IBind _)] ++ _) i
  | c==1 = SLine (format "// bound variable %s is inlined" [FS (pvar_id i)])
  | True = SLine (format "Node** %s;" [FS (pvar_id i)])

macro_fwrd_declare (_ ++ [(i, c, ILocal)] ++ _) i
  | c==1 = SLine (format "// local variable %s is inlined" [FS (temp_id i)])
  | True = SLine (format "Node** %s;" [FS (pvar_id i)])

------------------------------------------------------------------
