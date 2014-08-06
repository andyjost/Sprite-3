

import System
import IO
import FlatCurry
import FlatCurryRead

import Norm
import PPFlat

main = do
  args <- getArgs
  foldr ((>>) . process_file) (return ()) args

process_file file = do 
  flat <- readFlatCurry file

  zero_handle <- openFile (file ++ ".zero") WriteMode
  hPutStrLn zero_handle (PPFlat.execute flat)
  hClose zero_handle
  
  let norm = Norm.execute flat
  norm_handle <- openFile (file ++ ".norm") WriteMode
  hPutStr norm_handle (show norm)
  hClose norm_handle

  read_handle <- openFile (file ++ ".read") WriteMode
  hPutStrLn read_handle (PPFlat.execute norm)
  hClose read_handle
