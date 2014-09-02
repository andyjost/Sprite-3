#include "sprite/compiler.hpp"
#include "sprite/runtime.hpp"

using namespace sprite::backend;
using namespace sprite::compiler::member_labels; // for ND_* and VT_* enums.

void build_fwd_vt(sprite::compiler::ir_h const & ir)
{
  // Create the vtable for FWD nodes.
  function fwd_name = inline_<function>(
      ir.labelfun_t, ".fwd.name", {"node_p"}
    , [&]
      {
        value node_p = arg("node_p");
        node_p = bitcast(node_p.arrow(ND_SLOT0), *ir.node_t);
        return_(
            node_p.arrow(ND_VPTR).arrow(VT_LABEL)(node_p)
                .set_attribute(tailcall)
          );
      }
    );
  function fwd_arity = inline_<function>(
      ir.arityfun_t, "fwd.arity", {"node_p"}
    , [&]{
        value node_p = arg("node_p");
        node_p = bitcast(node_p.arrow(ND_SLOT0), *ir.node_t);
        return_(
            node_p.arrow(ND_VPTR).arrow(VT_ARITY)(node_p)
                .set_attribute(tailcall)
          );
      }
    );
  function fwd_succ = inline_<function>(
      ir.rangefun_t, "fwd.succ"
    , {"node_p", "begin_out_pp", "end_out_pp"}
    , [&]{
        value node_p = arg("node_p");
        node_p = bitcast(node_p.arrow(ND_SLOT0), *ir.node_t);
        node_p.arrow(ND_VPTR).arrow(VT_SUCC)(
            node_p, arg("begin_out_pp"), arg("end_out_pp")
          ).set_attribute(tailcall);
        return_();
      }
    );
  function fwd_N = inline_(
      ir.stepfun_t, ".fwd.N", {"node_p"}
    , [&]{
        value node_p = arg("node_p");
        node_p = bitcast(node_p.arrow(ND_SLOT0), *ir.node_t);
        sprite::compiler::vinvoke(node_p, VT_N, tailcall);
        return_();
      }
    );
  function fwd_H = inline_(
      ir.stepfun_t, ".fwd.H", {"node_p"}
    , [&]{
        value node_p = arg("node_p");
        node_p = bitcast(node_p.arrow(ND_SLOT0), *ir.node_t);
        sprite::compiler::vinvoke(node_p, VT_H, tailcall);
        return_();
      }
    );

  extern_(ir.vtable_t, SPRITE_FWD_VT_NAME)
      .set_initializer(_t(&fwd_name, &fwd_arity, &fwd_succ, &fwd_N, &fwd_H))
	  ;
}
