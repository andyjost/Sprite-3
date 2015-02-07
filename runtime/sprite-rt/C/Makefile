STATICLIB = sprite-rt-C-part.bc

.PHONY : all clean main.o

all : $(STATICLIB)

clean :
	rm -f *.bc *.ll *.d

-include ../../../Make.include

# Additional options that may be added to LOCAL_CFLAGS:
#     -DVERBOSEGC=1  Verbose garbage collector (use =2 for more verbosity).
LOCAL_CFLAGS += $(shell $(LLVM-CONFIG) --cppflags) -I.

.DELETE_ON_ERROR :
$(STATICLIB) : main.o
	# Note: pipefail is used because llvm-as returns non-error even if clang fails.
	bash -o pipefail -c '$(CLANG) $(LOCAL_CFLAGS) -std=c++11 -S -emit-llvm main.cpp -o - | $(LLVM-AS) > $(STATICLIB)'
	$(CLANG) -MM $(LOCAL_CFLAGS) main.cpp > main.d

# Import dependency info.
-include main.d
