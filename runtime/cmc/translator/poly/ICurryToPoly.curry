import ICurry
-- import Unsafe

equals typename = "=="++"."++typename
primitive x = "primitive."++x

icurryToPoly (IModule modname {-imported_list-}_ data_list {-funct_list-}_)
  = concat [make_funct modname onetype | onetype <- data_list]

-- The constructor of a builtin type has arity 1, the builtin value it hosts.
make_funct modname ((_,typename),[])
  = [(IFunction (modname,symbol) 2 [(1,1,(IPath (Arg 1))),(2,1,(IPath (Arg 2)))] 
        [(ATable 1 False (Reference [(1,1,(IPath (Arg 1)))]) [((IConstructor (modname,typename) 1),
           [(ATable 2 False (Reference [(2,1,(IPath (Arg 2)))]) [((IConstructor (modname,typename) 1),
              [Return (Applic False (modname,primitive (equals typename))
                     [(Reference [(1,1,(IPath (Arg 1)))]),(Reference [(2,1,(IPath (Arg 2)))])])])])])])]),
     (IFunction (modname,primitive symbol) 2 [] [(IExternal (modname++"."++primitive symbol))])]
  where symbol = equals typename

make_funct modname ((_,typename),clist@(_:_))
  = [IFunction (modname, equals typename) 2 var_list (make_table clist)]
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
