######## Makefile ########
# $Id: Makefile,v 1.2 1998/10/26 10:47:44 jacob Exp $

######## resource files ########
SOURCES = probability.c interactive.c display.c simulate.c
OBJECTS = $(SOURCES:.c=.o)

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

######## link options ########
LD = gcc
LIBDIR = 
LIBRARIES = #-lm 
LDFLAGS = $(LIBDIR)

######## rules ########

all: bbjd simbj

bbjd: $(OBJECTS)
	@echo --- linking: $@ ---
	$(LD) $(LDFLAGS) probability.o interactive.o display.o $(LIBRARIES) -o $@

simbj: $(OBJECTS)
	@echo --- linking: $@ ---
	$(LD) $(LDFLAGS) probability.o simulate.o $(LIBRARIES) -o $@

%o: %c
	@echo --- compiling: $< ---
	$(CC) $(CFLAGS) $(COPTS) -c $< -o $@

clean:
	rm -f probability.o interactive.o display.o simulate.o core

distclean: clean
	rm -f bbjd simbj .simopts .depend .depend.bak

depend:
	@touch .depend
	makedepend -- $(CFLAGS) -- $(SOURCES) -f .depend

.PHONY: dummy


-include .depend
