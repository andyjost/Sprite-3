data A = A | B Bool | C
main = cond (a1=:=True & x=:=(B a1)) x where x,a1 free
