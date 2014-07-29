
STATICLIB = sprite-rt.bc
STATICLLVM = sprite-rt.ll
SOURCES = $(wildcard *.cpp)

.PHONY : all clean llvm

all : $(STATICLIB)

-include ../Make.include

clean :
	rm -f $(STATICLIB) $(STATICLLVM)

$(STATICLIB) : $(SOURCES)
	$(CLANG) -S -emit-llvm $(SOURCES) -o - | $(LLVM-AS) > $@

$(STATICLLVM) : $(STATICLIB)
	$(LLVM-DIS) $(STATICLIB)

llvm : $(STATICLLVM)
