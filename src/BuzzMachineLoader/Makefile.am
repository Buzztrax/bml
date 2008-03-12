## $Id: Makefile.am,v 1.2 2007-11-10 19:06:37 ensonic Exp $

lib_LTLIBRARIES = libbuzzmachineloader.la

libbuzzmachineloader_la_SOURCES = \
    BuzzMachine.h \
    BuzzMachineCallbacks.cpp \
    BuzzMachineCallbacks.h \
    BuzzMachineCallbacksPre12.cpp \
    BuzzMachineCallbacksPre12.h \
    BuzzMachineLoader.cpp \
    BuzzMachineLoader.h \
    BuzzMachineVirtualCallbacks.cpp \
	CMachine.h \
    MachineDataImpl.cpp \
    MachineDataImpl.h \
    MachineInterface.h \
    OscTable.cpp \
    OscTable.h \
	mdk.h \
	mdkimp.cpp \
	mdkimp.h \
    bw.h \
    debug.cpp \
    debug.h \
    dsplib.h \
    resample.h \
    windows_compat.h

#    BuzzMDKHelper.cpp
#    BuzzMDKHelper.h

libbuzzmachineloader_la_CXXFLAGS = $(BML_CXXFLAGS)
libbuzzmachineloader_la_LDFLAGS = -lm -lpthread -ldl

             
