.PHONY : all clean install examples libs runtime tools uninstall

all : runtime examples tools

clean :
	cd src; $(MAKE) clean
	cd examples; $(MAKE) clean
	cd runtime; $(MAKE) clean
	cd tools; $(MAKE) clean

install : runtime tools
	cd runtime; $(MAKE) install
	cd tools; $(MAKE) install

uninstall :
	cd runtime; $(MAKE) uninstall
	cd tools; $(MAKE) uninstall
	-rmdir $(BININSTALL) 2>&1 > /dev/null
	-rmdir $(LIBINSTALL) 2>&1 > /dev/null

###

examples :
	cd examples; $(MAKE)

libs :
	cd src; $(MAKE)

runtime :
	cd runtime; $(MAKE)

tools :
	cd tools; $(MAKE)


-include Make.include
