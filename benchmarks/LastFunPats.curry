-- The classical function pattern:
last (_++[e]) = e

testlast n = last (map (const False) [1..n] ++ [True])

-- main = testlast 100000
main = testlast 100000
