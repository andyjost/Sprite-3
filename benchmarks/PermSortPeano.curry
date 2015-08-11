-- Permutation sort with Peano numbers and user-defined lists

data MyBool = MyTrue | MyFalse

ifThenElse :: MyBool -> a -> a -> a
ifThenElse MyTrue  x _ = x
ifThenElse MyFalse _ y = y

guard :: MyBool -> a -> a
guard MyTrue x = x


data Nat = O | S Nat

dec (S x) = x

leq O     _     = MyTrue
leq (S _) O     = MyFalse
leq (S x) (S y) = leq x y

isNat O = MyTrue
isNat (S x) = isNat x

add O n = n
add (S x) y = S (add x y)

double x = add x x

mult O _ = O
mult (S x) y = add y (mult x y)

two = S (S O)
three = S two
four = double two
nat14 = add two (mult three four)
nat15 = S nat14

----------------------------------------------------------------
data List a = Nil | Cons a (List a)

app Nil ys = ys
app (Cons x xs) ys = Cons x (app xs ys)

insert x Nil = Cons x Nil
insert x (Cons y ys) = (Cons x (Cons y ys)) ? (Cons y (insert x ys))

perm Nil = Nil
perm (Cons x xs) = insert x (perm xs)

sorted :: List Nat  -> List Nat
sorted Nil       = Nil
sorted (Cons x Nil) = Cons x Nil
sorted (Cons x (Cons y ys)) = guard (leq x y) (Cons x (sorted (Cons y ys)))

psort xs = sorted (perm xs)

descList up low =
  ifThenElse (leq low up) (Cons up (descList (dec up) low)) Nil

sortmain n = psort (Cons two (app (descList n three) (Cons (S O) Nil)))

--main = sortmain nat14
main = sortmain nat15

{-
redstar:~/sprite3/sprite.main/benchmarks> scc PermSortPeano
redstar:~/sprite3/sprite.main/benchmarks> \time ./a.out
(Cons (S O) (Cons (S (S O)) (Cons (S (S (S O))) (Cons (S (S (S (S O)))) (Cons (S (S (S (S (S O))))) (Cons (S (S (S (S (S (S O)))))) (Cons (S (S (S (S (S (S (S O))))))) (Cons (S (S (S (S (S (S (S (S O)))))))) (Cons (S (S (S (S (S (S (S (S (S O))))))))) (Cons (S (S (S (S (S (S (S (S (S (S O)))))))))) (Cons (S (S (S (S (S (S (S (S (S (S (S O))))))))))) (Cons (S (S (S (S (S (S (S (S (S (S (S (S O)))))))))))) (Cons (S (S (S (S (S (S (S (S (S (S (S (S (S O))))))))))))) (Cons (S (S (S (S (S (S (S (S (S (S (S (S (S (S O)))))))))))))) (Cons (S (S (S (S (S (S (S (S (S (S (S (S (S (S (S O))))))))))))))) Nil)))))))))))))))
170.70user 13.23system 3:03.78elapsed 100%CPU (0avgtext+0avgdata 1075068maxresident)k
0inputs+0outputs (0major+15388661minor)pagefaults 0swaps

-}

