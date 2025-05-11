# Makefile
# Copyright (C) 2008 Jean-Philippe Guillemin <h1p8r10n@yandex.com>
# license: This software is under GPL version 2 of license


NAME = netpkg
VERSION = 10.0
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

all : vfilter listbuilder $(NAME).po

vfilter : vfilter.o 
	gcc $(CFLAGS) $(DEFINE) -o $@ $^

vfilter.o : vfilter.c 
	gcc $(CFLAGS) $(DEFINE) -c $^
	
listbuilder : listbuilder.o 
	gcc $(CFLAGS) $(DEFINE) -o $@ $^

listbuilder.o : listbuilder.c 
	gcc $(CFLAGS) $(DEFINE) -c $^

debug: DEFINE += -DDEBUG
debug: CFLAGS += -g
debug: vfilter listbuilder


netpkg.po: 
	xgettext -s --from-code=UTF-8 --language=Shell -d netpkg -o netpkg.po zenpkg
	sed -i 's/CHARSET/UTF-8/' netpkg.po
	for PO in `ls po/*.po` ; do \
		msgmerge -U -N --previous $$PO netpkg.po; \
		LANGUAGE=`basename $$PO .po`; \
		echo -n "$$LANGUAGE.mo : "; \
		msgfmt -o mo/$$LANGUAGE.mo -v $$PO; \
	done;	
	rm -f po/*.po~

libinstall : vfilter listbuilder 
	strip vfilter	
	strip listbuilder
	install -D -m 0755 -o root -g root vfilter $(LIBEXECPATH)/vfilter
	install -D -m 0755 -o root -g root listbuilder $(LIBEXECPATH)/listbuilder

install : netpkg zenpkg netpkg.desktop libinstall $(NAME).po
	mkdir -p $(SMALLICONPATH) $(ICONPATH) $(DOCPATH) $(SOURCESPATH) $(DESKTOPPATH)
	install -D -m 0755 -o root -g root netpkg $(SBINPATH)/netpkg
	install -D -m 0755 -o root -g root zenpkg $(SBINPATH)/zenpkg
	install -D -m 0755 -o root -g root netpkg.desktop $(DESKTOPPATH)/netpkg.desktop
	install -D -m 644 -o root -g root netpkg.conf $(CONFIGPATH)/netpkg.conf
	install -D -m 644 -o root -g root icons/scalable/*.svg $(ICONPATH)/
	install -D -m 644 -o root -g root icons/16x16/*.png $(SMALLICONPATH)/
	install -m 644 -o root -g root doc/* $(DOCPATH)
	install -m 644 -o root -g root netpkg.d/* $(SOURCESPATH)
	install -m 644 -o root -g root netpkg.d/.* $(SOURCESPATH)
	for MO in `ls mo/*.mo` ; do \
		LANGUAGE=`basename $$MO .mo`; \
		mkdir -p $(LOCALEPATH)/$$LANGUAGE/LC_MESSAGES; \
		install -D -m 644 -o root -g root $$MO $(LOCALEPATH)/$$LANGUAGE/LC_MESSAGES/netpkg.mo; \
	done;
	mkdir -p $(VARPATH)
	install -D -m 644 -o root -g root DepsDB $(VARPATH)/DepsDB

uninstall :
	rm -f $(LIBEXECPATH)/vfilter $(LIBEXECPATH)/listbuilder
	rm -f $(SBINPATH)/netpkg 
	rm -f $(SBINPATH)/zenpkg
	rm -rf $(DOCPATH)
	rm -rf $(SOURCESPATH)
	rm -f $(CONFIGPATH)/netpkg.conf
	rm -f $(LOCALEPATH)/*/LC_MESSAGES/$(NAME).mo
	rm -f $(VARPATH)/DepsDB
  
clean :
	rm -f vfilter.o vfilter listbuilder.o listbuilder netpkg.po po/*.po~ mo/*.mo $(NAME).po


	




