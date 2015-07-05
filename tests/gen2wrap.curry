data A = A | B
data X = X A
main = cond (x=:=A) (X x) where x free
