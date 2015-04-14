STATICLIB = sprite-rt.bc
STATICLLVM = sprite-rt.ll
STATICLIB_LLVM_PART = llvm/sprite-rt-llvm-part.bc
STATICLIB_C_PART = C/sprite-rt-C-part.bc

.PHONY : all clean install llvm uninstall

all : $(STATICLIB)

clean :
	rm -f $(STATICLIB) $(STATICLLVM) *.bc
	$(MAKE) -C C clean
	$(MAKE) -C llvm clean

install : $(STATICLIB)
	cp $(STATICLIB) $(LIBINSTALL)

uninstall :
	-rm -f $(LIBINSTALL)/$(STATICLIB)

-include ../../Make.include

###

$(STATICLIB_C_PART) : C/*.[ch]pp C/*.def ../../include/sprite/basic_runtime.hpp
	# Runtime C++ part: C++ converted to bitcode using Clang.
	$(MAKE) -C C

$(STATICLIB_LLVM_PART) : llvm/*.[ch]pp
	# Runtime LLVM part: bitcode generated directly using the C++ API.
	$(MAKE) -C llvm

.DELETE_ON_ERROR :
$(STATICLIB) : $(STATICLIB_C_PART) $(STATICLIB_LLVM_PART)
	$(LLVM-LINK) $^ > $@

$(STATICLLVM) : $(STATICLIB)
	$(LLVM-DIS) $(STATICLIB)

llvm : $(STATICLLVM)
