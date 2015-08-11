-- Curry benchmark: naive reverse of a user-defined list

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

data MyList a = Cons a (MyList a) | Nil

data MyBool = MyTrue | MyFalse

append Nil xs = xs
append (Cons x xs) ys = Cons x (append xs ys)

rev Nil = Nil
rev (Cons x xs) = append (rev xs) (Cons x Nil)

natList O = Nil
natList (S x) = Cons (S x) (natList x)

isList Nil = MyTrue
isList (Cons _ xs) = isList xs

goal0 = rev (Cons MyTrue (Cons MyFalse (Cons MyFalse Nil)))

goal1 = rev (natList nat16)
goal2 = rev (natList nat256)
goal3 = isList (rev (natList nat4096))  -- 8.394.753 rev. steps
goal4 = isList (rev (natList nat16384)) -- 134.242.305 rev. steps

main = goal3
