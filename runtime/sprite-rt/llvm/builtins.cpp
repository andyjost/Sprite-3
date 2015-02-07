#include <inttypes.h>
#include "sprite/compiler.hpp"
#include "sprite/runtime.hpp"
#include <stdint.h>
#include <vector>

using namespace sprite::backend;
using namespace sprite::compiler::member_labels; // for ND_* and VT_* enums.
using sprite::compiler::rt_h;

#define PRINT_BUFFER_SIZE 64
globalvar & printbuffer(rt_h const & rt)
{
  static globalvar printbuffer =
      extern_(types::char_()[PRINT_BUFFER_SIZE], ".printbuffer")
          .set_initializer(std::vector<char>(PRINT_BUFFER_SIZE, '\0'));
  return printbuffer;
}

void build_vt_for_Char(rt_h const & rt)
{
  auto char_repr = extern_(rt.char_t[7][256], ".Char_repr")
      .set_initializer({
#include "sprite_ascii_tables.hpp"
      });

  // Create the vtable for Char nodes.
  function Char_name = inline_<function>(
      rt.labelfun_t, ".name.Char", {"node_p"}
    , [&] {
        value node_p = arg("node_p");
        value char_value = *bitcast(&node_p.arrow(ND_SLOT0), *rt.char_t);
        return_(&char_repr[char_value]);
      }
    );

  extern_(rt.vtable_t, sprite::compiler::get_vt_name("Char"))
      .set_initializer(_t(
          &rt.Cy_NoAction
        , &rt.Cy_NoAction
        , &Char_name
				, &rt.Cy_Sentinel()
        , &rt.Cy_Arity(0)
        , &rt.Cy_Succ(0)
        , &rt.Cy_Destroy(0)
        , &rt.CyVt_Equality("Char")
        , &rt.CyVt_Compare("Char")
        , &rt.CyVt_Show("Char")
        ))
		;
}
void build_vt_for_Int64(rt_h const & rt)
{
  // Create the vtable for Int64 nodes.
  function Int64_name = inline_<function>(
      rt.labelfun_t, ".name.Int64", {"node_p"}
    , [&] {
        value node_p = arg("node_p");
        value int_value = *bitcast(&node_p.arrow(ND_SLOT0), *types::int_(64));
        // The format string is super non-portable.
        ref format = local(*rt.char_t);
        if_(
            int_value <(signed_)(0)
          , [&]{ format = "(%" PRId64 ")"; }
          , [&]{ format = "%" PRId64; }
          );
        value rv = rt.snprintf(
            &printbuffer(rt), PRINT_BUFFER_SIZE, format, int_value
          );
        if_(rv ==(signed_)(0)
          , [&]{
              rt.perror("Error converting Int to string");
              rt.exit(1);
            }
          );
        return_(&printbuffer(rt));
      }
    );

  extern_(rt.vtable_t, sprite::compiler::get_vt_name("Int64"))
      .set_initializer(_t(
          &rt.Cy_NoAction
        , &rt.Cy_NoAction
        , &Int64_name
				, &rt.Cy_Sentinel()
        , &rt.Cy_Arity(0)
        , &rt.Cy_Succ(0)
        , &rt.Cy_Destroy(0)
        , &rt.CyVt_Equality("Int")
        , &rt.CyVt_Compare("Int")
        , &rt.CyVt_Show("Int")
        ))
		;
}

void build_vt_for_Float(rt_h const & rt)
{
  // Create the vtable for Float nodes.
  function Float_name = inline_<function>(
      rt.labelfun_t, ".name.Float", {"node_p"}
    , [&] {
        value node_p = arg("node_p");
        value double_value = *bitcast(&node_p.arrow(ND_SLOT0), *types::double_());
        ref format = local(*rt.char_t);
        if_(
            double_value <(signed_)(0)
          , [&]{ format = "(%g)"; }
          , [&]{ format = "%g"; }
          );
        value rv = rt.snprintf(
            &printbuffer(rt), PRINT_BUFFER_SIZE, format, double_value
          );
        if_(rv ==(signed_)(0)
          , [&]{
              rt.perror("Error converting Float to string");
              rt.exit(1);
            }
          );
        return_(&printbuffer(rt));
      }
    );

  extern_(rt.vtable_t, sprite::compiler::get_vt_name("Float"))
      .set_initializer(_t(
          &rt.Cy_NoAction
        , &rt.Cy_NoAction
        , &Float_name
				, &rt.Cy_Sentinel()
        , &rt.Cy_Arity(0)
        , &rt.Cy_Succ(0)
        , &rt.Cy_Destroy(0)
        , &rt.CyVt_Equality("Float")
        , &rt.CyVt_Compare("Float")
        , &rt.CyVt_Show("Float")
        ))
		;
}

