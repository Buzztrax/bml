# bml

## quick info
Please turn you browser to http://www.buzztrax.org to learn what this project
is about. Buzztrax is free software and distributed under the LGPL.

## build status
We are running continuous integration on drone.io

[![Build Status](https://drone.io/github.com/Buzztrax/bml/status.png)](https://drone.io/github.com/Buzztrax/bml/latest)

## intro
The libbml is a library that loads buzz machines from the windows music composer
Jeskola Buzz, so that Linux/Unix applications can use them.
It can load the original 32bit dll on x86-32bit linux and open source buzz machines
on any platform.

It consists of a c++ to c adapter (BuzzMachineLoader.{dll,so} to map the c++ 
buzzmachine api to c (to circumvent the different c++ abi of objects built with
different compilers). This would not be needed for native open source machines, but
the code is easier this way.

## requirements
* libc6-dev-x32 for support of 32bit windows dlls on x86-64bit systems

## building
Please note that loading the windows dlls only works on x86-32bit systems.
If one builds on a different system, the library can only support native
buzzmachines (see notes below).

## installing localy
Use following options for ./autogen.sh or ./configure

    --prefix=$HOME/buzztrax

when installing the package to e.g. $HOME/buzztrax remember to set these
environment variables:

    #libraries:
    export LD_LIBRARY_PATH=$HOME/buzztrax/lib:$LD_LIBRARY_PATH
    # pkg-config:
    export PKG_CONFIG_PATH="$PKG_CONFIG_PATH:$HOME/buzztrax/lib/pkgconfig"

## installing in /usr/local
The default value for the --prefix configure option is /usr/local. Also in that
case the variables mentioned in the last example need to be exported.

## machines
Export the path we you have the machines, e.g.:

    export BML_PATH=$HOME/buzztrax/lib/Gear:$HOME/buzztrax/lib/Gear/Effects:\
      $HOME/buzztrax/lib/Gear/Generators

One can e.g. have the native ones in Gear and the windows ones under
Gear/Effects and Gear/Generators. libbml will look by default under
$prefix/lib/Gear and below.

## native machines
As we manage to receive or find the sources of buzzmachines, we make them
available in the buzzmachines module in our repository. The machines in there
have been ported, so that they can be build on on many platforms and
architectures.

## windows machines
The emulation uses some parts of wine at runtime. It needs kernel32.dll, e.g.
kernel32.dll.so, MFC42.DLL, MSVCRT.DLL.
The original windows dll should also work.

Then you need to copy the all *.dll libraries from a windows buzz installation
to your LD_LIBARY_PATH. We recommend to copy them to $prefix/lib/win32.

Next copy the machines. You can e.g. just copy the whole Gear folder to
buzztrax-prefix/lib as well.
You might need to fix some machine names (e.g. names with '?' in them).

# usage
For testing, create a folder 'machines' and put some original buzz machines
in there. After building invoke :

    env LD_LIBRARY_PATH="./bml" ./bml/bmltest_info ./machines/mymachine.dll

or alternatively run the testmachine.sh script to probe a whole directory with
machines.

If you have compiled with logging (--enable-debug), you can set flags in 
BML_DEBUG env var toget more diagnostics. BML_DEBUG:

    1 : only logging from the windows adapter (BuzzMachineLoader.dll)
    2 : only logging from bml and dllwrapper

(or just set BML_DEBUG=255 to get as much as there is)
For even more logging edit dllwraper/wine/win32.c and enable

    #define DETAILED_OUT

## building the BuzzMachineLoader.dll
1. use MS developer studio
2. CC=/usr/local/bin/i386-mingw32-gcc CXX=/usr/local/bin/i386-mingw32-g++ make
   ^^^ does not work :/
3. ./configure --host=i386-mingw32 ... 
  cd src/BuzzMachineLoader
  Makefile.am: changes
  make
  make BuzzMachineLoader.dll

