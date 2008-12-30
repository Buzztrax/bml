## $Id$

lib_LTLIBRARIES = libbuzzmachineloader.la

libbuzzmachineloader_la_SOURCES = \
    BuzzMachineCallbacks.cpp \
    BuzzMachineCallbacksPre12.cpp \
    BuzzMachineLoader.cpp \
    BuzzMachineVirtualCallbacks.cpp \
    MachineDataImpl.cpp \
    OscTable.cpp \
    mdkimp.cpp \
    debug.cpp
	
noinst_HEADERS = \
    BuzzMachine.h \
    BuzzMachineCallbacks.h \
    BuzzMachineCallbacksPre12.h \
    BuzzMachineLoader.h \
    CMachine.h \
    MachineDataImpl.h \
    MachineInterface.h \
    OscTable.h \
    mdk.h \
    mdkimp.h \
    bw.h \
    debug.h \
    dsplib.h \
    resample.h

libbuzzmachineloader_la_CXXFLAGS = $(PTHREAD_CFLAGS) $(BML_CXXFLAGS)
libbuzzmachineloader_la_LIBADD = $(LIBM) $(PTHREAD_LIBS) $(BML_LIBS) -lstdc++
libbuzzmachineloader_la_LDFLAGS = -module -avoid-version
libbuzzmachineloader_la_LIBTOOLFLAGS = --tag=disable-static

AM_CPPFLAGS = -I$(top_srcdir)/dllwrapper/wine

