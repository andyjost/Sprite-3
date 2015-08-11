
last :: [a] -> a
last l | xs ++ [x] =:= l = x where xs,x free

goal0 = last (replicate 10000 True)
goal1 = last (replicate 100000 True)

main = goal1