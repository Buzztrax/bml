## $Id: Makefile.am,v 1.4 2005-05-26 16:25:55 ensonic Exp $

lib_LTLIBRARIES = libbml.la

INCLUDES = \
	-I$(top_srcdir)/dllwrapper1/wine \
	-I$(top_srcdir)/buzz \
	-I$(top_srcdir)/src \
	$(BML_CFLAGS)

libbml_la_SOURCES = bml.c

if USE_DLLWRAPPER1
libbml_la_LIBADD =$(BASE_DEPS_LIBS) \
	$(top_builddir)/dllwrapper1/wine/libloader.la
endif

if USE_DLLWRAPPER2
libbml_la_LIBADD = $(BASE_DEPS_LIBS) \
	$(top_builddir)/dllwrapper2/libloader.la
endif

libbml_la_CFLAGS = $(BML_CFLAGS)
libbml_la_LDFLAGS = -lm -lpthread

bin_PROGRAMS=bmltest_info bmltest_process

bmltest_info_SOURCES= bmltest_info.c
bmltest_info_LDADD = $(BASE_DEPS_LIBS) \
	libbml.la

bmltest_process_SOURCES= bmltest_process.c
bmltest_process_LDADD = $(BASE_DEPS_LIBS) \
	libbml.la

EXTRA_DIST = BuzzmachineLoader.dll

install-data-local:
	$(INSTALL) $(top_builddir)/src/BuzzMachineLoader.dll $(libdir)

uninstall-local:
	rm -r $(libdir)/BuzzMachineLoader.dll
