data A = A | B | C Bool Bool Bool
main = cond (a1=:=True & x=:=(C a1 a2 a3) & a3=:=False) x where x,a1,a2,a3 free
