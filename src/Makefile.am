## $Id$

SUBDIRS = . BuzzMachineLoader

lib_LTLIBRARIES = libbml.la

AM_CPPFLAGS = \
	-I$(top_srcdir)/dllwrapper/wine \
	-I$(top_srcdir)/src/BuzzMachineLoader \
	-I$(top_srcdir)/src

libbmldir = $(includedir)/libbml
libbml_la_SOURCES = bml.c
libbml_HEADERS = bml.h ./BuzzMachineLoader/BuzzMachineLoader.h

libbml_la_CFLAGS = $(BML_CFLAGS) $(ARCH_CFLAGS)
libbml_la_LIBADD = $(LIBM) -lpthread $(BML_LIBS) \
  $(top_builddir)/dllwrapper/wine/libloader.la

noinst_PROGRAMS=bmltest_info bmltest_process

bmltest_info_SOURCES= bmltest_info.c
bmltest_info_CFLAGS = $(BML_CFLAGS) $(ARCH_CFLAGS)
bmltest_info_LDADD = $(BML_LIBS) libbml.la

bmltest_process_SOURCES= bmltest_process.c
bmltest_process_CFLAGS = $(BML_CFLAGS) $(ARCH_CFLAGS)
bmltest_process_LDADD = $(BML_LIBS) libbml.la

if USE_X86
# can/should we use $(libdir)/win32 for the dlls
dlldir = $(libdir)/win32
dll_DATA = BuzzMachineLoader.dll
endif

EXTRA_DIST = $(dll_DATA)

