
.PHONY : all clean currylibs install uninstall

all :
	$(MAKE) -C sprite-rt all

clean :
	$(MAKE) -C sprite-rt clean

currylibs :
	$(MAKE) $(LIBINSTALL)/currylib/.curry/Prelude.read

install :
	@echo "Installing runtime at $(LIBINSTALL)"
	mkdir -p $(LIBINSTALL)
	$(MAKE) -C sprite-rt install
	cp -r cmc/ $(LIBINSTALL)
	rm -f $(LIBINSTALL)/cc 2> /dev/null
	ln -s `which $(LIB-CC)` $(LIBINSTALL)/cc
	rm -f $(LIBINSTALL)/llc 2> /dev/null
	ln -s `which $(LIB-LLC)` $(LIBINSTALL)/llc
	rsync -a currylib $(LIBINSTALL)
	$(MAKE) currylibs

uninstall :
	$(MAKE) -C sprite-rt uninstall
	-rm -rf $(LIBINSTALL)/cmc
	-rm -rf $(LIBINSTALL)/cc
	-rm -rf $(LIBINSTALL)/llc
	-rm -rf $(LIBINSTALL)/currylib

-include ../Make.include

# Compile the Prelude and append static content.
$(LIBINSTALL)/currylib/.curry/Prelude.read : $(LIBINSTALL)/currylib/Prelude.curry
	$(BININSTALL)/scc -c $(LIBINSTALL)/currylib/Prelude.curry
	cat Prelude.read.static_content >> $(LIBINSTALL)/currylib/.curry/Prelude.read

