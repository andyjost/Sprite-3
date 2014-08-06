STATICLIB = sprite-rt.bc
STATICLLVM = sprite-rt.ll
SOURCES = $(wildcard *.cpp)

.PHONY : all clean install llvm uninstall

all : $(STATICLIB)

clean :
	rm -f $(STATICLIB) $(STATICLLVM)

install : $(STATICLIB)
	@echo "Installing runtime at $(LIBINSTALL)"
	mkdir -p $(LIBINSTALL)
	cp $(STATICLIB) $(LIBINSTALL)

uninstall :
	-rm -f $(LIBINSTALL)/$(STATICLIB)

-include ../Make.include

###

$(STATICLIB) : $(SOURCES)
	$(CLANG) -S -emit-llvm $(SOURCES) -o - | $(LLVM-AS) > $@

$(STATICLLVM) : $(STATICLIB)
	$(LLVM-DIS) $(STATICLIB)

llvm : $(STATICLLVM)
