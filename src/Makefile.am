## $Id$

# https://sourceforge.net/tracker/?func=detail&atid=476037&aid=1981932&group_id=55124
#MAKEFLAGS = -j1

SUBDIRS = . BuzzMachineLoader

lib_LTLIBRARIES = libbml.la

INCLUDES = \
	-I$(top_srcdir)/dllwrapper1/wine \
	-I$(top_srcdir)/src/BuzzMachineLoader \
	-I$(top_srcdir)/src

libbmldir = $(includedir)/libbml
libbml_la_SOURCES = bml.c
libbml_HEADERS = bml.h ./BuzzMachineLoader/BuzzMachineLoader.h

if USE_DLLWRAPPER1
WRAPPER_LIBS = $(top_builddir)/dllwrapper1/wine/libloader.la
endif
if USE_DLLWRAPPER2
WRAPPER_LIBS = $(top_builddir)/dllwrapper2/libloader.la
endif

libbml_la_CFLAGS = $(BML_CFLAGS) $(ARCH_CFLAGS)
libbml_la_LDFLAGS = -lm -lpthread -ldl
libbml_la_LIBADD = $(BML_LIBS) $(WRAPPER_LIBS)

bin_PROGRAMS=bmltest_info bmltest_process

bmltest_info_SOURCES= bmltest_info.c
bmltest_info_LDADD = $(BML_LIBS) -ldl libbml.la
bmltest_info_CFLAGS = $(BML_CFLAGS) $(ARCH_CFLAGS)

bmltest_process_SOURCES= bmltest_process.c
bmltest_process_LDADD = $(BML_LIBS) -ldl libbml.la
bmltest_process_CFLAGS = $(BML_CFLAGS) $(ARCH_CFLAGS)

dlldir = $(libdir)
dll_DATA = BuzzMachineLoader.dll

EXTRA_DIST = $(dll_DATA)

