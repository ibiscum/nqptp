SUBDIRS = src tests

# Remove stale top-level build artifacts from pre-reorg layouts so
# distclean returns the tree to a configured-source state.
LEGACY_TOP_BUILD_ARTIFACTS = \
	$(top_builddir)/nqptp \
	$(top_builddir)/debug.o \
	$(top_builddir)/general-utilities.o \
	$(top_builddir)/nqptp.o \
	$(top_builddir)/nqptp-clock-sources.o \
	$(top_builddir)/nqptp-message-handlers.o \
	$(top_builddir)/nqptp-utilities.o \
	$(top_builddir)/gitversion.h \
	$(top_builddir)/gitversion-stamp

distclean-local:
	rm -f $(LEGACY_TOP_BUILD_ARTIFACTS)
	rm -rf $(top_builddir)/.deps

AUTOTOOLS_GENERATED_FILES = \
	$(top_srcdir)/Makefile.in \
	$(top_srcdir)/src/Makefile.in \
	$(top_srcdir)/tests/Makefile.in \
	$(top_srcdir)/aclocal.m4 \
	$(top_srcdir)/ar-lib \
	$(top_srcdir)/compile \
	$(top_srcdir)/config.guess \
	$(top_srcdir)/config.h.in \
	$(top_srcdir)/config.sub \
	$(top_srcdir)/configure \
	$(top_srcdir)/depcomp \
	$(top_srcdir)/install-sh \
	$(top_srcdir)/ltmain.sh \
	$(top_srcdir)/mdate-sh \
	$(top_srcdir)/missing \
	$(top_srcdir)/py-compile \
	$(top_srcdir)/test-driver \
	$(top_srcdir)/ylwrap

maintainer-clean-local:
	rm -f $(AUTOTOOLS_GENERATED_FILES)
	rm -f $(PACKAGE)-$(VERSION).tar.gz $(PACKAGE)-$(VERSION).tar.bz2 $(PACKAGE)-$(VERSION).tar.xz $(PACKAGE)-$(VERSION).zip
	find $(top_srcdir) -type f -name '*~' -delete
	rm -rf $(top_srcdir)/autom4te.cache

EXTRA_DIST = \
	check-gitversion \
	nqptp.service.in \
	nqptp.freebsd \
	README.md \
	RELEASE_NOTES.md \
	CHANGELOG.md
