#pragma once
#include "sprite/backend/core/fwd.hpp"
#include "sprite/backend/support/exceptions.hpp"
#include "sprite/backend/support/type_traits.hpp"
#include "llvm/ADT/ArrayRef.h"
#include <initializer_list>
#include <memory>
#include <type_traits>

namespace sprite { namespace backend
{
  // Defined below.
  namespace aux { template<typename Derived> struct check_extents; }

  /**
   * @brief Extends llvm::ArrayRef<T>.
   *
   * The LLVM @p ArrayRef cannot be constructed from
   * <tt>std::initializer_list</tt> or <tt>std::array</tt>.  This class is
   * nearly identical, but adds additional constructors and handles array types
   * slightly differently.
   *
   * The second template parameter is the required array extent (if any).  It
   * is used when handling cases where T is itself an array type of fixed
   * extent.  Zero indicates no constraint.
   */
  // Implementation note: the second template paramter should be a simple
  // size_t, but that triggers an internal error in GCC 4.8.1, for some reason.
  // Using integral_constant is a simple workaround.
  template<typename T, typename Extent>
  struct array_ref
    : llvm::ArrayRef<T>
    , private aux::check_extents<array_ref<T,Extent>>
  {
    using llvm::ArrayRef<T>::ArrayRef;
    using value_type = T; 
    static size_t constexpr required_extent = Extent::value;
    template<typename> friend class aux::check_extents;

    array_ref(std::initializer_list<T> args)
      : llvm::ArrayRef<T>(args.begin(), args.size())
    {}

    // Corrects the buggy implementation provided by LLVM.
    template<typename A>
    array_ref(const std::vector<T, A> &Vec)
      : llvm::ArrayRef<T>(
            Vec.empty() ? (T*)0 : std::addressof(Vec.front())
          , Vec.size()
          )
    {}

    /// Initializes from std::array.
    template<size_t N>
    array_ref(std::array<T,N> const & args)
      : llvm::ArrayRef<T>(args.begin(), N)
    {}

    /// Accept zero-sized arrays.
    array_ref(T(&arr)[0])
      : llvm::ArrayRef<T>()
    {}
  };

  /**
   * @brief Implement <tt>array_ref<T[N]></tt> as
   * <tt>array_ref<array_ref<T,N>></tt>.
   *
   * Also, add an annotation to the inner type to check the array extent on
   * construction.
   */
  template<typename T, size_t N, typename Extent>
  struct array_ref<T[N], Extent>
    // This messy type is just the above, replacing T[N] with array_ref<T,N>
    // (almost).
    : array_ref<
          array_ref<T, std::integral_constant<size_t,N>>, Extent
        >
  {
    using array_ref<
        array_ref<T, std::integral_constant<size_t,N>>, Extent
      >::array_ref;
  };

  /**
   * @brief Disallow <tt>array_ref<T[]></tt>.
   *
   * The array extent is required, since @p array_ref is already the first
   * dimension, which is the only one that can be unspecified.
   */
  template<typename T, typename Extent>
  struct array_ref<T[],Extent>
    : array_ref<array_ref<T>, Extent>
  { using array_ref<array_ref<T>, Extent>::array_ref; };

  namespace aux
  {
    /**
     * @brief Validator run by @p array_ref during construction, after
     * <tt>llvm::ArrayRef</tt> has been constructed.
     */
    template<typename Derived> struct check_extents
    {
      check_extents()
      {
        if(Derived::required_extent)
        {
          Derived const * const this_ = static_cast<Derived const *>(this);
          if(Derived::required_extent != this_->size())
          {
            throw value_error(
                "Bad array extent in initializer (expected "
                  + std::to_string(Derived::required_extent)
                  + " but got " + std::to_string(this_->size()) + ")."
              );
          }
        }
      }
    };
  }
}}
