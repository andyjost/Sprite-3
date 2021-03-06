-- The FlatCurry versions are the same

t1 =
  let x = let y = 2:z
              z = 3:y
          in zip y z
  in (x,x)

t2 = 
  let y = 2:z
      z = 3:y
      x = zip y  z
  in (x,x)

few t = let (a,b) = t in (take 2 a, take 2 b)

main = (few t1, few t2)


------ CORRECT ANSWER BELOW GENERATED BY cytest.py using pakcs ------
--> (([(2,3),(3,2)],[(2,3),(3,2)]),([(2,3),(3,2)],[(2,3),(3,2)]))
--$?-> 0
