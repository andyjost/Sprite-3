data Nat = Zero | Succ Nat

add Zero y = y
add (Succ x) y = add x (Succ y)
half (add x x) = x

two = Succ (Succ Zero)
main = half two
