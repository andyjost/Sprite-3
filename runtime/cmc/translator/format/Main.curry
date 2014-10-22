import XFormat

main = do
  putStr (format "Number 3 %d \n" [FI 3])
  putStr (format "String \"abc\" %s (unquoted)\n" [FS "abc"])
  putStr (format "Number 3 %d, string \"abc\" %s (unquoted)\n" [FI 3, FS "abc"])
