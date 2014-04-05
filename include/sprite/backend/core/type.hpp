#pragma once
#include "sprite/backend/config.hpp"
#include "sprite/backend/core/object.hpp"
#include "sprite/backend/core/operator_flags.hpp"
#include "sprite/backend/support/array_ref.hpp"
#include "sprite/backend/support/special_values.hpp"
#include "sprite/backend/support/type_traits.hpp"
#include "llvm/IR/DerivedTypes.h"

namespace sprite { namespace backend
{
  /**
   * @brief Wrapper for @p llvm::Type objects.
   *
   * Elaborated types can be created by using the @p * operator (to create
   * pointers), the @p [] operator (to create arrays), or the @p () operator
   * (to create functions).
   *
   * Constant values can be created using the call operator, as in
   * <tt>i32(42)</tt> to create a 32-bit integer constant with value @p 42
   * (assuming @p i32 is a type wrapper for the 32-bit integer type).
   */
  template<typename T> struct typeobj : object<T>
  {
    using basic_type = Type;
    using object<T>::object;

    /**
     * @brief Creates a pointer type.
     *
     * @snippet types.cpp Creating pointer types
     */
    pointer_type operator*() const;
  
    /**
     * @brief Creates an array type.
     *
     * @snippet types.cpp Creating array types
     */
    array_type operator[](size_t size) const;

    /**
     * @brief Creates a function type.
     *
     * If the last argument is @p dots, then the function is marked varargs.
     *
     * @param[in] argtypes
     *   An argument pack containing any number of arguments that are
     *   convertible to llvm::Type *.
     *
     * @snippet types.cpp Creating function types
     */
    template<
        typename... Args
      , SPRITE_ENABLE_FOR_FUNCTION_PROTOTYPES(Args...)
      >
    function_type operator()(Args &&... argtypes) const;

    /**
     * @brief Creates a constant of the type represented by @p this, using @p
     * arg as the initializer.
     *
     * Synonymous with @p get_constant.  Implemented in get_constant.hpp.
     */
    template<
        typename Arg
      , SPRITE_ENABLE_FOR_CONSTANT_INITIALIZERS(Arg)
      >
    constant operator()(Arg &&) const;

    /**
     * @brief Creates a constant of the (array) type represented by @p this,
     * using @p arg as the initializer.
     *
     * Synonymous with @p get_constant.  Implemented in get_constant.hpp.  This
     * needs to be mentioned explicitly so that @p std::initializer_list will
     * be accepted.
     */
    constant operator()(any_array_ref const &) const;

  private:

    static_assert(
        std::is_base_of<basic_type, T>::value, "Expected an LLVM Type object"
      );
  };

  inline bool operator==(type const & lhs, type const & rhs)
    { return lhs.ptr() == rhs.ptr(); }

  inline bool operator!=(type const & lhs, type const & rhs)
    { return !(lhs == rhs); }

  /// Returns the size in bytes.
  uint64_t sizeof_(type const &);

  // Array query functions.
  // Returns the array element type.
  type element_type(array_type const &);
  type_with_flags element_type(array_type_with_flags const &);

  /// Returns the array length.
  uint64_t len(array_type const &);
  /// Removes all array extents.
  type remove_all_extents(array_type const &);

  // Pointer query functions.
  // Returns the pointer element type.
  type element_type(pointer_type const &);

  // Function query functions.
  // Returns the function result type.
  type result_type(function_type const &);
  // Returns the type of function parameter i.
  type element_type(function_type const &, unsigned);
  // Returns the number of function parameters.
  unsigned len(function_type const &);

  // Struct query functions.
  /// Returns the number of elements in the struct.
  uint64_t len(struct_type const &);
  /// Returns the type at index i.
  type element_type(struct_type const &, unsigned i);

  // Generic query functions.
  type_with_flags element_type(type_with_flags const &);
  type element_type(type const &, unsigned);
  uint64_t len(type const &);
  type remove_all_extents(type const &);
  type result_type(type const &);
}}

namespace sprite { namespace backend { namespace types
{
  /**
   * @brief Creates an integer type of the specified size (default is the
   * native size).
   */
  integer_type int_(unsigned numBits = sizeof(int) * 8);

  /// Creates an integer type the width of a native long.
  integer_type long_();

  /// Creates an integer type the width of a native long long.
  integer_type long_long();

