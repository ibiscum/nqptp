#!/usr/bin/env python3
"""Unit checks for journald logging configuration + fallback wiring."""

import pathlib
import sys


ROOT = pathlib.Path(__file__).resolve().parent
CONFIGURE = ROOT / "configure.ac"
DEBUG_C = ROOT / "debug.c"
MAKEFILE_AM = ROOT / "Makefile.am"


def require(condition, message):
    if not condition:
        raise AssertionError(message)


def main():
    configure = CONFIGURE.read_text(encoding="utf-8")
    debug_c = DEBUG_C.read_text(encoding="utf-8")
    makefile_am = MAKEFILE_AM.read_text(encoding="utf-8")

    require("AC_ARG_WITH([journald-logging]" in configure,
            "configure should expose --with-journald-logging")
    require("CONFIG_USE_JOURNALD_LOGGING" in configure,
            "configure should define CONFIG_USE_JOURNALD_LOGGING when enabled")
    require("SYSTEMD_CFLAGS" in configure and "SYSTEMD_LIBS" in configure,
            "configure should export systemd compile/link flags")

    require("AM_CPPFLAGS = $(SYSTEMD_CFLAGS)" in makefile_am,
            "Makefile.am should apply SYSTEMD_CFLAGS")
    require("LDADD = $(SYSTEMD_LIBS)" in makefile_am,
            "Makefile.am should link SYSTEMD_LIBS")

    require("#ifdef CONFIG_USE_JOURNALD_LOGGING" in debug_c,
            "debug backend should compile journald path conditionally")
    require("sd_journal_send(" in debug_c,
            "debug backend should use native journald API when enabled")
    require("fprintf(stderr, \"%s\\n\", message);" in debug_c,
            "debug backend should keep stderr fallback")

    print("ALL NQPTP LOGGING CONFIG SOURCE INVARIANT CHECKS PASSED")


if __name__ == "__main__":
    try:
        main()
    except AssertionError as e:
        print(f"FAILED: {e}")
        sys.exit(1)
