CXX=i386-mingw32-g++

#DLLWRAP_FLAGS = --nodelete --export-all-symbols --add-stdcall-alias --kill-at --mno-cygwin \
#	--as=i386-mingw32-as -driver-name $(CXX) --dlltool-name=i386-mingw32-dlltool

DLLWRAP_FLAGS = --nodelete --kill-at --mno-cygwin \
	--as=i386-mingw32-as -driver-name $(CXX) --dlltool-name=i386-mingw32-dlltool

all: dsplib.dll

dsplib.dll:  dsplib.o Makefile dsplib.def
	i386-mingw32-dllwrap $(DLLWRAP_FLAGS) \
	--def dsplib.def \
	--output-lib dsplib.lib \
	-o dsplib.dll \
	*.o -lmingwex -lm

dsplib.o: dsplib.cpp
	$(CXX) -D_MSC_VER -c dsplib.cpp -o dsplib.o

clean::
	-rm *.o *.dll
