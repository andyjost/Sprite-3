.PHONY : all clean examples

all : examples

libs :
	cd src; $(MAKE)

examples :
	cd examples; $(MAKE)

clean :
	cd src; $(MAKE) clean
	cd examples; $(MAKE) clean
	cd runtime; $(MAKE) clean
	cd tools; $(MAKE) clean
