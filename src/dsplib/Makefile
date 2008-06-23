MINGW_PREFIX=i386-mingw32
#MINGW_PREFIX=i586-mingw32msvc

CXX=$(MINGW_PREFIX)-g++

DLLWRAP_FLAGS = --nodelete --kill-at --mno-cygwin \
	--as=$(MINGW_PREFIX)-as -driver-name $(CXX) --dlltool-name=$(MINGW_PREFIX)-dlltool

all: dsplib.dll

install: dsplib.dll
	cp dsplib.dll /usr/local/lib/Gear/

dsplib.dll: dsplib.o Makefile dsplib.def
	$(MINGW_PREFIX)-dllwrap $(DLLWRAP_FLAGS) \
	--def dsplib.def \
	--output-lib dsplib.lib \
	-o dsplib.dll \
	*.o -lmingwex -lm

dsplib.o: dsplib.cpp
	$(CXX) -D_MSC_VER -c dsplib.cpp -o dsplib.o

clean::
	-rm *.o *.dll
