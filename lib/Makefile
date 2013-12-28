-include ../Make.include

SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:.cpp=.o)
STATICLIB = sprite-llvm.a

.PHONY : all clean

all : $(STATICLIB)

clean :
	rm -f $(OBJECTS) $(STATICLIB) *.d

# Import dependency info for extant .o files.
-include $(OBJECTS:.o=.d)

.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $@
	$(CC) -MM $(CFLAGS) $< > $(@:.o=.d)

$(STATICLIB) : $(OBJECTS)
	ar rcs $@ $(OBJECTS)

