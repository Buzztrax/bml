# $Id: configure.ac,v 1.6 2006-08-04 21:49:18 ensonic Exp $

# Initialise the autoconf/automake tool
AC_INIT([bml], [0.0.1], [buzztard-devel@lists.sourceforge.net])
AC_CONFIG_SRCDIR([ChangeLog])
AC_CONFIG_HEADERS(config.h)

# Initialise the automake tool
AM_INIT_AUTOMAKE([check-news])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_ISC_POSIX
AC_PROG_LIBTOOL
AM_PROG_CC_STDC

# enable runtime debugging code
AC_MSG_CHECKING(whether to enable runtime debugging code)
AC_ARG_ENABLE(
  debug,
  AC_HELP_STRING([--enable-debug],[enable runtime debugging code (default=no)]),
  ,
  [enable_debug="no"])
AC_MSG_RESULT($enable_debug)
if test "$enable_debug" = "yes"; then
  AC_DEFINE(LOG, [1], [enable runtime debugging code])
  DEBUG_CFLAGS="-O0 -Wall -Werror -g"
else
  DEBUG_CFLAGS=""
fi

# enable coverage analysis
AC_MSG_CHECKING(whether to enable coverage analysis)
AC_ARG_ENABLE(
	coverage,
	AC_HELP_STRING([--enable-coverage],[enable coverage analysis (default=no)]),
	,
	[enable_coverage="no"])
AC_MSG_RESULT($enable_coverage)
if test "$enable_coverage" = "yes"; then
	#AC_DEFINE(APP_COVERAGE, [1], [enable coverage analysis])
	AM_CONDITIONAL(USE_COVERAGE, true)
	COVERAGE_CFLAGS="-O0 -g -fprofile-arcs -ftest-coverage"
else
	AM_CONDITIONAL(USE_COVERAGE, false)
	COVERAGE_CFLAGS=""
fi

AC_HEADER_STDC([])
AC_CHECK_HEADERS(sys/mman.h)

AC_DEFINE(USE_DLLWRAPPER1, [1], [use the avifile dll wrapper])
AM_CONDITIONAL(USE_DLLWRAPPER1, true)

#AC_DEFINE(USE_DLLWRAPPER2, [0], [use the libfst dll wrapper])
AM_CONDITIONAL(USE_DLLWRAPPER2, false)

BML_CFLAGS="$DEBUG_CFLAGS $COVERAGE_CFLAGS"
AC_SUBST(BML_CFLAGS)

AC_CONFIG_FILES( \
  Makefile \
  libbml.pc \
  src/Makefile \
  dllwrapper1/Makefile \
  dllwrapper1/wine/Makefile \
  dllwrapper2/Makefile \
)
AC_OUTPUT
