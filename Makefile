# Makefile for figlet version 2.2.2 (05 July 2005) 
# adapted from Makefile for figlet version 2.2 (15 Oct 1996)
# Copyright 1993, 1994,1995 Glenn Chappell and Ian Chai
# Copyright 1996, 1997, 1998, 1999, 2000, 2001 John Cowan
# Copyright 2002 Christiaan Keet

# Please make sure DESTDIR, MANDIR, DEFAULTFONTDIR and
#   DEFAULTFONTFILE are defined to reflect the situation
#   on your computer.  See README for details.

# Don't change this even if your shell is different. The only reason
# for changing this is if sh is not in the same place.
SHELL = /bin/sh
CC = gcc
CFLAGS = -g

# Where the executables should be put
DESTDIR = /usr/local/bin

# Where the man page should be put
MANDIR = /usr/local/man/man6

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

SOURCES = figlet.c zipio.c crc.c inflate.c

all: figlet chkfont

figlet: $(SOURCES)
	$(CC) $(CFLAGS) -DDEFAULTFONTDIR=\"$(DEFAULTFONTDIR)\" \
	   -DDEFAULTFONTFILE=\"$(DEFAULTFONTFILE)\" \
	   $(SOURCES) -o figlet
	chmod a+x figlet

chkfont: chkfont.c
	$(CC) $(CFLAGS) -o chkfont chkfont.c

clean:
	rm -f *.o figlet chkfont

install: figlet chkfont
	mkdir -p $(DEFAULTFONTDIR)
	cp figlet $(DESTDIR)
	cp figlet.6 $(MANDIR)
	cp chkfont $(DESTDIR)
	cp figlist $(DESTDIR)
	cp showfigfonts $(DESTDIR)
	cp fonts/*.flf $(DEFAULTFONTDIR)
	cp fonts/*.flc $(DEFAULTFONTDIR)
