#include "basic_runtime.hpp"
#include <boost/dynamic_bitset.hpp>
#include <memory>

extern "C"
{
  extern sprite::compiler::aux_t Cy_NextChoiceId;
}

namespace sprite { namespace compiler
{
  enum class ChoiceState { UNDETERMINED=0, LEFT=1, RIGHT=2 };

  // Stores a fingerprint, which inticates which choices have been made.
  //
  // For N choices, 2N bits are stored.  For choice K, bit 2K indicates whether
  // K is determined and bit 2K+1 indicates whether the determined choice is
  // left (bit unset) or right (bit set).
  struct Fingerprint
  {
    Fingerprint() : bits() {}

    std::shared_ptr<Fingerprint> clone() const
    {
      std::shared_ptr<Fingerprint> copy(new Fingerprint(*this));
      return copy;
    }

    void set_left(aux_t id) { check_alloc(id); set_left_no_check(id); }
    void set_right(aux_t id) { check_alloc(id); set_right_no_check(id); }

    void set_left_no_check(aux_t id) { bits.set(2*id); bits.reset(2*id+1); }
    void set_right_no_check(aux_t id) { bits.set(2*id); bits.set(2*id+1); }

    void check_alloc(aux_t id) const
    {
      size_t const needed = 2*(id+1);
      if(needed > bits.size()) { bits.resize(needed); }
    }

    ChoiceState test(aux_t id) const
    {
      check_alloc(id);
      return test_no_check(id);
    }

    ChoiceState test_no_check(aux_t id) const
    {
      return static_cast<ChoiceState>(
          (bits.test(2*id) ? 1 : 0) * (bits.test(2*id+1) ? 2 : 1)
        );
    }

    bool choice_is_made(aux_t id) const
    {
      size_t const id2 = 2*id;
      return id2 < bits.size() && bits.test(id2);
    }

    bool choice_is_left_no_check(aux_t id) const
      { return !bits.test(2*id+1); }

    size_t size() const { return bits.size() / 2; }

  private:

    mutable boost::dynamic_bitset<> bits;
  };
}}
