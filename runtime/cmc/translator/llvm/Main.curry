import System
import Time
import IO
import ReadShowTerm
import ICurry
import Block
import ICurryToCpp

main = do
  args <- getArgs
  foldr ((>>) . process_file) (return ()) args

process_file file = do 
  icur_handle <- openFile (file ++ ".icur") ReadMode
  icur_contents <- hGetContents icur_handle
  -- putStrLn (show contents)  

  let icurry = readQTerm icur_contents
  -- putStrLn (show icurry)

  time <- getLocalTime
  let timeStamp = toTimeString time ++ " " ++ toDayString time

--  let hpp = ICurryToCpp.icurryToHpp icurry timeStamp
--  -- putStrLn (show hpp)
--  hpp_handle <- openFile (file ++ ".hpp") WriteMode
--  hPutStrLn hpp_handle (blockToString hpp)

  let cpp = ICurryToCpp.icurryToCpp icurry timeStamp
  -- putStrLn (show cpp)
  cpp_handle <- openFile (file ++ ".cpp") WriteMode
  hPutStrLn cpp_handle (blockToString cpp)

