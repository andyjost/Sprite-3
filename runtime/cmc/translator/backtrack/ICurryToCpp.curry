import IO
import List
import SetFunctions
import ICurry
import Block
import Format
import Utils
import MacroSymbols
import MacroBodies
import MacroEqual
import MacroCompare


-- Construct an intermediate representation between
-- icurry and cpp format to simplify printing.
-- Indented printing is a big help in reading the
-- generated code and verifying its correctness.

icurryToCpp (IModule name _ data_list funct_list) timestamp
  = Block 0 [
      -- SLine (format "// %s" [FS timestamp]),
      SLine (format "#include \"%s.hpp\"" [FS name]),
      SLine "#include \"Compare.hpp\"",
      NL,
      SLine (format "namespace %s {" [FS (translate name)]),
      Block 1 (map toBody funct_list),
      Block 1 [toEqual data_list c | (_, cl) <- data_list , c <- cl],
      SLine "}",
      toMain name funct_list
    ]

icurryToHpp (IModule name imported_list data_list funct_list) timestamp
  = Block 0 [
      -- SLine (format "// %s" [FS timestamp]),
      SLine "#pragma once",
      NL,
      import_default,
      Block 0 (map toCppImported imported_list),
      NL,
      SLine (format "namespace %s {" [FS (translate name)]),
      SLine "  using namespace Engine;",
      NL,
      Block 1 (toDataList data_list),
      Block 1 (map toFunction funct_list),
      SLine "}"
    ]
  where import_default
          = Block 0 [
              SLine "#include <string>",
              SLine "#include <iostream>",
              SLine "#include \"Engine.hpp\"",
              SLine "#include \"Litint.hpp\"",
              SLine "#include \"Litchar.hpp\"",
              NL
            ]
        toCppImported imported
          = SLine (format "#include \"%s.hpp\"" [FS imported])

toDataList data_list
  = [toConstructor c | (_, cl) <- data_list, c <- zip [0..] cl]

-- TODO: skip toConstructor ???
toConstructor iconstr
  = macro_constructor iconstr

-- TODO: skip toFunction ???
toFunction (IFunction qname arity _ _)
  = macro_operation qname arity


toEqual data_list a@(IConstructor qname arity)
  = Block 0 [
      macro_equal constr_list qname arity,
      macro_compare constr_list qname arity
    ]
  where find (_ ++ [(_, b@(_ ++ [(IConstructor q _)] ++ _))] ++ _) (IConstructor q _) = b
  	constr_list = find data_list a

-- TODO: skip toBody ???
toBody (IFunction qname arity vtable expr)
  = macro_body qname arity vtable expr

toMain qual funct_list
  = if isEmpty (set1 has_main funct_list) 
    then NOP
    else create_host qual
  where has_main (_ ++ [IFunction (_,"main") 0 _ _] ++ _) = True
  
create_host qual
  = Block 0 [
      NL,
      SLine "int main() {",
      SLine (format "  Engine::Node** x = %s::_main::make();" [FS (translate qual)]),
      SLine "  do {",
      SLine "    try {",
      SLine "      Engine::nfun(x);",
      SLine "      if ((*x)->get_kind() == FAIL) {",
      SLine "        std::cout << \"F \" << std::endl;",
      SLine "      } else {",
      SLine "        std::cout << \"V \" <<  (*x)->to_s() << std::endl;",
      SLine "      }",
      SLine "    } catch (const char* msg) {",
      SLine "        std::cout << msg << std::endl;",
      SLine "        return 1;",
      SLine "    } catch (std::string &msg) {",
      SLine "        std::cout << msg << std::endl;",
      SLine "        return 1;",
      SLine "    }",
      SLine "  } while (Engine::backtrack());",
      SLine "  std::cout << \"Z \" <<  (*x)->to_s() << std::endl;",
      SLine "  return 0;",
      SLine "}"
    ]
