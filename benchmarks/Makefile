SHELL := /bin/bash
BENCHMARKS :=      \
    ExpVarFunPats  \
    Half           \
    Last           \
    LastFunPats    \
    PaliFunPats    \
    Primes         \
    PrimesBuiltin  \
    PrimesPeano    \
    Queens         \
    QueensUser     \
    Reverse        \
    ReverseBuiltin \
    ReverseHO      \
    ReverseUser    \
    ShareNonDet    \
    Tak            \
    TakPeano       \
  ##

  ## Giving junk output:
    # ExpSimpFunPats

  ## A bit to slow:
    #  PermSortPeano
    # PermSort

  ## Not compiling:
    #  NDNums
    #  RegExp

KICS2_EXES = $(BENCHMARKS:=.kics2)
SPRITE_EXES = $(BENCHMARKS:=.sprite)

KICS2_LOGS = $(BENCHMARKS:=.kics2.run)
SPRITE_LOGS = $(BENCHMARKS:=.sprite.run)

KICS2 = $(shell which kics2)
SCC = $(shell which scc)

.PHONY : clean clean-exes clean-logs compile compile-kics2 compile-sprite run run-kics2 run-sprite

result.log : $(KICS2_LOGS) $(SPRITE_LOGS)
	@rm -f $@
	@{\
		set -e ;\
		echo '======================================================' ;\
		echo 'Test            KiCS2      Sprite     Comment         ' ;\
		echo '------------------------------------------------------' ;\
		for name in $(BENCHMARKS); do \
			k_ans="$$name.kics2.run" ;\
			s_ans="$$name.sprite.run" ;\
			comment='' ;\
			if [ "`head -1 $$k_ans`" != "`head -1 $$s_ans`" ] ;\
				then comment='answers differ' ;\
			fi ;\
			k_time=`perl -ne 'print $$1 if /(\S+)elapsed/' $$name.kics2.run` ;\
			s_time=`perl -ne 'print $$1 if /(\S+)elapsed/' $$name.sprite.run` ;\
			printf %-16s%-11s%-11s%s\\n $$name $$k_time $$s_time $$comment ;\
		done ;\
	} >> $@
	@echo Results written to $@

run : run-kics2 run-sprite
run-kics2 : $(KICS2_LOGS)
run-sprite : $(SPRITE_LOGS)

compile : compile-kics2 compile-sprite
compile-kics2 : $(KICS2_EXES)
compile-sprite : $(SPRITE_EXES)

clean : clean-log clean-exe
clean-logs :
	rm -f $(KICS2_LOGS) $(SPRITE_LOGS)
clean-exes : clean
	rm -f $(KICS2_EXES) $(SPRITE_EXES)

$(KICS2_EXES): %.kics2: %.curry $(KICS2)
	kics2 :l $< :save :q
	mv $(<:.curry=) $@
$(SPRITE_EXES): %.sprite: %.curry $(SCC)
	scc -o $@ $<

$(KICS2_LOGS) : %.kics2.run : %.curry %.kics2
	@echo Starting $(<:.curry=.kics2) at $(shell date +%r)...
	@rm -f $@
	@PATH=.:$(PATH) timeout 20m time $(<:.curry=.kics2) >> $@ 2>&1
$(SPRITE_LOGS) : %.sprite.run : %.curry %.sprite
	@echo Starting $(<:.curry=.sprite) at $(shell date +%r)...
	@rm -f $@
	@PATH=.:$(PATH) timeout 20m time $(<:.curry=.sprite) >> $@ 2>&1


