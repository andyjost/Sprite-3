#pragma once
namespace sprite { namespace compiler
{
  // Provides the rule for getting the vtable symbol name for a built-in type.
  // The basenames come from builtins.def.
  inline std::string get_vt_name(std::string const & basename)
    { return ".vt." + basename; }
}}
