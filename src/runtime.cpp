#include "sprite/runtime.hpp"
#include "sprite/compiler.hpp" 

namespace sprite { namespace compiler
{
  function rt_h::Cy_Arity(size_t arity) const
  {
    type const ty = arityfun_t;
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

  function rt_h::Cy_Label(std::string const & label) const
  {
    return static_<function>(
        labelfun_t, flexible(".name"), {}, [&] { return_(label); }
      );
  }

  function rt_h::Cy_Succ(size_t arity, bool force_compile) const
  {
    std::string const name = ".succ." + std::to_string(arity);

    // When preparing the runtime library, force_compile=true is used to force
    // compilation.
    if(!force_compile)
    {
      // Link to a predefined funtion.
      if(arity < SPRITE_PREDEF_ARITY_LIMIT)
        return extern_<function>(rangefun_t, name);

      // Find a previously-defined function in this compilation unit.
      {
        function const f(scope::current_module()->getFunction(name));
        if(f.ptr()) return f;
      }
    }

    // Compile a new function.
    return extern_<function>(
        rangefun_t, name, {"node_p", "begin_out_pp", "end_out_pp"}
      , [&]
      {
        ref begin_out_pp = arg("begin_out_pp");
        ref end_out_pp = arg("end_out_pp");

        if(arity == 0)
        {
          begin_out_pp = nullptr;
          end_out_pp = nullptr;
          return_();
        }
        else
        {
          value node_p = arg("node_p");
          value begin_cp = arity < 3
            ? static_cast<value>(&node_p.arrow(ND_SLOT0))
            : static_cast<value>(*bitcast(&node_p.arrow(ND_SLOT0), ***types::char_()))
            ;
          begin_out_pp = begin_cp;

          size_t sizeof_ptr = sizeof_(*node_t);
          type size_t_ = types::int_(sizeof_ptr * 8);
          end_out_pp = typecast(begin_cp, size_t_) + arity * sizeof_ptr;

          return_();
        }
      });
  }

  function rt_h::Cy_Destroy(size_t arity) const
  {
    if(arity < 3)
      return Cy_NoAction;
    else
    {
      // FIXME: some (all?) of these should be defined in the runtime library.
      function destroy = static_<function>(
          stepfun_t, flexible(".destroy." + std::to_string(arity)), {"node_p"}
        );
      if(destroy->size() == 0) // no body
      {
        scope _ = destroy;
        Cy_ArrayDealloc(arity, arg("node_p").arrow(ND_SLOT0));
        return_();
      }
      return destroy;
    }
  }

  global rt_h::CyVt_PolyFunction(
      std::string const & tag
    , std::string const & str1, std::string const & str2
    ) const
  {
    if(str2 == "")
    {
      // Built-in type.  str1: typename
      return extern_(
          vtable_t, ".vt.OPER.Prelude.primitive." + tag + "." + str1
        );
    }
    else
    {
      // Normal type.  str1: module, str2: typename
      return extern_(
          vtable_t, ".vt.OPER." + str1 + "." + tag + "." + str2
        );
    }
  }

  label rt_h::make_restart_point() const
  {
    label redo;
    goto_(redo);
    scope::update_current_label_after_branch(redo);
    return label([&]{ this->CyMem_Collect(); goto_(redo); });
  }

  value rt_h::node_alloc(type const & ty, label const & eh) const
  {
    value const head = CyMem_FreeList;
    if_(
        head == (*char_t)(nullptr)
      , [&] { goto_(eh); }
      , [&] { CyMem_FreeList = *bitcast(head, **char_t); }
      );
    return bitcast(head, ty);
  }
}}
