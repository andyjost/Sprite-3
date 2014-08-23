-- Same as Flatcurry QName
type IName = (String,String)

-- Same as FlatCurry Literal
data BuiltinVariant
  = Bint   Int
  | Bfloat Float
  | Bchar  Char

data IModule 
  = IModule
    String                      -- this module name
    [String]                    -- imported modules names
    [(IName,[IConstructor])]    -- constructors grouped by type
    [IFunction]                 -- functions

data IConstructor
  = IConstructor 
    IName    -- qualified FLP name
    Int      -- arity

data IFunction
  = IFunction
    IName		-- qualified FLP name
    Int         	-- arity
    [VariableDef]       -- variable table
    [Statement]  	-- function body

-- A LHS variable is either an argument or an argument of a variable
-- In Arg j, j is the argument
-- In Rel v t j, v is the variable this variable is relative to,
-- t is an IName naming the type of v, and j the argument
data LHSvar = Arg Int | Rel Int IName Int

-- The kinds of variables in FlatCurry
data IKind = IPath LHSvar | IBind | IFree

-- variable index, reference count and kind
type VariableDef = (Int, Int, IKind)

data Expression
  = Exempt
  | Reference [VariableDef]
  | BuiltinVariant BuiltinVariant
  | Applic Bool IName [Expression]
  | PartApplic Int Expression
  -- TODO: IOr should be removed -- WHY?
  | IOr Expression Expression

-- Define a node of an expression. The expression is implied.
-- The node defined by [] is the root.
-- The node defined by [(s1,n1)...(sk,nk)] is the k-th successor
-- of the node defined by [(s1,n1)...(s_{k-1},n_{k-1}),(sk,nk)],
-- In (si,ni),  is the label of the predecessor of the defined ni successor.
type LPath = [(IName,Int)]

data Statement
  = IExternal String         -- ???
  | Comment String
  | DeclareLHSVar [VariableDef]
  | DeclareFreeVar VariableDef
  -- next 3 are for let blocks
  | Forward VariableDef
  | Assign Int Expression
  | Initialize VariableDef Expression
  -- Fill v1 p v2 means replace the node of v1 at p with v2
  -- where v1 and v2 are indexes of variables and p a path in v1.
  | Fill Int LPath Int
  | Return Expression
  | ATable Int Bool Expression [(IConstructor,[Statement])]
  | BTable Int Bool Expression [(BuiltinVariant,[Statement])]  
