DESTDIR=/usr
ccwm: main.c
	$(CC) -O3 -Wall -Wextra -pedantic -lX11 -o ccwm $<

install: ccwm
	cp -f ccwm "$(DESTDIR)$(PREFIX)/bin"
	chmod 755 "$(DESTDIR)$(PREFIX)/bin/ccwm"

uninstall:
	rm -f "$(DESTDIR)$(PREFIX)/bin/ccwm"
	rm -f "/bin/ccwm"

