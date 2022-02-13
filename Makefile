DESTDIR=/usr
st: main.c
	$(CC) -O3 -Wall -Wextra -pedantic -lX11 -o cwm $<

install: cwm
	cp -f cwm "$(DESTDIR)$(PREFIX)/bin"
	chmod 755 "$(DESTDIR)$(PREFIX)/bin/cwm"

uninstall:
	rm -f "$(DESTDIR)$(PREFIX)/bin/cwm"
	rm -f "/bin/cwm"

