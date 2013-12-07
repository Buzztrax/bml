SUBDIRS = BuzzMachineLoader

if USE_DLLWRAPPER
  DLLWRAPPER_LIB=$(top_builddir)/dllwrapper/libloader.la
else
  DLLWRAPPER_LIB=
endif

lib_LTLIBRARIES = libbml.la

AM_CPPFLAGS = \
	-I$(top_srcdir)/dllwrapper \
	-I$(top_srcdir)/src/BuzzMachineLoader \
	-I$(top_srcdir)/src \
	-DNATIVE_BML_DIR="\"$(pkglibdir)\""

AM_CFLAGS = $(PTHREAD_CFLAGS) $(BML_CFLAGS) $(ARCH_CFLAGS)

libbmldir = $(includedir)/libbml
libbml_HEADERS = bml.h ./BuzzMachineLoader/BuzzMachineLoader.h
libbml_la_SOURCES = bml.c
libbml_la_LIBADD = $(LIBM) $(PTHREAD_LIBS) $(BML_LIBS) $(DLLWRAPPER_LIB)
libbml_la_LDFLAGS = \
	-export-symbols-regex ^_?\(bml_\|bmlw_\|bmln_\).* \
	-version-info @BML_VERSION_INFO@

noinst_PROGRAMS=bmltest_info bmltest_process

bmltest_info_SOURCES= bmltest_info.c bmltest_info.h
bmltest_info_LDADD = $(LIBM) $(PTHREAD_LIBS) $(BML_LIBS) libbml.la

bmltest_process_SOURCES= bmltest_process.c  bmltest_process.h
bmltest_process_LDADD = $(LIBM) $(PTHREAD_LIBS) $(BML_LIBS) libbml.la

if USE_DLLWRAPPER
dlldir = $(libdir)/win32
dll_DATA = BuzzMachineLoader.dll
endif

# unconditionally dist the dll
EXTRA_DIST = BuzzMachineLoader.dll

