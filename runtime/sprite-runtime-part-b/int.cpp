#include <inttypes.h>
#include "sprite/compiler.hpp"
#include "sprite/runtime.hpp"
#include <stdint.h>
#include <vector>

using namespace sprite::backend;
using namespace sprite::compiler::member_labels; // for ND_* and VT_* enums.

void build_vt_for_int64(sprite::compiler::ir_h const & ir)
{
  type char_ = types::char_();
  type int_ = get_type<int>();
  type size_t_= get_type<size_t>();
  type void_ = types::void_();

  // int snprintf(char *str, size_t size, const char *format, ...);
  function const snprintf = extern_(int_(*char_, size_t_, *char_, dots), "snprintf");

  // void exit(int status);
  function const exit = extern_(void_(int_), "exit");

  // A static buffer used to return dynamic string representations.
  size_t const PRINT_BUFFER_SIZE = 64;
  globalvar printbuffer = extern_(types::char_()[PRINT_BUFFER_SIZE], ".printbuffer")
      .set_initializer(std::vector<char>(PRINT_BUFFER_SIZE, '\0'));

  // Create the vtable for int64 nodes.
  function int64_name = inline_<function>(
      ir.labelfun_t, ".name.int64", {"node_p"}
    , [&] {
        // The format string is super non-portable.
        value node_p = arg("node_p");
        value x = typecast(node_p.arrow(ND_SLOT0), types::int_(64));
        value rv = snprintf(&printbuffer, PRINT_BUFFER_SIZE, "%" PRId64, x);
        if_(rv ==(signed_)(0), [&]{ exit(1); });
        return_(&printbuffer);
      }
    );

  extern_(ir.vtable_t, sprite::compiler::get_vt_name("int64"))
      .set_initializer(_t(
          &int64_name
        , &get_arity_function(ir, 0)
        , &get_succ_function(ir, 0)
        , &get_null_step_function(ir)
        , &get_null_step_function(ir)
        ))
	  ;
}
