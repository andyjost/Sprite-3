// Contains an example from the LLVM documentation.  This demonstrates how the
// "ref" class translates into various GEP instructions.  See
// http://llvm.org/docs/GetElementPtr.html.

//     struct munger_struct {
//       int f1;
//       int f2;
//     };
//     void munge(struct munger_struct *P) {
//       P[0].f1 = P[1].f1 + P[2].f2;
//     }

#include "sprite/backend.hpp"

using namespace sprite::backend;

int main()
{
  module m("munger");
  scope _ = m;

  type void_ = types::void_();
  type int_ = types::int_();
  type munger_struct = types::struct_("munger_struct", {int_ /*f1*/, int_ /*f2*/});
  enum MUNGER_MEMBER { F1, F2 };

  extern_(
      void_(*munger_struct), "munger", {"P"}
    , [&] {
        value P = arg("P");
        P[0].dot(F1) = P[1].dot(F1) + P[2].dot(F2);
      }
    );
  extern_(
      int_(), "main", {}
    , [&]{
        return_(0);
      }
    );

  m.ptr()->dump();
  return 0;
}

