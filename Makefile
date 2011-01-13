# Makefile for figlet version 2.3.0 (11 Jan 2011) 
# adapted from Makefile for figlet version 2.2.2 (05 July 2005) 
# adapted from Makefile for figlet version 2.2 (15 Oct 1996)
# Copyright 1993, 1994,1995 Glenn Chappell and Ian Chai
# Copyright 1996, 1997, 1998, 1999, 2000, 2001 John Cowan
# Copyright 2002 Christiaan Keet
# Copyright 2011 Claudio Matsuoka

# Please notice that to follow modern standards and ease third-party
# package creation, binaries are now installed under BINDIR, and DESTDIR
# is reserved for the installation pathname prefix.
#
# Please make sure BINDIR, MANDIR, DEFAULTFONTDIR and
#   DEFAULTFONTFILE are defined to reflect the situation
#   on your computer.  See README for details.

# Don't change this even if your shell is different. The only reason
# for changing this is if sh is not in the same place.
SHELL = /bin/sh

# The C compiler and linker to use
CC	= gcc
CFLAGS	= -g -O2 -Wall
LD	= gcc
LDFLAGS =

# Where the executables should be put
BINDIR	= /usr/local/bin

# Where the man page should be put
MANDIR	= /usr/local/man

# Where figlet will search first for fonts (the ".flf" files).
DEFAULTFONTDIR = /usr/local/share/figlet
# Use this definition if you can't put things in /usr/local/share/figlet
DEFAULTFONTDIR = fonts

# The filename of the font to be used if no other is specified
#   (standard.flf is recommended, but any other can be used).
#   This font file should reside in the directory specified by
#   DEFAULTFONTDIR.
DEFAULTFONTFILE = standard.flf

##
##  END OF CONFIGURATION SECTION
##

VERSION	= 2.2.3
DIST	= figlet-$(VERSION)
OBJS	= figlet.o zipio.o crc.o inflate.o
BINS	= figlet chkfont
MANUAL	= figlet.6 chkfont.6 figlist.6 showfigfonts.6
DFILES	= Makefile Makefile.tc $(MANUAL) $(OBJS:.o=.c) figlist showfigfonts \
	  CHANGES FAQ README LICENSE figfont.txt crc.h inflate.h zipio.h

.c.o:
	$(CC) -c $(CFLAGS) -DDEFAULTFONTDIR=\"$(DEFAULTFONTDIR)\" \
		-DDEFAULTFONTFILE=\"$(DEFAULTFONTFILE)\" -o $*.o $<

all: $(BINS)

figlet: $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

chkfont: chkfont.o
	$(LD) $(LDFLAGS) -o $@ chkfont.o

clean:
	rm -f *.o *~ core figlet chkfont

install: all
	mkdir -p $(DESTDIR)$(BINDIR)
	mkdir -p $(DESTDIR)$(MANDIR)/man6
	mkdir -p $(DESTDIR)$(DEFAULTFONTDIR)
	cp $(BINS) $(DESTDIR)$(BINDIR)
	cp $(MANUAL) $(DESTDIR)$(MANDIR)/man6
	cp fonts/*.flf $(DESTDIR)$(DEFAULTFONTDIR)
	cp fonts/*.flc $(DESTDIR)$(DEFAULTFONTDIR)

dist:
	rm -Rf $(DIST) $(DIST).tar.gz
	mkdir $(DIST)/
	cp $(DFILES) $(DIST)/
	mkdir $(DIST)/fonts
	cp fonts/*.fl[fc] $(DIST)/fonts
	tar cvf - $(DIST) | gzip -9c > $(DIST).tar.gz
	rm -Rf $(DIST)
	ls -l $(DIST).tar.gz

$(OBJS) chkfont.o getopt.o: Makefile
chkfont.o: chkfont.c
crc.o: crc.c crc.h
figlet.o: figlet.c zipio.h
getopt.o: getopt.c
inflate.o: inflate.c inflate.h
zipio.o: zipio.c zipio.h inflate.h crc.h
