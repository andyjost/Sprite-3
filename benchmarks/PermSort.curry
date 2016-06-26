
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
  N   KICS2   SPRITE*   SPRITE
  10  0.20    0.06      0.04
  11  0.54    0.38      0.11
  12  1.57    3.25      0.32
  13  4.81   36.79      0.92
  14 14.63       -      2.75
  15 42.72*   3841.     8.15*

  SPRITE* = prior to copy-on-write fingerprints
  SPRITE = with copy-on-write fingerprints.

  - Note: omitted data for SPRITE* were never collected.
  - Two values marked * were spliced in from the benchmark data taken in a
    separate run.  Same machine, different date.
-}
