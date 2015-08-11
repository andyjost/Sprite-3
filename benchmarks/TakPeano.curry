data MyBool = MyTrue | MyFalse

ifThenElse :: MyBool -> a -> a -> a
ifThenElse MyTrue  x _ = x
ifThenElse MyFalse _ y = y

data Nat = O | S Nat

add O     y = y
add (S x) y = S (add x y)

double x = add x x

dec (S x) = x

leq O     _     = MyTrue
leq (S _) O     = MyFalse
leq (S x) (S y) = leq x y

tak :: Nat -> Nat -> Nat -> Nat
tak x y z = ifThenElse (leq x y)
                       z
                       (tak (tak (dec x) y z)
                            (tak (dec y) z x)
                            (tak (dec z) x y))

two = S (S O)
four = double two
n8 = double four
n16 = double n8
n24 = add n8 n16
n27 = add (S two) n24


goal0 = tak n24 n16 n8
goal1 = tak n27 n16 n8
--goal2 = tak 33 17 8

main = goal1
