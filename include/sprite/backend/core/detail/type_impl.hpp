#include "sprite/backend/core/detail/flag_checks.hpp"
#include "sprite/backend/core/operator_flags.hpp"
#include "sprite/backend/support/casting.hpp"
#include "sprite/backend/support/exceptions.hpp"
#include <limits>
#include <string>
#include <utility>

namespace sprite { namespace backend
{
  // ====== Implementation details for typeobj.
  template<typename T>
  pointer_type typeobj<T>::operator*() const
    { return pointer_type(SPRITE_APICALL(this->ptr()->getPointerTo())); }
    
  template<typename T>
  array_type typeobj<T>::operator[](size_t size) const
  {
    auto const size_ = static_cast<uint64_t>(size);
    return array_type(SPRITE_APICALL(ArrayType::get(this->ptr(), size_)));
  }

  //@{
  /// Checks that any ellipsis appears in the last position.
  template<typename...Args
    , typename = typename std::enable_if<sizeof...(Args) == 0>::type
    >
  constexpr bool ellipsis_is_last_arg()
    { return true; }

  template<typename Arg, typename...Args>
  constexpr bool ellipsis_is_last_arg()
  {
    return 
        (!std::is_same<Arg, ellipsis>::value || sizeof...(Args) == 0)
          && ellipsis_is_last_arg<Args...>()
      ;
  }
  //@}

  template<typename T>
  template<typename... Args, typename>
  function_type typeobj<T>::operator()(Args &&... argtypes) const
  {
    static_assert(
        ellipsis_is_last_arg<Args...>()
      , "The ellipsis (i.e., dots) may only appear in the last position when "
        "forming a funtion type."
      );
      
    Type * tmp[sizeof...(Args)]{ptr(std::forward<Args>(argtypes))...};
    Type ** end = &tmp[0] + sizeof...(Args);
    // If the last pointer is null, then the last argument was an ellipsis.
    bool const varargs = !*(end - 1);
    if(varargs) --end;
    array_ref<Type*> args(&tmp[0], end);
    return function_type(SPRITE_APICALL(
        FunctionType::get(this->ptr(), args, varargs)
      ));
  }

  inline type element_type(array_type const & ty)
    { return type(SPRITE_APICALL(ty->getElementType())); }

  inline type_with_flags element_type(array_type_with_flags const & ty)
    { return std::make_tuple(element_type(ty.arg()), ty.flags()); }

  inline uint64_t len(array_type const & ty)
    { return SPRITE_APICALL(ty->getNumElements()); }

  inline type remove_all_extents(array_type const & ty)
  {
    type elem = nullptr;
    array_type t = ty;
    while(true)
    {
      elem = element_type(t);
      t = dyn_cast<array_type>(elem);
      if(!t.ptr()) return elem;
    }
  }

  inline type element_type(pointer_type const & ty)
    { return type(SPRITE_APICALL(ty->getElementType())); }

  inline type result_type(function_type const & ty)
    { return type(SPRITE_APICALL(ty->getReturnType())); }

  inline type element_type(function_type const & ty, unsigned i)
    { return type(SPRITE_APICALL(ty->getParamType(i))); }

  inline unsigned len(function_type const & ty)
    { return SPRITE_APICALL(ty->getNumParams()); }

  inline uint64_t len(struct_type const & ty)
    { return SPRITE_APICALL(ty->getNumElements()); }

  inline type element_type(struct_type const & ty, unsigned i)
  {
    assert(ty->indexValid(i));
    return type(SPRITE_APICALL(ty->getTypeAtIndex(i)));
  }

  inline type_with_flags element_type(type_with_flags const & arg)
  {
    auto const a = dyn_cast<array_type>(arg);
    if(a.ptr()) return element_type(a);

    // The pointer type is not allowed to have flags.
    SPRITE_ALLOW_FLAGS(arg.flags(), "element_type", 0);

    auto const c = dyn_cast<pointer_type>(arg);
    if(c.ptr()) return element_type(c);
    throw type_error("Expected array or pointer type.");
  }

  inline type element_type(type const & arg, unsigned i)
  {
    auto const a = dyn_cast<function_type>(arg);
    if(a.ptr()) return element_type(a, i);
    throw type_error("Expected function type.");
  }

  inline uint64_t len(type const & arg)
  {
    auto const a = dyn_cast<array_type>(arg);
    if(a.ptr()) return len(a);
    auto const b = dyn_cast<struct_type>(arg);
    if(b.ptr()) return len(b);
    auto const c = dyn_cast<function_type>(arg);
    if(c.ptr()) return len(c);
    throw type_error("Expected array, function, or struct type.");
  }

  inline type remove_all_extents(type const & arg)
  {
    array_type const a = dyn_cast<array_type>(arg);
    if(a.ptr()) return remove_all_extents(a);
    throw type_error("Expected array type.");
  }

  inline type result_type(type const & arg)
  {
    auto const a = dyn_cast<function_type>(arg);
    if(a.ptr()) return result_type(a);
    throw type_error("Expected function type.");
  }

  namespace aux
  {
    /// Terminating case for building types from a tuple.
    template<typename T, size_t I=0, typename S>
    inline typename std::enable_if<I == std::tuple_size<T>::value, void>::type
    build_types(S &)
    {}
  
    /// Iterating case for building types from a tuple.
    template<typename T, size_t I=0, typename S>
    inline typename std::enable_if<I < std::tuple_size<T>::value, void>::type
    build_types(S & s)
    {
      using element_type = typename std::tuple_element<I, T>::type;
      static_assert(
          !std::is_array<element_type>::value
            || std::extent<element_type>::value != 0
        , "Struct elements that are arrays require extents."
        );
      s.push_back(get_type<typename std::tuple_element<I, T>::type>());
      return build_types<T, I+1, S>(s);
    }
  }

  template<typename T>
  inline typename std::enable_if<is_tuple<T>::value, struct_type>::type
  get_type()
  {
    llvm::SmallVector<type, std::tuple_size<T>::value> elems;
    aux::build_types<T>(elems);
    return types::struct_(elems);
  }
}}

namespace sprite { namespace backend { namespace types
{
  inline integer_type long_() { return int_(sizeof(long) * 8); }
  inline integer_type long_long() { return int_(sizeof(long long) * 8); }
  inline integer_type char_() { return int_(8); }
  inline integer_type bool_() { return int_(1); }
}}}

