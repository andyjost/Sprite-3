-- Benchmark to compare BFS vs. IDS

-- return nondeterministically all numbers starting from a given value
f n = f (n+1) ? n

main | f 0 == 25000 = success

