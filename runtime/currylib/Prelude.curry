----------------------------------------------------------------------------
--- The standard prelude of Curry.
--- All top-level functions defined in this module
--- are always available in any Curry program.
----------------------------------------------------------------------------

module Prelude where

-- Lines beginning with "--++" are part of the prelude
-- but cannot parsed by the compiler

-- Infix operator declarations:


infixl 9 !!
infixr 9 .
infixl 7 *, `div`, `mod`, `quot`, `rem`
infixl 6 +, -
-- infixr 5 :                          -- declared together with list
infixr 5 ++
infix  4 =:=, ==, /=, <, >, <=, >=, =:<=, =:<<=
infix  4  `elem`, `notElem`
infixr 3 &&
infixr 2 ||
infixl 1 >>, >>=
infixr 0 $, $!, $!!, $#, $##, `seq`, &, &>, ?


-- externally defined types for numbers and characters
data Int
data Float
data Char


type String = [Char]

-- Some standard combinators:

--- Function composition.
(.)   :: (b -> c) -> (a -> b) -> (a -> c)
f . g = \x -> f (g x)

--- Identity function.
id              :: a -> a
id x            = x

--- Constant function.
const           :: a -> _ -> a
const x _       = x

--- Converts an uncurried function to a curried function.
curry           :: ((a,b) -> c) -> a -> b -> c
curry f a b     =  f (a,b)

--- Converts an curried function to a function on pairs.
uncurry         :: (a -> b -> c) -> (a,b) -> c
uncurry f (a,b) = f a b

--- (flip f) is identical to f but with the order of arguments reversed.
flip            :: (a -> b -> c) -> b -> a -> c
flip  f x y     = f y x

--- Repeats application of a function until a predicate holds.
until          :: (a -> Bool) -> (a -> a) -> a -> a
until p f x     = if p x then x else until p f (f x)

--- Evaluates the first argument to head normal form (which could also
--- be a free variable) and returns the second argument.
seq     :: _ -> a -> a
seq external

--- Evaluates the argument to head normal form and returns it.
--- Suspends until the result is bound to a non-variable term.
ensureNotFree :: a -> a
ensureNotFree external

--- Evaluates the argument to spine form and returns it.
--- Suspends until the result is bound to a non-variable spine.
ensureSpine :: [a] -> [a]
ensureSpine l = ensureList (ensureNotFree l)
 where ensureList []     = []
       ensureList (x:xs) = x : ensureSpine xs

--- Right-associative application.
($)     :: (a -> b) -> a -> b
f $ x   = f x

--- Right-associative application with strict evaluation of its argument
--- to head normal form.
($!)    :: (a -> b) -> a -> b
f $! x  = x `seq` f x

--- Right-associative application with strict evaluation of its argument
--- to normal form.
($!!)   :: (a -> b) -> a -> b
f $!! x | x=:=y = f y  where y free

