
STATICLIB = sprite-rt.bc
STATICLLVM = sprite-rt.ll
SOURCES = $(wildcard *.cpp)

.PHONY : all clean llvm

all : $(STATICLIB)

-include ../Make.include

clean :
	rm -f $(STATICLIB) $(STATICLLVM)

$(STATICLIB) : $(SOURCES)
	clang++ -S -emit-llvm $(SOURCES) -o - | llvm-as > $@

$(STATICLLVM) : $(STATICLIB)
	llvm-dis $(STATICLIB)

llvm : $(STATICLLVM)
