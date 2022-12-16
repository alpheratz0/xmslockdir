.POSIX:
.PHONY: all clean install uninstall dist

include config.mk

all: xmslockdir

xmslockdir: xmslockdir.o
	$(CC) $(LDFLAGS) -o xmslockdir xmslockdir.o $(LDLIBS)

clean:
	rm -f xmslockdir xmslockdir.o xmslockdir-$(VERSION).tar.gz

install: all
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f xmslockdir $(DESTDIR)$(PREFIX)/bin
	chmod 755 $(DESTDIR)$(PREFIX)/bin/xmslockdir
	cp -f xmslockdir-toggle $(DESTDIR)$(PREFIX)/bin
	chmod 755 $(DESTDIR)$(PREFIX)/bin/xmslockdir-toggle
	mkdir -p $(DESTDIR)$(MANPREFIX)/man1
	cp -f xmslockdir.1 $(DESTDIR)$(MANPREFIX)/man1
	chmod 644 $(DESTDIR)$(MANPREFIX)/man1/xmslockdir.1

dist: clean
	mkdir -p xmslockdir-$(VERSION)
	cp -R COPYING config.mk Makefile README xmslockdir.1 xmslockdir.c xmslockdir-toggle xmslockdir-$(VERSION)
	tar -cf xmslockdir-$(VERSION).tar xmslockdir-$(VERSION)
	gzip xmslockdir-$(VERSION).tar
	rm -rf xmslockdir-$(VERSION)

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/xmslockdir
	rm -f $(DESTDIR)$(PREFIX)/bin/xmslockdir-toggle
	rm -f $(DESTDIR)$(MANPREFIX)/man1/xmslockdir.1
