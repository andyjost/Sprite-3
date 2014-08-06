-- Convert FlatCurry to ICurry
-- Sergio Antoy
-- Mon Apr 28 10:51:09 PDT 2014

import System
import IO
import FlatCurry
import FlatCurryRead


import ICurry
import TypeTable
import VarTable
import CountRef
import PPFlat
import FlatToICurry
import PPICurry
import FixCount
import RemoveInnerCases
import RemoveInnerLets

-- The number of references is particularly interesting when a
-- variable is never referenced, since then the variable can be
-- ignored.
-- 
-- If the path of a variable y is defined as relative to the path of
-- another variable x, then if x has zero references and y references
-- only x, then x too has zero references.


main = do
  args <- getArgs
  foldr ((>>) . process_file) (return ()) args

process_file file = do 
  flat <- readFlatCurry file
  -- putStrLn "--------- FLAT INITIAL ---------"
  -- putStrLn (PPFlat.execute flat)

  -- Find constructors used by the program, but defined in other modules.
  modules <- readFlatCurryIntWithImports file
  let type_table = TypeTable.execute modules
  -- putStrLn (ppTypeTable type_table)

  -- Replace case statements that are arguments of an application
  -- with new functions.
  let non_inner_cases = RemoveInnerCases.execute flat
  -- putStrLn "--------- NO INNER CASES ---------"
  -- putStrLn (PPFlat.execute non_inner_cases)

  -- Replace let-blocks that are arguments of an application
  -- with new functions.
  let non_inner_lets = RemoveInnerLets.execute non_inner_cases
  -- putStrLn "--------- NO INNER LETS ---------"
  -- putStrLn (PPFlat.execute non_inner_lets)

  -- construct a table of the variables used by each operation
  let prelim_var_tables = VarTable.execute non_inner_lets
  -- putStrLn "--------- PRELIM VAR TABLE ---------"
  -- putStrLn (show prelim_var_tables)

  -- Update the variable table with a counter of the number of
  -- references to each variable
  let var_tables = CountRef.execute prelim_var_tables non_inner_lets
  -- putStrLn "--------- VAR TABLE ---------"
  -- putStrLn (show var_tables)

{-  NO LONGER
  -- Remove Let and Free blocks
  let norm_flat = Norm.execute non_inner_lets
  -- putStrLn "--------- FLAT NORMAL ---------"
  -- putStrLn (PPFlat.execute norm_flat)
-}

  let icurry = FlatToICurry.execute type_table var_tables non_inner_lets
  let n = FixCount.execute icurry
  -- force the execution of FixCount
  icurry_handle <- seq n (openFile (file ++ ".icur") WriteMode)
  hPutStr icurry_handle (show icurry)
  hClose icurry_handle

  -- putStrLn "--------- ICURRY ---------"
  -- putStrLn (show icurry)
  -- putStrLn (PPICurry.execute icurry)

  read_handle <- openFile (file ++ ".read") WriteMode
  hPutStrLn read_handle (PPICurry.execute icurry)
  hClose read_handle
