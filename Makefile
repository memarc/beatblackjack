######## Makefile ########
# $Id: Makefile,v 1.3 2000/01/12 22:29:38 jacob Exp $

######## compiler options ########
CC = gcc
LANG_OPTIONS = 
WARN_OPTIONS = -Wall -Wtraditional -Wpointer-arith \
               -Wconversion -Wstrict-prototypes -Wmissing-prototypes \
               -Wmissing-declarations -Wreturn-type \
               -Wnested-externs -Wwrite-strings -Wcast-qual
INCDIR = -I. -I/usr/include
CFLAGS = $(DEFINES) $(LANG_OPTIONS) $(WARN_OPTIONS) $(INCDIR)
COPTS = -O2

######## compile options for perlwrappers ########
PERLWRAPPER=swig -perl5 -Isrc/ -dnone
PERLINC=/usr/lib/perl5/5.00502/i586-linux/CORE
PERLOPTS=$(COPTS) $(DEFINES) $(LANG_OPTIONS) -Dbool=char

######## link options ########
LD = gcc
SHARED_LD = gcc -shared
LIBDIR = 
LIBRARIES = #-lm 
LDFLAGS = $(LIBDIR)

######## install options ########
INSTALLBASE = /usr/local/bbjd
INSTALLDIR = install -m 755 -d 
INSTALLFILE = install -m 644
INSTALLEXE = install -m 755

######## rules ########

default: bbjd simbj basicbbjd basicsimbj

perlmodules: cgi-bin/bbjd.pm cgi-bin/bbjd.so cgi-bin/simbj.pm cgi-bin/simbj.so cgi-bin/basicsimbj.pm cgi-bin/basicsimbj.so

all: default perlmodules

bbjd: src/probability.o src/interactive.o src/display.o
	@echo --- linking: $@ ---
	$(LD) $(LDFLAGS) $^ $(LIBRARIES) -o $@

basicbbjd: src/basic_probability.o src/interactive.o src/display.o
	@echo --- linking: $@ ---
	$(LD) $(LDFLAGS) $^ $(LIBRARIES) -o $@

cgi-bin/bbjd.pm: cgi-bin/bbjd.so

cgi-bin/bbjd.so: src/probability_wrap.o src/probability.o
	@echo --- linking: $@ ---
	$(SHARED_LD) $^ -o $@

simbj: src/probability.o src/simulate.o src/simbj.o
	@echo --- linking: $@ ---
	$(LD) $(LDFLAGS) $^ $(LIBRARIES) -o $@

basicsimbj: src/basic_probability.o src/simulate.o src/simbj.o
	@echo --- linking: $@ ---
	$(LD) $(LDFLAGS) $^ $(LIBRARIES) -o $@

cgi-bin/simbj.so: src/simbj_wrap.o src/simulate.o src/probability.o 
	@echo --- linking: $@ ---
	$(SHARED_LD) $^ -o $@

cgi-bin/basicsimbj.so: src/basicsimbj_wrap.o src/simulate.o src/basic_probability.o 
	@echo --- linking: $@ ---
	$(SHARED_LD) $^ -o $@

src/%_wrap.o: src/%_wrap.c
	@echo --- compiling: $< ---
	$(CC) -I$(PERLINC) $(PERLOPTS) -c $< -o $@

src/%o: src/%c
	@echo --- compiling: $< ---
	$(CC) $(CFLAGS) $(COPTS) -c $< -o $@

src/probability_wrap.c: src/probability.i src/probability.h
	@echo --- generating: $@ ---
	$(PERLWRAPPER) $< 
	mv bbjd.pm cgi-bin/

src/simbj_wrap.c cgi-bin/simbj.pm: src/simbj.i src/simulate.h
	@echo --- generating: $@ ---
	$(PERLWRAPPER) $<
	mv simbj.pm cgi-bin/

src/basicsimbj_wrap.c cgi-bin/basicsimbj.pm: src/basicsimbj.i src/simulate.h
	@echo --- generating: $@ ---
	$(PERLWRAPPER) $<
	mv basicsimbj.pm cgi-bin/

install: all
	@echo --- installing ---
	$(INSTALLDIR) $(INSTALLBASE)
	$(INSTALLFILE) Interactive $(INSTALLBASE)
	$(INSTALLFILE) README $(INSTALLBASE)
	$(INSTALLFILE) Simulation $(INSTALLBASE)
	$(INSTALLFILE) Theory $(INSTALLBASE)
	$(INSTALLDIR) $(INSTALLBASE)/bin
	$(INSTALLEXE) bbjd $(INSTALLBASE)/bin
	$(INSTALLEXE) basicbbjd $(INSTALLBASE)/bin
	$(INSTALLEXE) simbj $(INSTALLBASE)/bin
	$(INSTALLEXE) basicsimbj $(INSTALLBASE)/bin
	$(INSTALLDIR) $(INSTALLBASE)/cgi-bin
	$(INSTALLFILE) cgi-bin/basicsimbj.pm $(INSTALLBASE)/cgi-bin
	$(INSTALLEXE) cgi-bin/basicsimbj.so $(INSTALLBASE)/cgi-bin
	$(INSTALLFILE) cgi-bin/bbjd.pm $(INSTALLBASE)/cgi-bin
	$(INSTALLEXE) cgi-bin/bbjd.so $(INSTALLBASE)/cgi-bin
	$(INSTALLEXE) cgi-bin/bjtables.pl $(INSTALLBASE)/cgi-bin
	$(INSTALLEXE) cgi-bin/playbj.pl $(INSTALLBASE)/cgi-bin
	$(INSTALLFILE) cgi-bin/simbj.pm $(INSTALLBASE)/cgi-bin
	$(INSTALLEXE) cgi-bin/simbj.so $(INSTALLBASE)/cgi-bin
	$(INSTALLDIR) $(INSTALLBASE)/htdocs
	$(INSTALLFILE) htdocs/*.gif $(INSTALLBASE)/htdocs
	$(INSTALLFILE) htdocs/*.html $(INSTALLBASE)/htdocs
	$(INSTALLFILE) htdocs/*.js $(INSTALLBASE)/htdocs
	$(INSTALLDIR) $(INSTALLBASE)/htdocs/cards
	$(INSTALLFILE) htdocs/cards/*.gif $(INSTALLBASE)/htdocs/cards

clean:
	rm -f src/*.o src/*_wrap.c core

distclean: clean
	rm -f bbjd basicbbjd simbj basicsimbj cgi-bin/*.pm cgi-bin/*.so .simopts .depend .depend.bak

tar.gz: distclean
	cd ..; tar czf bbjd-src-1.0.3.tar.gz bbjd/

depend:
	@touch .depend
	makedepend -- $(CFLAGS) -I$(PERLINC) -- src/*.c -f .depend

-include .depend

