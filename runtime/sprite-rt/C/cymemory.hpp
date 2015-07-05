// This file may only be included from main.cpp.
#include "boost-pool-1.46/pool.hpp"
#include "basic_runtime.hpp"
#include <deque>

#ifdef VERBOSEGC
#include <iostream>
#include <fstream>
#include <string>
#endif

// Size of a node in bytes.
#define NODE_BYTES sizeof(::sprite::compiler::node)

extern "C"
{
  // The head of the free list.
  void * CyMem_FreeList = nullptr;
  extern sprite::compiler::vtable CyVt_Fwd __asm__(".vt.fwd");

  // The computation roots.
  sprite::compiler::node * CyMem_NextComputationRoot();
}

namespace sprite { namespace compiler
{
  extern std::deque<node*> CyMem_Roots;

  #ifdef VERBOSEGC
  // This debugging timer is specific to x86_64 architecture.
  typedef unsigned long long int ticks;
  static inline ticks getticks()
  {
    unsigned int a, d;
    asm volatile("rdtsc" : "=a" (a), "=d" (d)); // x86-64
    return ((ticks)a) | (((ticks)d) << 32);
  }

  size_t n_gc = 0;
  ticks gc_time = 0;
  double gc_pct_sum = 0;

  static struct GcReport
  {
    static long double get_cpu_frequency()
    {
      try
      {
        std::ifstream in("/proc/cpuinfo");
        std::string line;
        while(std::getline(in, line))
        {
          if(line.substr(0, 7) == "cpu MHz")
          {
            size_t const n = line.find_first_of("0123456789", 8);
            if(n == line.npos) return 1.0;
            return std::stold(line.substr(n)) * 1000000;
          }
        }
      }
      catch(...) { /* ignore errors */ }
      return 1.0;
    }

    ~GcReport()
    {
      try
      {
        ticks const elapsed = (getticks() - start_time);
        long double const freq = get_cpu_frequency();
        std::cout
          << "====== Garbage Collection Summary ======\n"
          << "    Program elapsed time (s) : " << (elapsed/freq) << "\n"
          << "    CPU MHz                  : " << freq << "\n"
          << "    GC elapsed time (s)      : " << (gc_time/freq) << "\n"
          << "    GC % elapsed time        : " << ((double)gc_time/elapsed)*100 << "\n"
          << "    GC # calls               : " << n_gc << "\n"
          << "    GC avg % freed           : " << (gc_pct_sum/n_gc) << "\n"
          << std::endl;
      }
      catch(...) {}
    }

  private:
    ticks start_time = getticks();

  } _gc_report;
  #endif

  /**
   * @brief A memory pool with integrated garbage collection.
   *
   * Extends Boost.Pool (specifically for our type node) with an additional
   * member collect, which performs garbage collection through a mark-sweep
   * algorithm.
   */
  template<typename UserAllocator = boost::default_user_allocator_new_delete>
  struct NodePool : public boost::pool<UserAllocator>
  {
    /// The base pool type.
    typedef boost::pool<UserAllocator> base_type;

    /// The size type for the base pool.
    typedef typename base_type::size_type size_type;

    explicit NodePool(
        size_type nrequested_size
      , size_type nnext_size = 32
      , size_type nmax_size = 0
      )
    : base_type(nrequested_size, nnext_size, nmax_size)
    {
      assert(nnext_size > 0);
    }

    // pre: !this->store().empty() (same as !!CyMem_FreeList)
    void * malloc BOOST_PREVENT_MACRO_SUBSTITUTION()
      { return (this->store().malloc)(); }

    // Perform collection and maybe allocate a new block.
    void collect();

  private:
    // Run the collector.  Returns the number of chunks freed and a pointer
    // to the first chunk freed.
    std::pair<size_type,void*> collect_only();
  };

