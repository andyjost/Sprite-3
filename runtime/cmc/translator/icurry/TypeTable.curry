-- Build a table of all the types.
-- Each type has the list of its constructors with the arity

module TypeTable where

import FlatCurry

execute modules = concatMap get_type_list modules

-- ignore type synonyms
get_type_list (Prog _ _ type_list _ _)
  = map get_type filtered_type_list
  where filtered_type_list = [x | x @ (Type _ _ _ _) <- type_list]

get_type (Type tname _ _  consDecl_list)
  = (tname, [(cname,int) | Cons cname int _ _ <- consDecl_list])

-- TODO: Could remove from the table unused types

ppTypeTable tt = foldr ((++) . ppType) "" tt
ppType (tname, cs) = "\nType " ++ ppIName tname ++ "\n" ++ foldr ((++) . ppCons) "" cs
ppCons (cname, arity) = "  " ++ ppIName cname ++ " " ++ show arity ++ "\n"
ppIName (s,n) = s ++ "." ++ n

