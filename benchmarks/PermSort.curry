
insert x [] = [x]
insert x (y:ys) = x:y:ys ? y : (insert x ys)

perm [] = []
perm (x:xs) = insert x (perm xs)

sorted :: [Int] -> [Int]
sorted []       = []
sorted [x]      = [x]
sorted (x:y:ys) | x <= y = x : sorted (y:ys)

psort xs = sorted (perm xs)

sortmain n = psort (2:[n,n-1 .. 3]++[1])

main = sortmain 15

{-
  N   KICS2   SPRITE
  10  0.20    0.06
  11  0.54    0.38
  12  1.57    3.25
  15 42.93    3841.   (2672.74)

redstar:~/sprite3/sprite.main/benchmarks> \time ./a.out
[1,2,3,4,5,6,7,8,9,10,11,12,13,14,15]
1550.51user 1124.28system 44:32.74elapsed 100%CPU (0avgtext+0avgdata 634692maxresident)k
0inputs+0outputs (0major+1042342196minor)pagefaults 0swaps


-}
