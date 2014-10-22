module XFormat(Formattable(..), format) where

data Formattable
  = FS String
  | FI Int
  | FC Char
  | FF Float

format :: String -> [Formattable]-> String

format f l
  = case (f,l) of
      ("",[])  -> ""
      ("",x:_) -> extra_client x
      (_,_:_)  -> replace f l
      (s,[])   -> finish s
      _        -> missing_client f

replace s (x:xs)
  = prefix ++ replacement ++ format suffix xs
  where (prefix, code, suffix) = split s
        replacement = match code x

split s 
  = case s of
      "" -> ("","","")
      '%':c:rest -> ("",['%',c],rest)
      '\\':'%':rest -> let (pre,code,suf) = split rest in ('%':pre,code,suf)
      c:rest        -> let (pre,code,suf) = split rest in (c:pre,code,suf)

match code item
  = case (code,item) of
      ("%d", FI i) -> show i
      ("%s", FS s) -> s
      ("%c", FC c) -> [c]
      ("%f", FF f) -> show f
      _            -> error ("format error wrong code \"" ++ code ++ "\" for client " ++ show item)

extra_client (FI i) = error ("format client\""++show i++"\" without code")
extra_client (FS s) = error ("format client\""++s++"\" without code")
missing_client s = error ("format code \""++s++"\" without client")

finish s
  | code == "" = pre
  | otherwise = missing_client code
  where (pre,code,_) = split s

{- Unit testing

main = do
  putStr (format "Number 3 %d \n" [FI 3])
  putStr (format "String \"abc\" %s\n" [FS "abc"])
  putStr (format "Number 3 %d, string \"abc\" %s\n" [FI 3, FS "abc"])

-}
