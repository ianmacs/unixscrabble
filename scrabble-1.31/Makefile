SHELL   = /bin/sh
CC      = cc

# Add the flag -DCOMPILE_GCC if you are compiling with gcc instead of cc.
# For BSD systems, use -DBSD; for System V, use -DUSG.
#
# Add the flag -DCOMPRESSED_DICT if your dictionary file is compressed.
# Add the flag -DPRINT_CPU_TILES if you want the computer players' tiles to
#   be displayed at all times.
# Add the flag -DDBLSIZE_BOARD if you want the board width to be 29 characters;
#   the default width is 15 characters.

PREFIX = /opt/scrabble
BINDIR = $(PREFIX)/bin
LIBDIR = $(PREFIX)/lib
MANDIR = $(PREFIX)/man/man6


CFLAGS  = -O -DUSG -DDICT_FILE=\"$(LIBDIR)/scrabbledict\"

LIBS    = /usr/lib/libtermcap.a

scrabble: cmove.o misc.o init.o smain.o pmove.o score.o
		$(CC) smain.o cmove.o misc.o init.o pmove.o score.o -o $@ $(LIBS)

clean:
	rm -f scrabble *.o core

install:
	mkdir -p -m 755 $(PREFIX) $(BINDIR) $(LIBDIR) $(MANDIR)
	bsdinst -m 755 scrabble $(BINDIR)
	bsdinst -m 644 scrabbledict $(LIBDIR)
	bsdinst -m 644 scrabble.6 $(MANDIR)

cmove.o pmove.o init.o misc.o score.o smain.o: scrab.h globals.h
