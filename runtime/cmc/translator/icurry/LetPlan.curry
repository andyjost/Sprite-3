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
  = (sorted_list, action_list)
  where -- the list of the let-bound variables
        index_list = [j | (j, _) <- bind_list]
        -- for each let-bound variables find all the variables in its binding
        depend_list = [(i, occur expr) | (i, expr) <- bind_list]
        -- remove non let-bound variables from the binings
	removed_list = [(i, myfilt dep) | (i, dep) <- depend_list]
                     where myfilt dep = filter (\x -> elem x index_list) dep
        -- sort by length of dependencies
        sorted_list = combined_sort removed_list
        action_list = toAction sorted_list

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


combined_sort [] = []
combined_sort list@(_:_)
  = min : combined_sort rest
  where (min@(j,_) : tmp) = mergeSort (\ (_,us) (_,vs) -> length us <= length vs) list
        rest = [(k,[p | p <- lk, not (p==j)]) | (k,lk) <- tmp]

------------------------------------------------------------------

toAction list 
  = before ++ center ++ after
  where tmp = nub [y | (_,l) <- list, y <- l]
        center = [(if elem x tmp then Passign else Pinitialize) x | (x,_) <- list]
        after = [Pfill x y | (x,l) <- list, y <- nub l]
        before = map Pforward tmp

-- This works only under the assumption that a binding
-- is functional: not multibranch tables or let blocks
-- REMEMBER that the path is reversed !
find_path (Var i) i path = path
find_path (Comb _ qname (x ++ [expr] ++ _)) i path = find_path expr i ((qname, 1 + length x) : path)

find_path (Case _ _ _) _ _ = error "Untraversed Casein find path"
find_path (Typed _ _) _ _ = error "Unexpected Typed expression in find path"

find_path_set expr j = sortValues (set3 find_path expr j [])
