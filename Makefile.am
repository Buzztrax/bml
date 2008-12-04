## $Id$

if USE_X86
  DLLWRAPPER=dllwrapper
else
  DLLWRAPPER=
endif

SUBDIRS = ${DLLWRAPPER} src

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libbml.pc

EXTRA_DIST = \
	libbml.pc.in
