// Implements @p Fingerprint as a tree to minimize copying.
//
// The tree comprises branch nodes and (leaf) block nodes.  Only blocks contain
// fingerprint data, indicating whether a particular choice has been made and
// whether it is LEFT or RIGHT.  Each block holds FP_BLOCK_SIZE elements of the
// fingerprint.  Internal (branch) nodes each have FP_BRANCH_SIZE children and
// a reference count, which is used to implement a copy-on-write policy.
// Early, when very few choice IDs have been introduced to the program, a
// fingerprint tree is nothing more than a block node.  As larger IDs are
// introduced, levels are added to the fingerprint trees so that they can
// accomodate more choices.  For instance, when the size first becomes larger
// than FP_BLOCK_SIZE, a branch node is inserted and filled with zeroed-out
// blocks.  Because of the copy-on-write policy, the FP_BRANCH_SIZE-1 new
// blocks are actually references to the same block.
//
// Given an ID, the corresponding block is located as follows based on the
// current tree depth.  First, we store the offset by selected the
// FP_BLOCK_MASK bits of the ID and right-shift those bits away.  Then, working
// from most significant to least significant bits, we select groups of
// FP_BRANCH_MASK bits.  Each group provides an index into branch node at the
// current depth (beginning with the root).  The depth of the fingerprint tree
// indicates how many groups to process.
//    
// A simple cache is implemented.  It stores the most-recently-used block and
// its associated tag.  When choices from the same block are used in a
// temporally localized fashion, this avoids repeatedly traversing the branches
// of the tree.
//
// Examples
// --------
//
// 1) A fingerprint tree with depth=0.  It contains FP_BLOCK_SIZE choices.
//
//    ==BLOCK==
//     used=...   (FP_BLOCK_SIZE bits indicating which IDs are used)
//     lr=...     (FP_BLOCK_SIZE bits indicating LEFT/RIGHT for used IDs).
//    =========
//    
// 2) A fingerprint tree with depth=1 (depicted for FP_BRANCH_SIZE=4).  It
// contains FP_BRANCH_SIZE * FP_BLOCK_SIZE choices.  Each increment of the
// depth increases the number of IDs by a factor of FP_BRANCH_SIZE.
//    
//                     ====BRANCH====
//                      refcount=...
//                     ==============
//                    /    /    \    \
//                 /      /      \      \
//              /        /        \        \
//           /          /          \          \
//        /            /            \            \
//    ==BLOCK==    ==BLOCK==    ==BLOCK==    ==BLOCK==
//     used=...     used=...     used=...     used=...
//     lr=...       lr=...       lr=...       lr=...
//    =========    =========    =========    =========
//

#pragma once
#include <cassert>
#include "basic_runtime.hpp"
#include <boost/integer.hpp>
#include <boost/integer/static_log2.hpp>
#include "boost-pool-1.46/pool.hpp"

// The results of using a fingerprint cache appear mixed but overall
// beneficial.
#define USE_FP_CACHE

extern "C"
{
  extern sprite::compiler::aux_t Cy_NextChoiceId;
}

namespace sprite { namespace compiler
{
  enum class ChoiceState { UNDETERMINED=0, LEFT=1, RIGHT=2 };

  // Each bit block contains however many choices fit into one pointer (each
  // choice requires two bits).  On x86_64, for instance, there are 32 choices
  // per block.
  auto constexpr FP_BLOCK_SIZE = sizeof(void*)*8/2;
  auto constexpr FP_BLOCK_SHIFT = boost::static_log2<FP_BLOCK_SIZE>::value;
  auto constexpr FP_BLOCK_MASK = FP_BLOCK_SIZE-1;
  
  // Each branch has four successors.  FP_BRANCH_SHIFT can be tuned.
  auto constexpr FP_BRANCH_SHIFT = 2;
  auto constexpr FP_BRANCH_SIZE = 1<<FP_BRANCH_SHIFT;
  auto constexpr FP_BRANCH_MASK = FP_BRANCH_SIZE-1;

  auto constexpr FP_CACHE_TAG_MASK = ~FP_BLOCK_MASK;

  namespace fingerprints
  {
    struct Branch;

    // The pool used to allocate branches.
    extern boost::pool<> branch_pool;

    // Stores FP_BLOCK_SIZE bits of fingerprint data.  If the @p used bit is set,
    // then the corresponding choice is made.  If so, then @p lr indicates
    // whether that choice is left or right.
    struct Block
    {
      boost::int_t<FP_BLOCK_SIZE>::exact used = 0;
      boost::int_t<FP_BLOCK_SIZE>::exact lr = 0;
    };

