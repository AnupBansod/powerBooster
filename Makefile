VERSION = 1.0

BINDIR=/usr/bin
LOCALESDIR=/usr/share/locale
MANDIR=/usr/share/man/man8
WARNFLAGS=-Wall -Wshadow -W -Wformat -Wimplicit-function-declaration -Wimplicit-int
CFLAGS?=-O1 -g ${WARNFLAGS}
CC?=gcc

CFLAGS+=-D VERSION=\"$(VERSION)\"

OBJS = powerbooster.o keyboardhit.o ahci-alpm.o sata.o bluetooth.o ondemand.o usb.o wifi-new.o alsa-power.o ethernet.o urbnum.o misc.o

powerbooster: $(OBJS) Makefile
	$(CC) ${CFLAGS} $(LDFLAGS) $(OBJS) -lpthread -lncursesw -o powerbooster

powerbooster.8.gz: powerbooster.8
	gzip -c $< > $@

install: powerbooster powerbooster.8.gz
	mkdir -p ${DESTDIR}${BINDIR}
	cp powerbooster ${DESTDIR}${BINDIR}
	mkdir -p ${DESTDIR}${MANDIR}
	cp powerbooster.8.gz ${DESTDIR}${MANDIR}

clean:
	rm -f *~ powerbooster powerbooster.8.gz po/powerbooster.pot DEADJOE svn-commit* *.o *.orig 

