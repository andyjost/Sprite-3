-- Datatype for regular expressions over an alphabet:

data RE a = Lit a
          | Alt  (RE a) (RE a)
          | Conc (RE a) (RE a)
          | Star (RE a)

-- My characters:

data Chr = A | B | C | D | E

-- Example: regular expression (a|b|c)

abc = Alt (Alt (Lit A) (Lit B)) (Lit C)

-- Example: regular expression (ab*)

abstar = Conc (Lit A) (Star (Lit B))

-- Example: regular expression (ab*c)

abstarc = Conc abstar (Lit C)

-- Extension: operator plus for regular expressions

plus re = Conc re (Star re)

-- Semantics of regular expressions

sem :: RE a -> [a]
sem (Lit c)    = [c]
sem (Alt  a b) = sem a ? sem b
sem (Conc a b) = sem a ++ sem b
sem (Star a)   = [] ? sem (Conc a (Star a))

-- Examples:

test1 = sem abc

-- -> "a" oder "b" oder "c"

test2 = sem abstar

-- -> "a" oder "ab" oder "abb" ...

-- Matching:

match :: RE a -> [a] -> Success
match r s | sem r == s = success

test3 = match (Star abc) [A,B,A,C,A]
test4 = match abstar [E]

-- grep:

grep :: RE a -> [a] -> Success
grep r s = xs ++ sem r ++ ys =:= s  where xs,ys free

-- Examples:

test5 = grep abc [D,B,E]
test6 = grep abstar [D,A,B,E]

biggrep n =
  grep abstarc (take n (concatMap (\i->A : take i (repeat B)) [1..]) ++ [A,B,C])

main = biggrep 200000
