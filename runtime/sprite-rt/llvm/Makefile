SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:.cpp=.o)
EXECUTABLE = generate-llvm
STATICLIB = sprite-rt-llvm-part.bc

.PHONY : all clean

all : $(STATICLIB)

clean :
	rm -f *.bc $(OBJECTS) $(EXECUTABLE) *.ll *.d

-include ../../../Make.include

CFLAGS += -I.

###

# Import dependency info for extant .o files.
-include $(OBJECTS:.o=.d)

.cpp.o :
	$(CC) $(CFLAGS) -c $< -o $@
	$(CC) -MM $(CFLAGS) $< > $(@:.o=.d)

$(EXECUTABLE) : $(OBJECTS) $(SPRITE_LIB)
	$(CC) $^ $(LDFLAGS) -o $@

$(STATICLIB) : $(EXECUTABLE)
	./$< > $@

