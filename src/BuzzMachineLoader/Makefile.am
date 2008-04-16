## $Id$

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
    resample.h

libbuzzmachineloader_la_CXXFLAGS = $(BML_CXXFLAGS)
libbuzzmachineloader_la_LDFLAGS = -lm -lpthread -ldl

INCLUDES = \
	-I$(top_srcdir)/dllwrapper1/wine
    
