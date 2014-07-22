/**
 * @file
 * Compiles a simple Curry test program from a literal encoding (not from
 * source) and dumps the LLVM IR.
 */

#include "sprite/curryinput.hpp"
#include "sprite/compiler.hpp"
#include "sprite/backend/support/testing.hpp"
#include "llvm/Bitcode/ReaderWriter.h"
#include "llvm/Support/raw_ostream.h"
#include <cstdlib>
#include <iostream>

namespace tgt = sprite::backend;

sprite::curry::Library makeplain()
{
  // This little test program is based on the following Curry program:
  //
  //    data Nat = Zero | Succ Nat
  //    
  //    half Zero = Zero
  //    half (Succ Zero) = Zero
  //    half (Succ (Succ x)) = Succ (half x)
  //    
  //    data MyList a = MyNil | MyCons a (MyList a)
  //    
  //    myappend MyNil y = y
  //    myappend (MyCons x xs) y = MyCons x (myappend xs y)
  //    
  //    main = (half (Succ Zero), myappend (MyCons Zero  MyNil) MyNil)

  using namespace sprite::curry;

  DataType && MyList {"MyList", {{"MyCons", 2}, {"MyNil", 0}}};
  DataType && Nat {"Nat", {{"Zero", 0}, {"Succ", 1}}};
  Function && half {
      "half"  /* name */
    , 1       /* arity */
    , {       /* paths */
          {1, {{{"plain", "half"}, 0}}}
        , {2, {{{"plain", "half"}, 0}, {{"plain", "Succ"}, 0}}}
        , {3, {{{"plain", "half"}, 0}, {{"plain", "Succ"}, 0}, {{"plain", "Succ"}, 0}}}
        }
    , Branch { /* def */
          "", true, 1
        , {
              {{"plain", "Zero"}, Expr{{"plain", "Zero"}}}
            , {{"plain", "Succ"}
                , Branch {
                      "_1", true, 2
                    , {
                          {{"plain", "Zero"}, Expr{{"plain", "Zero"}}}
                        , {{"plain", "Succ"}, Expr{{"plain", "Succ"}, {Expr{{"plain", "half"}, {Ref{3}}}}} }
                        }
                    }
                }
            }
        }
    };
  Function && myappend {
      "myappend" /* name */
    , 2          /* arity */
    , {          /* paths */
          {1, {{{"plain", "myappend"}, 0}}}
        , {2, {{{"plain", "myappend"}, 1}}}
        , {3, {{{"plain", "myappend"}, 0}, {{"plain", "MyCons"}, 0}}}
        , {4, {{{"plain", "myappend"}, 0}, {{"plain", "MyCons"}, 1}}}
        }
    , Branch {   /* def */
          "", true, 1
        , {
              {{"plain", "MyNil"}, Ref{2}}
            , {{"plain", "MyCons"}, Expr{{"plain", "MyCons"}, {Ref{3}, Expr{{"plain", "myappend"}, {Ref{4}, Ref{2}}}}}}
            }
        }
    };

  Module && plain {
      "plain"          /* name */
    , {"Prelude"}      /* imports */
    , {MyList, Nat}    /* datatypes */
    , {half, myappend} /* functions */
    };

  Library const lib {{plain}};
  return lib;
}

int main()
{
  sprite::init_debug();

  // The symbol tables for this library.
  sprite::compiler::LibrarySTab stab;
  sprite::curry::Library lib = makeplain();
  // prettyprint(lib);
  sprite::compiler::compile(lib, stab);

  // Create a main function for testing.
  auto & module_stab = stab.modules.at("plain");
  auto & compiler = *module_stab.compiler;
  tgt::scope _ = module_stab.module_ir;

  // Declare the main function.
  tgt::extern_(
      tgt::types::int_(32)(), "main", {}
    , [&]{
        using namespace sprite::curry; // for Expr etc.
        using namespace sprite::compiler::member_labels; // for VT_N.

        tgt::value root_p = compiler.node_alloc();
        Qname const half{"plain", "half"};
        Qname const Succ{"plain", "Succ"};
        Qname const Zero{"plain", "Zero"};
        root_p = construct(compiler, root_p, {half, {{Succ, {Zero}}}});
        module_stab.printexpr(root_p, true);
        compiler.clib.printf("\n");
        sprite::compiler::vinvoke(root_p, VT_N);
        module_stab.printexpr(root_p, true);
        compiler.clib.printf("\n");

        // Qname const myappend{"plain", "myappend"};
        // Qname const MyCons{"plain", "MyCons"};
        // Qname const MyNil{"plain", "MyNil"};
        // root_p = construct(compiler, root_p
        //   , {myappend, {{MyCons, {Zero, MyNil}}, MyNil}}
        //   );
        // module_stab.printexpr(root_p, true);
        // compiler.clib.printf("\n");
        // sprite::compiler::vinvoke(root_p, VT_N);
        // module_stab.printexpr(root_p, true);
        // compiler.clib.printf("\n");

        tgt::return_(0);
      }
    );
  
  // Write a bitcode file, interpret it, and compare its output with the
  // golden.
  {
    std::string err;
    llvm::raw_fd_ostream fout("plaintest.bc", err, llvm::raw_fd_ostream::F_Binary);
    llvm::WriteBitcodeToFile(module_stab.module_ir.ptr(), fout);
  }
  std::system("lli plaintest.bc > plaintest.result");
  int const status = std::system("diff plaintest.result plaintest.au");
  return WEXITSTATUS(status);
}
