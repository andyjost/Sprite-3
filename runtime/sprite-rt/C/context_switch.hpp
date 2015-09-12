#pragma once

// Other platforms may be added as needed.
#ifdef _POSIX_C_SOURCE
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <time.h>
#include <boost/context/all.hpp>

// DEBUG
#include <iostream>

namespace sprite { namespace compiler
{
  void _os_error(char const * where)
  {
    std::string msg("Error in POSIX ");
    msg = where;
    perror(msg.c_str());
    exit(EXIT_FAILURE);
  }

  // Represents a timer interval.
  struct Interval
  {
    // The interval is 1/50th of a sec to begin with.
    Interval() : data({{0,0}, {0,20000000}}) {}

    // Increase the timer interval.
    Interval & operator++()
    {
      data.it_value.tv_sec *= 2;
      data.it_value.tv_nsec *= 2;
      while(data.it_value.tv_nsec > 1000000000L)
      {
        data.it_value.tv_sec++;
        data.it_value.tv_nsec -= 1000000000L;
      }
      return *this;
    }

    itimerspec const & get() const { return data; }

  private:

    itimerspec data;
  };

  // Represents a timer used to trigger context switches.
  struct Timer
  {
    Timer()
    {
      sigevent sevp;
      sevp.sigev_notify = SIGEV_NONE;
      if(timer_create(CLOCK_THREAD_CPUTIME_ID, &sevp, &timerid))
        _os_error("timer_create");
    }

    // Indicates whether the timer has expired.
    explicit operator bool() const
    {
      itimerspec timedata;
      if(timer_gettime(timerid, &timedata))
        _os_error("timer_gettime");
      return timedata.it_value.tv_sec != 0 || timedata.it_value.tv_nsec != 0;
    }

    // Sets the timer.
    void operator()(Interval const & ivl)
    {
      if(timer_settime(timerid, 0, &ivl.get(), nullptr))
        _os_error("timer_settime");
    }

  private:

    timer_t timerid;
  };

}}

#else
#error "Context switches are not availble on this platform."
#endif

extern "C"
{
  // The "main" function for performing a subcomputation.  The argument is a
  // pointer to an instance of Cy_EvalFrame.  This function normalizes the expression.
  void CyContext_SubcomputationMain(intptr_t);

  void make_context(void * context, void * stack_pointer);
}

namespace sprite { namespace compiler
{
  // extern boost::context::fcontext_t CyContext_MainFiber;

  // Functions to manage auxiliary stacks.
  // void * alloc_stack();
  // void dealloc_stack(void *);

  // A context used to manipulate one thread for cooperative multitasking.
  struct Fiber
  {
    // Creates a new fiber for execution.
    Fiber() : stack_pointer(malloc(stack_size())), context()
    {
      make_context(&context, stack_pointer);
      // // make_fcontext creates an fcontext_t on the stack.  Since it is POD, we
      // // can just copy it into this object.
      // context = *boost::context::make_fcontext(
      //       stack_pointer, stack_size(), &CyContext_SubcomputationMain
      //     );
    }

  private:

    struct main_fiber_tag {};
    Fiber(main_fiber_tag) : stack_pointer(nullptr), context() {}

  public:

    // Returns the main fiber (i.e., the initial context of the thread executing main).
    static Fiber & main_fiber();

    Fiber(Fiber const &) = delete;
    Fiber & operator=(Fiber const &) = delete;

    Fiber(Fiber && arg)
      : stack_pointer(arg.stack_pointer), context(arg.context)
    {
      arg.stack_pointer = nullptr;
    }

    Fiber & operator=(Fiber && arg)
    {
      std::swap(stack_pointer, arg.stack_pointer);
      context = arg.context;
      return *this;
    }

    ~Fiber() { if(stack_pointer) free(stack_pointer); }

    // Switch from this fiber (must be currently executing) to the one given.
    void switch_to(Fiber & to, intptr_t arg=0) const
    {
      // Note: preserve_fpu = false;
      boost::context::jump_fcontext(&context, &to.context, arg, false);
    }

    static size_t constexpr stack_size() { return 10*1024*1024; }

  private:

    void * stack_pointer;
    mutable boost::context::fcontext_t context;
  };

  boost::context::fcontext_t fcm,fc1,fc2;
  
  void f1(intptr_t)
  {
      std::cout<<"f1: entered"<<std::endl;
      std::cout<<"f1: call jump_fcontext( & fc1, fc2, 0)"<< std::endl;
      boost::context::jump_fcontext(&fc1,&fc2,0);
      std::cout<<"f1: return"<<std::endl;
      boost::context::jump_fcontext(&fc1,&fcm,0);
  }
  
  void f2(intptr_t)
  {
      std::cout<<"f2: entered"<<std::endl;
      std::cout<<"f2: call jump_fcontext( & fc2, fc1, 0)"<<std::endl;
      boost::context::jump_fcontext(&fc2,&fc1,0);
      // BOOST_ASSERT(false&&!"f2: never returns");
  }
  
  void testtest()
  {
    std::size_t size(8192);
    void* sp1(std::malloc(size));
    void* sp2(std::malloc(size));
    
    fc1= *boost::context::make_fcontext(sp1,size,f1);
    fc2= *boost::context::make_fcontext(sp2,size,f2);
    std::cout<<"main: call jump_fcontext( & fcm, fc1, 0)"<<std::endl;
    boost::context::jump_fcontext(&fcm,&fc1,0);
  }
}}

