STATICLIB = sprite-rt-C-part.bc

.PHONY : all clean

all : $(STATICLIB)

clean :
	rm -f *.bc *.ll

-include ../../../Make.include

.DELETE_ON_ERROR :
$(STATICLIB) : main.cpp
	# Note: pipefail is used because llvm-as returns non-error even if clang fails.
	bash -o pipefail -c '$(CLANG) -S -emit-llvm main.cpp -o - | $(LLVM-AS) > $(STATICLIB)'
