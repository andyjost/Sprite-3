#include "sprite/runtime.hpp"
#include "sprite/compiler.hpp" 

namespace sprite { namespace compiler
{
  function get_arity_function(ir_h const & ir, size_t arity)
  {
    type const ty = ir.arityfun_t;
    std::string const name = ".arity." + std::to_string(arity);

    // Link to a predefined funtion.
    if(arity < SPRITE_PREDEF_ARITY_LIMIT)
      return extern_<function>(ty, name);

    // Find a previously-defined function in this compilation unit.
    function const f(scope::current_module()->getFunction(name));
    if(f.ptr()) return f;

    // Make a new function.
    return inline_<function>(ty, name, {}, [&] { return_(arity); });
  }

  function get_label_function(ir_h const & ir, std::string const & label)
  {
    return static_<function>(
        ir.labelfun_t, flexible(".name"), {}, [&] { return_(label); }
      );
  }

  function get_null_step_function(ir_h const & ir)
    { return extern_<function>(ir.stepfun_t, ".nullstep"); }

  function get_generic_show_function(ir_h const & ir)
    { return extern_<function>(ir.showfun_t, "sprite_generic_show"); }


  // Fills in the body of a successor function.  This function does not appear
  // in the corresponding header file, but is used when preparing the runtime
  // library.
  function make_succ_function(function fun, ir_h const & ir, int64_t arity)
  {
    scope _ = fun;
    ref begin_out_pp = arg("begin_out_pp");
    ref end_out_pp = arg("end_out_pp");

    if(arity == 0)
    {
      begin_out_pp = nullptr;
      end_out_pp = nullptr;
      return_();
      return fun;
    }

    value node_p = arg("node_p");
    value begin_cp = arity < 3
      ? static_cast<value>(&node_p.arrow(ND_SLOT0))
      : static_cast<value>(*bitcast(&node_p.arrow(ND_SLOT0), ***types::char_()))
      ;
    begin_out_pp = begin_cp;

    size_t sizeof_ptr = sizeof_(*ir.node_t);
    type size_t_ = types::int_(sizeof_ptr * 8);
    end_out_pp = typecast(begin_cp, size_t_) + arity * sizeof_ptr;

    return_();
    return fun;
  }

  // Makes a function that returns the successor range of a node.  Returns a
  // pointer to that function.
  function get_succ_function(ir_h const & ir, size_t arity)
  {
    type const ty = ir.rangefun_t;
    std::string const name = ".succ." + std::to_string(arity);

    // Link to a predefined funtion.
    if(arity < SPRITE_PREDEF_ARITY_LIMIT)
      return extern_<function>(ty, name);

    // Find a previously-defined function in this compilation unit.
    {
      function const f(scope::current_module()->getFunction(name));
      if(f.ptr()) return f;
    }

    // Make a new function.
    function f = inline_<function>(
        ty, name, {"node_p", "begin_out_pp", "end_out_pp"}
      );
    return make_succ_function(f, ir, arity);
  }
}}
