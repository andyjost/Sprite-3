-- KICS2 --
g x y = case x ? y of
      []  -> 40
      [False] -> 50

main = g [] [False]


------ CORRECT ANSWER BELOW GENERATED BY cytest.py using pakcs ------
--> 40
--> 50
--$?-> 0