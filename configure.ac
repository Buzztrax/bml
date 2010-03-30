dnl $Id$

dnl Initialise the autoconf/automake tool
AC_PREREQ(2.61)
AC_INIT([bml],[0.6.0],[buzztard-devel@lists.sourceforge.net])
AM_INIT_AUTOMAKE([-Wall -Werror check-news std-options])
AC_CONFIG_SRCDIR([ChangeLog])
AC_CONFIG_HEADERS(config.h)
AC_CONFIG_MACRO_DIR(m4)
AC_CANONICAL_HOST([])

dnl Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_ISC_POSIX
AC_PROG_LIBTOOL
AM_PROG_AS
AM_PROG_CC_C_O

dnl check target cpu architecture
ARCH_CFLAGS=""
ARCH_CCASFLAGS=""
case "x${host_cpu}" in
  xi?86 | k?)
    HAVE_X86=yes
    ;;
  xx86_64|xamd64)
    HAVE_X86=no
    #ARCH_CFLAGS="-m32"
    #ARCH_CCASFLAGS="-m32"
    ;;
  *)
    HAVE_X86=no
    ;;
esac
if test x$HAVE_X86 = xyes; then
  AC_DEFINE(HAVE_X86, 1, [Defined if host is x86])
fi
AM_CONDITIONAL(USE_X86, test x$HAVE_X86 = xyes)

dnl enable runtime debugging code
AC_MSG_CHECKING(whether to enable runtime debugging code)
AC_ARG_ENABLE(
  debug,
  AS_HELP_STRING([--enable-debug],[enable runtime debugging code (default=no)]),
  ,
  [enable_debug="no"])
AC_MSG_RESULT($enable_debug)
if test "$enable_debug" = "yes"; then
  AC_DEFINE(LOG, [1], [enable runtime debugging code])
  DEBUG_CFLAGS="-O0 -Wall -Werror -g"
else
  DEBUG_CFLAGS=""
fi

dnl enable coverage analysis
AC_MSG_CHECKING(whether to enable coverage analysis)
AC_ARG_ENABLE(
	coverage,
	AS_HELP_STRING([--enable-coverage],[enable coverage analysis (default=no)]),
	,
	[enable_coverage="no"])
AC_MSG_RESULT($enable_coverage)
if test "$enable_coverage" = "yes"; then
  COVERAGE_CFLAGS="-O0 -g -fprofile-arcs -ftest-coverage"
  COVERAGE_LIBS="-lgcov"
  AC_PATH_PROG(locv_path, [lcov], [no])
  if test "x$lcov_path" != "xno" ; then
    LCOV_VERSION=`lcov --version|head -n 1| 's/^.* version //'`
    LCOV_MAJOR=`echo $LCOV_VERSION | cut -d. -f1 | sed s/[a-zA-Z\-].*//g`
    LCOV_MINOR=`echo $LCOV_VERSION | cut -d. -f2 | sed s/[a-zA-Z\-].*//g`
    if [[ "$LCOV_MAJOR" -lt "1" ]]; then
      AC_MSG_WARN([lcov >= 1.6 is required, you have $LCOV_VERSION])
      lcov_path="no"
    elif [[ "$LCOV_MAJOR" -eq "1" ]] && \
         [[ "$LCOV_MINOR" -lt "6" ]]; then
      AC_MSG_WARN([lcov >= 1.6 is required, you have $LCOV_VERSION])
      lcov_path="no"
    fi
  fi
else
  COVERAGE_CFLAGS=""
  COVERAGE_LIBS=""
fi
AM_CONDITIONAL(USE_LCOV, test ! x$lcov_path = xyes)

dnl chek for heaers
AC_HEADER_STDC([])
AC_CHECK_HEADERS( \
  alloca.h assert.h byteswap.h dirent.h execinfo.h kstat.h libgen.h malloc.h \
  netdb.h ucontext.h \
  sys/mman.h sys/mixer.h sys/times.h sys/ioctl.h sys/param.h \
)

AC_CHECK_DECL(sysi86,[
	AC_DEFINE(HAVE_SYSI86,1,[Define this if the 'sysi86' function is declared in sys/sysi86.h])
      ],,[
#include <sys/sysi86.h>
])

dnl check for libraries
AC_CHECK_LIBM
AC_SUBST(LIBM)
ACX_PTHREAD
AC_CHECK_LIB([rt], [main])

AC_CHECK_FUNCS([vsscanf clock_gettime])

AC_CHECK_FUNC(dlopen,
    [AC_DEFINE(HAVE_LIBDL,1,[We can use libdl functions])],
    [AC_CHECK_LIB(dl, dlopen,
         [
           DL_LIBS=-ldl
           AC_DEFINE(HAVE_LIBDL,1,[We can use libdl functions])
         ],
         [])
    ])

# Compilation and Linker flags
BML_CFLAGS="$DEBUG_CFLAGS $COVERAGE_CFLAGS"
# the -Wno-non-virtual-dtor is needed to suppress : virtual methods with no virtual destructor
BML_CXXFLAGS="$DEBUG_CFLAGS $COVERAGE_CFLAGS -Wno-non-virtual-dtor"
BML_LIBS="$COVERAGE_LIBS $DL_LIBS"
AC_SUBST(BML_CFLAGS)
AC_SUBST(BML_CXXFLAGS)
AC_SUBST(BML_LIBS)
AC_SUBST(ARCH_CFLAGS)
AC_SUBST(ARCH_CCASFLAGS)

SHAVE_INIT([.])
AC_CONFIG_FILES(Makefile shave shave-libtool \
  libbml.pc \
  src/Makefile \
  src/BuzzMachineLoader/Makefile \
  dllwrapper/Makefile \
  dllwrapper/wine/Makefile \
  tests/bt-cfg.sh \
)
AC_OUTPUT

echo "
Configuration
	Version                    : ${VERSION}
	Source code location       : ${srcdir}
	Prefix                     : ${prefix}
	Compiler                   : ${CC}

	x86 win32 buzzmachines     : $HAVE_X86

	Debug                      : ${enable_debug}
	Coverage profiling         : ${enable_coverage}
"

if test ! `echo "$PKG_CONFIG_PATH:/usr" | tr \: \\n | grep $prefix` ; then
echo "
You are installing buzztard libraries into a location that is not yet known to
system. In order for other buzztard modules to find them and the application to
work with all features please register the location (see README for details):
export LD_LIBRARY_PATH=$prefix/lib:\$LD_LIBRARY_PATH
export PKG_CONFIG_PATH="\$PKG_CONFIG_PATH:$prefix/lib/pkgconfig"
"
fi

