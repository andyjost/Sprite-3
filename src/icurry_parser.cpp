#include "sprite/curryinput.hpp"
#include "sprite/icurry_parser.hpp"
#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>

#define SPRITE_SKIPSPACE(rv)                                     \
    while(!ifs.eof() && std::isspace(ifs.peek())) { ifs.get(); } \
    if(ifs.eof()) return rv;                                     \
  /**/

#define SPRITE_SKIPSPACE_EXPECTING(next)      \
    while(std::isspace(c)) { c = ifs.get(); } \
    if(c != next) throw ParseError();         \
  /**/

// The most-recently-read word of input.
static std::string word;

namespace sprite { namespace curry
{
  // Reads a double-quoted string.  Returns the ccontents without quotes.
  std::string read_quoted_string(std::istream & ifs)
  {
    int c = ifs.get();
    SPRITE_SKIPSPACE_EXPECTING('"')
  
    std::stringstream tmp;
    c = ifs.get();
    while(c && c != '"')
    {
      if(c == '\\')
        tmp.put(ifs.get());
      else
        tmp.put(c);
      c = ifs.get();
    }
    return tmp.str();
  }
  
  std::istream & operator>>(std::istream & ifs, Qname & qname)
  {
    int c = ifs.get();
    SPRITE_SKIPSPACE_EXPECTING('(')
    qname.module = read_quoted_string(ifs);
    c = ifs.get();
    SPRITE_SKIPSPACE_EXPECTING(',')
    qname.name = read_quoted_string(ifs);
    c = ifs.get();
    SPRITE_SKIPSPACE_EXPECTING(')')
    return ifs;
  }
  
  void read_import_list(std::istream & ifs, std::vector<std::string> & imports)
  {
    // The "import" keyword has already been read.
    std::stringstream tmp;
    int c = ifs.get();
    while(true)
    {
      while(std::isspace(c) && c != '\n') { c = ifs.get(); }
      if(!c || c == '\n') break;
      while(c && !std::isspace(c)) { tmp.put(c); c = ifs.get(); }
      imports.push_back(tmp.str());
      tmp.str("");
    }
  }
  
  void read_constructor(
      std::istream & ifs, std::string const & module_name, Constructor & constructor
    )
  {
    // The "constructor" keyword has already been read.
    // FIXME: The module name is not needed here.
    Qname qname;
    ifs >> qname;
    if(qname.module != module_name) throw ParseError();
    constructor.name = qname.name;
    ifs >> constructor.arity;
  }
  
  void read_path(std::istream & ifs, std::vector<Function::PathElem> & paths)
  {
    // The "path" keyword and path id have already been read.
    int c = ifs.get();
    SPRITE_SKIPSPACE_EXPECTING('[')
    c = ifs.get();
    Function::PathElem path;
  
    while(true)
    {
      SPRITE_SKIPSPACE_EXPECTING('(')
      ifs >> path.qname;
      c = ifs.get();
      SPRITE_SKIPSPACE_EXPECTING(',')
      ifs >> path.idx;
      c = ifs.get();
      SPRITE_SKIPSPACE_EXPECTING(')')
  
      paths.push_back(path);
  
      c = ifs.get();
      while(std::isspace(c)) { c = ifs.get(); }
  
      if(c == ',') { c = ifs.get(); continue; }
      else if(c == ']') break;
      else throw ParseError();
    }
  }
  
  Rule read_rule(std::istream & ifs)
  {
    char c;
    if(word == "Node")
    {
      Expr expr;
      ifs >> expr.qname;
  
      // Search until the end of the current line (or commoa or close paren) for
      // an open paren.
      for(c = ifs.peek(); c; c = ifs.peek())
      {
        if(c == '\n') { ifs.get(); return expr; }
        if(c == ')' || c == ',') return expr;
        if(std::isspace(c)) { ifs.get(); continue; }
        if(c == '(') { ifs.get(); break; }
        throw ParseError();
      }
  
      // Parse the first subexpression.
      ifs >> word;
      expr.args.push_back(read_rule(ifs));
  
      // Parse more subexpressions.
      for(c = ifs.get(); c; c = ifs.get())
      {
        if(std::isspace(c)) continue;
        if(c == ',')
        {
          ifs >> word;
          expr.args.push_back(read_rule(ifs));
        }
        else if(c == ')') return expr;
        else throw ParseError();
      }
    }
    else if(word == "Var")
    {
      Ref ref;
      ifs >> ref.pathid;
      return ref;
    }
    throw ParseError();
  }
  
