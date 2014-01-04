SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:.cpp=.o)
EXECUTABLES = $(OBJECTS:.o=.exe)
TESTS = $(EXECUTABLES:.exe=.test)
BAREWORDS = $(EXECUTABLES:.exe=)

.PHONY : all clean $(BAREWORDS)
.SECONDARY : $(EXECUTABLES)

all : $(TESTS)

# Include default definitions and rules.
include ../Make.include

# Import dependency info for extant .o files.
-include $(OBJECTS:.o=.d)

clean :
	rm -f $(EXECUTABLES) $(OBJECTS) $(TESTS) *.d

.cpp.o :
	$(CC) $(CFLAGS) -c $< -o $@
	$(CC) -MM $(CFLAGS) $< > $(@:.o=.d)

$(EXECUTABLES) : $(SPRITE_RT_LIB)

%.exe : %.o
	$(CC) $< $(SPRITE_RT_LIB) $(LDFLAGS) -o $@

# Allow a bareword to be used as a target.  E.g., allow invocation as "make
# hello" instead of requiring "make hello.test".
$(BAREWORDS) :
	@$(MAKE) $@.test

# Runs a test program.  A log file ending with .test is created to indicate the
# test was run successfully.  If the test program fails, .failed is appended to
# the log file name so that this rule will be triggered again in the next build
# cycle.
%.test : %.exe
	@echo "Running code examples for $<."
	@(                                                              \
	    bash -c './$< 2>&1 | tee $@;  test $${PIPESTATUS[0]} -eq 0' \
			  && (echo -e "**** PASSED ****\n" && rm -f $@.failed)      \
	  ) || (echo -e "**** FAILED ****\n" && mv $@ $@.failed)