  // pre: the free list must be empty.
  // post: the free list may not be empty.
  template<typename UserAllocator>
  std::pair<typename NodePool<UserAllocator>::size_type, void *>
  NodePool<UserAllocator>::collect_only()
  {
    #if VERBOSEGC > 0
      n_gc++;
      ticks t0 = getticks();
    #endif

    #if VERBOSEGC > 1
      std::cout << "\nStarting collection." << std::endl;
    #endif
  
    // Mark phase.
    std::deque<node*> roots;

    // Add roots from computations.
    while(node * p = CyMem_NextComputationRoot())
      roots.push_back(p);
    // Add other roots.
    for(node * p: CyMem_Roots)
      roots.push_back(p);

    while(!roots.empty())
    {
      node * parent = roots.back();
      roots.pop_back();

      #if VERBOSEGC > 2
        std::cout
            << "   [mark] @" << parent << " " << parent->vptr->label(parent)
            << std::endl;
      #endif

        parent->mark = 1;
        node ** begin, ** end;
        parent->vptr->gcsucc(parent, &begin, &end);
        for(; begin!=end; ++begin)
          roots.push_back(*begin);
    }

    #if VERBOSEGC > 1
      ticks tm = getticks();
      std::cout << "Mark phase takes " << (tm-t0) << " ticks." << std::endl;
    #endif
  
    // Sweep phase.
    #if VERBOSEGC > 0
      size_t total = 0;
    #endif
    size_t n = 0;
    void * last = 0; // last item in the free list (first chunk freed).
    // An iterator over the memory block list.
    typedef boost::details::PODptr<size_type> blockptr;
    const size_type partition_size = this->alloc_size();
    for(blockptr ptr = this->list; ptr.valid(); ptr = ptr.next())
    {
      #if VERBOSEGC > 0
        total += (ptr.end() - ptr.begin());
      #endif
      for(char * i = ptr.begin(); i != ptr.end(); i += partition_size)
      {
        node * const node_p = reinterpret_cast<node *>(i);
        if(node_p->mark == 0)
        {
          #if VERBOSEGC > 2
            std::cout << "   [free] @" << node_p << std::endl;
          #endif
          ++n;
          if(!last) last = node_p;

          // When freeing a node with arity > 2, we must also free its
          // successor list.  But the collector may run after some nodes have
          // been allocated but before they are initialized.  So how can we
          // tell the difference between initialized and uninitialized chunks?
          // We can use the fact that the first word of every chunk is a
          // pointer -- either the chunk is an initialized node, whose first
          // word is a vtable pointer, or it came from the free list, in which
          // case the first word points to another chunk.  In either case it is
          // always possible to access the fourth word of the dereferenced
          // pointer -- i.e., the position that would be slot0 if the pointer
          // is to a node, or the fourth entry in the vtable if the pointer is
          // to a vtable.  Since nodes may only contain pointers to other nodes
          // in slot1, we can discriminate by placing an arbitrary sentinel
          // value that is NOT a valid node pointer at the proper position of
          // the vtable.  This implementation uses &CyVt_Fwd.
          // std::cout << "a" << std::endl;
          if(node_p->vptr && node_p->vptr->sentinel == &CyVt_Fwd)
            node_p->vptr->destroy(node_p);
          this->free(node_p);
        }
        else
        {
          #if VERBOSEGC > 2
            std::cout
                << "   [keep] @" << node_p << " " << node_p->vptr->label(node_p)
                << std::endl;
          #endif
          node_p->mark = 0;
        }
      }
    }
    #if VERBOSEGC > 0
      ticks t1 = getticks();
      gc_time += (t1-t0);
      total /= partition_size;
      float const pct = total == 0 ? 0 : 100.0 * n / total;
      gc_pct_sum += pct;
    #endif

    #if VERBOSEGC > 1
      std::cout << "Sweep phase takes " << (t1-tm) << " ticks." << std::endl;
      std::cout << "Done collecting: freed " << n << " out of "
        << total << " chunks (" << pct << "%); " << (total-n) << " remain."
        << std::endl;
    #endif
    return std::make_pair(n,last);
  }
  
