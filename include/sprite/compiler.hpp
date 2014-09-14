#pragma once
#include "sprite/backend.hpp"
#include "sprite/backend/support/testing.hpp"
#include "sprite/curryinput.hpp"
#include "sprite/runtime.hpp"
#include <unordered_map>
#include <iterator>

namespace sprite { namespace compiler
{
  using namespace sprite::backend;

  enum Tag { FAIL= -4, FWD= -3, CHOICE= -2, OPER= -1, CTOR=0 };
  enum { TAGOFFSET = 4 };

  // ===========================
  // ====== Symbol tables ======
  // ===========================

  /**
   * @brief Node symbol table.
   */
  struct NodeSTab
  {
    NodeSTab(
        curry::Constructor const & source_
      , sprite::backend::globalvar const & vtable_
      , int32_t tag_
      )
      : source(&source_), vtable(vtable_), tag(tag_)
    {}

    NodeSTab(
        curry::Function const & source_
      , sprite::backend::globalvar const & vtable_
      , int32_t tag_
      )
      : source(reinterpret_cast<curry::Constructor const *>(&source_))
      , vtable(vtable_), tag(tag_)
    {}

    // Disabled because globalvar is a reference.
    NodeSTab & operator=(NodeSTab const &) = delete;

    // Information about the source declaration for this node.  Note: this may
    // be a Constructor or Function.  The structs are layout-compatible in the
    // head, so using a reinterpret_cast may be safe (depending on the value of
    // the tag).
    curry::Constructor const * source;

    // The vtable for this node, in LLVM IR.
    sprite::backend::globalvar vtable;

    // The tag associated with the node.  Note: tag==OPER indicates a function;
    // other tag values (i.e., tag>=CTOR) indicate constructors.
    int32_t tag;
  };

  struct LibrarySTab;
  struct ModuleCompiler;

  /// Module symbol table.
  struct ModuleSTab
  {
    // Initialize while creating a new LLVM module for holding the IR.
    ModuleSTab(LibrarySTab const &, curry::Module const &, llvm::LLVMContext &);

    // Initialize from an existing LLVM module.
    ModuleSTab(
        LibrarySTab const &, curry::Module const &
      , sprite::backend::module const & M
      );

    // Disabled because globalvar is a reference (see NodeSTab).
    ModuleSTab & operator=(ModuleSTab const &) = delete;

    // Information about the source declaration for this module.
    curry::Module const * source;

    // The module, in LLVM IR.
    sprite::backend::module module_ir;

    // The node information.
    std::unordered_map<std::string, NodeSTab> nodes;

    // Everything needed to compile code in this module.
    std::shared_ptr<ModuleCompiler> compiler;
  };

  struct LibrarySTab
  {
    // The module information.
    std::unordered_map<std::string, ModuleSTab> modules;

    /// Move the contents of another LibrarySTab into this one.
    LibrarySTab & merge_from(LibrarySTab & arg)
    {
      if(modules.empty())
        modules = std::move(arg.modules);
      else
      {
        for(auto && item: arg.modules)
          modules.emplace(std::move(item));
        arg.modules.clear();
      }
      return *this;
    }
  };

  /**
   * @brief Contains module-specific data used by the compiler.
   *
   * This object serves as a single place to hold data the compiler needs while
   * compiling a module.  These data include the IR definitions (and any other
   * headers) included in target program, and a reference to the library symbol
   * table.
   */
  struct ModuleCompiler
  {
    ModuleCompiler(compiler::LibrarySTab const & lib_stab_)
      : lib_stab(lib_stab_), rt(ir)
    {}

    // ==================
    // ====== Data ======
    // ==================

    // The symbol table to use for looking up symbols.
    compiler::LibrarySTab const & lib_stab;

    // The C library, in the target program.
    sprite::backend::testing::clib_h clib;

    // The Sprite IR library, in the target program.
    compiler::ir_h ir;

    // The Sprite runtime library, in the target program.
    compiler::rt_h rt;

    // ========================
    // ====== Algorithms ======
    // ========================

    /// Allocates storage for a node.  The return type is i8*.
    value node_alloc() const
      { return this->clib.malloc(sizeof_(this->ir.node_t)); }

    /// Initializes a node.
    void node_init(
        value node_p, compiler::NodeSTab const & node_stab
      ) const
    {
      using namespace member_labels;
      node_p.arrow(ND_VPTR) = bitcast(&node_stab.vtable, *this->ir.vtable_t);
      node_p.arrow(ND_TAG) = node_stab.tag;
    }
  };

  // ==============================
  // ====== Module functions ======
  // ==============================

  /// Prints a module.
  void prettyprint(sprite::curry::Library const & lib);

  /**
   * @brief Updates the symbol table with the result of compiling the given
   * Curry library.
   *
   * If the symbol table already contains IR for any given module (e.g.,
   * because it was previously loaded from a .bc file), then the symbol tables
   * are updated without actually compiling any code.
   */
  void compile(
      sprite::curry::Library const & lib
    , sprite::compiler::LibrarySTab & stab
    , llvm::LLVMContext & context
    );

  /// Constructs an expression at the given node address.
  // FIXME: document parameters.
  // Returns the root of the expression as a node_t*.
  value construct(
      ModuleCompiler const & compiler
    , value const & root_p
    , sprite::curry::Rule const & expr
    );

  //@{
  /// Calls the virtual function of the given node.
  value vinvoke(value const & root_p, member_labels::VtMember mem);
  value vinvoke(
      value const & root_p, member_labels::VtMember mem, attribute attr
    );
  //@}

  /// Gets the vtable pointer from a node pointer.  Skips FWD nodes.
  value get_vtable(value node_p);
}}

