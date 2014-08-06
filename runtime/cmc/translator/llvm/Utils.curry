import Char

------------------------------------------------------------------
-- Stuff

qualify mod (q,n)
  | mod == q = n
  | otherwise = q ++ "::" ++ n

