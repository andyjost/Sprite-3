#include "sprite/curryinput.hpp"
#include "sprite/icurry_parser.hpp"
#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>
#include <limits>

// Enable to add debug output statements.
#define DEBUGSTMT(stmt)
// #define DEBUGSTMT(stmt) stmt

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

  bool isodigit(int c)
  {
    switch(c)
    {
      case '0': case '1': case '2': case '3':
      case '4': case '5': case '6': case '7':
        return true;
    }
    return false;
  }

  char read_escape(std::istream & ifs)
  {
    switch(ifs.get())
    {
      case '\'': return '\'';
      case '\"': return '\"';
      case '?':  return '?';
      case '\\': return '\\';
      case 'a':  return '\a';
      case 'b':  return '\b';
      case 'f':  return '\f';
      case 'n':  return '\n';
      case 'r':  return '\r';
      case 't':  return '\t';
      case 'v':  return '\v';
      case 'X':
      {
        // hex
        std::string s;
        while(std::isxdigit(ifs.peek()))
        {
          if(s.size() == 2)
          {
            std::cerr
              << "wide hex character sequences are not supported"
              << std::endl;
            throw ParseError();
          }
          s.push_back(ifs.get());
        }
        if(s.size() == 0)
          throw ParseError();
        return static_cast<char>(std::stoul(s, nullptr, 16));
      }
      case 'u': case 'U':
        std::cerr << "Unicode character literal not supported." << std::endl;
        break;
      case '0': case '1': case '2': case '3': case '4':
      case '5': case '6': case '7': case '8': case '9':
      {
        // octal
        std::string s;
        while(isodigit(ifs.peek()) && s.size() != 3)
          s.push_back(ifs.get());
        if(s.size() == 0)
          throw ParseError();
        return static_cast<char>(std::stoul(s, nullptr, 8));
      }
    }
    throw ParseError();
  }

  // Reads a string quoted with delim.  Returns the contents without
  // quotes.
  std::string read_quoted_string(std::istream & ifs, char delim = '"')
  {
    int c = ifs.get();
    SPRITE_SKIPSPACE_EXPECTING(delim)
  
    std::stringstream tmp;
    c = ifs.get();
    while(c && c != delim)
    {
      if(c == '\\')
        tmp.put(read_escape(ifs));
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
    while(true)
    {
      ifs >> word;
      if(word == "data" || word == "function")
        return;
      imports.push_back(word);
    }
    // std::stringstream tmp;
    // // Read up to the newline.
    // ifs.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    // int c = ifs.get();
    // while(true)
    // {
    //   while(std::isspace(c) && c != '\n') { c = ifs.get(); }
    //   if(!c || c == '\n') break;
    //   while(c && !std::isspace(c)) { tmp.put(c); c = ifs.get(); }
    //   word = tmp.str();
    //   // Note: "data", being a reserved keyword, is not a valid module name.
    //   // It indicates an empty import list, here.
    //   if(word == "data")
    //     return false;
    //   imports.push_back(word);
    //   tmp.str("");
    // }
    // return true;
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
    DEBUGSTMT(
        std::cout << "Read constructor \"" << qname.str() << "\"" << std::endl;
      )
  }
  
  size_t read_variable_ref(std::istream & ifs)
  {
    size_t id;
    ifs >> id;
    base_one_to_zero(id);
    return id;
  }

  Rule return_term(bool is_partial, Term && term)
  {
    if(is_partial)
      return Partial(term);
    else
      return term;
  }

  Rule read_rule(std::istream & ifs)
  {
    char c;
    bool const is_partial = (word == "partial");
    bool const is_choice = (word == "Or");
    if(word == "Node" || is_partial || is_choice)
    {
      Term term;
      if(is_partial)
      {
        // Discard the effective arity.
        int64_t x;
        ifs >> x;
      }
      else if(is_choice)
      {
        // Treat the literal Or as if it were a node called "Prelude.?".  That
        // node will not fail symbol lookup, but it creates an infinite cycle.
        // That will be fixed by modifying the symbol table for Prelude.? to
        // refer to the built-in choice node.
        term.qname = Qname{"Prelude", "?"};
      }
      else
      {
        // Otherwise read the node label.
        ifs >> term.qname;
      }
  
      // Search until the end of the current line (or comma or close paren) for
      // an open paren.
      for(c = ifs.peek(); c; c = ifs.peek())
      {
        if(c == '\n')
          { ifs.get(); return return_term(is_partial, std::move(term)); }
        if(c == ')' || c == ',')
          return return_term(is_partial, std::move(term));
        if(std::isspace(c)) { ifs.get(); continue; }
        if(c == '(') { ifs.get(); break; }
        throw ParseError();
      }

      // Parse the first subexpression.
      ifs >> word;
      if(is_partial)
      {
        Rule rule = read_rule(ifs);
        if(!rule.getterm()) throw ParseError();
        term = *rule.getterm();
      }
      else
        term.args.push_back(read_rule(ifs));

      // Parse more subexpressions.
      for(c = ifs.get(); c; c = ifs.get())
      {
        if(std::isspace(c)) continue;
        if(c == ',')
        {
          // There should only be one arg list for partials.
          if(is_partial) throw ParseError();
          ifs >> word;
          term.args.push_back(read_rule(ifs));
        }
        else if(c == ')')
          return return_term(is_partial, std::move(term));
        else throw ParseError();
      }
    }
    else if(word == "var")
    {
      Ref ref;
      ref.pathid = read_variable_ref(ifs);
      return ref;
    }
    else if(word == "exempt")
      return curry::Fail();
    else if(word == "char")
    {
      std::stringstream ss(read_quoted_string(ifs, '\''));
      if(ss.str().size() != 1)
        throw ParseError();
      return ss.str()[0];
    }
    else if(word == "int")
    {
      int64_t x;
      ifs >> x;
      return x;
    }
    else if(word == "float")
    {
      double x;
      ifs >> x;
      return x;
    }
    throw ParseError();
  }

  Term read_term(std::istream & ifs)
  {
    Rule rule = read_rule(ifs);
    if(Term const * term = rule.getterm())
      return *term;
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
      size_t num;
      ifs >> num; // unused

      // Read the number of cases.
      ifs >> num;
  
      // Read the isflex property.
      ifs >> word;
      if(word == "flex")
        branch.isflex = true;
      else if(word == "rigid")
        branch.isflex = false;
      else throw ParseError();
  
      // Read the branch condition.
      ifs >> word;
      branch.condition = read_rule(ifs);
      if(!branch.condition.getvar() && !branch.condition.getterm())
        throw ParseError();
  
      // Read 'num' cases.
      for(; num; --num)
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
        else throw ParseError();
      }
      return branch;
    }
    else if(word == "return")
    {
      ifs >> word;
      return read_rule(ifs);
    }
    else if(word == "initialize" || word == "forward")
    {
      NLTerm nlterm;
      while(word == "initialize" || word == "forward" || word == "assign")
      {
        bool const was_forward = word == "forward";
        NLTerm::Step step;
        if(word == "assign")
        {
          // Parse: varid
          step.varid = read_variable_ref(ifs);
        }
        else
        {
          // Parse: (varid,_,'IBind').
          int c = ifs.get();
          SPRITE_SKIPSPACE_EXPECTING('(')
          step.varid = read_variable_ref(ifs);
          c = ifs.get();
          SPRITE_SKIPSPACE_EXPECTING(',')
          size_t unused;
          ifs >> unused;
          c = ifs.get();
          SPRITE_SKIPSPACE_EXPECTING(',')
          char cword[6];
          ifs.get(cword, sizeof(cword));
          if(std::string(cword) != "IBind") throw ParseError();
          c = ifs.get();
          SPRITE_SKIPSPACE_EXPECTING(')')
        }

        // Skip "forward" lines.
        ifs >> word;
        if(was_forward) continue;

        if(word != "Node") throw ParseError();
        step.term = read_term(ifs);
        nlterm.steps.push_back(step);

        ifs >> word;
      }

      // Skip "fill" lines.
      while(word == "fill")
      {
        // Parse: varid 'in' varid' \n at ... \n
        read_variable_ref(ifs);
        ifs >> word;
        if(word != "in") throw ParseError();
        read_variable_ref(ifs);
        ifs >> word;
        if(word != "at") throw ParseError();
        ifs.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        ifs >> word;
      }

      // Parse the result term.
      if(word != "return") throw ParseError();
      ifs >> word;
      // nlterm.result = read_term(ifs);
      nlterm.result.reset(new Rule(read_rule(ifs)));
      return Rule(nlterm);
    }
    else if(
        word == "declare_lhs_var" || word == "declare_free_var"
     || word == "comment"
     )
    {
      // Ignore the line.
      ifs.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      ifs >> word;
      goto redo;
    }
    else if(word == "external")
    {
      ExternalCall external;
      ifs >> external.qname;
      return Rule(external);
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
    if(!module_name.empty() && qname.module != module_name)
      throw ParseError();
    function.name = qname.name;
    ifs >> function.arity;
    DEBUGSTMT(
        std::cout << "Read function \"" << qname.str() << "\"" << std::endl;
      )
  
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
        ifs >> word;
        if(word == "IFree")
        {
          path.base = curry::freevar;
          path.idx = 0; // unused
        }
        else if(word == "IBind")
        {
          path.base = curry::bind;
          path.idx = 0; // unused
        }
        else
        {
          std::stringstream ss(word);
          ss.exceptions(ifs.exceptions());
          ss >> path.base;
          if(path.base == 0)
            path.base = curry::nobase;
          else
            base_one_to_zero(path.base);

          // Get the next index.
          ifs >> path.idx;
          base_one_to_zero(path.idx);

          // Get the term typename.  If there is a base path, read it from the
          // input.  Otherwise, it is the name of this function.
          if(path.base == curry::nobase)
            path.typename_ = qname;
          else
            ifs >> path.typename_;
        }
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

  std::istream & operator>>(std::istream & ifs, Function & fun)
  {
    read_function(ifs, "", fun);
    return ifs;
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
      {
        DEBUGSTMT(std::cout << "Reading imports" << std::endl;)
        read_import_list(ifs, mod.imports);
        goto redo_no_input;
      }
      else if(word == "data")
      {
        DEBUGSTMT(std::cout << "Reading data" << std::endl;)
        std::string name;
        ifs >> name;
        // Skip over empty "data" declarations.  If the next token is not
        // "constructor", then go back to matching.
        if(ifs >> word)
        {
          if(word != "constructor") goto redo_no_input;
          mod.datatypes.emplace_back();
          mod.datatypes.back().name = name;
          do
          {
            if(word != "constructor") goto redo_no_input;
            mod.datatypes.back().constructors.emplace_back();
            read_constructor(ifs, mod.name, mod.datatypes.back().constructors.back());

            // EOF OK here.
            word.clear();
            SPRITE_SKIPSPACE(ifs)
          } while(ifs >> word);
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
