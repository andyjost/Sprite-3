-- The usual Peano numbers:
data Nat = O | S Nat

isNat O = MyTrue
isNat (S x) = isNat x

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


data List a = Nil | Cons a (List a)

data MyBool = MyTrue | MyFalse

ifThenElse :: MyBool -> a -> a -> a
ifThenElse MyTrue  x _ = x
ifThenElse MyFalse _ y = y

suCC :: Nat -> Nat
suCC x = add x (S O)

-- define "is-not-divisor" by subtraction:
nodiv :: Nat -> Nat -> MyBool
nodiv d n = nodiv_sub n d
 where
  nodiv_sub O     O     = MyFalse
  nodiv_sub (S x) O     = nodiv d (S x)
  nodiv_sub O     (S _) = MyTrue
  nodiv_sub (S x) (S m) = nodiv_sub x m

the_filter :: List Nat -> List Nat
the_filter (Cons n ns) = myfilter (nodiv n) ns

primes :: List Nat
primes = mymap myhead (myiterate the_filter (myiterate suCC (S (S O))))


myfilter :: (Nat -> MyBool) -> List Nat -> List Nat
myfilter _ Nil         = Nil
myfilter p (Cons x xs) = ifThenElse (p x) (Cons x (myfilter p xs))
                                          (myfilter p xs)

myiterate :: (a -> a) -> a -> List a
myiterate f x = Cons x (myiterate f (f x))

mymap :: (a -> b) -> List a -> List b
mymap _ Nil         = Nil
mymap f (Cons x xs) = Cons (f x) (mymap f xs)

myhead :: List a -> a
myhead (Cons x _) = x

at :: List a -> Nat -> a
at (Cons x _) O = x
at (Cons _ xs) (S n) = at xs n

mytake O _ = Nil
mytake (S n) (Cons x xs) = Cons x (mytake n xs)

goal0 = isNat (at primes nat256)
--goal1 = at primes 24001
--goal2 = at primes 54001

main = goal0
