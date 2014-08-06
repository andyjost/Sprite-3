import Char

------------------------------------------------------------------
-- Stuff

-- a Curry program variable
pvar_id i = "var_" ++ show i

isIdentChar c = c=='_' || isAlpha c || isDigit c

translate x = '_' : conv_non_id_char x

conv_non_id_char [] = []
conv_non_id_char (x:xs)
  | isIdentChar x = x : conv_non_id_char xs
  | otherwise = "_0x" ++ charToHex x ++ conv_non_id_char xs

charToHex c 
  | x < 256 = [y !! (x `div` 16), y !! (x `mod` 16)]
  | otherwise = error ("charToHex error \"" ++ show c ++ "\"")
  where x = ord c
        y = "0123456789ABCDEF"

-- Qualify starting at the outermost namespace "::"
-- to correctly reference a symbol with the same name
-- as the file in which it is defined.
-- In the C++ code, this translates into a class
-- with the same name as the namespace in which it is defined.
qualify (q,n) = "::"++translate q++"::"++translate n

suffix2string [] = ""
suffix2string (x:xs) = "_" ++ show x ++ suffix2string xs
