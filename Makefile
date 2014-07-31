.PHONY : all clean examples

all : examples

libs :
	cd lib; $(MAKE)

examples :
	cd examples; $(MAKE)

clean :
	cd lib; $(MAKE) clean
	cd examples; $(MAKE) clean
	cd runtime; $(MAKE) clean
	cd tools; $(MAKE) clean
