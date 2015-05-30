data MyList a = MyCons a (MyList a) | MyNil
main :: MyList Bool
main = cond ((MyCons True MyNil)=:=x) x where x free
