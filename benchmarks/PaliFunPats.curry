--------------------------------------------------------------------------------
-- define a palindrome constraint with functional patterns:
pali :: [a] -> Success
pali (xs ++ reverse xs) = success
pali (xs ++ _ : reverse xs) = success

test1 = pali [True,False,True] --> success
test2 = (pali [True,False,False,True]) --> success
test3 = (pali [True,False]) --> fail

longPali n = take n (repeat True) ++ take n (repeat False) ++ [False] ++
             take n (repeat False) ++ take n (repeat True)

main = pali (longPali 100)