void build_vt_for_choice(rt_h const & rt)
{
  function choice_name = inline_<function>(
      rt.labelfun_t, ".name.choice", {"node_p"}
    , [&] {
        value node_p = arg("node_p");
        value id = node_p.arrow(ND_AUX);
        value rv = rt.snprintf(
            &printbuffer(rt), PRINT_BUFFER_SIZE, "?%" PRId32, id
          );
        if_(rv ==(signed_)(0)
          , [&]{
              rt.perror("Error converting Choice ID to string");
              rt.exit(1);
            }
          );
        return_(&printbuffer(rt));
      }
    );
  extern_(rt.vtable_t, sprite::compiler::get_vt_name("choice"))
      .set_initializer(_t(
          &rt.Cy_NoAction
        , &rt.Cy_NoAction
        , &choice_name
				, &rt.Cy_Sentinel()
        , &rt.Cy_Arity(2)
        , &rt.Cy_Succ(2)
        , &rt.Cy_Destroy(2)
        , rt.CyVt_Equality("choice")
        , rt.CyVt_Compare("choice")
        , rt.CyVt_Show("choice")
        ))
		;
}

void build_vt_for_fwd(rt_h const & rt)
{
  // Create the vtable for FWD nodes.
  function fwd_name = inline_<function>(
      rt.labelfun_t, ".fwd.name", {"node_p"}
    , [&]
      {
        value node_p = arg("node_p");
        node_p = bitcast(node_p.arrow(ND_SLOT0), *rt.node_t);
        return_(
            node_p.arrow(ND_VPTR).arrow(VT_LABEL)(node_p)
                .set_attribute(tailcall)
          );
      }
    );
  function fwd_arity = inline_<function>(
      rt.arityfun_t, "fwd.arity", {"node_p"}
    , [&]{
        value node_p = arg("node_p");
        node_p = bitcast(node_p.arrow(ND_SLOT0), *rt.node_t);
        return_(
            node_p.arrow(ND_VPTR).arrow(VT_ARITY)(node_p)
                .set_attribute(tailcall)
          );
      }
    );
  function fwd_succ = inline_<function>(
      rt.rangefun_t, "fwd.succ"
    , {"node_p", "begin_out_pp", "end_out_pp"}
    , [&]{
        value node_p = arg("node_p");
        node_p = bitcast(node_p.arrow(ND_SLOT0), *rt.node_t);
        node_p.arrow(ND_VPTR).arrow(VT_SUCC)(
            node_p, arg("begin_out_pp"), arg("end_out_pp")
          ).set_attribute(tailcall);
        return_();
      }
    );
  function fwd_N = inline_(
      rt.stepfun_t, ".fwd.N", {"node_p"}
    , [&]{
        value node_p = arg("node_p");
        node_p = bitcast(node_p.arrow(ND_SLOT0), *rt.node_t);
        sprite::compiler::vinvoke(node_p, VT_N, tailcall);
        return_();
      }
    );
  function fwd_H = inline_(
      rt.stepfun_t, ".fwd.H", {"node_p"}
    , [&]{
        value node_p = arg("node_p");
        node_p = bitcast(node_p.arrow(ND_SLOT0), *rt.node_t);
        sprite::compiler::vinvoke(node_p, VT_H, tailcall);
        return_();
      }
    );

  extern_(rt.vtable_t, sprite::compiler::get_vt_name("fwd"))
      .set_initializer(_t(
          &fwd_H
        , &fwd_N
        , &fwd_name
        , &rt.Cy_Sentinel()
        , &fwd_arity
        , &fwd_succ
        , &rt.Cy_Destroy(1)
        , rt.CyVt_Equality("fwd")
        , rt.CyVt_Compare("fwd")
        , rt.CyVt_Show("fwd")
        ))
		;
}
