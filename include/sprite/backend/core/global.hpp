#pragma once
#include "sprite/backend/config.hpp"
#include "sprite/backend/core/constant.hpp"
#include "sprite/backend/core/ref.hpp"
#include "sprite/backend/support/type_erasures.hpp"
#include "llvm/IR/GlobalValue.h"
#include "llvm/IR/GlobalVariable.h"

namespace sprite { namespace backend
{
  // FIXME: becuase globalvar uses basic_reference as its base, but this class
  // uses constobj, there is no automatic conversion, and things like
  // operator[] do not easily forward.  Can globalobj use basic_reference, too
  // (i.e., even for functions?).
  template<typename T> struct globalobj : constobj<T>
  {
    using basic_type = GlobalValue;
    using constobj<T>::constobj;

    /// Sets the initializer for a global variable (excluding arrays).
    template<typename U, typename = DISABLE_IF_ARRAY_LIKE(U)>
    globalvar set_initializer(U const & value);

    /// Sets the initializer for a global array variable.
    globalvar set_initializer(any_array_ref const & value);

    /**
     * @brief Takes the address of a global value.
     *
     * @snippet constexprs.cpp Computing addresses
     */
    constant operator&() const;

  private:

    static_assert(
        std::is_base_of<basic_type, T>::value
      , "Expected an LLVM GlobalValue object"
      );
  };

  template<> struct globalobj<GlobalVariable> : basic_reference<globalvaraddr>
  {
    // using basic_reference<globalobj<T>>::basic_reference;
    using basic_reference<globalvaraddr>::basic_reference;

    /// Sets the initializer for a global variable (excluding arrays).
    template<typename U, typename = DISABLE_IF_ARRAY_LIKE(U)>
    globalvar & set_initializer(U const & value);

    /// Sets the initializer for a global array variable.
    globalvar & set_initializer(any_array_ref const & value);
  };
}}

#include "sprite/backend/core/detail/global_impl.hpp"

