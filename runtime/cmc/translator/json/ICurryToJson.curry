import ICurry
import JSON
import Float

icurryToJson (IModule name imported_list data_list funct_list)
  = JO [("module", 
       JO (("name",JS name) :
           ("import", JA (map JS imported_list)) :
	   ("declare_constructors", JA (toData data_list)) :
           ("declare_functions", JA (map toFunction funct_list)) :
           [] ) )]

toData data_list
  = map toConstructor (concatMap ( \x -> zip [0..] x) data_list)

toConstructor (index, IConstructor (_,name) arity)
  = JO [("constructor", 
     JO [("name",JS name),("arity",JN (i2f arity)),("index",JN (i2f index))] )]
toFunction (IFunction (_,name) arity scope)
  = JO [("function",
     JO ([("name",JS name),
          ("arity", JN (i2f arity))] ++
          toScope scope))]
          
 
toScope (Scope_Expr var_list expr)
  = [("declare_variables", JA (map toVariable var_list)),
        ("return", toExpr expr)]

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

toExpr (AlgebraicTable suffix flex expr entry_list)
  = JO [("algebraic_table", 
       JO [("suffix", JS suffix),
       	   ("flexible", if flex then JTrue else JFalse),
	   ("selector", toExpr expr),
	   ("branches", JA (map toAlgEntry entry_list))])]
toAlgEntry (IConstructor cname arity, scope)
 = JO ([("label", JO [(toQName cname),("arity", JN (i2f arity))])] ++ (toScope scope))
toExpr (BuiltinTable suffix flex expr entry_list)
  = JO [("builtin_table", 
       JO [("suffix", JS suffix),
       	   ("flexible", if flex then JTrue else JFalse),
	   ("selector", toExpr expr),
	   ("branches", JA (map toLitEntry entry_list))])]
toLitEntry (literal, expr)
  = JO [("label", toLitCase literal),("expr", toExpr expr)]
toExpr (IOr expr_l expr_r)
  = JO [("or", JO [("left", toExpr expr_l), ("right", toExpr expr_r)])]
toExpr Exempt = JO [("exempt", JNull)]