    // Holds either a branch or block (leaf).
    union Node
    {
      Block block;
      Branch * branch;

      Node() : block() {}
      Node(Node const & arg, size_t depth);

      // Conventional copy is forbidden (the depth argument is required).
      Node(Node const & arg) = delete;
      Node(Node && arg) = delete;
      Node & operator=(Node const & arg) = delete;
      Node & operator=(Node && arg) = delete;
    };
    
    // Stores FP_BRANCH_SIZE successors and a reference count.
    struct Branch
    {
      Node next[FP_BRANCH_SIZE];
      size_t refcount;

      Branch(size_t refs) : next(), refcount(refs) {}

      // Construct a terminal branch.  The first block of bits is copied.  The
      // rest are set to zero.
      Branch(Block const & block, size_t refs) : next(), refcount(refs)
        { next[0].block = block; }

      // Construct a non-terminal branch, up one level in the tree, where all
      // successors point to @p branch.
      Branch(Branch * branch, size_t refs) : refcount(refs)
      {
        for(size_t i=0; i<FP_BRANCH_SIZE; ++i)
          next[i].branch = branch;
        branch->refcount += FP_BRANCH_SIZE;
      }

      // Construct a non-terminal branch, up one level in the tree, where the
      // first branch points to @p branch and the rest point to @p rest.
      Branch(Branch * first, Branch * rest, size_t refs) : refcount(refs)
      {
        next[0].branch = first; // Note: no change to first->refcount.
        for(size_t i=1; i<FP_BRANCH_SIZE; ++i)
          next[i].branch = rest;
        rest->refcount += FP_BRANCH_SIZE - 1;
      }

      // Effects copy-on-write.
      void make_unique(Node & slot, size_t depth)
      {
        if(refcount > 1)
        {
          refcount--;
          auto copy = new Branch(1);
          if(depth == 0)
          {
            for(size_t i=0; i<FP_BRANCH_SIZE; ++i)
              copy->next[i].block = next[i].block;
          }
          else
          {
            for(size_t i=0; i<FP_BRANCH_SIZE; ++i)
            {
              auto p = next[i].branch;
              copy->next[i].branch = p;
              p->refcount++;
            }
          }
          slot.branch = copy;
        }
      }

      // Release this branch.  Decrement its reference count and reclaim
      // resources as required.
      void release(size_t depth)
      {
        if(depth>0) // safe to call with typeof(*this) == Block.
        {
          if(--refcount == 0)
          {
            depth--;
            for(size_t i=0; i<FP_BRANCH_SIZE; ++i)
              next[i].branch->release(depth);
            delete this;
          }
        }
      }

      void * operator new(size_t sz) { return branch_pool.malloc(); }
      void operator delete(void * px) { branch_pool.free(px); }
    };

    Node::Node(Node const & arg, size_t depth)
    {
      if(depth != 0)
      {
        branch = arg.branch;
        branch->refcount++;
      }
      else
        block = arg.block;
    }

    #if defined(USE_FP_CACHE) && defined(VERBOSEFP)
    extern size_t cache_tries;
    extern size_t cache_hits;
    extern size_t cache_total_depth;
    #endif
  }

  // Implements the fingerprint as a tree structure.
  struct Fingerprint
  {
  private:

    using Branch = fingerprints::Branch;
    using Block = fingerprints::Block;
    using Node = fingerprints::Node;

  public:

    Fingerprint() {}

    Fingerprint(Fingerprint const & arg)
      : depth(arg.depth), id_bound(arg.id_bound), root(arg.root, depth)
    {
      #ifdef USE_FP_CACHE
      arg.cached_block = nullptr;
      #endif
    }

    Fingerprint(Fingerprint && arg)
      : depth(arg.depth), id_bound(arg.id_bound), root(arg.root, depth)
    {
      #ifdef USE_FP_CACHE
      arg.cached_block = nullptr;
      #endif
    }

    Fingerprint & operator=(Fingerprint const & arg)
    {
      if(this != &arg)
      {
        root.branch->release(depth); // safe if this is in the "block" state
        depth = arg.depth;
        id_bound = arg.id_bound;
        new(&this->root) Node(arg.root, depth);
        #ifdef USE_FP_CACHE
        cached_block = arg.cached_block = nullptr;
        #endif
      }
      return *this;
    }

    Fingerprint & operator=(Fingerprint && arg)
      { return (*this = arg); }

    void set_left(aux_t id) { check_alloc(id); set_left_no_check(id); }
    void set_right(aux_t id) { check_alloc(id); set_right_no_check(id); }

