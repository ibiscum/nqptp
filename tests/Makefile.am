AM_CFLAGS = -fno-common -Wall -Wextra -pthread --include=$(top_builddir)/config.h
AM_CPPFLAGS = -I$(top_srcdir)/include $(CMOCKA_CFLAGS) $(SYSTEMD_CFLAGS) -DTOP_SRCDIR=\"$(top_srcdir)\"

check_PROGRAMS = unit-general-utilities unit-nqptp-source-invariants unit-nqptp-utilities-source-invariants unit-nqptp-clock-sources-source-invariants unit-nqptp-logging-config-source-invariants unit-nqptp-message-handlers-source-invariants regression-multi-client-isolation
unit_general_utilities_SOURCES = \
	unit/test_general_utilities.c \
	$(top_srcdir)/src/general-utilities.c \
	$(top_srcdir)/src/debug.c
unit_general_utilities_LDADD = $(CMOCKA_LIBS) $(SYSTEMD_LIBS)
unit_nqptp_source_invariants_SOURCES = unit/test_nqptp_source_invariants.c
unit_nqptp_source_invariants_LDADD = $(CMOCKA_LIBS)
unit_nqptp_utilities_source_invariants_SOURCES = unit/test_nqptp_utilities_source_invariants.c
unit_nqptp_utilities_source_invariants_LDADD = $(CMOCKA_LIBS)
unit_nqptp_clock_sources_source_invariants_SOURCES = unit/test_nqptp_clock_sources_source_invariants.c
unit_nqptp_clock_sources_source_invariants_LDADD = $(CMOCKA_LIBS)
unit_nqptp_logging_config_source_invariants_SOURCES = unit/test_nqptp_logging_config_source_invariants.c
unit_nqptp_logging_config_source_invariants_LDADD = $(CMOCKA_LIBS)
unit_nqptp_message_handlers_source_invariants_SOURCES = unit/test_nqptp_message_handlers_source_invariants.c
unit_nqptp_message_handlers_source_invariants_LDADD = $(CMOCKA_LIBS)
regression_multi_client_isolation_SOURCES = regression/test_multi_client_isolation.c
regression_multi_client_isolation_LDADD = $(CMOCKA_LIBS)

TESTS = $(check_PROGRAMS)

EXTRA_DIST = unit/test_general_utilities.c unit/test_nqptp_source_invariants.c unit/test_nqptp_utilities_source_invariants.c unit/test_nqptp_clock_sources_source_invariants.c unit/test_nqptp_logging_config_source_invariants.c unit/test_nqptp_message_handlers_source_invariants.c regression/test_multi_client_isolation.c
