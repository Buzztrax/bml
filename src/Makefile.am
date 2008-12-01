## $Id$

SUBDIRS = . BuzzMachineLoader

lib_LTLIBRARIES = libbml.la

AM_CPPFLAGS = \
	-I$(top_srcdir)/dllwrapper1/wine \
	-I$(top_srcdir)/src/BuzzMachineLoader \
	-I$(top_srcdir)/src

libbmldir = $(includedir)/libbml
libbml_la_SOURCES = bml.c
libbml_HEADERS = bml.h ./BuzzMachineLoader/BuzzMachineLoader.h

libbml_la_CFLAGS = $(BML_CFLAGS) $(ARCH_CFLAGS)
libbml_la_LIBADD = -lm -lpthread -ldl $(BML_LIBS) \
  $(top_builddir)/dllwrapper1/wine/libloader.la

noinst_PROGRAMS=bmltest_info bmltest_process

bmltest_info_SOURCES= bmltest_info.c
bmltest_info_CFLAGS = $(BML_CFLAGS) $(ARCH_CFLAGS)
bmltest_info_LDADD = $(BML_LIBS) -ldl libbml.la

bmltest_process_SOURCES= bmltest_process.c
bmltest_process_CFLAGS = $(BML_CFLAGS) $(ARCH_CFLAGS)
bmltest_process_LDADD = $(BML_LIBS) -ldl libbml.la

dlldir = $(libdir)
dll_DATA = BuzzMachineLoader.dll

EXTRA_DIST = $(dll_DATA)

