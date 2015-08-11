/**
 * @file
 * @brief Contains utility functions related to the trees defined in
 * curryinput.hpp.
 */

#include "sprite/curryinput.hpp"

namespace sprite { namespace curry
{
  bool contains_variable(Definition const &, size_t);

  // Identifies which pathids in a function should be passed to the aux
  // function for a given branch.  Returns a vector of pathids.
  std::vector<size_t> find_pathids_that_are_aux_arguments(
      Function const &, Branch const &
    );
}}
