STATICLIB = sprite-rt-C-part.bc

.PHONY : all clean main.o

all : $(STATICLIB)

clean :
	rm -f *.bc *.ll *.d

-include ../../../Make.include

# Additional options that may be added to LOCAL_CFLAGS:
#     -DVERBOSEGC=1  Verbose garbage collector.
#         =1  Prints a summary of the time spent collecting.
#         =2  Prints information every time the collector runs.
#         =3  Prints information about every node during collection.
#     -DNOBYPASS=1  Disable code to bypass choices in the runtime library.
#         Note: bypassing appears incomplete and may be removed.
#     -g Add debug symbols.  Allows debugging into the runtime library.
#
# Note: llvm-config is used to access SmallVector.h.  It may add NDEBUG.
LOCAL_CFLAGS += $(shell $(LLVM-CONFIG) --cppflags) $(RT-CFLAGS) -std=c++11 -I. -DNOBYPASS

.DELETE_ON_ERROR :
$(STATICLIB) : main.o
	# Note: pipefail is used because llvm-as returns non-error even if clang fails.
	bash -o pipefail -c '$(CLANG) $(LOCAL_CFLAGS) -S -emit-llvm main.cpp -o - | $(LLVM-AS) > $(STATICLIB)'
	$(CLANG) -MM $(LOCAL_CFLAGS) main.cpp > main.d

# Import dependency info.
-include main.d
