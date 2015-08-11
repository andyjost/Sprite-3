-- Haskell benchmark: naive reverse on buil-in lists

data Nat = O | S Nat

add O n = n
add (S x) y = S (add x y)

double x = add x x

mult O _ = O
mult (S x) y = add y (mult x y)

two = S (S O)
four = double two
nat16 = mult four four
nat256 = mult nat16 nat16
nat4096 = mult nat256 nat16
nat16384 = mult nat4096 four

data MyBool = MyTrue | MyFalse

append [] xs = xs
append (x:xs) ys = x : (append xs ys)

rev [] = []
rev (x:xs) = append (rev xs) [x]

natList O = []
natList (S x) = (S x) : (natList x)

isList [] = MyTrue
isList (_:xs) = isList xs

goal1 = rev (natList nat16)
goal2 = rev (natList nat256)
goal3 = isList (rev (natList nat4096))
goal4 = isList (rev (natList nat16384))

main = goal3
