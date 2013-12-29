/**
 * @file
 * @brief Defines type erasure constructs @p any_array_ref and @p any_tuple_ref.
 */

#pragma once
#include "sprite/llvm/config.hpp"
#include "sprite/llvm/core/wrappers_fwd.hpp"
#include "sprite/llvm/support/wrap.hpp"
#include <functional>
#include <memory>
#include <tuple>
#include <type_traits>

namespace sprite { namespace llvm
{
  namespace aux
  {
    template<typename Factory=type_factory> struct any_arrayref_impl;
    template<typename Factory=type_factory> struct any_tupleref_impl;
  }

  /**
   * @brief Holds an @p ArrayRef<T> for any @p T.
   *
   * The template parameter @p Factory defaulted to a forward-declared class
   * prevents complaints from the compiler about incomplete types in methods
   * that return a wrapper object.
   */
  typedef aux::any_arrayref_impl<> any_array_ref;

  /**
   * @brief An alias for @p any_array_ref.
   *
   * Simplifies the use of @p std::initializer_list as the right-hand side of
   * operators (other than assignment), when a generic array is acceptable.
   *
   * @snippet misc.cpp Using _a and _t
   */
  typedef any_array_ref _a;

  /**
   * @brief Holds a reference to @p std::tuple<Ts...> for any @p Ts.
   *
   * The template parameter @p Factory defaulted to a forward-declared class
   * prevents complaints from the compiler about incomplete types in methods
   * that return a wrapper object.
   */
  typedef aux::any_tupleref_impl<> any_tuple_ref;

  /**
   * @brief an alias for std::make_tuple.
   *
   * For symmetry with @p _a, this simplifies construction of a @p std::tuple
   * as a right-hand side argument.
   *
   * @snippet misc.cpp Using _a and _t
   */
  template<typename...T>
  inline auto _t(T && ... ts) -> decltype(std::make_tuple(ts...))
    { return std::make_tuple(ts...); }

  namespace aux
  {
    /**
     * @brief Implements @p any_array_ref.
     *
     * Since the storage is always the same size for all types, this is
     * optimized to store the @p model instance in place within the main
     * object.
     */
    template<typename Policy, typename Factory>
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

      /// Visit from the modulo operation.
      constantobj<Constant, Factory>
      _accept_modulo(typeobj<ArrayType, Factory> const & tp) const
        { return this->store()->accept_modulo(tp); }

      constantobj<Constant, Factory>
      _accept_modulo(typeobj<StructType, Factory> const & tp) const
        { return this->store()->accept_modulo(tp); }

    protected:

      struct concept
      {
        concept() {}
        // Non-copyable.
        concept(concept const &) = delete;
        concept & operator=(concept const &) = delete;

        // Applies operator% (type instantiation) to the sequence.
        virtual constantobj<Constant, Factory>
            accept_modulo(typeobj<ArrayType, Factory> const &) const = 0;
        virtual constantobj<Constant, Factory>
            accept_modulo(typeobj<StructType, Factory> const &) const = 0;

        virtual size_t size() const = 0;
        virtual void copy_at(void * addr) const = 0;
        virtual ~concept() {}
      };

      template<typename...T>
      struct model : concept
      {
        // Indicates how the target object should be stored.
        typedef typename Policy::template storage<T...>::type storage_type;

        // Indicates how the target object should be passed.
        typedef typename Policy::template reference<T...>::type reference_type;

        model(storage_type const & value) : m_obj(value) {}

        virtual size_t size() const override
          { return Policy::size(this->ref()); }

        virtual void copy_at(void * addr) const override
          { new(addr) model<T...>(m_obj); }

        virtual ~model() {}

        virtual constantobj<Constant, Factory>
        accept_modulo(typeobj<ArrayType, Factory> const & tp) const override
          { return _modulo(tp, this->ref()); }

        virtual constantobj<Constant, Factory>
        accept_modulo(typeobj<StructType, Factory> const & tp) const override
          { return _modulo(tp, this->ref()); }

        // Returns the object as a reference type.
        reference_type ref() const { return m_obj; }
        storage_type m_obj;
      };

      concept * store()
        { return reinterpret_cast<concept *>(&m_store); }

      concept const * store() const
        { return reinterpret_cast<concept const *>(&m_store); }

      // Every model object has the same size.
      typedef
          typename std::aligned_storage<sizeof(model<int>)>::type storage_type;
      storage_type m_store;
    };

    /// Indicates an @p ArrayRef should be stored by value in @p any_array_ref.
    struct array_ref_policy
    {
      template<typename...T> struct storage
        { typedef ArrayRef<T...> type; };
      template<typename...T> struct reference
        { typedef ArrayRef<T...> const & type; };
      template<typename...T>
      static size_t size(ArrayRef<T...> const & array)
        { return array.size(); }
    };

    template<typename Factory>
    struct any_arrayref_impl : any_containerref_impl<array_ref_policy, Factory>
    {
      template<typename T>
      any_arrayref_impl(ArrayRef<T> const & value) { init(value); }

      /// Construct an any_arrayref_impl from a SmallVector.
      template<typename T, typename U>
      any_arrayref_impl(const llvm_::SmallVectorTemplateCommon<T, U> &Vec)
        { init(ArrayRef<T>(Vec)); }

      /// Construct an any_arrayref_impl from a std::vector.
      template<typename T, typename A>
      any_arrayref_impl(const std::vector<T, A> &Vec)
        { init(ArrayRef<T>(Vec)); }

      /// Construct an any_arrayref_impl from a std::initializer_list.
      template<typename T>
      any_arrayref_impl(const std::initializer_list<T> &obj)
        { init(ArrayRef<T>(obj)); }

      /// Construct an any_arrayref_impl from a C array.
      template<typename T, size_t N>
      any_arrayref_impl(const T (&Arr)[N])
        { init(ArrayRef<T>(Arr)); }

    private:

      template<typename T>
      void init(ArrayRef<T> const & value)
      {
        typedef typename any_arrayref_impl::template model<T> model_type;
        new(this->store()) model_type(value);
      }
    };

    /// Indicates a @p tuple should be stored by reference in @p any_tuple_ref.
    struct tuple_ref_policy
    {
      template<typename...T> struct storage
        { typedef std::reference_wrapper<std::tuple<T...> const> type; };
      template<typename...T> struct reference
        { typedef std::tuple<T...> const & type; };
      template<typename...T>
      static size_t size(std::tuple<T...> const &)
        { return std::tuple_size<std::tuple<T...>>::value; }
    };

    template<typename Factory>
    struct any_tupleref_impl : any_containerref_impl<tuple_ref_policy, Factory>
    {
      template<typename...T>
      any_tupleref_impl(std::tuple<T...> const & value)
      {
        typedef typename any_tupleref_impl::template model<T...> model_type;
        new(this->store()) model_type(std::cref(value));
      }
    };
  }
}}
