SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:.cpp=.o)
EXECUTABLE = generate-sprite-runtime-part-b
STATICLIB = sprite-runtime-part-b.bc

.PHONY : all clean

all : $(STATICLIB)

clean :
	rm -f *.bc $(OBJECTS) $(EXECUTABLE)

-include ../../Make.include

###

.cpp.o :
	$(CC) $(CFLAGS) -c $< -o $@

$(EXECUTABLE) : $(OBJECTS) $(SPRITE_LIB)
	$(CC) $^ $(LDFLAGS) -o $@

$(STATICLIB) : $(EXECUTABLE)
	./$< > $@
