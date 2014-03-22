#pragma once
#include "sprite/backend/config.hpp"
#include "sprite/backend/core/constant.hpp"
#include "sprite/backend/support/type_erasures.hpp"
#include "llvm/IR/GlobalValue.h"
#include "llvm/IR/GlobalVariable.h"

namespace sprite { namespace backend
{
  namespace aux
  {
    /**
     * @brief The result of @p operator[] applied to a global variable.
     *
     * Represents some indexing done relative to a global variable.  This
     * object can be further indexed with @p operator[] or its address taken
     * with the unary @p operator&.
     */
    template<typename T> struct address_calculation
    {
      // Note: operator[] modifies the index list in place.
      address_calculation & operator[](size_t i);

      // Note: operator[] modifies the index list in place.
      template<typename Index>
      typename std::enable_if<is_constarg<Index>::value, address_calculation &>::type
      operator[](Index const & i);

      constant operator&() const;

      globalobj<T> const & get_base() const
        { return base; }

      llvm::SmallVector<Constant *, 4> const & get_indices() const
        { return indices; }

    private:

      template<typename T_> friend struct globalobj;

      address_calculation(globalobj<T> const & base_)
        : base(base_), indices()
      {}

      globalobj<T> base;
      llvm::SmallVector<Constant *, 4> indices;
    };
  }

  template<typename T> struct globalobj : constobj<T>
  {
    using basic_type = GlobalValue;
    using constobj<T>::constobj;

    /// Sets the initializer for a global variable (excluding arrays).
    template<typename U, typename = DISABLE_IF_ARRAY_LIKE(U)>
    globalvar set_initializer(U const & value);

    /// Sets the initializer for a global array variable.
    globalvar set_initializer(any_array_ref const & value);

    /// Begins indexing into a global value.
    aux::address_calculation<GlobalVariable> operator[](size_t i) const;

    /// Begins indexing into a global value.
    template<typename Index>
    typename std::enable_if<
        is_constarg<Index>::value, aux::address_calculation<GlobalVariable>
      >::type
    operator[](Index const & i) const;

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

  template<> struct globalobj<GlobalVariable> : constobj<GlobalVariable>
  {
    using constobj<GlobalVariable>::constobj;

    /// Sets the initializer for a global variable (excluding arrays).
    template<typename U, typename = DISABLE_IF_ARRAY_LIKE(U)>
    globalvar & set_initializer(U const & value);

    /// Sets the initializer for a global array variable.
    globalvar & set_initializer(any_array_ref const & value);

    /// Begins indexing into a global variable.
    aux::address_calculation<GlobalVariable> operator[](size_t i) const;

    /// Begins indexing into a global variable.
    template<typename Index>
    typename std::enable_if<
        is_constarg<Index>::value, aux::address_calculation<GlobalVariable>
      >::type
    operator[](Index const & i) const;

    /**
     * @brief Takes the address of a global variable.
     *
     * @snippet constexprs.cpp Computing addresses
     */
    constant operator&() const;
  };
}}

#include "sprite/backend/core/detail/global_impl.hpp"

