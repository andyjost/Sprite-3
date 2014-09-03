#include "sprite/compiler.hpp"
#include "sprite/runtime.hpp"

using namespace sprite::backend;

void build_vt_for_fail(sprite::compiler::ir_h const & ir)
{
  // Create the vtable for FAIL nodes.
  function fail_name = inline_<function>(
      ir.labelfun_t, ".fail.name", {"node_p"}
    , [&] { return_("failed"); }
    );
  function fail_arity = inline_<function>(
      ir.arityfun_t, "fail.arity", {"node_p"}
    , [&] { return_(0); }
    );
  function fail_succ = inline_<function>(
      ir.rangefun_t, "fail.succ"
    , {"node_p", "begin_out_pp", "end_out_pp"}
    , [&]{
        ref begin_out_pp = arg("begin_out_pp");
        ref end_out_pp = arg("end_out_pp");
        begin_out_pp = nullptr;
        end_out_pp = nullptr;
        return_();
      }
    );
  function fail_N = inline_(
      ir.stepfun_t, ".fail.N", {"node_p"}
    , [&]{ return_(); }
    );
  function fail_H = inline_(
      ir.stepfun_t, ".fail.H", {"node_p"}
    , [&]{ return_(); }
    );

  extern_(ir.vtable_t, sprite::compiler::get_vt_name("fail"))
      .set_initializer(_t(&fail_name, &fail_arity, &fail_succ, &fail_N, &fail_H))
	  ;
}