--- Right-associative application with strict evaluation of its argument
--- to a non-variable term.
($#)    :: (a -> b) -> a -> b
f $# x  = f $! (ensureNotFree x)

--- Right-associative application with strict evaluation of its argument
--- to ground normal form.
($##)   :: (a -> b) -> a -> b
f $## x | x=:=y = y==y `seq` f y  where y free

--- Aborts the execution with an error message.
error :: String -> _
error x = prim_error $## x

prim_error    :: String -> _
prim_error external

--- A non-reducible polymorphic function.
--- It is useful to express a failure in a search branch of the execution.
--- It could be defined by: `failed = head []`
failed :: _
failed external


-- Boolean values
-- already defined as builtin, since it is required for if-then-else
data Bool = False | True

--- Sequential conjunction on Booleans.
(&&)            :: Bool -> Bool -> Bool
True  && x      = x
False && _      = False


--- Sequential disjunction on Booleans.
(||)            :: Bool -> Bool -> Bool
True  || _      = True
False || x      = x


--- Negation on Booleans.
not             :: Bool -> Bool
not True        = False
not False       = True

--- Useful name for the last condition in a sequence of conditional equations.
otherwise       :: Bool
otherwise       = True


--- The standard conditional. It suspends if the condition is a free variable.
if_then_else           :: Bool -> a -> a -> a
if_then_else b t f = case b of True  -> t
                               False -> f

--- Equality on finite ground data terms.
(==)            :: a -> a -> Bool
(==) external

--- Disequality.
(/=)            :: a -> a -> Bool
x /= y          = not (x==y)

--- Ordering type. Useful as a result of comparison functions.
data Ordering = LT | EQ | GT

--- Comparison of arbitrary ground data terms.
--- Data constructors are compared in the order of their definition
--- in the datatype declarations and recursively in the arguments.
compare :: a -> a -> Ordering
compare external

--- Less-than on ground data terms.
(<)   :: a -> a -> Bool
x < y = case compare x y of LT -> True
                            _  -> False

--- Greater-than on ground data terms.
(>)   :: a -> a -> Bool
x > y = case compare x y of GT -> True
                            _  -> False

--- Less-or-equal on ground data terms.
(<=)  :: a -> a -> Bool
x <= y = not (x > y)

--- Greater-or-equal on ground data terms.
(>=)  :: a -> a -> Bool
x >= y = not (x < y)

--- Maximum of ground data terms.
max :: a -> a -> a
max x y = if x >= y then x else y

--- Minimum of ground data terms.
min :: a -> a -> a
min x y = if x <= y then x else y


-- Pairs

--++ data (a,b) = (a,b)

--- Selects the first component of a pair.
fst             :: (a,_) -> a
fst (x,_)       = x

--- Selects the second component of a pair.
snd             :: (_,b) -> b
snd (_,y)       = y


-- Unit type
--++ data () = ()


-- Lists

--++ data [a] = [] | a : [a]

--- Computes the first element of a list.
head            :: [a] -> a
head (x:_)      = x

--- Computes the remaining elements of a list.
tail            :: [a] -> [a]
tail (_:xs)     = xs

--- Is a list empty?
null            :: [_] -> Bool
null []         = True
null (_:_)      = False

--- Concatenates two lists.
--- Since it is flexible, it could be also used to split a list
--- into two sublists etc.
(++)            :: [a] -> [a] -> [a]
[]     ++ ys    = ys
(x:xs) ++ ys    = x : xs++ys

--- Computes the length of a list.
length          :: [_] -> Int
length []       = 0
length (_:xs)   = 1 + length xs

--- List index (subscript) operator, head has index 0.
(!!)            :: [a] -> Int -> a
(x:xs) !! n | n==0      = x
            | n>0       = xs !! (n-1)

--- Map a function on all elements of a list.
map             :: (a->b) -> [a] -> [b]
map _ []        = []
map f (x:xs)    = f x : map f xs

--- Accumulates all list elements by applying a binary operator from
--- left to right. Thus,
---
---     foldl f z [x1,x2,...,xn] = (...((z `f` x1) `f` x2) ...) `f` xn
foldl            :: (a -> b -> a) -> a -> [b] -> a
foldl _ z []     = z
foldl f z (x:xs) = foldl f (f z x) xs

--- Accumulates a non-empty list from left to right.
foldl1           :: (a -> a -> a) -> [a] -> a
foldl1 f (x:xs)  = foldl f x xs

--- Accumulates all list elements by applying a binary operator from
--- right to left. Thus,
---
---     foldr f z [x1,x2,...,xn] = (x1 `f` (x2 `f` ... (xn `f` z)...))
foldr            :: (a->b->b) -> b -> [a] -> b
foldr _ z []     = z
foldr f z (x:xs) = f x (foldr f z xs)

--- Accumulates a non-empty list from right to left:
foldr1              :: (a -> a -> a) -> [a] -> a
foldr1 _ [x]        = x
foldr1 f (x1:x2:xs) = f x1 (foldr1 f (x2:xs))

--- Filters all elements satisfying a given predicate in a list.
filter            :: (a -> Bool) -> [a] -> [a]
filter _ []       = []
filter p (x:xs)   = if p x then x : filter p xs
                           else filter p xs

--- Joins two lists into one list of pairs. If one input list is shorter than
--- the other, the additional elements of the longer list are discarded.
zip               :: [a] -> [b] -> [(a,b)]
zip []     _      = []
zip (_:_)  []     = []
zip (x:xs) (y:ys) = (x,y) : zip xs ys

--- Joins three lists into one list of triples. If one input list is shorter
--- than the other, the additional elements of the longer lists are discarded.
zip3                      :: [a] -> [b] -> [c] -> [(a,b,c)]
zip3 []     _      _      = []
zip3 (_:_)  []     _      = []
zip3 (_:_)  (_:_)  []     = []
zip3 (x:xs) (y:ys) (z:zs) = (x,y,z) : zip3 xs ys zs

--- Joins two lists into one list by applying a combination function to
--- corresponding pairs of elements. Thus `zip = zipWith (,)`
zipWith                 :: (a->b->c) -> [a] -> [b] -> [c]
zipWith _ []     _      = []
zipWith _ (_:_)  []     = []
zipWith f (x:xs) (y:ys) = f x y : zipWith f xs ys

--- Joins three lists into one list by applying a combination function to
--- corresponding triples of elements. Thus `zip3 = zipWith3 (,,)`
zipWith3                        :: (a->b->c->d) -> [a] -> [b] -> [c] -> [d]
zipWith3 _ []     _      _      = []
zipWith3 _ (_:_)  []     _      = []
zipWith3 _ (_:_)  (_:_)  []     = []
zipWith3 f (x:xs) (y:ys) (z:zs) = f x y z : zipWith3 f xs ys zs

--- Transforms a list of pairs into a pair of lists.
unzip               :: [(a,b)] -> ([a],[b])
unzip []            = ([],[])
unzip ((x,y):ps)    = (x:xs,y:ys) where (xs,ys) = unzip ps

--- Transforms a list of triples into a triple of lists.
unzip3              :: [(a,b,c)] -> ([a],[b],[c])
unzip3 []           = ([],[],[])
unzip3 ((x,y,z):ts) = (x:xs,y:ys,z:zs) where (xs,ys,zs) = unzip3 ts

--- Concatenates a list of lists into one list.
concat            :: [[a]] -> [a]
concat l          = foldr (++) [] l

--- Maps a function from elements to lists and merges the result into one list.
concatMap         :: (a -> [b]) -> [a] -> [b]
concatMap f       = concat . map f

--- Infinite list of repeated applications of a function f to an element x.
--- Thus, `iterate f x = [x, f x, f (f x),...]`
iterate           :: (a -> a) -> a -> [a]
iterate f x       = x : iterate f (f x)

--- Infinite list where all elements have the same value.
--- Thus, `repeat x = [x, x, x,...]`
repeat            :: a -> [a]
repeat x          = x : repeat x

--- List of length n where all elements have the same value.
replicate         :: Int -> a -> [a]
replicate n x     = take n (repeat x)

--- Returns prefix of length n.
take              :: Int -> [a] -> [a]
take n l          = if n<=0 then [] else takep n l
   where takep _ []     = []
         takep m (x:xs) = x : take (m-1) xs

--- Returns suffix without first n elements.
drop              :: Int -> [a] -> [a]
drop n l          = if n<=0 then l else dropp n l
   where dropp _ []     = []
         dropp m (_:xs) = drop (m-1) xs

--- (splitAt n xs) is equivalent to (take n xs, drop n xs)
splitAt           :: Int -> [a] -> ([a],[a])
splitAt n l       = if n<=0 then ([],l) else splitAtp n l
   where splitAtp _ []     = ([],[])
         splitAtp m (x:xs) = let (ys,zs) = splitAt (m-1) xs in (x:ys,zs)

--- Returns longest prefix with elements satisfying a predicate.
takeWhile          :: (a -> Bool) -> [a] -> [a]
takeWhile _ []     = []
takeWhile p (x:xs) = if p x then x : takeWhile p xs else []

--- Returns suffix without takeWhile prefix.
dropWhile          :: (a -> Bool) -> [a] -> [a]
dropWhile _ []     = []
dropWhile p (x:xs) = if p x then dropWhile p xs else x:xs

--- (span p xs) is equivalent to (takeWhile p xs, dropWhile p xs)
span               :: (a -> Bool) -> [a] -> ([a],[a])
span _ []          = ([],[])
span p (x:xs)
       | p x       = let (ys,zs) = span p xs in (x:ys, zs)
       | otherwise = ([],x:xs)

--- (break p xs) is equivalent to (takeWhile (not.p) xs, dropWhile (not.p) xs).
--- Thus, it breaks a list at the first occurrence of an element satisfying p.
break              :: (a -> Bool) -> [a] -> ([a],[a])
break p            = span (not . p)

--- Breaks a string into a list of lines where a line is terminated at a
--- newline character. The resulting lines do not contain newline characters.
lines        :: String -> [String]
lines []     = []
lines (x:xs) = let (l,xs_l) = splitline (x:xs) in l : lines xs_l
 where splitline []     = ([],[])
       splitline (c:cs) = if c=='\n'
                          then ([],cs)
                          else let (ds,es) = splitline cs in (c:ds,es)

--- Concatenates a list of strings with terminating newlines.
unlines    :: [String] -> String
unlines ls = concatMap (++"\n") ls

--- Breaks a string into a list of words where the words are delimited by
--- white spaces.
words      :: String -> [String]
words s    = let s1 = dropWhile isSpace s
              in if s1=="" then []
                           else let (w,s2) = break isSpace s1
                                 in w : words s2
 where
   isSpace c = c == ' '  || c == '\t' || c == '\n' || c == '\r'

--- Concatenates a list of strings with a blank between two strings.
unwords    :: [String] -> String
unwords ws = if ws==[] then []
                       else foldr1 (\w s -> w ++ ' ':s) ws

--- Reverses the order of all elements in a list.
reverse    :: [a] -> [a]
reverse    = foldl (flip (:)) []

--- Computes the conjunction of a Boolean list.
and        :: [Bool] -> Bool
and        = foldr (&&) True

--- Computes the disjunction of a Boolean list.
or         :: [Bool] -> Bool
or         = foldr (||) False

--- Is there an element in a list satisfying a given predicate?
any        :: (a -> Bool) -> [a] -> Bool
any p      = or . map p

--- Is a given predicate satisfied by all elements in a list?
all        :: (a -> Bool) -> [a] -> Bool
all p      = and . map p

--- Element of a list?
elem       :: a -> [a] -> Bool
elem x     = any (x==)

--- Not element of a list?
notElem    :: a -> [a] -> Bool
notElem x  = all (x/=)

--- Looks up a key in an association list.
lookup            :: a -> [(a,b)] -> Maybe b
lookup _ []       = Nothing
lookup k ((x,y):xys)
      | k==x      = Just y
      | otherwise = lookup k xys

--- Generates an infinite sequence of ascending integers.
enumFrom               :: Int -> [Int]                   -- [n..]
enumFrom n             = n : enumFrom (n+1)

--- Generates an infinite sequence of integers with a particular in/decrement.
enumFromThen           :: Int -> Int -> [Int]            -- [n1,n2..]
enumFromThen n1 n2     = iterate ((n2-n1)+) n1

--- Generates a sequence of ascending integers.
enumFromTo             :: Int -> Int -> [Int]            -- [n..m]
enumFromTo n m         = if n>m then [] else n : enumFromTo (n+1) m

--- Generates a sequence of integers with a particular in/decrement.
enumFromThenTo         :: Int -> Int -> Int -> [Int]     -- [n1,n2..m]
enumFromThenTo n1 n2 m = takeWhile p (enumFromThen n1 n2)
                         where p x | n2 >= n1  = (x <= m)
                                   | otherwise = (x >= m)


--- Converts a character into its ASCII value.
ord :: Char -> Int
ord c = prim_ord $# c

prim_ord :: Char -> Int
prim_ord external

--- Converts an ASCII value into a character.
chr :: Int -> Char
chr n = prim_chr $# n

prim_chr :: Int -> Char
prim_chr external


-- Types of primitive arithmetic functions and predicates

--- Adds two integers.
(+)   :: Int -> Int -> Int
x + y = (prim_Int_plus $# y) $# x

prim_Int_plus :: Int -> Int -> Int
prim_Int_plus external

--- Subtracts two integers.
(-)   :: Int -> Int -> Int
x - y = (prim_Int_minus $# y) $# x

prim_Int_minus :: Int -> Int -> Int
prim_Int_minus external

--- Multiplies two integers.
(*)   :: Int -> Int -> Int
x * y = (prim_Int_times $# y) $# x

prim_Int_times :: Int -> Int -> Int
prim_Int_times external

--- Integer division. The value is the integer quotient of its arguments
--- and always truncated towards negative infinity.
--- Thus, the value of <code>13 `div` 5</code> is <code>2</code>,
--- and the value of <code>-15 `div` 4</code> is <code>-4</code>.
div   :: Int -> Int -> Int
x `div` y = (prim_Int_div $# y) $# x

prim_Int_div :: Int -> Int -> Int
prim_Int_div external

--- Integer remainder. The value is the remainder of the integer division and
--- it obeys the rule <code>x `mod` y = x - y * (x `div` y)</code>.
--- Thus, the value of <code>13 `mod` 5</code> is <code>3</code>,
--- and the value of <code>-15 `mod` 4</code> is <code>-3</code>.
mod   :: Int -> Int -> Int
x `mod` y = (prim_Int_mod $# y) $# x

prim_Int_mod :: Int -> Int -> Int
prim_Int_mod external

--- Returns an integer (quotient,remainder) pair.
--- The value is the integer quotient of its arguments
--- and always truncated towards negative infinity.
divMod :: Int -> Int -> (Int, Int)
divMod x y = (x `div` y, x `mod` y)

--- Integer division. The value is the integer quotient of its arguments
--- and always truncated towards zero.
--- Thus, the value of <code>13 `quot` 5</code> is <code>2</code>,
--- and the value of <code>-15 `quot` 4</code> is <code>-3</code>.
quot   :: Int -> Int -> Int
x `quot` y = (prim_Int_quot $# y) $# x

prim_Int_quot :: Int -> Int -> Int
prim_Int_quot external

--- Integer remainder. The value is the remainder of the integer division and
--- it obeys the rule <code>x `rem` y = x - y * (x `quot` y)</code>.
--- Thus, the value of <code>13 `rem` 5</code> is <code>3</code>,
--- and the value of <code>-15 `rem` 4</code> is <code>-3</code>.
rem   :: Int -> Int -> Int
x `rem` y = (prim_Int_rem $# y) $# x

prim_Int_rem :: Int -> Int -> Int
prim_Int_rem external

--- Returns an integer (quotient,remainder) pair.
--- The value is the integer quotient of its arguments
--- and always truncated towards zero.
quotRem :: Int -> Int -> (Int, Int)
quotRem x y = (x `quot` y, x `rem` y)

--- Unary minus. Usually written as "- e".
negate :: Int -> Int
negate x = 0 - x

--- Unary minus on Floats. Usually written as "-e".
negateFloat :: Float -> Float
negateFloat x = prim_negateFloat $# x

prim_negateFloat :: Float -> Float
prim_negateFloat external


-- Constraints
data Success -- = Success

--- The equational constraint.
--- (e1 =:= e2) is satisfiable if both sides e1 and e2 can be
--- reduced to a unifiable data term (i.e., a term without defined
--- function symbols).
(=:=)   :: a -> a -> Success
(=:=) external

--- The always satisfiable constraint.
success :: Success
success external

--- Concurrent conjunction on constraints.
--- An expression like (c1 & c2) is evaluated by evaluating
--- the constraints c1 and c2 in a concurrent manner.
(&)     :: Success -> Success -> Success
(&) external

--- Constrained expression.
--- An expression like (c &> e) is evaluated by first solving
--- constraint c and then evaluating e.
(&>)          :: Success -> a -> a
c &> x | c = x


-- Maybe type

data Maybe a = Nothing | Just a

maybe              :: b -> (a -> b) -> Maybe a -> b
maybe n _ Nothing  = n
maybe _ f (Just x) = f x


-- Either type

data Either a b = Left a | Right b

either               :: (a -> c) -> (b -> c) -> Either a b -> c
either f _ (Left x)  = f x
either _ g (Right x) = g x


-- Monadic IO

data IO _  -- conceptually: World -> (a,World)

--- Sequential composition of actions.
--- @param a - An action
--- @param fa - A function from a value into an action
--- @return An action that first performs a (yielding result r)
---         and then performs (fa r)
(>>=)             :: IO a -> (a -> IO b) -> IO b
(>>=) external

--- The empty action that directly returns its argument.
return            :: a -> IO a
return external

--- Sequential composition of actions.
--- @param a1 - An action
--- @param a2 - An action
--- @return An action that first performs a1 and then a2
(>>)              :: IO _ -> IO b        -> IO b
a >> b            = a >>= (\_ -> b)

--- The empty action that returns nothing.
done              :: IO ()
done              = return ()

--- An action that puts its character argument on standard output.
putChar           :: Char -> IO ()
putChar c = prim_putChar $# c

prim_putChar           :: Char -> IO ()
prim_putChar external

--- An action that reads a character from standard output and returns it.
getChar           :: IO Char
getChar external

--- An action that (lazily) reads a file and returns its contents.
readFile          :: String -> IO String
readFile f = prim_readFile $## f

prim_readFile          :: String -> IO String
prim_readFile external
-- for internal implementation of readFile:
prim_readFileContents          :: String -> String
prim_readFileContents external

--- An action that writes a file.
--- @param filename - The name of the file to be written.
--- @param contents - The contents to be written to the file.
writeFile         :: String -> String -> IO ()
writeFile f s = (prim_writeFile $## f) s

prim_writeFile         :: String -> String -> IO ()
prim_writeFile external

--- An action that appends a string to a file.
--- It behaves like writeFile if the file does not exist.
--- @param filename - The name of the file to be written.
--- @param contents - The contents to be appended to the file.
appendFile        :: String -> String -> IO ()
appendFile f s = (prim_appendFile $## f) s

prim_appendFile         :: String -> String -> IO ()
prim_appendFile external

--- Action to print a string on stdout.
putStr            :: String -> IO ()
putStr []         = done
putStr (c:cs)     = putChar c >> putStr cs

--- Action to print a string with a newline on stdout.
putStrLn          :: String -> IO ()
putStrLn cs       = putStr cs >> putChar '\n'

--- Action to read a line from stdin.
getLine           :: IO String
getLine           = do c <- getChar
                       if c=='\n' then return []
                                  else do cs <- getLine
                                          return (c:cs)

----------------------------------------------------------------------------
-- Error handling in the I/O monad:

--- The (abstract) type of error values.
--- Currently, it distinguishes between general IO errors,
--- user-generated errors (see 'userError'), failures and non-determinism
--- errors during IO computations. These errors can be caught by 'catch'
--- and shown by 'showError'.
--- Each error contains a string shortly explaining the error.
--- This type might be extended in the future to distinguish
--- further error situations.
data IOError
  = IOError     String -- normal IO error
  | UserError   String -- user-specified error
  | FailError   String -- failing computation
  | NondetError String -- non-deterministic computation

--- A user error value is created by providing a description of the
--- error situation as a string.
userError :: String -> IOError
userError s = UserError s

--- Raises an I/O exception with a given error value.
ioError :: IOError -> IO _
ioError err = error (showError err)

--- Shows an error values as a string.
showError :: IOError -> String
showError (IOError     s) = "i/o error: "    ++ s
showError (UserError   s) = "user error: "   ++ s
showError (FailError   s) = "fail error: "   ++ s
showError (NondetError s) = "nondet error: " ++ s

--- Catches a possible error or failure during the execution of an
--- I/O action. `(catch act errfun)` executes the I/O action
--- `act`. If an exception or failure occurs
--- during this I/O action, the function `errfun` is applied
--- to the error value.
catch :: IO a -> (IOError -> IO a) -> IO a
catch external

----------------------------------------------------------------------------

--- Converts an arbitrary term into an external string representation.
show    :: _ -> String
show x = prim_show $## x

prim_show    :: _ -> String
prim_show external

--- Converts a term into a string and prints it.
print   :: _ -> IO ()
print t = putStrLn (show t)

--- Solves a constraint as an I/O action.
--- Note: the constraint should be always solvable in a deterministic way
doSolve :: Success -> IO ()
doSolve constraint | constraint = done


-- IO monad auxiliary functions:

--- Executes a sequence of I/O actions and collects all results in a list.
sequenceIO       :: [IO a] -> IO [a]
sequenceIO []     = return []
sequenceIO (c:cs) = do x  <- c
                       xs <- sequenceIO cs
                       return (x:xs)

--- Executes a sequence of I/O actions and ignores the results.
sequenceIO_        :: [IO _] -> IO ()
sequenceIO_         = foldr (>>) done

--- Maps an I/O action function on a list of elements.
--- The results of all I/O actions are collected in a list.
mapIO             :: (a -> IO b) -> [a] -> IO [b]
mapIO f            = sequenceIO . map f

--- Maps an I/O action function on a list of elements.
--- The results of all I/O actions are ignored.
mapIO_            :: (a -> IO _) -> [a] -> IO ()
mapIO_ f           = sequenceIO_ . map f

--- Folds a list of elements using an binary I/O action and a value
--- for the empty list.
foldIO :: (a -> b -> IO a) -> a -> [b] -> IO a
foldIO _ a []      =  return a
foldIO f a (x:xs)  =  f a x >>= \fax -> foldIO f fax xs

--- Apply a pure function to the result of an I/O action.
liftIO :: (a -> b) -> IO a -> IO b
liftIO f m = m >>= return . f

--- Like `mapIO`, but with flipped arguments.
---
--- This can be useful if the definition of the function is longer
--- than those of the list, like in
---
--- forIO [1..10] $ \n -> do
---   ...
forIO :: [a] -> (a -> IO b) -> IO [b]
forIO xs f = mapIO f xs

--- Like `mapIO_`, but with flipped arguments.
---
--- This can be useful if the definition of the function is longer
--- than those of the list, like in
---
--- forIO_ [1..10] $ \n -> do
---   ...
forIO_ :: [a] -> (a -> IO b) -> IO ()
forIO_ xs f = mapIO_ f xs

--- Performs an `IO` action unless the condition is met.
unless :: Bool -> IO () -> IO ()
unless p act = if p then done else act

--- Performs an `IO` action when the condition is met.
when :: Bool -> IO () -> IO ()
when p act = if p then act else done

----------------------------------------------------------------
-- Non-determinism and free variables:

--- Non-deterministic choice _par excellence_.
--- The value of `x ? y` is either `x` or `y`.
--- @param x - The right argument.
--- @param y - The left argument.
--- @return either `x` or `y` non-deterministically.
(?)   :: a -> a -> a
x ? _ = x
_ ? y = y


--- Evaluates to a fresh free variable.
unknown :: _
unknown = let x free in x

----------------------------------------------------------------
--- Identity function used by the partial evaluator
--- to mark expressions to be partially evaluated.
PEVAL   :: a -> a
PEVAL x = x

--- Evaluates the argument to normal form and returns it.
normalForm :: a -> a
normalForm x | x =:= y = y where y free

--- Evaluates the argument to ground normal form and returns it.
--- Suspends as long as the normal form of the argument is not ground.
groundNormalForm :: a -> a
groundNormalForm x | y == y = y where y = normalForm x

-- Only for internal use:
-- Represenation of higher-order applications in FlatCurry.
apply :: (a -> b) -> a -> b
apply external

-- Only for internal use:
-- Representation of conditional rules in FlatCurry.
cond :: Success -> a -> a
cond external

-- Only for internal use:
-- letrec ones (1:ones) -> bind ones to (1:ones)
letrec :: a -> a -> Success
letrec external

--- Non-strict equational constraint. Used to implement functional patterns.
(=:<=) :: a -> a -> Success
(=:<=) external

--- Non-strict equational constraint for linear functional patterns.
--- Thus, it must be ensured that the first argument is always (after evalutation
--- by narrowing) a linear pattern. Experimental.
(=:<<=) :: a -> a -> Success
(=:<<=) external

--- internal function to implement =:<=
ifVar :: _ -> a -> a -> a
ifVar external

--- internal operation to implement failure reporting
failure :: _ -> _ -> _
failure external

-- the end of the standard prelude
