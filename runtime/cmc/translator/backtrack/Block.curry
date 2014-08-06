module Block(Block(..),blockToString) where

import List

data Block
  = Block Int [Block]
  | SLine String
  | NL
  | NOP

blockToString block = pp 0 block

pp :: Int -> Block -> String
pp k (Block n list) = foldr ( \x y -> pp (k+n) x ++ y) "" list
pp k (SLine x) = indent k ++ (fixnewline x)
pp _ NL = "\n" 
pp _ NOP = ""
indent k = "\n" ++ take (2*k) (repeat ' ')

-- Copied from Char.curry in the pakcs lib
-- TODO: I do not understand the \xa0
fixnewline [] = []
fixnewline (x:xs) =
  case x of
    '\n' -> '\\':'n' : fixnewline xs
    '\t' -> '\\':'t' : fixnewline xs
    '\r' -> '\\':'r' : fixnewline xs
    '\f' -> '\\':'f' : fixnewline xs
    '\v' -> '\\':'v' : fixnewline xs
    -- '\xa0' -> "\\xa0" ++ fixnewline xs
    _    -> x : fixnewline xs

