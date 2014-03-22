/**
 * @file
 * @brief Defines type erasure constructs @p any_array_ref and @p any_tuple_ref.
 */

#pragma once
#include "sprite/backend/config.hpp"
#include "sprite/backend/core/fwd.hpp"
#include "sprite/backend/core/constant.hpp"
#include "sprite/backend/core/operator_flags.hpp"
#include "sprite/backend/support/array_ref.hpp"
#include "sprite/backend/support/type_traits.hpp"
#include <cstring>
#include <functional>
#include <initializer_list>
#include <memory>
#include <tuple>
#include <type_traits>
#include <vector>

#define DISABLE_IF_ARRAY_LIKE(T)                          \
    typename std::enable_if<                              \
          !std::is_constructible<any_array_ref, T>::value \
        >::type                                           \
  /**/

#define ENABLE_IF_ARRAY_LIKE(T)                          \
    typename std::enable_if<                             \
          std::is_constructible<any_array_ref, T>::value \
        >::type                                          \
  /**/

namespace sprite { namespace backend
{
  struct any_array_ref;

  /**
   * @brief An alias for @p any_array_ref.
   *
   * Simplifies the use of @p std::initializer_list as the right-hand side of
   * operators (other than assignment), when a generic array is acceptable.
   *
   * @snippet misc.cpp Using _a and _t
   */
  using _a = any_array_ref;

  /**
   * @brief an alias for std::make_tuple.
   *
   * For symmetry with @p _a, this simplifies construction of a @p std::tuple
   * as a right-hand side argument.
   *
   * @snippet misc.cpp Using _a and _t
   */
  template<typename...T>
  inline auto _t(T && ... ts)
    -> decltype(std::make_tuple(std::forward<T>(ts)...))
    { return std::make_tuple(std::forward<T>(ts)...); }

  namespace aux
  {
    /**
     * @brief Implements @p any_array_ref.
     *
     * Since the storage is always the same size for all types, this is
     * optimized to store the @p model instance in place within the main
     * object.
     */
    template<typename Policy>
    struct any_containerref_impl
    {
    protected:

      // OK for the base class to use, but only if this object will be
      // immediately initialized in the constructor body.
      any_containerref_impl() {}

    public:

      /// Copy.
      any_containerref_impl(any_containerref_impl const & arg)
        { arg.store()->copy_at(this->store()); }

      /// Assignment.
      any_containerref_impl& operator=(any_containerref_impl const & arg)
      {
        if(this != &arg)
        {
          this->store()->~concept();
          arg.store()->copy_at(this->store());
        }
        return *this;
      }
      
      /// Destroy.
      ~any_containerref_impl()
        { this->store()->~concept(); }

      /// Sequence size.
      size_t size() const { return this->store()->size(); }

      /**
       * @brief Returns the data as a string_ref, if the contained type is some sort
       * of char array.  Otherwise, the the result holds a null pointer.
       */
      string_ref string() const { return this->store()->string(); }

      /// Visit from the modulo operation.
      constant _accept_modulo(array_type_with_flags const & ty) const
        { return this->store()->accept_modulo(ty); }

      constant _accept_modulo(struct_type const & ty) const
        { return this->store()->accept_modulo(ty); }

    protected:

      struct concept
      {
        concept() {}
        // Non-copyable.
        concept(concept const &) = delete;
        concept & operator=(concept const &) = delete;
        virtual ~concept() {}

        // Applies operator% (type instantiation) to the sequence.
        virtual constant accept_modulo(
            array_type_with_flags const &
          ) const = 0;
        virtual constant accept_modulo(struct_type const &) const = 0;

        virtual string_ref string() const = 0;
        virtual size_t size() const = 0;
        virtual void copy_at(void * addr) const = 0;
      };

      template<typename...T>
      struct model : concept
      {
        // Indicates how the target object should be stored.
        using storage_type = typename Policy::template storage<T...>::type;

        // Indicates how the target object should be passed.
        using reference_type = typename Policy::template reference<T...>::type;

        model(storage_type const & value) : m_obj(value) {}

        virtual ~model() {}

        virtual constant accept_modulo(
            array_type_with_flags const & ty
          ) const override
          { return ty % this->ref(); }

        virtual constant accept_modulo(struct_type const & ty) const override
          { return ty % this->ref(); }

