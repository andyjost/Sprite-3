#pragma once
#include <unistd.h>
#include "sprite/backend.hpp"
#include "sprite/basic_runtime.hpp"

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
    type int_t = get_type<int>();
    type i32_t = types::int_(32);
    type i64_t = types::int_(64);
    type size_t_t = get_type<size_t>();
    type FILE_p = *types::struct_("FILE");

    type tag_t = types::int_(16);
    type mark_t = types::int_(16);
    type aux_t = types::int_(32);

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
            *vtable_t /*vptr*/, tag_t /*tag*/, mark_t /*mark*/, aux_t /*aux*/
          , *char_t /*slot0*/, *char_t /*slot1*/
          }
        #if 0
        , {"vptr", "tag", "mark", "aux", "slot0", "slot1"}
        #endif
        );

      /**
       * @brief Each Curry symbol has a static vtable holding its non-instance
       * data.
       */
      vtable_t = types::struct_(
          "vtable"
        , {
              *stepfun_t  /*H*/
            , *stepfun_t  /*N*/
            , *labelfun_t /*label*/
            , *vtable_t   /*sentinel -- must be fourth!*/
            , *arityfun_t /*arity*/
            , *rangefun_t /*succ*/
            , *rangefun_t /*gcsucc*/
            , *stepfun_t  /*destroy*/
            , *vtable_t   /*equality*/
            , *vtable_t   /*comparison*/
            , *vtable_t   /*show*/
            }
        #if 0
        , {"H", "N", "label", "sentinel", "arity", "succ", "destroy", "equality", "comparison", "show"}
        #endif
        );
    }
  };

  // FIXME: using strings to index via dot and arrow would be more convenient.
  namespace member_labels
  {
    enum VtMember {
        VT_H, VT_N, VT_LABEL, VT_SENTINEL, VT_ARITY, VT_SUCC, VT_GCSUCC, VT_DESTROY
      , VT_EQUALITY, VT_COMPARISON, VT_SHOW
      };
    enum NdMember { ND_VPTR, ND_TAG, ND_MARK, ND_AUX, ND_SLOT0, ND_SLOT1 };
  }
  using namespace member_labels;

  // Provides the rule for getting the vtable symbol name for a built-in type.
  // The basenames come from builtins.def.
  inline std::string get_vt_name(std::string const & basename)
    { return ".vt." + basename; }

  // LLVM declarations for the Sprite runtime.
  struct rt_h : ir_h
  {
    // Memory system.
    function const Cy_ArrayAllocTyped = extern_((**node_t)(aux_t), "Cy_ArrayAllocTyped");
    function const Cy_ArrayDealloc = extern_(void_t(aux_t, *char_t), "Cy_ArrayDealloc");
    function const CyMem_Collect = extern_(void_t(), "CyMem_Collect");
    globalvar const CyMem_FreeList = extern_(*char_t, "CyMem_FreeList").as_globalvar();
    function const _CyMem_PushRoot = extern_(void_t(*node_t), "CyMem_PushRoot");
    function const _CyMem_PopRoot = extern_(void_t(), "CyMem_PopRoot");
    void CyMem_PushRoot(value root_p, bool enable_tracing) const;
    void CyMem_PopRoot(bool enable_tracing) const;

    // Tracing.
    function const CyTrace_Indent = extern_(void_t(), "CyTrace_Indent");
    function const CyTrace_Dedent = extern_(void_t(), "CyTrace_Dedent");
    function const CyTrace_ShowIndent = extern_(void_t(), "CyTrace_ShowIndent");

    function const Cy_Suspend = extern_(void_t(), "Cy_Suspend");

    // Creates a new basic block at the current point in the code stream and
    // makes it the default insertion point.  Creates another basic block that
    // calls the garbage collector and then returns to this point.  Returns the
    // second one.
    label make_restart_point() const;

    // Macro-like function.  Must be called from a label scope.  Either
    // allocates a new node and assigns it to the ref, or jumps to the label.
    value node_alloc(type const & ty, label const &) const;

    function_type const yieldfun_t = void_t(*node_t);
    function const Cy_Eval = extern_(void_t(*node_t, *yieldfun_t), "Cy_Eval");
    function const Cy_Normalize = extern_(void_t(*node_t), "Cy_Normalize");
    function const Cy_CyStringToCString =
        extern_(void_t(*node_t, FILE_p), "Cy_CyStringToCString");
    function const Cy_NoAction = extern_(void_t(*node_t), "Cy_NoAction");
    function const Cy_Repr = extern_(reprfun_t, "Cy_Repr");


    // Returns an arity function for the specified arity.
    function Cy_Arity(size_t arity) const;
  
    // Returns a label function for a symbol with a fixed name.
    function Cy_Label(std::string const & label) const;
  
    // Returns a function that returns the successor range of a node.  Returns
    // a pointer to that function.  The second argument is for internal use only.
    function Cy_Succ(size_t arity, bool=false) const;

    // Returns a function that frees the associated successor array, if any.
    function Cy_Destroy(size_t arity) const;

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

    // Gets the vtable for show.
    global CyVt_Show(
        std::string const & str1, std::string const & str2 = std::string()
      ) const
    { return CyVt_PolyFunction("show", str1, str2); }

    // Used to implement polymorphic functions.
    global CyVt_PolyFunction(
        std::string const & tag
      , std::string const & str1, std::string const & str2
      ) const;

    // The global counter giving the next available choice id.
    globalvar Cy_NextChoiceId = extern_(aux_t, "Cy_NextChoiceId").as_globalvar();

    // Free variables are printed as _a, _b, . . .  This function resets the
    // sequence to _a, and is called between producing values.
    function const CyFree_ResetCounter = extern_(void_t(), "CyFree_ResetCounter");

    // External functions.
    // void exit(int status);
    function const exit = extern_(void_t(int_t), "exit");
    // FILE *fdopen(int fd, const char *mode);tp
    function const fdopen = extern_(FILE_p(int_t, *char_t), "fdopen");
    // int fflush(FILE *stream);
    function const fflush = extern_(int_t(FILE_p), "fflush");
    // int fprintf(FILE *stream, const char *format, ...);
    function const fprintf = extern_(int_t(FILE_p, *char_t, dots), "fprintf");
    // int fputs(const char *s, FILE *stream);
    function const fputs = extern_(int_t(*char_t, FILE_p), "fputs");
    // void free(void *ptr);
    function const free = extern_(void_t(*char_t), "free");
    // int isprint(int c);
    function const isprint = extern_(int_t(int_t), "isprint");
    // void *malloc(size_t size);
    function const malloc = extern_((*char_t)(size_t_t), "malloc");
    // void perror(char const *);
    function const perror = extern_(void_t(*char_t), "perror");
    // int printf(const char *format, ...);
    function const printf = extern_(int_t(*char_t, dots), "printf");
    // int putchar(int c);
    function const putchar = extern_(int_t(int_t), "putchar");
    // int puts(const char *s, FILE *stream);
    function const puts = extern_(int_t(*char_t), "puts");
    // int snprintf(char *str, size_t size, const char *format, ...);
    function const snprintf = extern_(int_t(*char_t, size_t_t, *char_t, dots), "snprintf");

    function const stdin_ = extern_(FILE_p(), "Cy_stdin");
    function const stdout_ = extern_(FILE_p(), "Cy_stdout");
    function const stderr_ = extern_(FILE_p(), "Cy_stderr");

    // Present in every vtable.  Use by the garbage collector to detect whether
    // memory chunks are initialized.
    global Cy_Sentinel() const { return this->fwd_vt; }

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
