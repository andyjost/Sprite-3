/**
 * @file
 * @brief Defines type erasure constructs @p AnyArrayRef and @p AnyTupleRef.
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
    template<typename Factory=TypeFactory> struct AnyArrayRefImpl;
    template<typename Factory=TypeFactory> struct AnyTupleRefImpl;
  }

  /**
   * @brief Holds an @p ArrayRef<T> for any @p T.
   *
   * The template parameter @p Factory defaulted to a forward-declared class
   * prevents complaints from the compiler about incomplete types in methods
   * that return a wrapper object.
   */
  typedef aux::AnyArrayRefImpl<> AnyArrayRef;

  /**
   * @brief An alias for @p AnyArrayRef.
   *
   * Simplifies the use of @p std::initializer_list as the right-hand side of
   * operators (other than assignment), when a generic array is acceptable.
   *
   * @snippet misc.cpp Using _a and _t
   */
  typedef AnyArrayRef _a;

  /**
   * @brief Holds a reference to @p std::tuple<Ts...> for any @p Ts.
   *
   * The template parameter @p Factory defaulted to a forward-declared class
   * prevents complaints from the compiler about incomplete types in methods
   * that return a wrapper object.
   */
  typedef aux::AnyTupleRefImpl<> AnyTupleRef;

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
     * @brief Implements @p AnyArrayRef.
     *
     * Since the storage is always the same size for all types, this is
     * optimized to store the @p Model instance in place within the main
     * object.
     */
    template<typename Policy, typename Factory>
    struct AnyContainerRefImpl
    {
    protected:

      // OK for the base class to use, but only if this object will be
      // immediately initialized in the constructor body.
      AnyContainerRefImpl() {}

    public:

      /// Copy.
      AnyContainerRefImpl(AnyContainerRefImpl const & arg)
        { arg.store()->copy_at(this->store()); }

      /// Assignment.
      AnyContainerRefImpl& operator=(AnyContainerRefImpl const & arg)
      {
        if(this != &arg)
        {
          this->store()->~Concept();
          arg.store()->copy_at(this->store());
        }
        return *this;
      }
      
      /// Destroy.
      ~AnyContainerRefImpl()
        { this->store()->~Concept(); }

      /// Sequence size.
      size_t size() const { return this->store()->size(); }

      /// Visit from the modulo operation.
      ConstantWrapper<Constant, Factory>
      _accept_modulo(TypeWrapper<ArrayType, Factory> const & tp) const
        { return this->store()->accept_modulo(tp); }

      ConstantWrapper<Constant, Factory>
      _accept_modulo(TypeWrapper<StructType, Factory> const & tp) const
        { return this->store()->accept_modulo(tp); }

    protected:

      struct Concept
      {
        Concept() {}
        // Non-copyable.
        Concept(Concept const &) = delete;
        Concept & operator=(Concept const &) = delete;

        // Applies operator% (type instantiation) to the sequence.
        virtual ConstantWrapper<Constant, Factory>
            accept_modulo(TypeWrapper<ArrayType, Factory> const &) const = 0;
        virtual ConstantWrapper<Constant, Factory>
            accept_modulo(TypeWrapper<StructType, Factory> const &) const = 0;

        virtual size_t size() const = 0;
        virtual void copy_at(void * addr) const = 0;
        virtual ~Concept() {}
      };

      template<typename...T>
      struct Model : Concept
      {
        // Indicates how the target object should be stored.
        typedef typename Policy::template Storage<T...>::type storage_type;

        // Indicates how the target object should be passed.
        typedef typename Policy::template Reference<T...>::type reference_type;

        Model(storage_type const & value) : m_obj(value) {}

        virtual size_t size() const override
          { return Policy::size(this->ref()); }

        virtual void copy_at(void * addr) const override
          { new(addr) Model<T...>(m_obj); }

        virtual ~Model() {}

        virtual ConstantWrapper<Constant, Factory>
        accept_modulo(TypeWrapper<ArrayType, Factory> const & tp) const override
          { return _modulo(tp, this->ref()); }

        virtual ConstantWrapper<Constant, Factory>
        accept_modulo(TypeWrapper<StructType, Factory> const & tp) const override
          { return _modulo(tp, this->ref()); }

        // Returns the object as a reference type.
        reference_type ref() const { return m_obj; }
        storage_type m_obj;
      };

      Concept * store()
        { return reinterpret_cast<Concept *>(&m_store); }

      Concept const * store() const
        { return reinterpret_cast<Concept const *>(&m_store); }

      // Every Model object has the same size.
      typedef
          typename std::aligned_storage<sizeof(Model<int>)>::type storage_type;
      storage_type m_store;
    };

    /// Indicates an @p ArrayRef should be stored by value in @p AnyArrayRef.
    struct ArrayRefPolicy
    {
      template<typename...T> struct Storage
        { typedef ArrayRef<T...> type; };
      template<typename...T> struct Reference
        { typedef ArrayRef<T...> const & type; };
      template<typename...T>
      static size_t size(ArrayRef<T...> const & array)
        { return array.size(); }
    };

    template<typename Factory>
    struct AnyArrayRefImpl : AnyContainerRefImpl<ArrayRefPolicy, Factory>
    {
      template<typename T>
      AnyArrayRefImpl(ArrayRef<T> const & value) { init(value); }

      /// Construct an AnyArrayRefImpl from a SmallVector.
      template<typename T, typename U>
      AnyArrayRefImpl(const llvm_::SmallVectorTemplateCommon<T, U> &Vec)
        { init(ArrayRef<T>(Vec)); }

      /// Construct an AnyArrayRefImpl from a std::vector.
      template<typename T, typename A>
      AnyArrayRefImpl(const std::vector<T, A> &Vec)
        { init(ArrayRef<T>(Vec)); }

      /// Construct an AnyArrayRefImpl from a std::initializer_list.
      template<typename T>
      AnyArrayRefImpl(const std::initializer_list<T> &obj)
        { init(ArrayRef<T>(obj)); }

      /// Construct an AnyArrayRefImpl from a C array.
      template<typename T, size_t N>
      AnyArrayRefImpl(const T (&Arr)[N])
        { init(ArrayRef<T>(Arr)); }

    private:

      template<typename T>
      void init(ArrayRef<T> const & value)
      {
        typedef typename AnyArrayRefImpl::template Model<T> model_type;
        new(this->store()) model_type(value);
      }
    };

    /// Indicates a @p tuple should be stored by reference in @p AnyTupleRef.
    struct TupleRefPolicy
    {
      template<typename...T> struct Storage
        { typedef std::reference_wrapper<std::tuple<T...> const> type; };
      template<typename...T> struct Reference
        { typedef std::tuple<T...> const & type; };
      template<typename...T>
      static size_t size(std::tuple<T...> const &)
        { return std::tuple_size<std::tuple<T...>>::value; }
    };

    template<typename Factory>
    struct AnyTupleRefImpl : AnyContainerRefImpl<TupleRefPolicy, Factory>
    {
      template<typename...T>
      AnyTupleRefImpl(std::tuple<T...> const & value)
      {
        typedef typename AnyTupleRefImpl::template Model<T...> model_type;
        new(this->store()) model_type(std::cref(value));
      }
    };
  }
}}
