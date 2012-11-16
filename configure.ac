dnl Initialise autoconf/automake/libtool
AC_PREREQ(2.61)
AC_INIT([bml],[0.7.0],[buzztard-devel@lists.sourceforge.net])
AC_CANONICAL_TARGET([])
AM_INIT_AUTOMAKE([-Wall -Werror check-news std-options])
AC_CONFIG_SRCDIR([README])
AC_CONFIG_HEADERS(config.h)
AC_CONFIG_MACRO_DIR(m4)
LT_PREREQ([2.2])
LT_INIT

# Support silent build rules, requires at least automake-1.11. Disable by
# either passing --disable-silent-rules to configure or passing V=1 to make
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

dnl
dnl current  : the interface         : MAJOR + MINOR
dnl revision : of the same interface : MICRO
dnl age      : MINOR
dnl
dnl 1: when releasing a changed version : MICRO++
dnl 2: when the interface has changed (adding functions) : MINOR++, MICRO=0
dnl 3: when the interface has changed (removing functions) : MAJOR++, MINOR=0, MICRO=0
dnl
BML_MAJOR_VERSION=0
BML_MINOR_VERSION=7
BML_MICRO_VERSION=0
BML_VERSION=$BML_MAJOR_VERSION.$BML_MINOR_VERSION.$BML_MICRO_VERSION
BML_VERSION_INFO=`expr $BML_MAJOR_VERSION + $BML_MINOR_VERSION`:$BML_MICRO_VERSION:$BML_MINOR_VERSION
BML_VERSION_NUMBER=`expr $BML_MAJOR_VERSION \* 10000 + $BML_MINOR_VERSION \* 100 + $BML_MICRO_VERSION`

AC_SUBST(BML_MAJOR_VERSION)
AC_SUBST(BML_MINOR_VERSION)
AC_SUBST(BML_MICRO_VERSION)
AC_SUBST(BML_VERSION)
AC_SUBST(BML_VERSION_INFO)
AC_SUBST(BML_VERSION_NUMBER)

AC_DEFINE_UNQUOTED(PACKAGE_VERSION_NUMBER, ${BML_VERSION_NUMBER}, [version as a number])

VERSION=${BML_VERSION}


dnl Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AM_PROG_AS
AM_PROG_CC_C_O

m4_ifdef([AM_PROG_AR], [AM_PROG_AR])

dnl common switches for all buzztard modules

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


dnl check target cpu architecture
ARCH_CFLAGS=""
ARCH_CCASFLAGS=""
case "x${target_cpu}" in
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
  AC_DEFINE(HAVE_X86, 1, [Defined if target is x86])
fi
AM_CONDITIONAL(USE_X86, test x$HAVE_X86 = xyes)


dnl check for headers
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


AC_CONFIG_FILES(Makefile \
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

