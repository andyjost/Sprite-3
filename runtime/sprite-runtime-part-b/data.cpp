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
        if_(rv ==(signed_)(0), [&]{ lib.exit(1); });
        return_(&lib.printbuffer);
      }
    );

  extern_(ir.vtable_t, sprite::compiler::get_vt_name("Int64"))
      .set_initializer(_t(
          &Int64_name
        , &get_arity_function(ir, 0)
        , &get_succ_function(ir, 0)
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
            &lib.printbuffer, PRINT_BUFFER_SIZE, "%f", double_value
          );
        if_(rv ==(signed_)(0), [&]{ lib.exit(1); });
        return_(&lib.printbuffer);
      }
    );

  extern_(ir.vtable_t, sprite::compiler::get_vt_name("Float"))
      .set_initializer(_t(
          &Float_name
        , &get_arity_function(ir, 0)
        , &get_succ_function(ir, 0)
        , &get_null_step_function(ir)
        , &get_null_step_function(ir)
        ))
	  ;
}
