/**
 * @file
 * @brief Constructs for inserting instructions into a scope.
 */

#pragma once
#include "sprite/backend/core/castexpr.hpp"
#include "sprite/backend/core/detail/current_builder.hpp"
#include "sprite/backend/core/get_value.hpp"
#include "sprite/backend/core/instruction.hpp"
#include "sprite/backend/core/label.hpp"
#include "sprite/backend/core/type.hpp"
#include "llvm/IR/Instructions.h"
#include <functional>

namespace sprite { namespace backend
{
  /// Appends a return instruction to the active label scope.
  template<typename T>
  void return_(T && arg)
  {
    llvm::IRBuilder<> & bldr = current_builder();
    type const retty = type(bldr.GetInsertBlock()->getParent()->getReturnType());
    value const x = get_value(arg, retty);
    SPRITE_APICALL(bldr.CreateRet(x.ptr()));
  }

  //@{
  /// Appends a conditional branch instruction to the active label scope.
  template<typename T>
  void if_(T && cond, label const & true_, label const & false_)
  {
    llvm::IRBuilder<> & bldr = current_builder();
    value const cond_ = get_value(cond, types::bool_());
    SPRITE_APICALL(bldr.CreateCondBr(cond_.ptr(), true_.ptr(), false_.ptr()));
  }

  template<typename T>
  void if_(T && cond, label const & true_)
  {
    label c;
    if_(cond, true_, c);
    scope::set_continuation(c);
  }
  //@}

  /// Appends an unconditional branch instruction to the active label scope.
  inline void goto_(label const & target)
  {
    llvm::IRBuilder<> & bldr = current_builder();
    SPRITE_APICALL(bldr.CreateBr(target.ptr()));
  }

  namespace aux
  {
    template<typename T, typename U, typename FInt, typename FFP>
    value dispatch_binary_op(T const & lhs, U const & rhs, FInt const & fint, FFP const & ffp)
    {
      llvm::IRBuilder<> & bldr = current_builder();
      type const ty = coerce(get_type(lhs), get_type(rhs));
      value lhs_ = get_value(lhs, ty);
      value rhs_ = get_value(rhs, ty);
      if(ty->isIntegerTy())
        return value(fint(bldr, lhs_.ptr(), rhs_.ptr()));
      else if(ty->isFloatingPointTy())
        return value(ffp(bldr, lhs_.ptr(), rhs_.ptr()));
      throw runtime_error("Expected an integer or FP type.");
    }
  }

  // !!!!!! FIXME - need to think about signed/unsigned !!!!!!
  /// Appends a comparison instruction to the active label scope.
  #define SPRITE_GENERATE_COMPARE_OP(op, cmp)                   \
      template<typename T, typename U>                          \
      value operator op(T const & lhs, U const & rhs)           \
      {                                                         \
        using namespace std::placeholders;                      \
        static auto fint = std::bind(                           \
            &llvm::IRBuilder<>::CreateICmp, _1                  \
          /* need to address signed, unsigned */                \
          , llvm::CmpInst::ICMP_S##cmp, _2, _3, ""              \
          );                                                    \
        static auto ffp = std::bind(                            \
            &llvm::IRBuilder<>::CreateFCmp, _1                  \
          , llvm::CmpInst::FCMP_O##cmp, _2, _3, ""              \
          );                                                    \
        return aux::dispatch_binary_op(lhs, rhs, fint, ffp);    \
      }                                                         \
    /**/
  SPRITE_GENERATE_COMPARE_OP(<=, LE)
  SPRITE_GENERATE_COMPARE_OP(<, LT)
  // SPRITE_GENERATE_COMPARE_OP(==, EQ)
  // SPRITE_GENERATE_COMPARE_OP(!=, NE)
  SPRITE_GENERATE_COMPARE_OP(>, GT)
  SPRITE_GENERATE_COMPARE_OP(>=, GE)
  #undef SPRITE_GENERATE_COMPARE_OP

  #if 0
  template<typename T, typename U>
  value operator+(T const & lhs, U const & rhs)
  {
    llvm::IRBuilder<> & bldr = current_builder();
    type const ty = coerce(get_type(lhs), get_type(rhs));
    value lhs_ = get_value(lhs, ty);
    value rhs_ = get_value(rhs, ty);
    if(ty->isIntegerTy())
      return value(
          SPRITE_APICALL(bldr.CreateAdd(lhs_.ptr(), rhs_.ptr()))
        );
    else if(ty->isFloatingPointTy())
      return value(
          SPRITE_APICALL(bldr.CreateFAdd(lhs_.ptr(), rhs_.ptr()))
        );
    throw runtime_error("Expected an integer or FP type.");
  }

  template<typename T, typename U>
  value operator-(T const & lhs, U const & rhs)
  {
    llvm::IRBuilder<> & bldr = current_builder();
    type const ty = coerce(get_type(lhs), get_type(rhs));
    value lhs_ = get_value(lhs, ty);
    value rhs_ = get_value(rhs, ty);
    if(ty->isIntegerTy())
      return value(
          SPRITE_APICALL(bldr.CreateSub(lhs_.ptr(), rhs_.ptr()))
        );
    else if(ty->isFloatingPointTy())
      return value(
          SPRITE_APICALL(bldr.CreateFSub(lhs_.ptr(), rhs_.ptr()))
        );
    throw runtime_error("Expected an integer or FP type.");
  }
  #endif
}}
