-- ### This program is a stub under development ### --

import ICurry
import JSON
import Float

icurryToJson (IModule name imported_list data_list funct_list)
  = JO [("module", 
       JO (("name",JS name) :
           ("import", JA (map JS imported_list)) :
	   ("declare_data", JA (map toData data_list)) :
           ("declare_functions", JA (map toFunction funct_list)) :
           [] ) )]

toData ((_,name), clist)
  = JO [("name",JS name),
        ("declare_constructors",  JA (map toConstructor clist))]

toConstructor (IConstructor (_,name) arity)
  = JO [("name",JS name),
        ("arity",JN (i2f arity))]

toFunction (IFunction (_,name) arity table stmt_list)
  = JO [("function",
     JO [("name",JS name),
          ("arity", JN (i2f arity)),
          ("var_table", JS "*** N/A ***"),
          ("statements", JA (map toStmt stmt_list))])]

------------------------------------------------------------------

toStmt (IExternal string)
  = JO [("external", JS string)]

toStmt (Comment string)
  = JO [("comment", JS string)]

toStmt (Return expr)
  = JO [("return", JS (show expr))]

toStmt (DeclareLHSVar path)
  = JO [("declare_lhs_var", JS (show path))]

toStmt (DeclareFreeVar var)
  = JO [("declare_lhs_var", JS (show var))]

toStmt (Forward var)
  = JO [("forward", JS (show var))]

toStmt (Initialize i expr)
  = JO [("initialize", 
       JO [("variable", JS (show i)),
           ("expr", JS (show expr))])]

toStmt (Assign i expr)
  = JO [("assign", 
       JO [("variable", JS (show i)),
           ("expr", JS (show expr))])]

toStmt (Fill i path j)
  = JO [("fill", JS ((show i) ++ " " ++ (show path) ++ " " ++ (show i)))]

toStmt (ATable suffix flex expr branch_list)
  = JO [("atable", JN (i2f suffix))]

toStmt (BTable suffix flex expr branch_list)
  = JO [("btable", JN (i2f suffix))]

------------------------------------------------------------------

{-
toVariable (index, count, IBind expr)
  = JO [("rhs_var", JO [("index", JN (i2f index)),("count", JN (i2f count)),("expr", toExpr expr)])]
toVariable (index, count, IFree)
  = JO [("free_var", JO [("index", JN (i2f index)),("count", JN (i2f count))])]
toVariable (index, count, IPath path)
  = JO [("lhs_var", JO [("index", JN (i2f index)),("count", JN (i2f count)),("path", JA (map toPath path))])]

toQName (qual, name) = ("qname",JO [("module", JS qual),("name", JS name)])
toPath (qname, int)
  = JO [toQName qname,("argument", JN (i2f int))]

toExpr (IExternal name)
  = JO [("external", JS name)]
toExpr (ILet scope) = JO (toScope scope)
toExpr (FreeVar scope) = JO (toScope scope)
toExpr (VariableRef i)
  = JO [("variable", JO [("index", JN (i2f i))])]
toExpr (IBuiltin x)
  = JO [("literal", toLitCase x)]
toLitCase (Bint i)   = JO [("type", JS "integer"),("value", JN (i2f i))]
toLitCase (Bchar c)  = JO [("type", JS "character"),("value", JS [c])]
toExpr (FunctApply qname expr_list)
  = JO [("funct_apply", JO [toQName qname, ("arguments", JA (map toExpr expr_list))])]
toExpr (ConstrApply qname expr_list)
  = JO [("constr_apply", JO [toQName qname, ("arguments", JA (map toExpr expr_list))])]
toExpr (PartApplic missing expr)
  = JO [("part_apply", JO [("missing", JN (i2f missing)), ("expr", toExpr expr)])]
toExpr (Exempt)
  = JO [("expr", JS "exempt")]

-}
