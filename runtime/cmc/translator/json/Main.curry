import System
import IO
import ReadShowTerm
import ICurry
import PPICurry
import JSON
import ICurryToJson

main = do
  args <- getArgs
  foldr ((>>) . process_file) (return ()) args

process_file file = do 
  icur_handle <- openFile (file ++ ".icur") ReadMode
  icur_contents <- hGetContents icur_handle
  -- putStrLn (show contents)

  let code = readQTerm icur_contents
  putStrLn (PPICurry.execute code)

  let json = icurryToJson code
  json_handle <- openFile (file ++ ".json") WriteMode
  hPutStr json_handle (ppJsonColor json)

