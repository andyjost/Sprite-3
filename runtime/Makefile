
.PHONY : all clean currylibs install uninstall

all :
	$(MAKE) -C sprite-rt all

-include ../Make.include

clean :
	$(MAKE) -C sprite-rt clean

currylibs :
	$(MAKE) $(LIBINSTALL)/currylib/.curry/Prelude.bc

install :
	@echo "Installing runtime at $(LIBINSTALL)"
	mkdir -p $(LIBINSTALL)
	$(MAKE) -C sprite-rt install
	cp -r cmc/ $(LIBINSTALL)
	@
	@rm -f $(LIBINSTALL)/cc 2> /dev/null
	ln -s `which $(LIB-CC)` $(LIBINSTALL)/cc
	@test -e $(LIBINSTALL)/cc
	@
	@rm -f $(LIBINSTALL)/llc 2> /dev/null
	ln -s `which $(LIB-LLC)` $(LIBINSTALL)/llc
	@test -e $(LIBINSTALL)/llc
	@
	@rm -f $(LIBINSTALL)/opt 2> /dev/null
	ln -s `which $(LIB-OPT)` $(LIBINSTALL)/opt
	@test -e $(LIBINSTALL)/opt
	@
	@rm -f $(LIBINSTALL)/pakcs 2> /dev/null
	ln -s `which $(LIB-PAKCS)` $(LIBINSTALL)/pakcs
	@test -e $(LIBINSTALL)/pakcs
	@
	@rm -f $(LIBINSTALL)/parsecurry 2> /dev/null
	ln -s `which $(LIB-PARSECURRY)` $(LIBINSTALL)/parsecurry
	@test -e $(LIBINSTALL)/parsecurry
	@
ifdef LIB-BOOST-LIB-DIR
	@rm -f $(LIBINSTALL)/boost_lib_dir 2> /dev/null
	ln -s $(LIB-BOOST-LIB-DIR) $(LIBINSTALL)/boost_lib_dir
	@test -e $(LIBINSTALL)/boost_lib_dir
	@
endif
	rsync -a currylib $(LIBINSTALL)
	$(MAKE) currylibs

uninstall :
	$(MAKE) -C sprite-rt uninstall
	-rm -rf $(LIBINSTALL)/cmc
	-rm -rf $(LIBINSTALL)/cc
	-rm -rf $(LIBINSTALL)/llc
	-rm -rf $(LIBINSTALL)/opt
	-rm -rf $(LIBINSTALL)/currylib
	-rm -rf $(LIBINSTALL)/pakcs
	-rm -rf $(LIBINSTALL)/parsecurry
	-rm -rf $(LIBINSTALL)/boost_lib_dir

# Preprocess the Prelude and append static content.
$(LIBINSTALL)/currylib/.curry/Prelude.read : $(LIBINSTALL)/currylib/Prelude.curry
	$(BININSTALL)/scc -E $(LIBINSTALL)/currylib/Prelude.curry

# Preprocess the Prelude.
$(LIBINSTALL)/currylib/.curry/Prelude.bc : $(LIBINSTALL)/currylib/.curry/Prelude.read
	$(BININSTALL)/scc -c $(LIBINSTALL)/currylib/Prelude.curry

