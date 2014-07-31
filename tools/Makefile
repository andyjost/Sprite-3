SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:.cpp=.o)
EXECUTABLES = $(OBJECTS:.o=)

.PHONY : all clean $(BAREWORDS)
.SECONDARY : $(OBJECT)

all : $(EXECUTABLES)

# Include default definitions and rules.
include ../Make.include

# Import dependency info for extant .o files.
-include $(OBJECTS:.o=.d)

clean :
	rm -f $(EXECUTABLES) $(OBJECTS) *.d

.cpp.o :
	$(CC) $(CFLAGS) -c $< -o $@
	$(CC) -MM $(CFLAGS) $< > $(@:.o=.d)

$(EXECUTABLES) : $(SPRITE_LIB) $(SPRITE_RT_LIB)

$(EXECUTABLES) : % : %.o
	$(CC) $< $(SPRITE_LIB) $(LDFLAGS) -o $@