  template<typename UserAllocator>
  void NodePool<UserAllocator>::collect()
  {
    // Run the collector.
    std::pair<size_type,void *> const collected = collect_only();
    size_type const nfree = collected.first;
    void * last = collected.second;
  
    // If too little space was made, then allocate a new block, too.  Do so if
    // more than 1 part in 32 remains in use after collection, or if fewer than
    // abs_min chunks were made available.
    size_type const current_size = this->next_size - this->start_size;
    size_type const threshold = current_size >> 5;
    static size_type const abs_min = 256;
    if(current_size - nfree >= threshold || nfree < abs_min)
    {
      // Save the free list and then set it to empty in this object.
      void * const save_list = CyMem_FreeList;
      CyMem_FreeList = nullptr;
      assert(!save_list == !last); // both are null or both are non-null
  
      // This call will allocate a new block, since the free list is empty.
      void * ret = base_type::malloc();
      
      // If the old free list was not empty, insert it before the new block.
      if(last)
      {
        this->nextof(last) = CyMem_FreeList;
        CyMem_FreeList = save_list;
      }

      // Put back the allocated chunk.
      this->free(ret);
    }
  }

  // Used when monitoring GC allocations.
  struct BaseAllocator
  {
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;

    static char * malloc(size_type const bytes)
    {
      #if VERBOSEGC > 1
        float const n = bytes / NODE_BYTES;
        std::cout
            << "Request for a block of " << bytes
            << " bytes (" << n << " nodes)."
            << std::endl;
      #endif
      char * p = new (std::nothrow) char[bytes]();
      #if VERBOSEGC > 1
        std::cout
            << "block-alloc(("
            << reinterpret_cast<void *>(p) << ","
            << reinterpret_cast<void *>((p+bytes))
            << "))" << std::endl
          ;
      #endif
      return p;
    }
    static void free(char * const p)
    {
      #if VERBOSEGC > 1
      std::cout << "block-free " << reinterpret_cast<void *>(p) << std::endl;
      #endif
      delete[] p;
    }
  };

  // This replacement for SizeType is used as a handle to specialize
  // boost::simple_segregated_storage.  See specialization below.
  template<typename SizeType> struct SizeTypeOverride
  {
    typedef SizeType size_type;

    SizeTypeOverride(size_type size) : m_value(size) {}
    operator size_type () const { return m_value; }
    operator size_type & () { return m_value; }
  private:
    size_type m_value;
  };

  // Specifying this allocator in a boost::pool instance hijacks the low-level
  // implementation machinery.
  struct GlobalAllocator : BaseAllocator
  {
    typedef SizeTypeOverride<BaseAllocator::size_type> size_type;
  };
}}

namespace boost {

  // Specializes boost::pool.  Makes it a singleton whose free list is stored
  // in the global variable CyMem_FreeList.
  template <typename SizeType>
  class simple_segregated_storage<sprite::compiler::SizeTypeOverride<SizeType> >
  {
    public:
      typedef SizeType size_type;
  
    private:

      void * try_malloc_n(
          void * & start, size_type n, const size_type partition_size)
      {
        void * iter = nextof(start);
        while (--n != 0)
        {
          void * next = nextof(iter);
          if (next != static_cast<char *>(iter) + partition_size)
          {
            // next == 0 (end-of-list) or non-contiguous chunk found
            start = iter;
            return 0;
          }
          iter = next;
        }
        return iter;
      }
  
    protected:
      // Unused, but must be defined to avoid compile errors.  When boost::pool
      // is used as a base class of NodePool, the compiler automatically
      // generates ~pool, which needs first.  We don't really plan to ever call
      // ~pool, though.
      void * first;
  
      // Traverses the free list referred to by "CyMem_FreeList",
      //  and returns the iterator previous to where
      //  "ptr" would go if it was in the free list.
      // Returns 0 if "ptr" would go at the beginning
      //  of the free list (i.e., before "CyMem_FreeList")
      void * find_prev(void * const ptr)
      {
        // Handle border case
        if (CyMem_FreeList == 0 || std::greater<void *>()(CyMem_FreeList, ptr))
          return 0;
      
        void * iter = CyMem_FreeList;
        while (true)
        {
          // if we're about to hit the end or
          //  if we've found where "ptr" goes
          if (nextof(iter) == 0 || std::greater<void *>()(nextof(iter), ptr))
            return iter;
      
          iter = nextof(iter);
        }
      }
  
      static void * & nextof(void * const ptr)
        { return *(static_cast<void **>(ptr)); }
  
    public:
      simple_segregated_storage() {}
  
