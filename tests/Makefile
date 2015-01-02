SOURCES = $(wildcard *.curry)
EXECUTABLES = $(SOURCES:.curry=.exe)
RESULTS = $(SOURCES:.curry=.result)
BAREWORDS = $(EXECUTABLES:.exe=)

# Targets like plain.check are phony targets used to move failing tests to the
# known_failures subdirectory.
CHECKCURRY = $(SOURCES:.curry=.check)

# Targets like plain.clean are phony targets used to remove the PACKS answer
# from a .curry file.
CLEANGOLD = $(SOURCES:.curry=.clean)

# Targets like plain.gold are phony targets used to add the PACKS answer
# to a .curry file.
GOLDCURRY = $(SOURCES:.curry=.gold)

.PHONY : check clean cleangold run $(CHECKCURRY) $(CLEANGOLD) $(GOLDCURRY)

# Run the tests and validate Sprite vs. PAKCS.
run : $(RESULTS) cytest.py
	@python cytest.py validate $(RESULTS)

# Check all tests.  Move failing tests to known_failures.
check :
	@$(MAKE) $(CHECKCURRY)

# Clean up.
clean :
	@rm -f *.exe *.result

# Remove PAKCS answers from .curry files.
cleangold :
	@$(MAKE) $(CLEANGOLD)
	
# Add PAKCS answers to .curry files.
mkgold :
	@$(MAKE) $(GOLDCURRY)

%.exe : %.curry
	$(BININSTALL)/scc -o $@ $<

# Error lines from cytest.py are prepended with $$.  Grep is used to display
# those when make runs.  I don't know any way to log both stdout and a *copy*
# of stderr to a file.
%.result : %.gold %.exe cytest.py
	@python cytest.py compare $(<:.gold=.curry) > $@ 2>&1 || grep '\$$\$$' $@

# Checks the test result.  If failing, move the test to known_failures.
$(CHECKCURRY) : cytest.py
	@$(MAKE) $(@:.check=) || (                     \
	    mv $(@:.check=.curry) known_failures;      \
			rm -f $(@:.check=.exe) $(@:.check=.result) \
	  )

$(BAREWORDS) : cytest.py
	@$(MAKE) $@.result
	@python cytest.py validate $@.result

$(CLEANGOLD) :
	@python cytest.py clean $(@:.clean=.curry)

$(GOLDCURRY) :
	@python cytest.py mkgold $(@:.gold=.curry)

include ../Make.include

$(EXECUTABLES) : $(BININSTALL)/scc

