#pragma once
#include "sprite/backend/core/fwd.hpp"
#include <memory>

namespace sprite { namespace backend
{
  /**
   * @brief Used during function creation to specify which basic block is
   * currenty under construction.
   *
   * A basic block is created by defining a new lexical block, placing an
   * instance of Scope inside it, and, finally, specifying the commands as a
   * series of simple statements.
   *
   * @snippet hello_world.cpp Using scope
   */
  class scope
  {
  public:
    // Normal object management.  Movable but not copyable or
    // stack-constructible.
    scope(scope && arg);
    scope(scope const &) = delete;
    scope & operator=(scope const &) = delete;
    void * operator new(size_t) = delete;
    void operator delete(void *) = delete;
    ~scope();

    // Scope creation.
    scope(module);
    scope(function);
    scope(label);

    // Scope access.
    static module current_module();
    static function current_function();
    static label current_label();

    /**
     * @brief Replaces the current label with a new one, following a branch
     * instruction.
     */
    static void update_current_label_after_branch(label const &);

    /// Sets the default continuation.
    static void set_continuation(label const & src, label const & tgt);

    // The context comes from the current module, but exposing it in this way
    // may allow some compilation units to avoid a dependency on module.hpp.
    static llvm::LLVMContext & current_context();

    struct frame;

  private:

    std::unique_ptr<frame> m_frame;
  };
}}