  /// Creates a char type.
  integer_type char_();

  /// Creates a bool type.
  integer_type bool_();

  /**
   * @brief Creates a floating-point type (32-bit, by default).
   *
   * @p numBits should be 32, 64 or 128.
   */
  fp_type float_(size_t numBits);

  /// Creates the 32-bit floating-point type.
  fp_type float_();

  /// Creates the 64-bit floating-point type.
  fp_type double_();

  /// Creates the 128-bit floating-point type.
  fp_type long_double();

  /// Creates the void type.
  type void_();

  /** 
   * @brief Creates an anonymous struct (uniqued by structural equivalence).
   *
   * @snippet types.cpp Creating an anonymous struct
   */
  struct_type struct_(array_ref<type> const & elements);

  /**
   * @brief Gets a struct by name.
   *
   * If the struct has not been created, a new opaque struct will be created.
   *
   * @snippet types.cpp Creating opaque structs
   */
  struct_type struct_(string_ref const & name);

  /**
   * @brief Defines a struct.
   *
   * The struct must not already have a body (though, it may exist and be
   * opaque).
   *
   * @snippet types.cpp Creating structs
   */
  struct_type struct_(
      string_ref const & name, array_ref<type> const & elements
    );
}}}

namespace sprite { namespace backend
{
  //@{
  /**
   * @brief For a static type in the host language, constructs the
   * corresponding type in the target language.
   */
  template<typename T>
  inline typename std::enable_if<
      std::is_integral<T>::value && !std::is_same<T, bool>::value, integer_type
    >::type
  get_type()
    { return types::int_(sizeof(T) * 8); }

  template<typename T>
  inline typename std::enable_if<std::is_same<T, bool>::value, integer_type>::type
  get_type()
    { return types::bool_(); }

  template<typename T>
  inline typename std::enable_if<std::is_floating_point<T>::value, fp_type>::type
  get_type()
    { return types::float_(sizeof(T) * 8); }

  template<typename T>
  inline typename std::enable_if<std::is_array<T>::value, array_type>::type
  get_type()
  {
    type const base_type = get_type<typename std::remove_extent<T>::type>();
    constexpr size_t n = std::extent<T>::value;
    return base_type[n];
  }
  template<typename T>
  inline typename std::enable_if<std::is_pointer<T>::value, pointer_type>::type
  get_type()
  {
    using elem_type = typename std::remove_pointer<T>::type;
    return *get_type<elem_type>();
  }

  template<typename T>
  inline typename std::enable_if<std::is_same<T, void>::value, type>::type
  get_type()
    { return types::void_(); }

  /// Creates an anonymous struct type from a tuple of static types.
  template<typename T>
  inline typename std::enable_if<is_tuple<T>::value, struct_type>::type
  get_type();

  /// Creates an anonymous struct type from a sequence of 0 or 2 or more types.
  template<typename...T>
  inline typename std::enable_if<(sizeof...(T) != 1), struct_type>::type
  get_type()
    { return get_type<std::tuple<T...>>(); }
  //@}

  //@{
  /**
   * @brief Gets a type from any suitable argument.
   *
   * If the input object is already an LLVM Type, then its API pointer value
   * is extracted.  Otherwise, i.e., in the case of a raw initializer, a
   * constant value is created using @p get_constant.
   */
  // Applies when T can produce an LLVM Type.
  template<typename T>
  inline type get_type(T const & arg
    , typename std::enable_if<is_typearg<T>::value, En_>::type = En_()
    )
  { return type(ptr(arg)); }

  // Applies when T can produce an LLVM Value.
  template<typename T>
  inline type get_type(T const & arg
    , typename std::enable_if<is_valuearg<T>::value, En_>::type = En_()
    )
  { return type(ptr(arg)->getType()); }

  // Applies when T is a raw initializer (e.g., an int).  Returns the
  // best-matching LLVM type.
  template<typename T>
  inline type get_type(T const & arg
    , typename std::enable_if<
          !is_typearg<T>::value && !is_valuearg<T>::value, En_
        >::type = En_()
    )
  { return get_type<typename std::decay<T>::type>(); }

  // Allows @p get_type to accept @p arg_with_flags.
  template<typename T>
  inline type get_type(aux::arg_with_flags<T> const & arg)
    { return get_type(arg.arg()); }
  //@}
}}

#include "sprite/backend/core/detail/type_impl.hpp"
