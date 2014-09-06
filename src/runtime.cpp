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
    // TODO: use weak linkage to try and merge these symbols across compilation
    // units.
    return static_<function>(ty, name, {}, [&] { return_(arity); });
  }

  function get_label_function(ir_h const & ir, std::string const & label)
  {
    return static_<function>(
        ir.labelfun_t, flexible(".name"), {}, [&] { return_(label); }
      );
  }

  function get_null_step_function(ir_h const & ir)
    { return extern_<function>(ir.stepfun_t, ".nullstep"); }

  // Fills in the body of a successor function.  This function does not appear
  // in the corresponding header file, but is used when preparing the runtime
  // library.
  function make_succ_function(function fun, ir_h const & ir, int64_t arity)
  {
    if(arity == 0)
    {
      scope _ = fun;
      ref begin_out_pp = arg("begin_out_pp");
      ref end_out_pp = arg("end_out_pp");
      begin_out_pp = nullptr;
      end_out_pp = nullptr;
      return_();
    }
    else if(arity < 3)
    {
      scope _ = fun;
      // Compute the size of pointers and get an integer type with the same
      // bitwidth.
      size_t sizeof_ptr = sizeof_(*ir.node_t);
      type size_t_ = types::int_(sizeof_ptr * 8);

      // Get the function paramters.
      value node_p = arg("node_p");
      ref begin_out_pp = arg("begin_out_pp");
      ref end_out_pp = arg("end_out_pp");

      // Compute the begining address.
      value begin_cp = &node_p.arrow(ND_SLOT0);
      value begin = begin_cp;
      value end = typecast(begin_cp, size_t_) + arity * sizeof_ptr;

      // Assign the outputs.
      begin_out_pp = begin;
      end_out_pp = end;
      return_();
    }
    else
      assert(0 && "arity > 2 not supported");

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
    // TODO: use weak linkage to try and merge these symbols across compilation
    // units.
    function f = static_<function>(
        ir.rangefun_t, flexible(name)
      , {"node_p", "begin_out_pp", "end_out_pp"}
      );
    return make_succ_function(f, ir, arity);
  }
}}
