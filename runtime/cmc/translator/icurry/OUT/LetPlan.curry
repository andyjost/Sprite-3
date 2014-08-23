import List
import Sort
import SetFunctions
import FlatCurry
import ICurry

------------------------------------------------------------------
-- Stuff for let-blocks

-- this is only for variables in let-blocks
data Action
  = Pforward Int                 -- declare only, assign later
  | Pinitialize Int              -- declare and initialize
  | Passign Int                  -- assign, already declared
  | Pfill Int Int                -- fill value for mutual recursion

-- TODO: remove passed vtable

make_plan _ bind_list
  = (removed_list, plan) -- (index_list, depend_list, removed_list, sorted_list)
  where -- the list of the let-bound variables
        index_list = [j | (j, _) <- bind_list]
        -- for each let-bound variables find all the variables in its binding
        depend_list = [(i, occur expr) | (i, expr) <- bind_list]
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
          = let forwards = [Pforward k | k <- nub l
                                      , not (k == i)
                                      , not (Pinitialize k `elem` b)
                                      , not (Passign k `elem` b)]
                this = if Pforward i `elem` b
                         then Passign i 
                         else Pinitialize i 
                fill_rest = [Pfill k i | k <- nub l
                                      , not (Pinitialize k `elem` b)
                                      , not (Passign k `elem` b)]
            in (b++forwards++[this],a++fill_rest)

-- All the variables that occur in an expression
-- repeated each time for each occurrence.
-- The binding are assumed to be normalized (not free blocks or types expressions)
-- and functional (no jump tables and let blocks).
occur (Var j) = [j]
occur (Lit _) = []
occur (Comb _ _ expr_list) = concatMap occur expr_list
occur (Let _ _) = error "Unexpected Let block"
occur (Free _ _) = error "Unexpected Free block"
occur (Or _ _) =  error "Unexpected LHS non-determinism"
occur (Case _ _ _) = error "Unexpected multibranch statement"
occur (Typed _ _) = error "Unexpected typed expression"
       
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

------------------------------------------------------------------

-- This works only under the assumption that a binding
-- is functional: not multibranch tables or let blocks
-- REMEMBER that the path is reversed !
find_path (Var i) i path = path
find_path (Comb _ qname (x ++ [expr] ++ _)) i path = find_path expr i ((qname, 1 + length x) : path)
find_path (Typed expr _) i path = find_path expr i path

find_path_set expr j = sortValues (set3 find_path expr j [])