        virtual string_ref string() const override
          { return Policy::string(this->ref()); }

        virtual size_t size() const override
          { return Policy::size(this->ref()); }

        virtual void copy_at(void * addr) const override
          { new(addr) model<T...>(m_obj); }

      private:

        // Returns the object as a reference type.
        reference_type ref() const { return m_obj; }
        storage_type m_obj;
      };

      concept * store()
        { return reinterpret_cast<concept *>(&m_store); }

      concept const * store() const
        { return reinterpret_cast<concept const *>(&m_store); }

      // Every model object has the same size.
      using storage_type =
          typename std::aligned_storage<sizeof(model<int>)>::type;
      storage_type m_store;
    };

    /// Indicates an @p array_ref should be stored by value in @p any_array_ref.
    struct array_ref_policy
    {
      template<typename...T> struct storage
        { using type = array_ref<T...>; };

      template<typename...T> struct reference
        { using type = array_ref<T...> const &; };

      template<typename...T>
      static size_t size(array_ref<T...> const & array)
        { return array.size(); }

      template<typename...T>
      static string_ref string(array_ref<T...> const & array)
        { return string_ref(); }

      template<typename...T>
      static string_ref string(array_ref<char, T...> const & array)
      {
        return string_ref(
            array.data(), ::strnlen(array.data(), array.size())
          );
      }
    };

    /// Indicates a @p tuple should be stored by reference in @p any_tuple_ref.
    struct tuple_ref_policy
    {
      template<typename...T> struct storage
        { using type = std::reference_wrapper<std::tuple<T...> const>; };

      template<typename...T> struct reference
        { using type = std::tuple<T...> const &; };

      template<typename...T>
      static size_t size(std::tuple<T...> const &)
        { return std::tuple_size<std::tuple<T...>>::value; }

      template<typename...T>
      static string_ref string(std::tuple<T...> const & array)
        { return string_ref(); }
    };
  }

  struct any_array_ref : aux::any_containerref_impl<aux::array_ref_policy>
  {
    template<typename T>
    any_array_ref(array_ref<T> const & value) { init(value); }

    /**
     * @brief Accept anything @p array_ref can use for construction, if it has
     * the nested typename @p value_type.
     *
     * This includes <tt>std::vector</tt> and <tt>llvm::SmallVector</tt>.
     */
    template<typename T
      , typename = typename std::enable_if<
            std::is_constructible<array_ref<typename T::value_type>, T>::value
          >::type
      >
    any_array_ref(T const & arg)
      { init(array_ref<typename T::value_type>(arg)); }

    //@{
    /// Construct from a possibly-nested <tt>std::initializer_list</tt>.
    template<typename T> any_array_ref(SPRITE_INIT_LIST1(T) obj) { init(obj); }
    template<typename T> any_array_ref(SPRITE_INIT_LIST2(T) obj) { init(obj); }
    template<typename T> any_array_ref(SPRITE_INIT_LIST3(T) obj) { init(obj); }
    template<typename T> any_array_ref(SPRITE_INIT_LIST4(T) obj) { init(obj); }
    template<typename T> any_array_ref(SPRITE_INIT_LIST5(T) obj) { init(obj); }
    template<typename T> any_array_ref(SPRITE_INIT_LIST6(T) obj) { init(obj); }
    template<typename T> any_array_ref(SPRITE_INIT_LIST7(T) obj) { init(obj); }
    template<typename T> any_array_ref(SPRITE_INIT_LIST8(T) obj) { init(obj); }
    template<typename T> any_array_ref(SPRITE_INIT_LIST9(T) obj) { init(obj); }
    //@}

    /// Construct an any_array_ref from a C array.
    template<typename T, size_t N>
    any_array_ref(T const (&Arr)[N]) { init(array_ref<T>(Arr)); }

  private:

    template<typename ArrayLike> void init(ArrayLike const & value)
    {
      using model_type = model<typename ArrayLike::value_type>;
      new(this->store()) model_type(value);
    }
  };

  struct any_tuple_ref : aux::any_containerref_impl<aux::tuple_ref_policy>
  {
    template<typename...T>
    any_tuple_ref(std::tuple<T...> const & value)
      { new(this->store()) model<T...>(std::cref(value)); }
  };
}}
