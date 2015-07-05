data A = A Bool Bool Bool | B
main = cond (x=:=(A a1 a2 a3)) x where x,a1,a2,a3 free
