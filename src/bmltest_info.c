/* Buzz Machine Loader
 * Copyright (C) 2006 Buzztrax team <buzztrax-devel@buzztrax.org>
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
 * License along with this library; if not, see <http://www.gnu.org/licenses/>.
 */
/**
 * display machine info block
 *
 * invoke it e.g. as
 *   LD_LIBRARY_PATH=".:./BuzzMachineLoader/.libs" ./bmltest_info ../machines/elak_svf.dll
 *   LD_LIBRARY_PATH=".:./BuzzMachineLoader/.libs" ./bmltest_info ../../buzzmachines/Elak/SVF/liblibelak_svf.so
 *
 * the dll/so *must* be a buzz-machine, not much error checking ;-)
 */

#include "config.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

#include "bml.h"

static inline double
_get_timestamp (void)
{
#ifdef HAVE_CLOCK_GETTIME
  struct timespec now;

  clock_gettime (CLOCK_MONOTONIC, &now);
  return ((double)now.tv_sec + ((double)now.tv_nsec/1000000000.0));
#else
/*
  GTimeVal now;

  g_get_current_time (&now);
  return GST_TIMEVAL_TO_TIME (now);
*/
  return 0UL;
#endif
}

#ifdef HAVE_X86
#define bml(a) bmlw_ ## a
#include "bmltest_info.h"
#undef bml
#endif  /* HAVE_X86 */

#define bml(a) bmln_ ## a
#include "bmltest_info.h"
#undef bml

int main( int argc, char **argv ) {
  setlinebuf(stdout);
  setlinebuf(stderr);
  puts("main beg");

  if(bml_setup()) {
    char *lib_name;
    int sl;
    int i;

#ifdef HAVE_X86
    /* FIXME: if people have no real win32 dlls, only emulated things, this will crash here */
    bmlw_set_master_info(120,4,44100);
#endif
    bmln_set_master_info(120,4,44100);
    puts("  master info initialized");

    for(i=1;i<argc;i++) {
      lib_name=argv[i];
      sl=strlen(lib_name);
      if(sl>4) {
        if(!strcmp(&lib_name[sl-4],".dll")) {
#ifdef HAVE_X86
          bmlw_test_info(lib_name);
#else
          puts("no dll emulation on non x86 platforms");
#endif  /* HAVE_X86 */
        }
        else {
          bmln_test_info(lib_name);
        }
      }
    }
    bml_finalize();
  }

  puts("main end");
  return 0;
}

