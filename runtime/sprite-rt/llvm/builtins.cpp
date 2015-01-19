#include <inttypes.h>
#include "sprite/compiler.hpp"
#include "sprite/runtime.hpp"
#include <stdint.h>
#include <vector>

using namespace sprite::backend;
using namespace sprite::compiler::member_labels; // for ND_* and VT_* enums.
using sprite::compiler::rt_h;

#define PRINT_BUFFER_SIZE 64

struct Library
{
  type char_ = types::char_();
  type int_ = get_type<int>();
  type size_t_= get_type<size_t>();
  type void_ = types::void_();
  type ptrint = types::int_(sizeof_(*char_));
  type FILE_p = *types::struct_("FILE");

  // int snprintf(char *str, size_t size, const char *format, ...);
  function const snprintf = extern_(int_(*char_, size_t_, *char_, dots), "snprintf");

  // void exit(int status);
  function const exit = extern_(void_(int_), "exit");

  // void perror(char const *);
  function const perror = extern_(void_(*char_), "perror");

  // int fprintf(FILE *stream, const char *format, ...);
  function const fprintf = extern_(int_(FILE_p, *char_, dots), "fprintf");

  // int isprint(int c);
  function const isprint = extern_(int_(int_), "isprint");

  // A static buffer used to return dynamic string representations.
  globalvar printbuffer =
      extern_(types::char_()[PRINT_BUFFER_SIZE], ".printbuffer")
          .set_initializer(std::vector<char>(PRINT_BUFFER_SIZE, '\0'));
};

// Builds a repr function for a node with no successor.
function make_repr_for_nullary_node(
    rt_h const & rt
  , Library const & lib
  , function const & label
  )
{
  return extern_<function>(
      rt.reprfun_t, flexible(".repr"), {"root", "stream", "is_outer"}
    , [&] {
        value rv = lib.fprintf(arg("stream"), "%s", label(arg("root")));
        if_(rv <(signed_)(0)
          , [&]{
              lib.perror("Error printing string");
              lib.exit(1);
            }
          );
      }
    );
}

void build_vt_for_Char(rt_h const & rt)
{
  Library lib;

  auto char_repr = extern_(lib.char_[7][256], ".Char_repr")
      .set_initializer({
#include "sprite_ascii_tables.hpp"
      });

  // Create the vtable for Char nodes.
  function Char_name = inline_<function>(
      rt.labelfun_t, ".name.Char", {"node_p"}
    , [&] {
        value node_p = arg("node_p");
        value char_value = *bitcast(&node_p.arrow(ND_SLOT0), *lib.char_);
        return_(&char_repr[char_value]);
      }
    );

  extern_(rt.vtable_t, sprite::compiler::get_vt_name("Char"))
      .set_initializer(_t(
          &Char_name
        , &rt.Cy_Arity(0)
        , &rt.Cy_Succ(0)
        , &rt.CyVt_Equality("Char")
        , &rt.CyVt_Compare("Char")
        , &make_repr_for_nullary_node(rt, lib, Char_name)
        , &rt.Cy_NoAction
        , &rt.Cy_NoAction
        ))
	  ;
}
void build_vt_for_Int64(rt_h const & rt)
{
  Library lib;

  // Create the vtable for Int64 nodes.
  function Int64_name = inline_<function>(
      rt.labelfun_t, ".name.Int64", {"node_p"}
    , [&] {
        value node_p = arg("node_p");
        value int_value = *bitcast(&node_p.arrow(ND_SLOT0), *types::int_(64));
        // The format string is super non-portable.
        ref format = local(*lib.char_);
        if_(
            int_value <(signed_)(0)
          , [&]{ format = "(%" PRId64 ")"; }
          , [&]{ format = "%" PRId64; }
          );
        value rv = lib.snprintf(
            &lib.printbuffer, PRINT_BUFFER_SIZE, format, int_value
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

  extern_(rt.vtable_t, sprite::compiler::get_vt_name("Int64"))
      .set_initializer(_t(
          &Int64_name
        , &rt.Cy_Arity(0)
        , &rt.Cy_Succ(0)
        , &rt.CyVt_Equality("Int")
        , &rt.CyVt_Compare("Int")
        , &make_repr_for_nullary_node(rt, lib, Int64_name)
        , &rt.Cy_NoAction
        , &rt.Cy_NoAction
        ))
	  ;
}

void build_vt_for_Float(rt_h const & rt)
{
  Library lib;

  // Create the vtable for Float nodes.
  function Float_name = inline_<function>(
      rt.labelfun_t, ".name.Float", {"node_p"}
    , [&] {
        value node_p = arg("node_p");
        value double_value = *bitcast(&node_p.arrow(ND_SLOT0), *types::double_());
        ref format = local(*lib.char_);
        if_(
            double_value <(signed_)(0)
          , [&]{ format = "(%g)"; }
          , [&]{ format = "%g"; }
          );
        value rv = lib.snprintf(
            &lib.printbuffer, PRINT_BUFFER_SIZE, format, double_value
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

  extern_(rt.vtable_t, sprite::compiler::get_vt_name("Float"))
      .set_initializer(_t(
          &Float_name
        , &rt.Cy_Arity(0)
        , &rt.Cy_Succ(0)
        , &rt.CyVt_Equality("Float")
        , &rt.CyVt_Compare("Float")
        , &make_repr_for_nullary_node(rt, lib, Float_name)
        , &rt.Cy_NoAction
        , &rt.Cy_NoAction
        ))
	  ;
}

void build_vt_for_choice(rt_h const & rt)
{
  Library lib;

  function choice_name = inline_<function>(
      rt.labelfun_t, ".name.choice", {"node_p"}
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
  extern_(rt.vtable_t, sprite::compiler::get_vt_name("choice"))
      .set_initializer(_t(
          &choice_name
        , &rt.Cy_Arity(2)
        , &rt.Cy_Succ(2)
        , rt.CyVt_Equality("choice")
        , rt.CyVt_Compare("choice")
        , &make_repr_for_nullary_node(rt, lib, choice_name)
        , &rt.Cy_NoAction
        , &rt.Cy_NoAction
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
  function fwd_repr = inline_<function>(
      rt.reprfun_t, "fwd.repr"
    , {"node_p", "stream", "is_outer"}
    , [&]{
        value node_p = arg("node_p");
        node_p = bitcast(node_p.arrow(ND_SLOT0), *rt.node_t);
        node_p.arrow(ND_VPTR).arrow(VT_SHOW)(
            node_p, arg("stream"), arg("is_outer")
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
          &fwd_name, &fwd_arity, &fwd_succ
        , rt.CyVt_Equality("fwd")
        , rt.CyVt_Compare("fwd")
        , &fwd_repr
        , &fwd_N, &fwd_H
        ))
	  ;
}
