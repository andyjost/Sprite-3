#include <iosfwd>
#include <exception>

namespace sprite { namespace curry
{

  struct ParseError : std::exception
    { char const * what() const throw() { return "parse error"; } };
  
  struct Library;

  /// Parse a Curry library from ICurry.
  std::istream & operator>>(std::istream & ifs, Library & lib);
}}
