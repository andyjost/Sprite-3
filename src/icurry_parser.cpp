#include "sprite/curryinput.hpp"
#include "sprite/icurry_parser.hpp"
#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>
#include <limits>

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
  template<typename T> void base_one_to_zero(T & t)
  {
    if(t == 0)
      throw ParseError();
    --t;
  }

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
    SPRITE_SKIPSPACE(ifs);
    switch(ifs.peek())
    {
      // Parse "module.name" form.
      case '"':
      {
        std::string full = read_quoted_string(ifs);
        size_t pos = full.find('.');
        if(full.find('.', pos+1) != std::string::npos) throw ParseError();
        qname.module = full.substr(0, pos);
        qname.name = full.substr(pos+1);
        break;
      }
      // Parse ("module", "name") form.
      case '(':
      {
        int c = ifs.get();
        qname.module = read_quoted_string(ifs);
        c = ifs.get();
        SPRITE_SKIPSPACE_EXPECTING(',')
        qname.name = read_quoted_string(ifs);
        c = ifs.get();
        SPRITE_SKIPSPACE_EXPECTING(')')
        break;
      }
      default: throw ParseError();
    }
    return ifs;
  }
  
  void read_import_list(std::istream & ifs, std::vector<std::string> & imports)
  {
    // The "import" keyword has already been read.
    std::stringstream tmp;
    // Read up to the newline.
    ifs.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
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
  
  size_t read_variable_ref(std::istream & ifs)
  {
    size_t id;
    ifs >> id;
    base_one_to_zero(id);
    return id;
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
    else if(word == "var")
    {
      Ref ref;
      ref.pathid = read_variable_ref(ifs);
      return ref;
    }
    throw ParseError();
  }

  Definition read_definition(std::istream & ifs)
  {
    char c;
  redo:
    if(word == "ATable")
    {
      Branch branch;
  
      // Read the prefix.
      size_t unused;
      ifs >> unused;
  
      // Read the isflex property.
      ifs >> word;
      if(word == "flex")
        branch.isflex = true;
      else if(word == "rigid")
        branch.isflex = false;
      else throw ParseError();
  
      // Read the path id.
      ifs >> word;
      if(word != "var") throw ParseError();
      branch.pathid = read_variable_ref(ifs);
  
      // Read the cases.
      while(true)
      {
        SPRITE_SKIPSPACE(branch)
        c = ifs.peek();
        if(c == '(' || c == '"')
        {
          Case case_;
          ifs >> case_.qname;
          ifs >> word;
          if(word != "=>") throw ParseError();
          ifs >> word;
          case_.action = read_definition(ifs);
          branch.cases.push_back(std::move(case_));
        }
        else break;
      }
      return branch;
    }
    else if(word == "return")
    {
      ifs >> word;
      return read_rule(ifs);
    }
    else if(word == "declare_lhs_var" || word == "comment")
    {
      // Ignore the line.
      ifs.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      ifs >> word;
      goto redo;
    }
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
  
    // Parse table / [variable ...]
    ifs >> word;
    if(word != "table") throw ParseError();
    while(true)
    {
      ifs >> word;
      if(word == "variable")
      {
        // Get the path index and resize the vector, if needed.
        size_t id;
        ifs >> id;
        base_one_to_zero(id);
        if(id >= function.paths.size())
          function.paths.resize(id+1);

        // Get the base path.
        Function::PathElem & path = function.paths[id];
        ifs >> path.base;
        if(path.base == 0)
          path.base = curry::nobase;
        else
          base_one_to_zero(path.base);

        // Get the next index.
        ifs >> path.idx;
        base_one_to_zero(path.idx);
      }
      else break;
    }

    // Parse literal 'code'.
    if(word != "code") throw ParseError();

    // Discard comment lines.
    while(true)
    {
      ifs >> word;
      if(word == "comment")
      {
        // Read up to the newline.
        ifs.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
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

    redo_no_input:

      if(word == "import")
        read_import_list(ifs, mod.imports);
      else if(word == "data")
      {
        mod.datatypes.emplace_back();
        static size_t i = 0;
        mod.datatypes.back().name = "_typename" + std::to_string(i++);
        while(ifs >> word)
        {
          if(word != "constructor") goto redo_no_input;
          mod.datatypes.back().constructors.emplace_back();
          read_constructor(ifs, mod.name, mod.datatypes.back().constructors.back());

          // EOF OK here.
          word.clear();
          SPRITE_SKIPSPACE(ifs)
        }
      }
      else if(word == "function")
      {
        mod.functions.emplace_back();
        read_function(ifs, mod.name, mod.functions.back());
      }
      else if(word == "module")
        return ifs;
      else throw ParseError();

      // EOF OK here.
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
