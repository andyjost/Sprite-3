#include <inttypes.h>
#include "sprite/compiler.hpp"
#include "sprite/runtime.hpp"
#include <stdint.h>
#include <vector>

using namespace sprite::backend;
using namespace sprite::compiler::member_labels; // for ND_* and VT_* enums.

#define PRINT_BUFFER_SIZE 64

struct Library
{
  type char_ = types::char_();
  type int_ = get_type<int>();
  type size_t_= get_type<size_t>();
  type void_ = types::void_();
  type ptrint = types::int_(sizeof_(*char_));

  // int snprintf(char *str, size_t size, const char *format, ...);
  function const snprintf = extern_(int_(*char_, size_t_, *char_, dots), "snprintf");

  // void exit(int status);
  function const exit = extern_(void_(int_), "exit");

  // void perror(char const *);
  function const perror = extern_(void_(*char_), "perror");

  // int isprint(int c);
  function const isprint = extern_(int_(int_), "isprint");

  // A static buffer used to return dynamic string representations.
  globalvar printbuffer =
      extern_(types::char_()[PRINT_BUFFER_SIZE], ".printbuffer")
          .set_initializer(std::vector<char>(PRINT_BUFFER_SIZE, '\0'));
};

void build_vt_for_Char(sprite::compiler::ir_h const & ir)
{
  Library lib;

  // DEBUG
  // synthesize_function_from_icurry();

  auto char_repr = extern_(lib.char_[7][256], ".Char_repr")
      .set_initializer({
#include "sprite_ascii_tables.hpp"
      });

  // Create the vtable for Char nodes.
  function Char_name = inline_<function>(
      ir.labelfun_t, ".name.Char", {"node_p"}
    , [&] {
        value node_p = arg("node_p");
        value char_value = *bitcast(&node_p.arrow(ND_SLOT0), *lib.char_);
        return_(&char_repr[char_value]);
      }
    );

  extern_(ir.vtable_t, sprite::compiler::get_vt_name("Char"))
      .set_initializer(_t(
          &Char_name
        , &get_arity_function(ir, 0)
        , &get_succ_function(ir, 0)
        , &get_vt_for_primitive_equality(ir, "Char")
        , &get_null_step_function(ir)
        , &get_null_step_function(ir)
        ))
	  ;
}
void build_vt_for_Int64(sprite::compiler::ir_h const & ir)
{
  Library lib;

  // Create the vtable for Int64 nodes.
  function Int64_name = inline_<function>(
      ir.labelfun_t, ".name.Int64", {"node_p"}
    , [&] {
        value node_p = arg("node_p");
        value int_value = *bitcast(&node_p.arrow(ND_SLOT0), *types::int_(64));
        // The format string is super non-portable.
        value rv = lib.snprintf(
            &lib.printbuffer, PRINT_BUFFER_SIZE, "%" PRId64, int_value
          );
        if_(rv ==(signed_)(0)
          , [&]{
              lib.perror("Error converting Int to string");
              lib.exit(1);
            }
          );
        return_(&lib.printbuffer);
      }
    );

  extern_(ir.vtable_t, sprite::compiler::get_vt_name("Int64"))
      .set_initializer(_t(
          &Int64_name
        , &get_arity_function(ir, 0)
        , &get_succ_function(ir, 0)
        , &get_vt_for_primitive_equality(ir, "Int")
        , &get_null_step_function(ir)
        , &get_null_step_function(ir)
        ))
	  ;
}

void build_vt_for_Float(sprite::compiler::ir_h const & ir)
{
  Library lib;

  // Create the vtable for Float nodes.
  function Float_name = inline_<function>(
      ir.labelfun_t, ".name.Float", {"node_p"}
    , [&] {
        value node_p = arg("node_p");
        value double_value = *bitcast(&node_p.arrow(ND_SLOT0), *types::double_());
        value rv = lib.snprintf(
            &lib.printbuffer, PRINT_BUFFER_SIZE, "%g", double_value
          );
        if_(rv ==(signed_)(0)
          , [&]{
              lib.perror("Error converting Float to string");
              lib.exit(1);
            }
          );
        return_(&lib.printbuffer);
      }
    );

  extern_(ir.vtable_t, sprite::compiler::get_vt_name("Float"))
      .set_initializer(_t(
          &Float_name
        , &get_arity_function(ir, 0)
        , &get_succ_function(ir, 0)
        , &get_vt_for_primitive_equality(ir, "Float")
        , &get_null_step_function(ir)
        , &get_null_step_function(ir)
        ))
	  ;
}

void build_vt_for_choice(sprite::compiler::ir_h const & ir)
{
  Library lib;

  function choice_name = inline_<function>(
      ir.labelfun_t, ".name.choice", {"node_p"}
    , [&] {
        value node_p = arg("node_p");
        value id = node_p.arrow(ND_AUX);
        value rv = lib.snprintf(
            &lib.printbuffer, PRINT_BUFFER_SIZE, "?%" PRId32, id
          );
        if_(rv ==(signed_)(0)
          , [&]{
              lib.perror("Error converting Choice ID to string");
              lib.exit(1);
            }
          );
        return_(&lib.printbuffer);
      }
    );
  extern_(ir.vtable_t, sprite::compiler::get_vt_name("choice"))
      .set_initializer(_t(
          &choice_name
        , &get_arity_function(ir, 2)
        , &get_succ_function(ir, 2)
        , get_vt_for_primitive_equality(ir, "choice")
        , &get_null_step_function(ir)
        , &get_null_step_function(ir)
        ))
	  ;
}

void build_vt_for_fwd(sprite::compiler::ir_h const & ir)
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

  extern_(ir.vtable_t, sprite::compiler::get_vt_name("fwd"))
      .set_initializer(_t(
          &fwd_name, &fwd_arity, &fwd_succ
        , get_vt_for_primitive_equality(ir, "fwd")
        , &fwd_N, &fwd_H
        ))
	  ;
}