      void * segregate(
          void * const block,
          const size_type sz,
          const size_type partition_sz,
          void * const end)
      {
        // Get pointer to last valid chunk, preventing overflow on size calculations
        //  The division followed by the multiplication just makes sure that
        //    old == block + partition_sz * i, for some integer i, even if the
        //    block size (sz) is not a multiple of the partition size.
        char * old = static_cast<char *>(block)
            + ((sz - partition_sz) / partition_sz) * partition_sz;
      
        // Set it to point to the end
        nextof(old) = end;
      
        // Handle border case where sz == partition_sz (i.e., we're handling an array
        //  of 1 element)
        if (old == block)
          return block;
      
        // Iterate backwards, building a singly-linked list of pointers
        for (char * iter = old - partition_sz; iter != block;
            old = iter, iter -= partition_sz)
          nextof(iter) = old;
      
        // Point the CyMem_FreeList pointer, too
        nextof(block) = old;
      
        return block;
      }

      void add_block(void * const block,
          const size_type nsz, const size_type npartition_sz)
      {
        // Segregate this block and merge its free list into the
        //  free list referred to by "CyMem_FreeList"
        CyMem_FreeList = segregate(block, nsz, npartition_sz, CyMem_FreeList);
      }

      void add_ordered_block(void * const block,
          const size_type nsz, const size_type npartition_sz)
      {
        std::terminate();
        // throw RuntimeError("cannot call add_ordered_block from node pool");
        return;
        // // This (slower) version of add_block segregates the
        // //  block and merges its free list into our free list
        // //  in the proper order

        // // Find where "block" would go in the free list
        // void * const loc = find_prev(block);

        // // Place either at beginning or in middle/end
        // if (loc == 0)
        //   add_block(block, nsz, npartition_sz);
        // else
        //   nextof(loc) = segregate(block, nsz, npartition_sz, nextof(loc));
      }

  
      bool empty() const { return (CyMem_FreeList == 0); }
  
      // pre: !empty()
      void * malloc BOOST_PREVENT_MACRO_SUBSTITUTION()
      {
        void * const ret = CyMem_FreeList;
  
        // Increment the "CyMem_FreeList" pointer to point to the next chunk
        CyMem_FreeList = nextof(CyMem_FreeList);
        return ret;
      }
  
      // pre: chunk was previously returned from a malloc() referring to the
      //  same free list
      // post: !empty()
      void free BOOST_PREVENT_MACRO_SUBSTITUTION(void * const chunk)
      {
        nextof(chunk) = CyMem_FreeList;
        CyMem_FreeList = chunk;
      }
  
      void ordered_free(void * const chunk)
      {
        std::terminate();
        // throw RuntimeError("cannot call ordered_free from node pool");
        return;
        // // This (slower) implementation of 'free' places the memory
        // //  back in the list in its proper order.

        // // Find where "chunk" goes in the free list
        // void * const loc = find_prev(chunk);

        // // Place either at beginning or in middle/end
        // if (loc == 0)
        //   (free)(chunk);
        // else
        // {
        //   nextof(chunk) = nextof(loc);
        //   nextof(loc) = chunk;
        // }
      }
  
      // Note: if you're allocating/deallocating n a lot, you should
      //  be using an ordered pool.
      void * malloc_n(const size_type n, const size_type partition_size)
      {
        std::terminate();
        // throw RuntimeError("cannot malloc_n from node pool");
        return 0;
        // if(n == 0)
        //   return 0;
        // void * start = &CyMem_FreeList;
        // void * iter;
        // do
        // {
        //   if (nextof(start) == 0)
        //     return 0;
        //   iter = try_malloc_n(start, n, partition_size);
        // } while (iter == 0);
        // void * const ret = nextof(start);
        // nextof(start) = nextof(iter);
        // return ret;
      }
  
      void free_n(void * const chunks, const size_type n,
          const size_type partition_size)
      {
        // if(n != 0)
        //   add_block(chunks, n * partition_size, partition_size);
      }

      void ordered_free_n(void * const chunks, const size_type n,
          const size_type partition_size)
      {
        if(n != 0)
          add_ordered_block(chunks, n * partition_size, partition_size);
      }
  };
}
