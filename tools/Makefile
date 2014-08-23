SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:.cpp=.o)
EXECUTABLES = $(OBJECTS:.o=)

.PHONY : all clean $(BAREWORDS)
.SECONDARY : $(OBJECT)

all : $(EXECUTABLES)

clean :
	rm -f $(EXECUTABLES) $(OBJECTS) *.d

install :
	@echo "Installing tools at $(BININSTALL)"
	mkdir -p $(BININSTALL)
	cp $(EXECUTABLES) $(BININSTALL)

uninstall :
	-cd $(BININSTALL); rm -f $(EXECUTABLES)

include ../Make.include

###

# Import dependency info for extant .o files.
-include $(OBJECTS:.o=.d)

.cpp.o :
	$(CC) $(CFLAGS) -c $< -o $@
	$(CC) -MM $(CFLAGS) $< > $(@:.o=.d)

$(EXECUTABLES) : $(SPRITE_LIB) $(SPRITE_RT_LIB)

$(EXECUTABLES) : % : %.o
	$(CC) $< $(SPRITE_LIB) $(LDFLAGS) -o $@

