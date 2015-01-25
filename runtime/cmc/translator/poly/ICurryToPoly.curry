import ICurry
-- import Unsafe

-- Helpers.
applyC name args = Applic True ("Prelude", name) args
applyF name args = Applic False ("Prelude", name) args

-- Like ++ but for ICurry.
(++*) a b = applyF "++" [a, b]

-- Like : but for ICurry.
(:*) a b = applyC ":" [a, b]

nil = applyC "[]" []
litChar c = (BuiltinVariant (Bchar c)) :* nil

symname (name,_,_,_,_) = name
default (_,def,_,_,_) = def
conjunction (_,_,conj,_,_) = conj
comparison (_,_,_,comp,_) = comp
make_branch (_,_,_,_,mkbranch) = mkbranch

spec_equality = (equals, "True", "&&", "==", make_branch_equality)
spec_compare = (compares, "EQ", "compare_conjunction", "compare", make_branch_compare)

equals typename = "=="++"."++typename
compares typename = "compare"++"."++typename
shows typename = "show"++"."++typename
primitive x = "primitive."++x

icurryToPoly (IModule modname {-imported_list-}_ data_list {-funct_list-}_)
  = concat [
      concat [
          make_show modname onetype
        , make_binary_funct modname onetype spec_equality
        , make_binary_funct modname onetype spec_compare
        ]
      | onetype <- data_list
    ]


-- For a builtin type.
make_show modname ((_,typename),[])
  = [(IFunction (modname,symbol) 1 [(1,1,(IPath (Arg 1)))]
        [(ATable 1 False (Reference [(1,1,(IPath (Arg 1)))]) [((IConstructor (modname,typename) 1),
           [Return (Applic False (modname,primitive symbol)
               [(Reference [(1,1,(IPath (Arg 1)))])])])])]),
     (IFunction (modname,primitive symbol) 1 [] [(IExternal (modname++"."++primitive symbol))])]
  where symbol = shows typename

-- For a user-defined type.
make_show modname ((_,typename),clist@(_:_))
  = [(IFunction (modname,symbol) 1 var_list (make_table1 clist))]
  where symbol = shows typename
        var_list = [(1,1,(IPath (Arg 1)))] ++ mapping
        mapping = map to_var (map_variables1 clist)

-- The constructor of a builtin type has arity 1, the builtin value it hosts.
make_binary_funct modname ((_,typename),[]) spec
  = [(IFunction (modname,symbol) 2 [(1,1,(IPath (Arg 1))),(2,1,(IPath (Arg 2)))] 
        [(ATable 1 False (Reference [(1,1,(IPath (Arg 1)))]) [((IConstructor (modname,typename) 1),
           [(ATable 2 False (Reference [(2,1,(IPath (Arg 2)))]) [((IConstructor (modname,typename) 1),
              [Return (Applic False (modname,primitive ((symname spec) typename))
                     [(Reference [(1,1,(IPath (Arg 1)))]),(Reference [(2,1,(IPath (Arg 2)))])])])])])])]),
     (IFunction (modname,primitive symbol) 2 [] [(IExternal (modname++"."++primitive symbol))])]
  where symbol = (symname spec) typename

make_binary_funct modname ((_,typename),clist@(_:_)) spec
  = [IFunction (modname, (symname spec) typename) 2 var_list (make_table2 clist spec)]
  where var_list = [(1,count,(IPath (Arg 1))),(2,count,(IPath (Arg 2)))]
                     ++ mapping
        mapping = map to_var (map_variables2 clist)
        count = length mapping `div` 2 + 1

-- make a mapping from (constructor, variable_index) pairs to integers
-- for unary functions (show).
-- start at 2
map_variables1 clist
  = zip pairs [2..]
  where pairs = [((iname,1),k) 
                   | (IConstructor iname arity) <- clist, 
                     k <- [1..arity]]

-- make a mapping from (constructor, variable_index) pairs to integers
-- for binary functions (compare and ==).
-- start at 3
map_variables2 clist
  = zip pairs [3..]
  where pairs = [((iname,arg),k) 
                   | (IConstructor iname arity) <- clist, 
                     k <- [1..arity], 
                     arg <- [1,2]]

to_var (((iname,arg),k),pos)
  = (pos,1,(IPath (Rel arg iname k))) 

to_ref x = Reference [to_var x]

make_table1 clist = [
    ATable 1 False 
    (Reference [(1,1,(IPath (Arg 1)))]) -- first argument
    [make_branch_show ctor mapping | ctor <- clist]
  ]
  where mapping = map_variables1 clist

make_branch_show ctor mapping = (ctor, [recur1 ctor mapping])

make_table2 clist spec = [
    ATable 1 False 
    (Reference [(1,1,(IPath (Arg 1)))]) -- first argument
    [make_inner k ctor clist mapping spec | (k,ctor) <- zip [2..] clist]
  ]
  where mapping = map_variables2 clist
  
make_inner k ctor clist mapping spec = (ctor, [
    ATable k False (Reference [(2,1,(IPath (Arg 2)))]) -- second argument
    [(make_branch spec) ctor arg2 clist mapping | arg2 <- clist]
  ])

make_branch_equality ctor arg2 _ mapping = (arg2, [
  if ctor == arg2 
    then (recur2 ctor mapping spec_equality)
    else Return (applyC "False" [])
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
    LT -> Return (applyC "LT" [])
    GT -> Return (applyC "GT" [])
    EQ -> (recur2 ctor mapping spec_compare)
  ])

recur1 (IConstructor iname arity) mapping
  | arity == 0 = Return show_label
  -- Special case for lists.
  | iname == ("Prelude", ":") =
        Return (applyF "prim_show_list" [Reference [(1,1,(IPath (Arg 1)))]])
  -- Special case for tuples.
  | (fst iname) == "Prelude" && head (snd iname) == '(' =
        Return $ (litChar '(') ++* (applyF "show" [to_ref $ head subexpr]) ++* merge ',' (tail subexpr) ++* (litChar ')')
  | otherwise = Return $ litChar '(' ++* show_label ++* merge ' ' subexpr ++* (litChar ')')
  where subexpr = [(((iname',arity'),arg),k) 
                 | (((iname',arity'),arg),k) <- mapping, iname'==iname]
        show_label = applyF "prim_label" [Reference [(1,1,(IPath (Arg 1)))]]
        merge d [x] = litChar d ++* applyF "show" [to_ref x]
        merge d (a:b:c) = litChar d ++* applyF "show" [to_ref a] ++* merge d (b:c)

recur2 (IConstructor iname arity) mapping spec
  | arity == 0 = Return (applyC (default spec) [])
  | otherwise = Return (merge subexpr)
  where subexpr = [(((iname',arity'),arg),k) 
                 | (((iname',arity'),arg),k) <- mapping, iname'==iname]
        merge [x,y] = applyF (comparison spec) [to_ref x, to_ref y]
        merge (a:b:c:d) = applyF (conjunction spec) 
          [applyF (comparison spec) [to_ref a, to_ref b], merge (c:d)]
