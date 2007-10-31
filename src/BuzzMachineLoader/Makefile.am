## $Id: Makefile.am,v 1.1 2007-10-31 18:02:06 ensonic Exp $

lib_LTLIBRARIES = libbuzzmachineloader.la

libbuzzmachineloader_la_SOURCES = \
    BuzzMDKHelper.cpp \            
    BuzzMDKHelper.h \         
    BuzzMachine.h \
    BuzzMachineCallbacks.cpp \
    BuzzMachineCallbacks.h \
    BuzzMachineCallbacksPre12.cpp \
    BuzzMachineCallbacksPre12.h \
    BuzzMachineLoader.cpp \
    BuzzMachineLoader.h \
    BuzzMachineVirtualCallbacks.cpp \
    MachineDataImpl.cpp \
    MachineDataImpl.h \
    MachineInterface.h \
    OscTable.cpp \
    OscTable.h \
    bw.h \
    debug.cpp \
    debug.h \
    dsplib.h \           
    resample.h \
    windows_compat.h                    

libbuzzmachineloader_la_CXXFLAGS = $(BML_CFLAGS)
libbuzzmachineloader_la_LDFLAGS = -lm -lpthread -ldl

             