    void set_left_no_check(aux_t id)
    {
      auto & block = write_block(id);
      aux_t const offset = id & FP_BLOCK_MASK;
      block.used |= (1 << offset);
      block.lr &= ~(1 << offset);
    }

    void set_right_no_check(aux_t id)
    {
      auto & block = write_block(id);
      aux_t const offset = id & FP_BLOCK_MASK;
      block.used |= (1 << offset);
      block.lr |= (1 << offset);
    }

    // Check that the tree has enough space for @p id choices.  Expand it if
    // necessary.
    void check_alloc(aux_t id) const
    {
      while(id >= id_bound)
      {
        if(depth == 0)
          root.branch = new Branch(root.block, 1);
        else
        {
          Branch * p = new Branch(0);
          for(depth_type i=1; i<depth; ++i)
            p = new Branch(p, 0);
          // Note: no change to root.branch->refcount below (for the node
          // currently there).
          assert(p->refcount == 0);
          root.branch = new Branch(root.branch, p, 1);
        }
        depth += 1;
        id_bound <<= FP_BRANCH_SHIFT;
      }
    }

    ChoiceState test(aux_t id) const
    {
      check_alloc(id);
      return test_no_check(id);
    }

    ChoiceState test_no_check(aux_t id) const
    {
      auto & block = read_block(id);
      aux_t const offset = id & FP_BLOCK_MASK;
      auto used_value = (block.used & (1 << offset)) ? 1 : 0;
      auto lr_value = (block.lr & (1 << offset)) ? 2 : 1;
      return static_cast<ChoiceState>(used_value * lr_value);
    }

    bool choice_is_made(aux_t id) const
    {
      if(id >= id_bound) return false;
      auto & block = read_block(id);
      aux_t const offset = id & FP_BLOCK_MASK;
      return block.used & (1 << offset);
    }

    bool choice_is_left_no_check(aux_t id) const
      { return test_no_check(id) == ChoiceState::LEFT; }

    size_t size() const { return id_bound - 1; }

    ~Fingerprint() { root.branch->release(depth); }

  private:

    #ifdef USE_FP_CACHE
    bool try_cache(aux_t id) const
    {
      #ifdef VERBOSEFP
      fingerprints::cache_tries++;
      fingerprints::cache_total_depth += depth;
      #endif
      if(cached_block && (id &~ FP_CACHE_TAG_MASK) == cache_tag)
      {
        #ifdef VERBOSEFP
        fingerprints::cache_hits++;
        #endif
        return true;
      }
      return false;
    }
    #endif

    // Locates the block containing @p id for read access.
    Block const & read_block(aux_t id) const
    {
      #ifdef USE_FP_CACHE
      if(try_cache(id)) return *cached_block;
      #endif

      Node * p = &root;
      for(depth_type i=depth-1; i>=0; --i)
      {
        auto selector = id >> (i * FP_BRANCH_SHIFT + FP_BLOCK_SHIFT);
        auto which = FP_BRANCH_MASK & selector;
        p = &p->branch->next[which];
      }
      #ifdef USE_FP_CACHE
      cached_block = &p->block;
      cache_tag = id &~ FP_CACHE_TAG_MASK;
      #endif
      return p->block;
    }

    // Locates the block containing @p id for write access.
    Block & write_block(aux_t id) const
    {
      #ifdef USE_FP_CACHE
      if(try_cache(id)) return *cached_block;
      #endif

      Node * p = &root;
      for(depth_type i=depth-1; i>=0; --i)
      {
        auto selector = id >> (i * FP_BRANCH_SHIFT + FP_BLOCK_SHIFT);
        auto which = FP_BRANCH_MASK & selector;
        p->branch->make_unique(*p, i); // copy-on-write
        p = &p->branch->next[which];
      }
      #ifdef USE_FP_CACHE
      cached_block = &p->block;
      cache_tag = id &~ FP_CACHE_TAG_MASK;
      #endif
      return p->block;
    }

    #ifdef USE_FP_CACHE
    // Contains the cached block, if there is one.
    mutable Block * cached_block = nullptr;

    // The tag associated with the cached block.  Ignored if @p cached_block is
    // null.
    mutable aux_t cache_tag;
    #endif

    // Counts the number of levels of branches occurring above the blocks.  A
    // signed integer is used because negative depths occur in loop variables.
    // Use the same bit width as aux_t to try for better packing.
    using depth_type = boost::int_t<sizeof(aux_t)*8>::exact;
    mutable depth_type depth = 0;

    // The largest ID representable in the current tree, plus one.
    mutable size_t id_bound = FP_BLOCK_SIZE;

    // The root of the tree structure containing fingerprint data.
    mutable Node root;
  };
}}