  Definition read_definition(std::istream & ifs)
  {
    char c;
    if(word == "JumpTable")
    {
      Branch branch;
  
      // Read the prefix.
      while(std::isspace(ifs.peek())) { ifs.get(); }
      if(ifs.peek() == '[')
      {
        ifs.get();
        c = ifs.get();
        if(c != ']') throw ParseError();
        branch.prefix = "";
      }
      else
        branch.prefix = read_quoted_string(ifs);
  
      // Read the isflex property.
      ifs >> word;
      if(word == "True")
        branch.isflex = true;
      else if(word == "False")
        branch.isflex = false;
      else throw ParseError();
  
      // Read the path id.
      ifs >> word;
      if(word != "Var") throw ParseError();
      ifs >> branch.pathid;
  
      // Read the cases.
      while(true)
      {
        SPRITE_SKIPSPACE(branch)
        if(ifs.peek() == '(')
        {
          Case case_;
          ifs >> case_.qname;
          ifs >> word;
          case_.action = read_definition(ifs);
          branch.cases.push_back(std::move(case_));
        }
        else break;
      }
      return branch;
    }
    else if(word == "Node" || word == "Var")
      return read_rule(ifs);
    else throw ParseError();
    ifs >> word;
  }
  
  void read_function(
      std::istream & ifs, std::string const & module_name, Function & function
    )
  {
    // The "function" keyword has already been read.
    // FIXME: The module name is not needed here.
    Qname qname;
    ifs >> qname;
    if(qname.module != module_name) throw ParseError();
    function.name = qname.name;
    ifs >> function.arity;
  
    // Parse declare / [path ...]
    ifs >> word;
    if(word != "declare") throw ParseError();
    while(true)
    {
      ifs >> word;
      if(word == "path")
      {
        size_t id;
        ifs >> id;
        function.paths[id];
        read_path(ifs, function.paths[id]);
      }
      else break;
    }
  
    // Parse the rules.
    function.def = read_definition(ifs);
  }
  
  std::istream & operator>>(std::istream & ifs, Module & mod)
  {
    // The "module" keyword has already been read.
    mod.name = read_quoted_string(ifs);
    while(true)
    {
      ifs >> word;
      if(word == "import")
        read_import_list(ifs, mod.imports);
      else if(word == "constructor")
      {
        // FIXME: the constructors are not grouped into data types.
        mod.datatypes.emplace_back();
        static size_t i = 0;
        mod.datatypes.back().name = "_typename" + std::to_string(i++);
        mod.datatypes.back().constructors.emplace_back();
        read_constructor(ifs, mod.name, mod.datatypes.back().constructors.back());
      }
      else if(word == "function")
      {
        mod.functions.emplace_back();
        read_function(ifs, mod.name, mod.functions.back());
      }
      else if(word == "module")
        return ifs;
      else throw ParseError();

      word.clear();
      SPRITE_SKIPSPACE(ifs)
    }
  }
  
  std::istream & operator>>(std::istream & ifs, Library & lib)
  {
    SPRITE_SKIPSPACE(ifs)
    std::ios::iostate const flags = ifs.exceptions();
  
    try
    {
      ifs.exceptions(std::ios::failbit | std::ios::badbit);
      ifs >> word;
      while(true)
      {
        if(word != "module") throw ParseError();
        lib.modules.emplace_back();
        ifs >> lib.modules.back();
        if(ifs.eof()) return ifs;
      }
      ifs.exceptions(flags);
      return ifs;
    }
    catch(...)
    {
      ifs.exceptions(flags);
      throw ParseError();
    }
  }
}}
