#pragma once
#include "sprite/backend/config.hpp"
#include "sprite/backend/core/scope.hpp"
#include "sprite/backend/core/value.hpp"
#include "sprite/backend/support/exceptions.hpp"
#include "sprite/backend/support/type_traits.hpp"
#include "llvm/IR/BasicBlock.h"
#include <functional>

namespace sprite { namespace backend
{
  struct label : valueobj<llvm::BasicBlock>
  {
    explicit label(std::nullptr_t) : valueobj<llvm::BasicBlock>(nullptr)
    {}

    /// Creates a new BasicBlock in the current function scope.
    explicit label(twine const & name = "")
      : valueobj<llvm::BasicBlock>(init(name)), m_next(nullptr)
    {}

    label(label && arg)
      : valueobj<llvm::BasicBlock>(arg), m_next(arg.m_next)
    {
      arg.px = nullptr;
      arg.m_next = nullptr;
    }

    label & operator=(label && arg)
    {
      this->term();
      this->px = arg.px;
      this->m_next = arg.m_next;
      arg.px = nullptr;
      arg.m_next = nullptr;
      return *this;
    }

    label(label const &) = delete;
    label & operator=(label const &) = delete;

    /// Creates a new BasicBlock and fills it by calling the function.
    // Implicit conversion is allowed.
    template<
        typename T
      , typename = typename std::enable_if<
            is_code_block_specifier<T>::value
          >::type
      >
    label(T && body, twine const & name = "")
      : valueobj<llvm::BasicBlock>(init(name))
    {
      scope _ = *this;
      body();
      *this = std::move(scope::current_label());
    }

    ~label() { term(); }

    // Not for public use.
    void _set_continuation(label const & tgt)
    {
      if(m_next)
        throw runtime_error("Multiple continuations.");
      m_next = tgt.ptr();
      // assert(tgt);
      // if(this->px && !this->px->getTerminator())
      // {
      //   llvm::IRBuilder<> bldr(this->px);
      //   SPRITE_APICALL(bldr.CreateBr(tgt.ptr()));
      // }
    }

  private:

    // using valueobj<llvm::BasicBlock>::valueobj;
    label(llvm::BasicBlock * bb)
      : valueobj<llvm::BasicBlock>(bb)
    {}

    template<typename T> friend class globalobj;

    llvm::BasicBlock * init(twine const &);
// Implements the destructor.  Appends an unconditional jump to m_next if
    // no terminator was specified.  Deletes the block if it is empty and has
    // no predecessors.
    void term();

    // This variable specifies the optional block that logically follows this
    // one by default.  If no terminator instruction was added during block
    // construction, then the destructor for this block will append an
    // uncondition jump to the continuation target.
    llvm::BasicBlock * m_next = nullptr;
  };

  /**
   * @brief Used to accept code block specifiers as function arguments.
   *
   * Accepting an argument of type label && has the effect of creating a new
   * code block and filling it before the function call is made.  Sometimes a
   * delay is necessary, such as when the called function will provide a new
   * (function) scope in which to evaluate the code block.
   */
  using codeblock = std::function<void()>;
}}

