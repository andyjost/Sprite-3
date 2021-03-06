#include "sprite/compiler.hpp"
#include "sprite/runtime.hpp"
#include "sprite/backend/support/testing.hpp"
#include "llvm/Bitcode/ReaderWriter.h"
#include "llvm/Support/raw_ostream.h"

using namespace sprite::backend;
using sprite::compiler::rt_h;

void build_vt_for_Char(rt_h const & rt);
void build_vt_for_choice(rt_h const & rt);
void build_vt_for_Float(rt_h const & rt);
void build_vt_for_freevar(rt_h const & rt);
void build_vt_for_fwd(rt_h const & rt);
void build_vt_for_Int64(rt_h const & rt);
void build_vt_for_IO(rt_h const & rt);
void build_vt_for_PartialSpine(rt_h const & rt);
void build_vt_for_PartialTerminus(rt_h const & rt);

// A trivial node is one with label and arity, but no meaningful action for H
// or N.
void build_vt_for_trivial_node(
    rt_h const & rt, std::string const & name, size_t arity
  , sprite::compiler::tag_t tag
  )
{
  extern_(rt.vtable_t, sprite::compiler::get_vt_name(name))
      .set_initializer(_t(
          &rt.Cy_NoAction
        , &rt.Cy_NoAction
        , &rt.Cy_Label(name)
        , &rt.Cy_Sentinel()
        , tag
        , &rt.Cy_Arity(arity)
        , &rt.Cy_Succ(arity)
        , &rt.Cy_Succ(arity)
        , &rt.Cy_Destroy(arity)
        , &extern_(rt.vtable_t, ".vt.OPER.Prelude.prim_error_cmp_fun")
        , &extern_(rt.vtable_t, ".vt.OPER.Prelude.prim_error_cmp_fun")
        , &extern_(rt.vtable_t, ".vt.OPER.Prelude.prim_error_cmp_fun")
        , &extern_(rt.vtable_t, ".vt.OPER.Prelude.prim_error_cmp_fun")
        , &extern_(rt.vtable_t, ".vt.OPER.Prelude.prim_label")
        ))
    ;
}

void build_vt_for_binding(rt_h const & rt)
  { build_vt_for_trivial_node(rt, "binding", 2, sprite::compiler::BINDING); }
void build_vt_for_lazybinding(rt_h const & rt)
  { build_vt_for_trivial_node(rt, "lazybinding", 2, sprite::compiler::BINDING); }
void build_vt_for_failed(rt_h const & rt)
  { build_vt_for_trivial_node(rt, "failed", 0, sprite::compiler::FAIL); }
void build_vt_for_success(rt_h const & rt)
  { build_vt_for_trivial_node(rt, "Success", 0, sprite::compiler::CTOR); }
// FIXME: using a polymorphic function on a partial application will SEGV!
void build_vt_for_PartialSpine(rt_h const & rt)
  { build_vt_for_trivial_node(rt, "PartialSpine", 2, sprite::compiler::FAIL /*not used*/); }
void build_vt_for_PartialTerminus(rt_h const & rt)
{
  // The PartialTerminus reports the label of the bound function.
  function label = static_<function>(rt.labelfun_t, flexible(".name"), {"node_p"}
    , [&] {
        using namespace sprite::compiler::member_labels;
        value node_p = arg("node_p");
        value vt = bitcast(node_p.arrow(ND_SLOT0), *rt.vtable_t);
        return_(vt.arrow(VT_LABEL)(nullptr));
      }
    );

  extern_(rt.vtable_t, sprite::compiler::get_vt_name("PartialTerminus"))
      .set_initializer(_t(
          &rt.Cy_NoAction
        , &rt.Cy_NoAction
        , &label
        , &rt.Cy_Sentinel()
        , 0 /* not used */
        , &rt.Cy_Arity(0)
        , &rt.Cy_Succ(0)
        , &rt.Cy_Succ(0)
        , &rt.Cy_Destroy(0)
        , &extern_(rt.vtable_t, ".vt.OPER.Prelude.prim_error_cmp_fun")
        , &extern_(rt.vtable_t, ".vt.OPER.Prelude.prim_error_cmp_fun")
        , &extern_(rt.vtable_t, ".vt.OPER.Prelude.prim_error_cmp_fun")
        , &extern_(rt.vtable_t, ".vt.OPER.Prelude.prim_error_cmp_fun")
        , &extern_(rt.vtable_t, ".vt.OPER.Prelude.prim_label")
        ))
    ;
}

