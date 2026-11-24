bin_PROGRAMS = nqptp
nqptp_SOURCES = \
	nqptp.c \
	nqptp-clock-sources.c \
	nqptp-message-handlers.c \
	nqptp-utilities.c \
	general-utilities.c \
	debug.c

noinst_HEADERS = \
	$(top_srcdir)/include/debug.h \
	$(top_srcdir)/include/general-utilities.h \
	$(top_srcdir)/include/nqptp.h \
	$(top_srcdir)/include/nqptp-clock-sources.h \
	$(top_srcdir)/include/nqptp-message-handlers.h \
	$(top_srcdir)/include/nqptp-ptp-definitions.h \
	$(top_srcdir)/include/nqptp-shm-structures.h \
	$(top_srcdir)/include/nqptp-utilities.h

man_MANS = nqptp.8

AM_CFLAGS = -fno-common -Wall -Wextra -pthread --include=$(top_builddir)/config.h
AM_CPPFLAGS = -I$(top_srcdir)/include $(SYSTEMD_CFLAGS)
LDADD = $(SYSTEMD_LIBS)

if USE_GIT_VERSION
.PHONY: gitversion-check
gitversion-check:
	cd $(top_builddir)/src && $(top_srcdir)/check-gitversion

BUILT_SOURCES = gitversion-check
endif

DISTCLEANFILES = gitversion-stamp gitversion.h

install-exec-hook:
if BUILD_FOR_LINUX
if INSTALL_SYSTEMD_STARTUP
	[ -e $(DESTDIR)$(systemdsystemunitdir) ] || mkdir -p $(DESTDIR)$(systemdsystemunitdir)
	[ -e $(DESTDIR)$(systemdsystemunitdir)/nqptp.service ] || cp $(top_builddir)/nqptp.service $(DESTDIR)$(systemdsystemunitdir)
endif
endif

if BUILD_FOR_FREEBSD
if INSTALL_FREEBSD_STARTUP
	cp $(top_srcdir)/nqptp.freebsd /usr/local/etc/rc.d/nqptp
	chmod 555 /usr/local/etc/rc.d/nqptp
endif
endif
