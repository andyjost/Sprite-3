data A = A | B
data X = X A
main = cond (x=:=A) (X x) where x free

------ CORRECT ANSWER BELOW GENERATED BY cytest.py using pakcs ------
--> X A
--$?-> 0
