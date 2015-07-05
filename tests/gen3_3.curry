data A = A | B | C Bool Bool Bool
main = cond (x=:=(C a1 a2 a3)) x where x,a1,a2,a3 free
