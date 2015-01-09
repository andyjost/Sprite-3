import ICurry
-- import Unsafe

symname (name,_,_,_,_) = name
default (_,def,_,_,_) = ("Prelude", def)
conjunction (_,_,conj,_,_) = ("Prelude", conj)
comparison (_,_,_,comp,_) = ("Prelude", comp)
make_branch (_,_,_,_,mkbranch) = mkbranch

spec_equality = (equals, "True", "&&", "==", make_branch_equality)
spec_compare = (compares, "EQ", "compare_conjunction", "compare", make_branch_compare)

equals typename = "=="++"."++typename
compares typename = "compare"++"."++typename
primitive x = "primitive."++x

icurryToPoly (IModule modname {-imported_list-}_ data_list {-funct_list-}_)
  = concat [
      concat [
          make_funct modname onetype spec_equality,  make_funct modname onetype spec_compare
        ]
      | onetype <- data_list
    ]

-- The constructor of a builtin type has arity 1, the builtin value it hosts.
make_funct modname ((_,typename),[]) spec
  = [(IFunction (modname,symbol) 2 [(1,1,(IPath (Arg 1))),(2,1,(IPath (Arg 2)))] 
        [(ATable 1 False (Reference [(1,1,(IPath (Arg 1)))]) [((IConstructor (modname,typename) 1),
           [(ATable 2 False (Reference [(2,1,(IPath (Arg 2)))]) [((IConstructor (modname,typename) 1),
              [Return (Applic False (modname,primitive ((symname spec) typename))
                     [(Reference [(1,1,(IPath (Arg 1)))]),(Reference [(2,1,(IPath (Arg 2)))])])])])])])]),
     (IFunction (modname,primitive symbol) 2 [] [(IExternal (modname++"."++primitive symbol))])]
  where symbol = (symname spec) typename

make_funct modname ((_,typename),clist@(_:_)) spec
  = [IFunction (modname, (symname spec) typename) 2 var_list (make_table clist spec)]
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


make_table clist spec = [
    ATable 1 False 
    (Reference [(1,1,(IPath (Arg 1)))]) -- first argument
    [make_inner k ctor clist mapping spec | (k,ctor) <- zip [2..] clist]
  ]
  where mapping = map_variables clist
  
make_inner k ctor clist mapping spec = (ctor, [
    ATable k False (Reference [(2,1,(IPath (Arg 2)))]) -- second argument
    [(make_branch spec) ctor arg2 clist mapping | arg2 <- clist]
  ])

make_branch_equality ctor arg2 _ mapping = (arg2, [
  if ctor == arg2 
    then (recur ctor mapping spec_equality)
    else Return (Applic True ("Prelude","False") [])
  ])

-- compare ctor1 and ctor2 according the order in which they appear in clist
compare_ctors ctor1 ctor2 clist
  | ctor1 == ctor2 = EQ
  | otherwise      = aux clist
  where
    aux (c:clist')
      | ctor1 == c = LT
      | ctor2 == c = GT
      | otherwise  = aux clist'

make_branch_compare ctor arg2 clist mapping = (arg2, [
  case (compare_ctors ctor arg2 clist) of
    LT -> Return (Applic True ("Prelude", "LT") [])
    GT -> Return (Applic True ("Prelude", "GT") [])
    EQ -> (recur ctor mapping spec_compare)
  ])

recur (IConstructor iname arity) mapping spec
  | arity == 0 = Return (Applic True (default spec) [])
  | otherwise = Return (merge subexpr)
  where subexpr = [(((iname',arity'),arg),k) 
                 | (((iname',arity'),arg),k) <- mapping, iname'==iname]
        merge [x,y] = Applic False (comparison spec) [to_ref x, to_ref y]
        merge (a:b:c:d) = Applic False (conjunction spec) 
          [Applic False (comparison spec) [to_ref a, to_ref b], merge (c:d)]
        to_ref x = Reference [to_var x]