int main()
{
  module module_b("sprite_runtime_llvm_part");
  scope _ = module_b;
  rt_h rt;
  sprite::backend::testing::clib_h clib;

  // Declare the predefined arity and successor functions.
  for(size_t i=0; i<SPRITE_PREDEF_ARITY_LIMIT; ++i)
  {
    // Compile the arity function.
    extern_<function>(
        rt.arityfun_t, ".arity." + std::to_string(i), {"node_p"}
      , [&] { return_(i); }
      );

    // Compile the succ function.
    rt.Cy_Succ(i, /*force_compile*/ true);
  }

  // Build the vtables for built-in constructors.
  #define SPRITE_HANDLE_BUILTIN(name, arity) build_vt_for_##name(rt);
  #include "sprite/builtins.def"

  // Create stubs for unimplemented external functions.
  #define DECLARE_EXTERNAL_STUB(name)                                   \
      extern_<function>(                                                \
          rt.stepfun_t, "CyPrelude_" #name, {}                          \
        , [&] {                                                         \
            clib.fprintf(                                               \
                clib.fdopen(2, "w")                                     \
              , "External function \"" #name "\" is not implemented.\n" \
            );                                                          \
            clib.exit(EXIT_FAILURE);                                    \
          }                                                             \
        );                                                              \
    /**/

  // CMC Prelude externals
  // =======================
  // DECLARE_EXTERNAL_STUB(ensureNotFree)
  // DECLARE_EXTERNAL_STUB($!)
  // DECLARE_EXTERNAL_STUB($!!)
  // DECLARE_EXTERNAL_STUB($##)
  // DECLARE_EXTERNAL_STUB(error)
  // DECLARE_EXTERNAL_STUB(failed)
  // DECLARE_EXTERNAL_STUB(==)
  // DECLARE_EXTERNAL_STUB(compare)
  // DECLARE_EXTERNAL_STUB(ord)
  // DECLARE_EXTERNAL_STUB(chr)
  // DECLARE_EXTERNAL_STUB(+)
  // DECLARE_EXTERNAL_STUB(-)
  // DECLARE_EXTERNAL_STUB(*)
  // DECLARE_EXTERNAL_STUB(div)
  // DECLARE_EXTERNAL_STUB(mod)
  // DECLARE_EXTERNAL_STUB(quot)
  // DECLARE_EXTERNAL_STUB(rem)
  // DECLARE_EXTERNAL_STUB(negateFloat)
  // DECLARE_EXTERNAL_STUB(=:=)
  // DECLARE_EXTERNAL_STUB(success)
  // DECLARE_EXTERNAL_STUB(&)
  // DECLARE_EXTERNAL_STUB(>>=)
  // DECLARE_EXTERNAL_STUB(return)
  // DECLARE_EXTERNAL_STUB(putChar)
  DECLARE_EXTERNAL_STUB(getChar)
  DECLARE_EXTERNAL_STUB(prim_readFile)
  DECLARE_EXTERNAL_STUB(prim_readFileContents)
  DECLARE_EXTERNAL_STUB(prim_writeFile)
  DECLARE_EXTERNAL_STUB(prim_appendFile)
  DECLARE_EXTERNAL_STUB(catch)
  // DECLARE_EXTERNAL_STUB(show)
  // DECLARE_EXTERNAL_STUB(prim_label)
  // DECLARE_EXTERNAL_STUB(prim_show_string)
  // The choice (?) *function* should never be called.  The implementation
  // always places a special choice node in place of this function.
  DECLARE_EXTERNAL_STUB(?)
  // DECLARE_EXTERNAL_STUB(apply)
  // DECLARE_EXTERNAL_STUB(cond)
  DECLARE_EXTERNAL_STUB(letrec) // Unused in Sprite
  // DECLARE_EXTERNAL_STUB(=:<=)
  DECLARE_EXTERNAL_STUB(=:<<=)
  DECLARE_EXTERNAL_STUB(ifVar)
  DECLARE_EXTERNAL_STUB(failure)

  llvm::WriteBitcodeToFile(module_b.ptr(), llvm::outs());
}
