.PHONY : all clean install examples libs runtime tools uninstall

all : libs runtime tools examples

clean : uninstall
	$(MAKE) -C src clean
	$(MAKE) -C examples clean
	$(MAKE) -C runtime clean
	$(MAKE) -C tools clean

install : runtime tools
	$(MAKE) -C tools install
	$(MAKE) -C runtime install

uninstall :
	$(MAKE) -C runtime uninstall
	$(MAKE) -C tools uninstall
	-rmdir $(BININSTALL) 2>&1 > /dev/null
	-rmdir $(LIBINSTALL) 2>&1 > /dev/null

###

examples :
	$(MAKE) -C examples

libs :
	$(MAKE) -C src

runtime :
	$(MAKE) -C runtime

tools :
	$(MAKE) -C tools

-include Make.include
