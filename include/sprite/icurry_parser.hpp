#include <iosfwd>
#include <exception>

namespace sprite { namespace curry
{

  struct ParseError : std::exception
    { char const * what() const throw() { return "parse error"; } };
  
  struct Library;
  struct Function;

  /// Parse a Curry library from ICurry.
  std::istream & operator>>(std::istream & ifs, Library & lib);

  /// Parse a Curry Function from ICurry.
  std::istream & operator>>(std::istream & ifs, Function & fun);
}}
