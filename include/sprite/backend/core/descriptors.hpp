/**
 * @file
 * @brief Implements the condition argument used for branching constructs.
 */

#pragma once
#include "sprite/backend/core/get_value.hpp"
#include "sprite/backend/core/label.hpp"
#include "sprite/backend/core/type.hpp"
#include "sprite/backend/core/value.hpp"
#include "sprite/backend/support/type_traits.hpp"

namespace sprite { namespace backend
{
  /**
   * @brief A label descriptor.
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
        throw compile_error("An empty label is required.");
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

  /**
   * @brief A condition descriptor for loops.
   *
   * Accepts the condition argument used for looping constructs.  The condition
   * may be a constant value or some code the yields a value, which is always
   * interpreted as a Boolean value.  The condition is suitable for
   * re-evaluation.
   */
  struct loop_condition
  {
    //// Builds a condition descriptor from a constant.
    template<typename T, SPRITE_ENABLE_FOR_ALL_CONSTANT_INITIALIZERS(T)>
    loop_condition(T && arg) : m_cond(get_constant(types::bool_(), arg)), m_blk()
      {}

    //// Builds a condition descriptor from a code block that generates a value.
    template<typename T>
    loop_condition(
        T && cond
      , typename std::enable_if<
            is_code_block_specifier<T>::value
          >::type* = nullptr
      )
      : m_cond(nullptr), m_blk(std::bind(converter(), cond))
    {}

    value const & get() const
    {
      if(!m_cond.ptr())
      {
        assert(m_blk);
        m_cond = m_blk();
        m_blk = std::function<value()>();
      }
      assert(m_cond.ptr());
      return m_cond;
    }
    operator value const &() const { return this->get(); }

  protected:

    loop_condition(value const & cond) : m_cond(cond), m_blk() {}

  private:

    // Helps convert a function returning a user-specified type to a function
    // returning value. 
    struct converter
    {
      template<typename F> value operator()(F && f)
      {
        // Generate the block.
        f();
        label l = scope::current_label();
        assert(!l->empty());
        // Convert the last instruction to a Boolean.
        return get_value(types::bool_(), &l->back());
      }
    };

    mutable value m_cond;
    mutable std::function<value()> m_blk;
  };

  /**
   * @brief A condition descriptor for branches.
   *
   * Accepts the condition argument used for branching constructs.  The
   * condition may be anything accepted by loop_condition, or a plain value.
   * The condition is not suitable for re-evaluation.
   */
  struct branch_condition : loop_condition
  {
    template<typename T, SPRITE_ENABLE_FOR_ALL_VALUE_INITIALIZERS(T)>
    branch_condition(T && arg) : loop_condition(get_value(types::bool_(), arg))
      {}

    template<typename T>
    branch_condition(
        T && cond
      , typename std::enable_if<
            is_code_block_specifier<T>::value
          >::type* = nullptr
      )
      : loop_condition(cond)
    {}
  };
}}
