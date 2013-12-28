/**
 * @file
 * @brief Defines wrapper types.
 */

#pragma once
#include <limits>
#include "llvm/IR/Constants.h"
#include "llvm/ADT/SmallVector.h"
#include "sprite/llvm/core/context.hpp"
#include "sprite/llvm/core/wrappers_fwd.hpp"
#include "sprite/llvm/core/value.hpp"
#include "sprite/llvm/support/enablers.hpp"
#include "sprite/llvm/support/special_values.hpp"
#include "sprite/llvm/support/type_erasures.hpp"
#include <tuple>
#include <type_traits>
#include <utility>

namespace sprite { namespace llvm
{
  /**
   * @brief Base class used to wrap an LLVM API object.
   *
   * It is assumed LLVM will manage the lifetime of the raw pointer.  An
   * instance of this type can be created directly, though normally it's easier
   * to use one of the TypeFactory::wrap methods.
   */
  template<typename T, typename Factory> class Wrapper
  {
  protected:

    /// The wrapped pointer to an LLVM object.
    T * px;

    /// The @p TypeFactory associated with this object.
    Factory fx;

  public:

    /// The underlying LLVM type.
    typedef T element_type;

    /// The type factory type.
    typedef Factory factory_type;

    /// Regular constructor to capture an LLVM API object.
    template<
        typename U
      , typename = typename std::enable_if<std::is_base_of<T,U>::value>::type
      >
    Wrapper(U * p, Factory const & factory) : px(p), fx(factory) {}

    /// Implicitly-converting constructor.
    template<
        typename U
      , typename = typename std::enable_if<std::is_base_of<T,U>::value>::type
      >
    Wrapper(Wrapper<U, Factory> const & arg)
      : px(arg.ptr()), fx(arg.factory())
    {}

    // Default copy and assignment are OK.

    friend bool operator==(Wrapper const & lhs, Wrapper const & rhs)
      { return lhs.px == rhs.px && lhs.fx == rhs.fx; }

    friend bool operator!=(Wrapper const & lhs, Wrapper const & rhs)
      { return !(lhs == rhs); }

    /// Explicit conversion to @p bool.  True if the wrapped pointer is valid.
    explicit operator bool() const { return px; }

    /// Conversion to the LLVM object.
    explicit operator T *() const { assert(px); return px; }

    /// Named conversion to the LLVM object.
    T * ptr() const { assert(px); return px; }

    /// Member access for the LLVM object.
    T * operator->() const { assert(px); return px; }

    /// Get the associated type factory.
    Factory const & factory() const { return fx; }
  };

  /**
   * @brief Represents a basic block in the target program.
   *
   * This class is the destination for instructions added to the target program.
   * A @p Context may be used to set the destination for several instructions.
   */
  template<typename Factory>
  struct BasicBlockWrapper : Wrapper<llvm_::BasicBlock, Factory>
  {
    typedef Wrapper<llvm_::BasicBlock, Factory> base_type;
    using base_type::base_type;
  };

  namespace aux
  {
    /**
     * @brief Represents a sequence of constant values.
     *
     * This object is intended to be used only as a temporary, as in a
     * right-hand side initializer expression, or as function call argument.
     */
    template<typename Factory>
    struct RvalueSequence : std::vector<llvm_::Constant *>
    {
      /// Adds a constant value to this sequence.
      template<typename T>
      RvalueSequence & operator,(ConstantWrapper<T, Factory> const & rhs)
      {
        this->push_back(rhs.ptr());
        return *this;
      }
    };
  }

