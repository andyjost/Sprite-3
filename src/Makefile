SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:.cpp=.o)
STATICLIB = sprite.a

.PHONY : all clean

all : $(STATICLIB)

clean :
	rm -f $(OBJECTS) $(STATICLIB) *.d

-include ../Make.include

###

# Import dependency info for extant .o files.
-include $(OBJECTS:.o=.d)

# All object files depend on the generated file config.hpp.
$(OBJECTS) : $(CONFIG_HPP)

.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $@
	$(CC) -MM $(CFLAGS) $< > $(@:.o=.d)

$(STATICLIB) : $(OBJECTS)
	ar rcs $@ $(OBJECTS)

