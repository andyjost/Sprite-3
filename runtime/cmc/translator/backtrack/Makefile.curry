import IO
import System
import List
import ICurry

makefile file (IModule _ imported _ funct_list) = do
  handle <- openFile (file ++ ".make") WriteMode
  hPutStrLn handle "B"
  let isMain = find funct_list
  if isMain 
    then hPutStrLn handle "M"
    else done
  currypath <- System.getEnviron "CURRYPATH"
  do_all_path handle currypath
  foldr ((>>) . (do_imported handle)) done imported
  hPutStrLn handle "E"

find [] = False
find (IFunction (_,name) _ _ _ : xs) = name == "main" || find xs

do_imported handle name = hPutStrLn handle ("I " ++ name)

do_all_path handle currypath = do
  let elems = split (== ':') currypath
  foldr ((>>) . (do_path handle)) done elems
do_path handle elem = hPutStrLn handle ("C " ++ elem)
