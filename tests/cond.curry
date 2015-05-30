test0 = cond (aux i) i where
    i free
    aux a | a == b = success where b free

test1 = cond (aux i) i where
    i free
    aux a = a =:= b where b free

test2 = cond (aux i) i where
    i free
    aux a = a =:= a

test3 = cond (aux i) i where
    i free
    aux a = a =:= 1

test4 = let _ = aux i in i where
    i free
    aux a | a =:= 1 = 42

test5 = let x = aux i in x where
    i free
    aux a | a =:= 1 = 42

main = [test0, test1, test2, test3, test4, test5]
