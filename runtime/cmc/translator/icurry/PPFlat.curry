-- Print a FlatCurry program in a more readable form.
-- Types are ommited under the assumption that the program
-- is well-typed (or there would not be a FlatCurry).

module PPFlat where

import FlatCurry

execute (Prog name imported_list data_list funct_list _)
  = "\n" ++ "program " ++ show name ++
       foldr ((++) . ppImported) "" imported_list ++
       foldr ((++) . ppData) "" data_list ++
       foldr ((++) . ppFunction) "" funct_list

ppImported string
  = "\n  import \"" ++ string ++ "\""

ppData (Type qname _ _ cons_list)
  = "\n  type " ++ ppQName qname ++
    foldr ((++) . ppConstructor) "" cons_list

ppConstructor (Cons qname arity _ _)
  = "\n    constructor " ++ ppQName qname ++ " " ++ show arity

ppFunction (Func qname arity _ _ rule)
  = "\n  function " ++ ppQName qname ++ " " ++ show arity ++
    ppRule rule

ppRule (External string)
  = "\n    external \"" ++ string ++ "\""

ppRule (Rule var_list expr)
  = "\n    lhs_vars " ++ show var_list ++
    ppExpr 4 expr

------------------------------------------------------------------

ppExpr n (Var index)
  = ppIndent n ++ "var " ++ show index

ppExpr n (Lit (Intc x))
  = ppIndent n ++ "int " ++ show x

ppExpr n (Lit (Floatc x))
  = ppIndent n ++ "float " ++ show x

ppExpr n (Lit (Charc x))
  = ppIndent n ++ "char " ++ show x

-- TODO: a program transformation that reduces to FuncCall ? ConsCall
ppExpr n (Comb part qname expr_list)
  | part =:= (FuncPartCall ? ConsPartCall) miss
  = ppIndent n ++
       "Node parXtial (" ++ show miss ++ " ," ++
       ppIndent (n+2) ++ "Node " ++ ppQName qname ++ 
         ppArgList (n+4) expr_list ++ " )"
  where miss free

ppExpr n (Comb (FuncCall ? ConsCall) qname expr_list)
  = ppIndent n ++
       "Node " ++ ppQName qname ++
       ppArgList (n+2) expr_list

ppExpr n (Let index_expr_list expr)
  = ppIndent n ++ "let " ++ 
      foldr ((++) . ppPair (n+2)) "" index_expr_list ++
      ppExpr n expr
  where ppPair m (index,bind)
          = ppIndent m ++ "bind_var " ++ show index ++
              ppExpr (m+2) bind

ppExpr n (Free var_list expr)
  = ppIndent n ++ "free_vars " ++ show var_list ++
      ppExpr (n+2) expr

ppExpr n (Or exprl exprr)
  = ppIndent n ++ "Or " ++
      ppExpr (n+2) exprl ++
      ppExpr (n+2) exprr

ppExpr n (Typed expr _)
  = ppExpr n expr

ppExpr n (Case xtype expr branch_list)
  = ppIndent n ++ "case " ++ xshow xtype ++
      ppExpr (n+2) expr ++
      foldr ((++) . ppBranch n) "" branch_list
  where xshow Flex = "flex"
        xshow Rigid = "rigid"

------------------------------------------------------------------

ppBranch n (Branch (Pattern qname var_list) expr)
  = ppIndent n ++ ppQName qname ++
      ppIndent (n+2) ++ "lhs_vars " ++ show var_list ++
      ppExpr (n+4) expr

ppBranch n (Branch (LPattern lit) expr)
  = ppIndent n ++ xshow lit ++
      ppExpr (n+2) expr
  where xshow (Intc x)   = "int "   ++ show x
        xshow (Floatc x) = "float " ++ show x
        xshow (Charc x)  = "char "  ++ show x

ppArgList _ [] = ""

ppArgList n [x]
  = " (" ++ ppExpr n x ++ " )"

ppArgList n (x1:x2:xs)
  = " (" ++
       ppExpr n x1 ++
       foldr ((++) . aux) "" (x2:xs) ++
       " )"
    where aux x = " ," ++ ppExpr n x

ppQName (s,n) = "\"" ++ s ++ "." ++ n ++ "\""

ppIndent indent
  = "\n" ++ take indent (repeat ' ')
