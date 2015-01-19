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
    type bool_t = types::bool_();
    type i32_t = types::int_(32);
    type i64_t = types::int_(64);
    type FILE_p = *types::struct_("FILE");

    // Forward declarations.
    type node_t = types::struct_("node");
    type vtable_t = types::struct_("vtable");

    // The type of a step-performing function (i.e., N or H).
    function_type stepfun_t = void_t(*node_t);

    // The type of a function returning the node's label.
    function_type labelfun_t = (*char_t)(*node_t);

    // The type of a function returning the node's arity.
    function_type arityfun_t = i64_t(*node_t);

    // The type of a function returning the node's children.
    function_type rangefun_t = void_t(*node_t, ***node_t, ***node_t);

    // The type of a function that converts an expression into a string.
    function_type reprfun_t = void_t(*node_t, FILE_p, bool_t);

    ir_h()
    {
      /// A node is made up of a pointer-to-vtable, tag, and two data slots.
      node_t = types::struct_(
          "node"
        , {
            *vtable_t /*vptr*/, i32_t /*tag*/, i32_t /*aux*/
          , *char_t /*slot0*/, *char_t /*slot1*/
          }
        #if 0
        , {"vptr", "tag", "aux", "slot0", "slot1"}
        #endif
        );

      /**
       * @brief Each Curry symbol has a static vtable holding its non-instance
       * data.
       */
      vtable_t = types::struct_(
          "vtable"
        , {
              *labelfun_t /*label*/
            , *arityfun_t /*arity*/
            , *rangefun_t /*succ*/
            , *vtable_t   /*equality*/
            , *vtable_t   /*comparison*/
            , *reprfun_t  /*repr*/
            , *stepfun_t  /*N*/
            , *stepfun_t  /*H*/
            }
        #if 0
        , {"label", "arity", "succ", "equality", "N", "H"}
        #endif
        );
    }
  };

  // FIXME: using strings to index via dot and arrow would be more convenient.
  namespace member_labels
  {
    enum VtMember { VT_LABEL, VT_ARITY, VT_SUCC, VT_EQUALITY, VT_COMPARISON, VT_SHOW, VT_N, VT_H };
    enum NdMember { ND_VPTR, ND_TAG, ND_AUX, ND_SLOT0, ND_SLOT1 };
  }
  using namespace member_labels;

  // Provides the rule for getting the vtable symbol name for a built-in type.
  // The basenames come from builtins.def.
  inline std::string get_vt_name(std::string const & basename)
    { return ".vt." + basename; }

  // LLVM declarations for the Sprite runtime.
  struct rt_h : ir_h
  {
    function_type const yieldfun_t = void_t(*node_t);

    function const Cy_Eval = extern_(void_t(*node_t, *yieldfun_t), "Cy_Eval");
    function const Cy_Normalize = extern_(void_t(*node_t), "Cy_Normalize");
    function const Cy_Print = extern_(void_t(*node_t), "Cy_Print");
    function const Cy_PrintWithSuffix =
        extern_(void_t(*node_t, *char_t), "Cy_PrintWithSuffix");
    function const Cy_NoAction = extern_(void_t(*node_t), "Cy_NoAction");
    function const Cy_GenericRepr = extern_(reprfun_t, "Cy_GenericRepr");

    // Returns an arity function for the specified arity.
    function Cy_Arity(size_t arity) const;
  
    // Returns a label function for a symbol with a fixed name.
    function Cy_Label(std::string const & label) const;
  
    // Returns a function that returns the successor range of a node.  Returns
    // a pointer to that function.  The second argument is for internal use only.
    function Cy_Succ(size_t arity, bool=false) const;

    // Gets the vtable for equality.  For normal types, str1 is the module name
    // and str2 is the type name.  For built-in types, str1 is the built-in
    // type name and str2 is empty.
    global CyVt_Equality(
        std::string const & str1, std::string const & str2 = std::string()
      ) const
    { return CyVt_PolyFunction("==", str1, str2); }

    // Gets the vtable for compare.
    global CyVt_Compare(
        std::string const & str1, std::string const & str2 = std::string()
      ) const
    { return CyVt_PolyFunction("compare", str1, str2); }

    // Used to implement polymorphic functions.
    global CyVt_PolyFunction(
        std::string const & tag
      , std::string const & str1, std::string const & str2
      ) const;

    // The global counter giving the next available choice id.
    globalvar next_choice_id = extern_(
        types::int_(32), "next_choice_id"
      ).as_globalvar();

    // Built-in vtables.
    #define SPRITE_HANDLE_BUILTIN(name, _) global const name##_vt;
    #include "sprite/builtins.def"

    rt_h() : ir_h()
      #define SPRITE_HANDLE_BUILTIN(name, _)               \
        , name##_vt(extern_(vtable_t, get_vt_name(#name))) \
        /**/
      #include "sprite/builtins.def"
    {}
  };
}}