  /**
   * @brief Wrapper for @p llvm::Constant objects.
   *
   * Instances are returned when the @p % operator of @p TypeWrapper is
   * used to create a constant value.
   *
   * Multiple constants can be joined with the @p , (comma) operator to form a
   * sequence of constants, which can be used to instantiate a struct or array.
   * For instance, a two-element array of 32-bit integers could be instantiated
   * from a sequence of constants as shown below:
   *
   * @snippet constants.cpp Instantiating an array from a sequence
   * 
   * Implicitly converts to @p llvm::Constant *, so it can be used anywhere an
   * LLVM constant is expected.
   */
  template<typename T, typename Factory> struct ConstantWrapper
    : Wrapper<T, Factory>
  {
    using Wrapper<T, Factory>::Wrapper;

    /// Creates a sequence of Constant objects, useful for aggregate initialization.
    template<typename U>
    aux::RvalueSequence<Factory>
    operator,(ConstantWrapper<U, Factory> const & rhs) const
    {
      aux::RvalueSequence<Factory> seq;
      return (seq, *this, rhs);
    }

    /// Get the type of this constant.
    TypeWrapper<Type, Factory> type() const
      { return wrap(this->factory(), (*this)->getType()); }

    /// Get the value of this constant as the specified type.
    template<typename Target>
    Target value() const { return sprite::llvm::value<Target>(*this); }

  private:

    static_assert(
        std::is_base_of<Constant, T>::value, "Expected an LLVM Constant object"
      );
  };

  /**
   * @brief Wraps an LLVM Instruction object.
   */
  template<typename T, typename Factory> struct InstructionWrapper
    : Wrapper<T, Factory>
  {
    using Wrapper<T, Factory>::Wrapper;

    template<typename U>
    InstructionWrapper(U * u)
      : Wrapper<T, Factory>(u, Factory()) {}

  private:

    static_assert(
        std::is_base_of<llvm_::Instruction, T>::value
      , "Expected an LLVM Instruction object"
      );
  };

  /**
   * @brief The result of @p operator[] applied to a global value.
   *
   * Represents some indexing done relative to a global value.  This object can
   * be further indexed with @p operator[] or its address taken with the unary
   * @p operator&, or with the function @p address_inbounds.
   */
  template<typename T, typename Factory> struct GlobalValueProxy
  {
    GlobalValueProxy operator[](int64_t i) const;

    template<typename Index>
    typename std::enable_if<aux::is_constarg<Index>(), GlobalValueProxy>::type
    operator[](Index const & i) const;

    ConstantWrapper<Constant, Factory> operator&() const;

    GlobalValueWrapper<T, Factory> const & get_base() const
      { return base; }

    llvm_::SmallVector<Constant *, 4> const & get_indices() const
      { return indices; }

  private:

    template<typename T_, typename Factory_> friend struct GlobalValueWrapper;

    GlobalValueProxy(GlobalValueWrapper<T, Factory> const & base_)
      : base(base_), indices()
    {}

    GlobalValueWrapper<T, Factory> base;
    llvm_::SmallVector<Constant *, 4> indices;
  };

