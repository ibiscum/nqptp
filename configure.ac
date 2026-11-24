#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([nqptp], [1.2.9], [4265913+mikebrady@users.noreply.github.com])
: ${CFLAGS="-O3"}
: ${CXXFLAGS="-O3"}
AM_INIT_AUTOMAKE([foreign subdir-objects])
AC_CONFIG_MACRO_DIRS([m4])
AC_CANONICAL_HOST

build_linux=no
build_freebsd=no
build_openbsd=no
build_darwin=no

# Detect the target system
case "${host_os}" in
    linux*)
        build_linux=yes
        ;;
    freebsd*)
        build_freebsd=yes
        ;;
    openbsd*)
        build_openbsd=yes
        ;;
    darwin*)
        build_darwin=yes
        ;;
    *)
        AC_MSG_ERROR(["OS $host_os is not supported"])
        ;;
esac

# Pass the conditionals to automake
AM_CONDITIONAL([BUILD_FOR_LINUX], [test "$build_linux" = "yes"])
AM_CONDITIONAL([BUILD_FOR_FREEBSD], [test "$build_freebsd" = "yes"])
AM_CONDITIONAL([BUILD_FOR_OPENBSD], [test "$build_openbsd" = "yes"])
AM_CONDITIONAL([BUILD_FOR_DARWIN], [test "$build_darwin" = "yes"])

if test "x$build_linux" = "xyes" ; then
  AC_DEFINE([CONFIG_FOR_LINUX], 1, [Build for Linux.])
fi
if test "x$build_freebsd" = "xyes" ; then
  AC_DEFINE([CONFIG_FOR_FREEBSD], 1, [Build for FreeBSD.])
fi
if test "x$build_openbsd" = "xyes" ; then
  AC_DEFINE([CONFIG_FOR_OPENBSD], 1, [Build for OpenBSD.])
fi
if test "x$build_darwin" = "xyes" ; then
  AC_DEFINE([CONFIG_FOR_DARWIN], 1, [Build for macOS.])
fi

AC_CHECK_PROGS([GIT], [git])
if test -n "$GIT" && test -e ".git/index" ; then
  AC_DEFINE([CONFIG_USE_GIT_VERSION_STRING], 1, [Use the version string produced by running 'git describe --dirty'.])
fi
AM_CONDITIONAL([USE_GIT_VERSION], [test -n "$GIT" && test -e ".git/index" ])

SYSTEMD_CFLAGS=""
SYSTEMD_LIBS=""
CMOCKA_CFLAGS=""
CMOCKA_LIBS=""


# Check to see if we should include the systemd stuff to define it as a service
AC_ARG_WITH([systemd-startup],[AS_HELP_STRING([--with-systemd-startup],[install a systemd startup script during a make install])])
AM_CONDITIONAL([INSTALL_SYSTEMD_STARTUP], [test "x$with_systemd_startup" = "xyes"])

# Optionally enable native journald logging backend.
AC_ARG_WITH([journald-logging],
  [AS_HELP_STRING([--with-journald-logging@<:@=auto/yes/no@:>@],
    [enable native systemd journald logging backend @<:@default=auto@:>@])],
  [with_journald_logging=$withval],
  [with_journald_logging=auto])

# Check to see if we should include the FreeBSD stuff to define it as a service
AC_ARG_WITH([freebsd-startup],[AS_HELP_STRING([--with-freebsd-startup],[install a FreeBSD startup script during a make install])])
AM_CONDITIONAL([INSTALL_FREEBSD_STARTUP], [test "x$with_freebsd_startup" = "xyes"])

# Determins where to install the systemd unit when requested
AC_ARG_WITH([systemdsystemunitdir],
  [AS_HELP_STRING([--with-systemdsystemunitdir=DIR],
    [Directory for systemd service files @<:@auto@:>@])],
  [with_systemdsystemunitdir=$withval],
  [with_systemdsystemunitdir=auto])

AC_CONFIG_SRCDIR([src/nqptp.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
PKG_PROG_PKG_CONFIG

PKG_CHECK_MODULES([CMOCKA], [cmocka >= 1.1.0], [],
  [AC_MSG_ERROR([cmocka >= 1.1.0 is required for unit/regression tests])])

AS_CASE([$with_journald_logging],
  [yes|no|auto], [],
  [AC_MSG_ERROR([--with-journald-logging must be one of: auto, yes, no])])

AS_IF([test "x$with_journald_logging" != "xno"], [
  AS_IF([test "x$build_linux" != "xyes"], [
    AS_IF([test "x$with_journald_logging" = "xyes"], [
      AC_MSG_ERROR([native journald logging is only supported on Linux])
    ])
    AC_MSG_NOTICE([native journald logging disabled: non-Linux target])
  ], [
    AS_IF([test "x$PKG_CONFIG" != "x" && $PKG_CONFIG --exists libsystemd], [
      SYSTEMD_CFLAGS=`$PKG_CONFIG --cflags libsystemd`
      SYSTEMD_LIBS=`$PKG_CONFIG --libs libsystemd`
      AC_DEFINE([CONFIG_USE_JOURNALD_LOGGING], [1],
        [Enable native journald logging backend.])
      AC_MSG_NOTICE([native journald logging enabled])
    ], [
      AS_IF([test "x$with_journald_logging" = "xyes"], [
        AC_MSG_ERROR([--with-journald-logging=yes requested but libsystemd was not found])
      ])
      AC_MSG_NOTICE([native journald logging disabled: libsystemd not found])
    ])
  ])
])

# Checks for libraries.
AC_CHECK_LIB([pthread],[pthread_create], , AC_MSG_ERROR(pthread library needed))
if test "x$build_openbsd" = "xno" && test "x$build_darwin" = "xno" ; then
  # part of libc
  AC_CHECK_LIB([rt],[clock_gettime], , AC_MSG_ERROR(librt needed for shared memory library))
fi

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h inttypes.h netdb.h stdlib.h string.h sys/socket.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T
AC_TYPE_UINT32_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([clock_gettime inet_ntoa memset select socket strerror])

# Configure the systemd unit location
AS_IF([test "x$with_systemdsystemunitdir" = "xauto"],
  [AS_IF([test "x$PKG_CONFIG" != "x"],
    [with_systemdsystemunitdir=$($PKG_CONFIG --variable=systemdsystemunitdir systemd 2>/dev/null)],
    [with_systemdsystemunitdir=])
  AS_IF([test "x$with_systemdsystemunitdir" = "x"],
    [with_systemdsystemunitdir='${prefix}/lib/systemd/system'])])

AC_SUBST([systemdsystemunitdir], [$with_systemdsystemunitdir])
AC_SUBST([SYSTEMD_CFLAGS])
AC_SUBST([SYSTEMD_LIBS])
AC_SUBST([CMOCKA_CFLAGS])
AC_SUBST([CMOCKA_LIBS])

AS_IF([test "x$with_systemd_startup" = "xyes"],
  [AC_MSG_NOTICE([systemd unit directory: $with_systemdsystemunitdir])])


AC_CONFIG_FILES([Makefile src/Makefile tests/Makefile nqptp.service])
AC_OUTPUT
