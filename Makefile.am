## $Id$

if USE_X86
  DLLWRAPPER=dllwrapper
else
  DLLWRAPPER=
endif

SUBDIRS = ${DLLWRAPPER} src

ACLOCAL_AMFLAGS = -I m4

CONFIG_CLEAN_FILES = shave·shave-libtool

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libbml.pc

EXTRA_DIST = \
	libbml.pc.in
