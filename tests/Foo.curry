-- KICS2 --
module Foo where

data Peano = O | S Peano

toPeano :: Int -> Peano
toPeano n = if n==0 then O else S (toPeano (n-1))

equal :: Peano -> Peano -> Bool
equal O O = True
equal (S p) (S q) = equal p q
equal (S _) O = False
equal O (S _) = False

add :: Peano -> Peano -> Peano
add O     p = p
add (S p) q = S (add p q)

-- half y | equal (add x x) y = x where x free 
ite True t _ = t
ite False _ f = f
half y = ite (equal (add x x) y) x failed where x free


-- main = equal (toPeano 1) (half (toPeano 2))
main = equal (S O) (half (S (S O)))

