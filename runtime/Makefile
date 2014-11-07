STATICLIB = sprite-rt.bc
STATICLLVM = sprite-rt.ll
SOURCES = $(wildcard *.cpp)
STATICLIB_PART_B = sprite-runtime-part-b/sprite-runtime-part-b.bc

.PHONY : all clean install llvm uninstall

all : $(STATICLIB)

clean :
	rm -f $(STATICLIB) $(STATICLLVM) *.bc
	$(MAKE) -C sprite-runtime-part-b clean

install : $(STATICLIB)
	@echo "Installing runtime at $(LIBINSTALL)"
	mkdir -p $(LIBINSTALL)
	cp $(STATICLIB) $(LIBINSTALL)
	cp -r cmc/ $(LIBINSTALL)
	rm -f $(LIBINSTALL)/cc 2> /dev/null
	ln -s `which $(LIB-CC)` $(LIBINSTALL)/cc
	rm -f $(LIBINSTALL)/llc 2> /dev/null
	ln -s `which $(LIB-LLC)` $(LIBINSTALL)/llc
	rsync -a currylib $(LIBINSTALL)

uninstall :
	-rm -f $(LIBINSTALL)/$(STATICLIB)
	-rm -rf $(LIBINSTALL)/cmc
	-rm -rf $(LIBINSTALL)/cc
	-rm -rf $(LIBINSTALL)/llc

-include ../Make.include

###

.DELETE_ON_ERROR :
sprite-runtime-part-a.bc : sprite-runtime-part-a.cpp
	# Runtime part A: C++ converted to bitcode using Clang.
	# Note: pipefail is used because llvm-as returns non-error even if clang fails.
	bash -o pipefail -c '$(CLANG) -S -emit-llvm sprite-runtime-part-a.cpp -o - | $(LLVM-AS) > sprite-runtime-part-a.bc'

$(STATICLIB_PART_B) : sprite-runtime-part-b
	# Runtime part B: bitcode generated directly using the C++ API.
	$(MAKE) -C sprite-runtime-part-b

$(STATICLIB) : sprite-runtime-part-a.bc $(STATICLIB_PART_B)
	$(LLVM-LINK) $^ > $@

$(STATICLLVM) : $(STATICLIB)
	$(LLVM-DIS) $(STATICLIB)

llvm : $(STATICLLVM)
