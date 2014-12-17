import ICurry
-- import Unsafe

icurryToPoly (IModule modname {-imported_list-}_ data_list {-funct_list-}_)
  = [make_funct modname onetype | onetype <- data_list]

make_funct modname ((_,typename),clist)
  = IFunction (modname, ".equals."++typename) 2 var_list (make_table clist)
  where var_list = [(1,count,(IPath (Arg 1))),(2,count,(IPath (Arg 2)))]
                     ++ mapping
        mapping = map to_var (map_variables clist)
        count = length mapping `div` 2 + 1

-- make a mapping from (constructor, variable_index) pairs to integers
-- start at 3
map_variables clist
  = zip pairs [3..]
  where pairs = [((iname,arg),k) 
                   | (IConstructor iname arity) <- clist, 
                     k <- [1..arity], 
                     arg <- [1,2]]

to_var (((iname,arg),k),pos)
  = (pos,1,(IPath (Rel arg iname k))) 


make_table clist = [
    ATable 1 False 
    (Reference [(1,1,(IPath (Arg 1)))]) -- first argument
    [make_inner k ctor clist mapping | (k,ctor) <- zip [2..] clist]
  ]
  where mapping = map_variables clist

make_inner k ctor clist mapping = (ctor, [
    ATable k False (Reference [(2,1,(IPath (Arg 2)))]) -- second argument
    [make_branch ctor arg2 mapping | arg2 <- clist]
  ])

make_branch ctor arg2 mapping = (arg2, [
  if ctor == arg2 
    then (recur ctor mapping)
    else Return (Applic True ("Prelude","False") [])
  ])

recur (IConstructor iname arity) mapping
  | arity == 0 = Return (Applic True ("Prelude","True") [])
  | otherwise = Return (merge subexpr)
  where subexpr = [(((iname',arity'),arg),k) 
                 | (((iname',arity'),arg),k) <- mapping, iname'==iname]
        merge [x,y] = Applic False ("Prelude","==") [to_ref x, to_ref y]
        merge (a:b:c:d) = Applic False ("Prelude","&&") 
          [Applic False ("Prelude","==") [to_ref a, to_ref b], merge (c:d)]
        to_ref x = Reference [to_var x]
