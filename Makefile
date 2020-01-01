######## Makefile ########
# $Id: Makefile,v 1.3 2000/01/12 22:29:38 jacob Exp $

######## compiler options ########
CC = gcc
LANG_OPTIONS = 
WARN_OPTIONS = -Wall -Wpointer-arith -Wconversion -Wstrict-prototypes -Wmissing-prototypes \
               -Wmissing-declarations -Wreturn-type \
               -Wnested-externs -Wwrite-strings -Wcast-qual
INCDIR = -I. -I/usr/include
CFLAGS = $(DEFINES) $(LANG_OPTIONS) $(WARN_OPTIONS) $(INCDIR)
COPTS = -O2 -fPIC

######## compile options for pythonwrappers ########
PYTHONWRAPPER=swig -python  
PYTHONINC=/usr/include/python2.4

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

default: bbjd simbj basicbbjd basicsimbj pythonmodules

pythonmodules: bbjd.py _bbjd.so simbj.py _simbj.so

all: default pythonmodules

bbjd: probability.o interactive.o display.o
	@echo --- linking: $@ ---
	$(LD) $(LDFLAGS) $^ $(LIBRARIES) -o $@

basicbbjd: basic_probability.o interactive.o display.o
	@echo --- linking: $@ ---
	$(LD) $(LDFLAGS) $^ $(LIBRARIES) -o $@

bbjd.py: _bbjd.so

_bbjd.so: probability_wrap.o probability.o
	@echo --- linking: $@ ---
	$(SHARED_LD) $^ -o $@

simbj: probability.o simulate.o simbj.o
	@echo --- linking: $@ ---
	$(LD) $(LDFLAGS) $^ $(LIBRARIES) -o $@

simbj.py: _simbj.so

_simbj.so: simbj_wrap.o simulate.o probability.o 
	@echo --- linking: $@ ---
	$(SHARED_LD) $^ -o $@

basicsimbj: basic_probability.o simulate.o simbj.o
	@echo --- linking: $@ ---
	$(LD) $(LDFLAGS) $^ $(LIBRARIES) -o $@

basicsimbj.so: basicsimbj_wrap.o simulate.o basic_probability.o 
	@echo --- linking: $@ ---
	$(SHARED_LD) $^ -o $@

%_wrap.o: %_wrap.c
	@echo --- compiling: $< ---
	$(CC) -I$(PYTHONINC) $(COPTS) -c $< -o $@

%o: %c
	@echo --- compiling: $< ---
	$(CC) $(CFLAGS) $(COPTS) -c $< -o $@

probability_wrap.c: probability.i probability.h
	@echo --- generating: $@ ---
	$(PYTHONWRAPPER) $< 

simbj_wrap.c simbj.pm: simbj.i simulate.h
	@echo --- generating: $@ ---
	$(PYTHONWRAPPER) $<

basicsimbj_wrap.c basicsimbj.pm: basicsimbj.i simulate.h
	@echo --- generating: $@ ---
	$(PYTHONWRAPPER) $<

clean:
	rm -f *.o *_wrap.c core

distclean: clean
	rm -f bbjd basicbbjd simbj basicsimbj .simopts .depend .depend.bak *.so

depend:
	@touch .depend
	makedepend -- $(CFLAGS) -- *.c -f .depend

-include .depend

