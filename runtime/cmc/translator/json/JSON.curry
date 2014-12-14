------------------------------------------------------------------------------
--- Library for ECMA-404, the JSON Data Interchange Standard.
--- The syntax is defined at: http://www.json.org/
---
--- This library defines a datatype, Json, for JSON formatted data
--- and a few primitives to pretty print instances of this datatype.
---
--- Text, color-coded, and custom printing is available.
--- Custom printing decorations rely on a 4-tuple of strings 
--- (reset, keyword, array, number) where each component is an
--- ascii escape sequence, see http://ascii-table.com/ansi-escape-sequences.php .
---
--- reset => reset the graphic mode, e.g., "\027[m",
--- keyword => set the mode for keywords, e.g., "\027[1;34m",
--- array => set the mode for "[" and "]",
--- number => set the mode for numbers.
---
--- @author Sergio Antoy
--- @version April 2014 
--- @date Thu Apr 24 12:44:17 PDT 2014
--- @date Sat Dec 13 10:50:19 PST 2014
------------------------------------------------------------------------------

module JSON(Json(..),ppJsonColor,ppJsonText,ppJson) where

-- ----------------------------------------------------------------

-- JSON data definition: http://www.json.org/
data Json
  = JS String
  | JN Float
  | JO [(String, Json)]
  | JA [Json]
  | JTrue
  | JFalse
  | JNull

-- ----------------------------------------------------------------

-- http://ascii-table.com/ansi-escape-sequences.php

mode_text = (x, x, x, x) where x = ""
mode_color = ("\027[m", "\027[1;34m", "\027[1;31m", "\027[1;35m")
reset   (x,_,_,_) = x
keyword (_,x,_,_) = x
array   (_,_,x,_) = x
number  (_,_,_,x) = x

-- ----------------------------------------------------------------

--- Print using color decoration
ppJsonColor x = ppJson mode_color 0 x ++ "\n"
--- Print in plain text
ppJsonText x = ppJson mode_text 0 x ++ "\n"

-- ----------------------------------------------------------------
--- Print with passed in decoration
ppJson _ _ (JS jstring) = "\"" ++ jstring ++ "\" "

ppJson mode _ (JN jnumber)
  = number mode ++ adjust (show jnumber) ++ reset mode ++ " "
  -- there must be a better way, since: adjust (x ++ ".0") = x
  where adjust x = case reverse x of
                    '0':'.':y -> reverse y
                    _         -> x

ppJson mode n (JO pair_list)
  = "{ " ++ ppPairList mode (n+1) pair_list ++ ppIndent n ++ "} "

ppPairList _ _ [] = ""     -- an empty pair list should be rare
ppPairList mode n [x] = ppPair mode n x
ppPairList mode n (x: a@(_:_)) 
  = ppPair mode n x ++ ", " ++ ppPairList mode n a

ppPair mode n (string, value)
  = ppIndent n
      ++ "\"" ++ keyword mode ++ string ++ reset mode ++ "\" : "
      ++ ppJson mode n value

ppJson mode _ (JA [])        -- exception for empty arrays
  = array mode ++ "[]" ++ reset mode
ppJson mode n (JA a@(_:_))
  = array mode ++ "[ " ++ reset mode
      ++ ppIndent (n+1)
      ++ ppArray mode (n+1) a
      ++ ppIndent n
      ++ array mode ++ "] " ++ reset mode

ppArray mode n [x] = ppJson mode n x
ppArray mode n (x: a@(_:_)) 
  = ppJson mode n x ++ ", " ++ ppIndent n ++ ppArray mode n a

ppJson _ _ JTrue  = "true"
ppJson _ _ JFalse = "false"
ppJson _ _ JNull  = "null"

ppIndent n = "\n" ++ take (2*n) (repeat ' ')

------------------------------------------------------------------
-- Unit testing

-- represent a 17 cents change 

{-

test1 = JO [("tuple_change", 
           JO [("pennies",JN 2),
	       ("nickels",JN 1),
	       ("dimes",JN 1),
	       ("quarters",JN 0)])]
test2 = JO [("array_change",
      	   JA [JO [("coin",JS "penny")],
	       JO [("coin",JS "penny")],
	       JO [("coin",JS "nickel")],
	       JO [("coin",JS "dime")]])]
test3 = JA [JN 3.0, JN 3.14]

main 
  = do putStrLn (ppJsonColor test1)
       putStrLn (ppJsonColor test2)
       putStrLn (ppJsonColor test3)

-}
