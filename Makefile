# Makefile
# Copyright (C) 2008 Jean-Philippe Guillemin <h1p8r10n@yandex.com>
# license: This software is under GPL version 2 of license


NAME = netpkg
VERSION = 12.0
DESTDIR =
LIBSUFFIX = 64
PREFIX = /usr
LOCALEPATH = $(DESTDIR)$(PREFIX)/share/locale
SBINPATH = $(DESTDIR)$(PREFIX)/sbin
BINPATH = $(DESTDIR)$(PREFIX)/bin
LIBEXECPATH = $(DESTDIR)$(PREFIX)/libexec
MANPATH = $(DESTDIR)$(PREFIX)/man/man8
VARPATH = $(DESTDIR)/var/netpkg
DOCPATH = $(DESTDIR)$(PREFIX)/doc/$(NAME)-$(VERSION)
DESKTOPPATH = $(DESTDIR)/etc/xdg/autostart
ICONPATH = $(DESTDIR)$(PREFIX)/share/icons/hicolor/scalable/apps
SMALLICONPATH = $(DESTDIR)$(PREFIX)/share/icons/hicolor/16x16/apps
CONFIGPATH = $(DESTDIR)/etc
SOURCESPATH = $(DESTDIR)/etc/netpkg.d

DEFINE = -D_REENTRANT

all : vfilter

vfilter : vfilter.o
	gcc $(CFLAGS) $(DEFINE) -o $@ $^

vfilter.o : vfilter.c
	gcc $(CFLAGS) $(DEFINE) -c $^

debug: DEFINE += -DDEBUG
debug: CFLAGS += -g
debug: vfilter

libinstall : vfilter
	strip vfilter
	install -D -m 0755 -o root -g root vfilter $(LIBEXECPATH)/vfilter

install : netpkg zenpkg netpkg.desktop libinstall
	mkdir -p $(SMALLICONPATH) $(ICONPATH) $(DOCPATH) $(SOURCESPATH) $(DESKTOPPATH)
	install -D -m 0755 -o root -g root netpkg $(SBINPATH)/netpkg
	install -D -m 0755 -o root -g root zenpkg $(SBINPATH)/zenpkg
	install -D -m 0755 -o root -g root netpkg.desktop $(DESKTOPPATH)/netpkg.desktop
	install -D -m 0755 -o root -g root netpkg.8 $(MANPATH)/netpkg.8
	install -D -m 644 -o root -g root netpkg.conf $(CONFIGPATH)/netpkg.conf
	install -D -m 644 -o root -g root icons/scalable/*.svg $(ICONPATH)/
	install -D -m 644 -o root -g root icons/16x16/*.png $(SMALLICONPATH)/
	install -m 644 -o root -g root doc/* $(DOCPATH)
	install -m 644 -o root -g root netpkg.d/* $(SOURCESPATH)
	install -m 644 -o root -g root netpkg.d/.* $(SOURCESPATH)
	mkdir -p $(VARPATH)
	install -D -m 644 -o root -g root DepsDB $(VARPATH)/DepsDB

uninstall :
	rm -f $(LIBEXECPATH)/vfilter
	rm -f $(SBINPATH)/netpkg
	rm -f $(SBINPATH)/zenpkg
	rm -rf $(DOCPATH)
	rm -rf $(SOURCESPATH)
	rm -f $(CONFIGPATH)/netpkg.conf
	rm -f $(VARPATH)/DepsDB
	rm -f $(MANPATH)/netpkg.8

clean :
	rm -f vfilter.o vfilter







