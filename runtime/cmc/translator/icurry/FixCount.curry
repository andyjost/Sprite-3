module FixCount where

import ICurry

execute (IModule _ _ _ funct_list)
  = foldl fixFunct 0 funct_list
  

fixFunct c (IFunction _ _ table stmt_list)
  -- The addition with c is only to force the execution
  = c + foldl fixStmt (1 + maxlist table) stmt_list
  where maxlist [] = 0
        maxlist ((j,_,_):xs) = max j (maxlist xs)

{-
fixExpr c Exempt = c
fixExpr c (Reference _) = c
fixExpr c (BuiltinVariant _) = c
-- cases are outermost in expressions, no cases in a
fixExpr c (Applic _ _ _) = c
fixExpr c (PartApplic _ _) = c
fixExpr c (IOr expr1 expr2) = fixExpr (fixExpr c expr1) expr2
-}

-- There are no nested cases in any of these
fixStmt c (IExternal _) = c
fixStmt c (Comment _) = c
fixStmt c (DeclareLHSVar _) = c
fixStmt c (DeclareFreeVar _) = c
fixStmt c (Forward _) = c
fixStmt c (Assign _ _) = c
fixStmt c (Initialize _ _) = c
fixStmt c (Fill _ _ _) = c
fixStmt c (Return _) = c
-- There could be nested cases in the branches
fixStmt c (ATable c _ _ branch_list) 
  = foldl fixStmt (c+1) (concatMap snd branch_list)
  --  = foldl (\ n ss -> foldl fixStmt n ss) c (map snd branch_list)
fixStmt c (BTable c _ _ branch_list)
  = foldl fixStmt (c+1) (concatMap snd branch_list)

