import List
import ICurry
import Block
import Format
import Utils

icurryToCpp (IModule name _ data_list funct_list) timestamp
  = Block 0 [
      SLine "#include <iostream>",
      SLine "#include \"curryinput.hpp\"",
      SLine "// #include \"compiler.hpp\"",
      NL,
      SLine "using namespace sprite::curry;",
      SLine "using std::cout;",
      SLine "using std::endl;",
      NL,
      SLine "Library makeplain()",
      SLine "{",
      Block 1 [macro_type name t | t <- data_list],
      Block 1 [macro_operation name f | f <- funct_list],
      SLine "}"
    ]

macro_type name (t,cl)
  = SLine (format "DataType && %s {\"%s\", {%s}};" [x, x, FS (show_clist name cl)])
  where x = FS (qualify name t)

show_clist name cl 
  = concat (intersperse ", " [format "{\"%s\", %d}" [FS (qualify name q), FI a]
                                 | IConstructor q a <- cl])

macro_operation name (IFunction qname arity _ rule)
 = SLine (format "// function %s" [FS (qualify name qname)])
