#include "Engine.hpp"

namespace Engine {

  std::stack<Stack_Item> mystack;

  // backtrack a rewrite step
  bool backtrack() {
    while (! mystack.empty()) {
      Stack_Item& si = mystack.top();
      Node** indi = si.indi;
      Node* before = si.before;
      Node* after = si.after;
      mystack.pop();
      cout << "U " << after->to_s() << " <- " << before->to_s() << endl;
      // Restore as before the step 
      *indi = before;
      if (before->get_kind() == CHOICE) {
	// Find out if the reduction just backtracked 
	// reduced the choice to the left or right argument.
	// If it was not the right, reduce to the right now.
	//
	// This algorithm cannot distinguish the case when
	// the left and right arguments of the choice are the same.
	// However, in this case, there is no real choice.
	// The next test must check the right argument, not the left.

	// Watch out renaming arguments, arg1 is the first one !!!
	if (after != *((Choice*)before)->arg2) {
	  replace(indi, *((Choice*)before)->arg2);
	  return true;
	}
      }
    }
    return false;
  }

  Fail* Fail::instance = new Fail();

  int Variable::counter = 0;

}
