
last1 l | xs++[x] =:= l = x where xs,x free
last2 (_++[x]) = x
last3 l | xs++[x] =:<= l = x where xs,x free

