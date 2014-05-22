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
  /// Represents a branch target.
  struct label : valueobj<llvm::BasicBlock>
  {
    using valueobj<llvm::BasicBlock>::valueobj;

    /// Creates a new BasicBlock in the current function scope.
    explicit label(twine const & name = "")
      : valueobj<llvm::BasicBlock>(init(name)) // , m_next(nullptr)
    {}

    /// Creates a new BasicBlock and fills it by calling the function.
    // Implicit conversion is allowed.
    template<
        typename T
      , typename = typename std::enable_if<
            is_code_block_specifier<T>::value
          >::type
      >
    explicit label(T && body, twine const & name = "")
      : valueobj<llvm::BasicBlock>(init(name))
    {
      scope _ = *this;
      body();
    }

  private:

    template<typename T> friend class globalobj;
    llvm::BasicBlock * init(twine const &);
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

  /**
   * @brief An label descriptor.
   *
   * Used by certain functions that create elements of the CFG, such as @p if_.
   * The processes is devilishly complex if labels are allowed to be filled
   * before being added to the CFG, so this descriptor represents either an
   * empty label, or a code block that can be defered until after the proper
   * links have already been made.
   */
  struct labeldescr : label
  {
    labeldescr(label const & l) : label(l), m_body()
    {
      if(!l->empty())
        throw runtime_error("An empty label is required.");
    }

    labeldescr(codeblock const & b) : label(), m_body(b) {}

    template<
        typename T
      , typename = typename std::enable_if<
            is_code_block_specifier<T>::value
          >::type
      >
    labeldescr(T && body)
      : label(), m_body(body)
    {}

    void codegen() const
    {
      if(m_body)
      {
        scope _ = static_cast<label const &>(*this);
        m_body();
        m_body = codeblock();
      }
    }

  private:

    mutable codeblock m_body;
  };
}}

