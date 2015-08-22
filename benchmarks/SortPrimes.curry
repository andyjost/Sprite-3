-- Taken from KiCS2 paper (2011).  If sharing is effective then
--     time(goal1) + time(goal2) ~= time(goal3)

-- PermSort --
insert x [] = [x]
insert x (y:ys) = x:y:ys ? y : (insert x ys)

perm [] = []
perm (x:xs) = insert x (perm xs)

sorted :: [Int] -> [Int]
sorted []       = []
sorted [x]      = [x]
sorted (x:y:ys) | x <= y = x : sorted (y:ys)

psort xs = sorted (perm xs)

-- Primes --
suCC :: Int -> Int
suCC x = x + 1

isdivs :: Int  -> Int -> Bool
isdivs n x = mod x n /= 0

the_filter :: [Int] -> [Int]
the_filter (n:ns) = myfilter (isdivs n) ns

primes :: [Int]
primes = mymap myhead (myiterate the_filter (myiterate suCC 2))


myfilter :: (Int -> Bool) -> [Int] -> [Int]
myfilter _ []     = []
myfilter p (x:xs) = if p x then x : myfilter p xs
                                else myfilter p xs

myiterate :: (a -> a) -> a -> [a]
myiterate f x = x : myiterate f (f x)

mymap :: (a -> b) -> [a] -> [b]
mymap _ []     = []
mymap f (x:xs) = f x : mymap f xs

myhead :: [Int] -> Int
myhead (x : _) = x

goal1 = [primes!!1003, primes!!1002, primes!!1001, primes!!1000]
goal2 = psort [7949,7937,7933,7927]
goal3 = psort [primes!!1003, primes!!1002, primes!!1001, primes!!1000]

main = goal3
