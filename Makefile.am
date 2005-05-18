## $Id: Makefile.am,v 1.1 2005-05-18 12:53:45 ensonic Exp $

if USE_DLLWRAPPER1
DLLWRAPPER=dllwrapper1
endif

if USE_DLLWRAPPER2
DLLWRAPPER=dllwrapper2
endif

SUBDIRS = $(DLLWRAPPER) src

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libbml.pc

EXTRA_DIST = \
	libbml.pc.in
