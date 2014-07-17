#pragma once
#include "sprite/backend.hpp"
#include <unordered_map>
#include "sprite/backend/support/testing.hpp"

namespace sprite { namespace compiler
{
  using namespace sprite::backend;

  enum Tag { FAIL= -4, FWD= -3, CHOICE= -2, OPER= -1, CTOR=0 };

  // =======================
  // ====== Sprite IR ======
  // =======================

  struct ir_h
  {
    type void_t = types::void_();
    type char_t = types::char_();
    type i64_t = types::int_(64);
    type bool_t = types::bool_();

    // Forward declaration.
    type node_t = types::struct_("node");

    // The type of a step-performing function (i.e., N or H).
    type stepfun_t = void_t(*node_t);

    // The type of the printexpr function.
    type printexprfun_t = void_t(*node_t, bool_t);

    /**
     * @brief Each Curry symbol has a static vtable holding its non-instance
     * data.
     */
    type vtable_t = types::struct_(
        "vtable"
      , {
            *char_t /*label*/
          , i64_t /*arity*/
          , *stepfun_t /*N*/
          , *stepfun_t /*H*/
          }
      #if 0
      , {"label", "arity", "N", "H"}
      #endif
      );

    ir_h()
    {
      /// A node is made up of a pointer-to-vtable, tag, and two data slots.
      node_t = types::struct_(
          "node"
        , {*vtable_t /*vptr*/, i64_t /*tag*/, *char_t /*slot0*/, *char_t /*slot1*/}
        #if 0
        , {"vptr", "tag", "slot0", "slot1"}
        #endif
        );
    }
  };

  // FIXME: using strings to index via dot and arrow would be more convenient.
  namespace member_labels
  {
    enum VtMember { VT_LABEL, VT_ARITY, VT_N, VT_H };
    enum NdMember { ND_VPTR, ND_TAG, ND_SLOT0, ND_SLOT1 };
  }

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
    ModuleSTab(LibrarySTab &, curry::Module const &);

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

    // A function for printing expressions.
    sprite::backend::function printexpr = nullptr;
  };

  struct LibrarySTab
  {
    // The module information.
    std::unordered_map<std::string, ModuleSTab> modules;
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
    ModuleCompiler(compiler::LibrarySTab & lib_stab_)
      : lib_stab(lib_stab_)
    {}

    // ==================
    // ====== Data ======
    // ==================

    // The symbol table to be updated.
    compiler::LibrarySTab & lib_stab;

    // The C library, in the target program.
    sprite::backend::testing::clib_h clib;

    // The Sprite IR library, in the target program.
    compiler::ir_h ir;

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

  /// Compiles the source for a Curry library.  Updates the symbol table.
  void compile(
      sprite::curry::Library const & lib
    , sprite::compiler::LibrarySTab & stab
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
  /// Calls the N or H function of the given node.
  value invokeNH(value const & root_p, member_labels::VtMember mem);
  value invokeNH(
      value const & root_p, member_labels::VtMember mem, attribute attr
    );
  //@}
}}