  /**
   * @brief Wrapper for @p llvm::GlobalValue objects.
   *
   * Instances are returned by @ref def and the related functions @ref extern_,
   * @ref static_, and @ref inline_.  Note that a global value is characterized
   * by the presence of linkage, and oftentimes this wrapper holds a function
   * or global variable.
   *
   * For global variables, this wrapper allows the initializer to be set using
   * the @p = operator.  See the examples for the functions mentioned above.
   *
   * Implicitly converts to @p llvm::GlobalValue *, so it can be used anywhere
   * an LLVM global value is expected.
   */
  template<typename T, typename Factory> struct GlobalValueWrapper
    : ConstantWrapper<T, Factory>
  {
    using ConstantWrapper<T, Factory>::ConstantWrapper;

    /**
     * @brief Sets the initializer for a global variable from a single value.
     *
     * For initializing scalars, the value may be any legal initializer value
     * (i.e., anything LLVM will accept as an initializer for the given type).
     * For initializing aggregates, the "single value" could be a sequence of
     * constants created using the @p , (comma) operator of @ref
     * ConstantWrapper.
     *
     * @snippet defs.cpp Initializing a global from a value
     */
    template<typename U>
    GlobalValueWrapper & operator=(U const & value)
      { return this->set_initializer(value); }

    /**
     * @brief Sets the initializer for a global variable using an aggregate.
     *
     * All values of an aggregate must have the same type.  This form may be
     * used with a brace-enclosed initializer list.  Each element of the list
     * will be used to create a constant.
     *
     * @snippet defs.cpp Initializing a global from an aggregate
     */
    template<typename U>
    GlobalValueWrapper & operator=(std::initializer_list<U> values)
      { return this->set_initializer(values); }

    /**
     * @brief Sets the initializer for a global variable from heterogeneous
     * data.
     *
     * The values of the tuple can have different types.  This form may be used
     * to initialize an array, when the initializers do not all have the same
     * type, or it may be used to initialize a struct.
     *
     * @snippet defs.cpp Initializing a global from heterogeneous data
     */
    template<typename...U>
    GlobalValueWrapper & operator=(std::tuple<U...> const & values)
      { return this->set_initializer(values); }

    /**
     * @brief Sets the initializer for a global variable from a single value.
     *
     * Synonymous with @p operator=.
     */
    template<typename U>
    GlobalValueWrapper & set_initializer(U const & value);

    /**
     * @brief Sets the initializer for a global variable using an aggregate.
     *
     * Synonymous with @p operator=.
     */
    template<typename U>
    GlobalValueWrapper &
    set_initializer(std::initializer_list<U> const & values);

    /**
     * @brief Sets the initializer for a global variable from heterogeneous data.
     *
     * Synonymous with @p operator=.
     */
    template<typename...U>
    GlobalValueWrapper &
    set_initializer(std::tuple<U...> const &);

    /// Begins indexing into a global value.
    GlobalValueProxy<T, Factory> operator[](int64_t i) const;

    /// Begins indexing into a global value.
    template<typename Index>
    typename std::enable_if<
        aux::is_constarg<Index>(), GlobalValueProxy<T, Factory>
      >::type
    operator[](Index const & i) const;

    /**
     * @brief Takes the address of a global value.
     *
     * @snippet constexprs.cpp Computing addresses
     */
    ConstantWrapper<Constant, Factory> operator&() const;

  private:

    static_assert(
        std::is_base_of<llvm_::GlobalValue, T>::value
      , "Expected an LLVM GlobalValue object"
      );
  };

  /**
   * @brief Specializes @p GlobalValueWrapper for functions.
   */
  template<typename Factory>
  struct GlobalValueWrapper<llvm_::Function, Factory>
    : ConstantWrapper<llvm_::Function, Factory>
  {
    typedef ConstantWrapper<llvm_::Function, Factory> base_type;
    using base_type::base_type;

    /// Returns the function entry point.
    BasicBlockWrapper<Factory> entry() const
    {
      assert(this->px);
      if(this->px->empty())
      {
        llvm_::BasicBlock::Create(
            this->factory().context(), ".entry", this->px
          );
      }
      assert(!this->px->empty());
      return wrap(this->factory(), &this->px->front());
    }

    // Inserts a call instruction in the current context.
    template<typename... Args>
    InstructionWrapper<llvm_::CallInst, Factory>
      operator()(Args &&... args) const;
  };

  /**
   * @brief Wrapper for @p llvm::Type objects.
   *
   * Instances are returned when @p TypeFactory produces a new type.
   *
   * Elaborated types can be created by using the @p * operator (to create
   * pointers), the @p [] operator (to create arrays), or the @p () operator
   * (to create functions).
   *
   * Constant values can be created using the @p % operator, as in <tt>i32 %
   * 42</tt> to create a 32-bit integer constant with value @p 42 (assuming
   * @p i32 is a type wrapper for the 32-bit integer type).
   *
   * Implicitly converts to llvm::Type *, so it can be used anywhere an LLVM
   * type is expected.
   */
  template<typename T, typename Factory> struct TypeWrapper
    : Wrapper<T, Factory>
  {
    using Wrapper<T, Factory>::Wrapper;

    /**
     * @brief Creates a pointer type.
     *
     * @snippet types.cpp Creating pointer types
     */
    TypeWrapper<PointerType, Factory> operator*() const;

    /**
     * @brief Creates an array type.
     *
     * @snippet types.cpp Creating array types
     */
    TypeWrapper<ArrayType, Factory> operator[](uint64_t size) const;

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
    template<typename... Args>
    TypeWrapper<llvm_::FunctionType, Factory> operator()(Args &&... argtypes) const;

  private:

    static_assert(
        std::is_base_of<Type, T>::value, "Expected an LLVM Type object"
      );
  };
}}

// Include the implementation details.
#include "sprite/llvm/core/wrappers_impl.hpp"

