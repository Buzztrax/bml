## $Id$

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
