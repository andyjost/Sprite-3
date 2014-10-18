#pragma once
#include "sprite/backend.hpp"

// The number of pre-defined arity functions.
#define SPRITE_PREDEF_ARITY_LIMIT 10

namespace sprite { namespace compiler
{
  using namespace sprite::backend;

  // =======================
  // ====== Sprite IR ======
  // =======================

  // LLVM declarations for the Sprite IR.
  struct ir_h
  {
    type void_t = types::void_();
    type char_t = types::char_();
    type i64_t = types::int_(64);

    // Forward declaration.
    type node_t = types::struct_("node");

    // The type of a step-performing function (i.e., N or H).
    type stepfun_t = void_t(*node_t);

    // The type of a function returning the node's label.
    type labelfun_t = (*char_t)(*node_t);

    // The type of a function returning the node's arity.
    type arityfun_t = i64_t(*node_t);

    // The type of a function returning the node's children.
    type rangefun_t = void_t(*node_t, ***node_t, ***node_t);

    /**
     * @brief Each Curry symbol has a static vtable holding its non-instance
     * data.
     */
    type vtable_t = types::struct_(
        "vtable"
      , {
            *labelfun_t /*label*/
          , *arityfun_t /*arity*/
          , *rangefun_t /*succ*/
          , *stepfun_t  /*N*/
          , *stepfun_t  /*H*/
          }
      #if 0
      , {"label", "arity", "succ", "N", "H"}
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
    enum VtMember { VT_LABEL, VT_ARITY, VT_SUCC, VT_N, VT_H };
    enum NdMember { ND_VPTR, ND_TAG, ND_SLOT0, ND_SLOT1 };
  }
  using namespace member_labels;

  // Provides the rule for getting the vtable symbol name for a built-in type.
  // The basenames come from builtins.def.
  inline std::string get_vt_name(std::string const & basename)
    { return ".vt." + basename; }

  // Returns an arity function for the specified arity.
  function get_arity_function(ir_h const & ir, size_t arity);

  // Returns a label function for a symbol with a fixed name.
  function get_label_function(ir_h const & ir, std::string const & label);

  // Returns a step function that does nothing.
  function get_null_step_function(ir_h const & ir);

  // Returns a successor function that returns an empty range.
  function get_succ_function(ir_h const & ir, size_t arity);

  // LLVM declarations for the Sprite runtime.
  struct rt_h
  {
    type void_t = types::void_();
    type char_t = types::char_();
    type node_t = types::struct_("node");

    function const printexpr =
        extern_(void_t(*node_t, *char_t), "sprite_printexpr");

    function const normalize = extern_(void_t(*node_t), "sprite_normalize");

    // Exists only to simply the macro expansion below, in the constructor.
    char unused;

    // Built-in vtables.
    #define SPRITE_HANDLE_BUILTIN(name, _) global const name##_vt;
    #include "sprite/builtins.def"

    rt_h(ir_h const & ir)
      : unused()
      #define SPRITE_HANDLE_BUILTIN(name, _)                  \
        , name##_vt(extern_(ir.vtable_t, get_vt_name(#name))) \
        /**/
      #include "sprite/builtins.def"
    {}
  };

}}
