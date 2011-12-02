/* $Id$
 *
 * Buzz Machine Loader
 * Copyright (C) 2006 Buzztard team <buzztard-devel@lists.sf.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */
/*
 * process data with a buzz machine
 *
 * invoke it e.g. as
 *   LD_LIBRARY_PATH=".:./BuzzMachineLoader/.libs" ./bmltest_process ../machines/elak_svf.dll input.raw output1.raw
 *   LD_LIBRARY_PATH=".:./BuzzMachineLoader/.libs" ./bmltest_process ../../buzzmachines/Elak/SVF/libelak_svf.so input.raw output2.raw
 *   LD_LIBRARY_PATH=".:./BuzzMachineLoader/.libs" ./bmltest_process ~/buzztard/lib/Gear/Effects/Jeskola\ NiNjA\ dElaY.dll input.raw output1.raw
 *   LD_LIBRARY_PATH=".:./BuzzMachineLoader/.libs" ./bmltest_process ~/buzztard/lib/Gear/Jeskola_Ninja.so input.raw output1.raw
 *
 * aplay -fS16_LE -r44100 input.raw
 * aplay -fS16_LE -r44100 output1.raw
 *
 * plot [0:] [-35000:35000] 'output1.raw' binary format="%short" using 1 with lines
 *
 * create test-data:
 * gst-launch filesrc location=/usr/share/sounds/info.wav ! decodebin ! filesink location=input1.raw
 * dd count=10 if=/dev/zero of=input2.raw
 */

#include "config.h"

#define _ISOC99_SOURCE /* for isinf() and co. */
#define _BSD_SOURCE /* for setlinebuf() */
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>

//#include <fpu_control.h>

#include "bml.h"

// like MachineInterface.h::MAX_BUFFER_LENGTH
#define BUFFER_SIZE 256

#ifdef HAVE_X86
#define bml(a) bmlw_ ## a
#include "bmltest_process.h"
#undef bml
#endif  /* HAVE_X86 */

#define bml(a) bmln_ ## a
#include "bmltest_process.h"
#undef bml

int main( int argc, char **argv ) {
  setlinebuf(stdout);
  setlinebuf(stderr);
  puts("main beg");

  if(bml_setup()) {
    char *lib_name;
    int sl;

#ifdef HAVE_X86
    bmlw_set_master_info(120,4,44100);
#endif
    bmln_set_master_info(120,4,44100);
    puts("  master info initialized");

    if(argc>2) {
      lib_name=argv[1];
      sl=strlen(lib_name);
      if(sl>4) {
        if(!strcmp(&lib_name[sl-4],".dll")) {
#ifdef HAVE_X86
          bmlw_test_process(lib_name,argv[2],argv[3]);
#else
          puts("no dll emulation on non x86 platforms");
#endif  /* HAVE_X86 */
        }
        else {
          bmln_test_process(lib_name,argv[2],argv[3]);
        }
      }
    }
    else puts("    not enough args!");
    bml_finalize();
  }

  puts("main end");
  return 0;
}
