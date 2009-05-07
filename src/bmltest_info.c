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

#include "bml.h"

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

  if(bml_setup(0)) {
    char *lib_name;
    int sl;
    int i;

#ifdef HAVE_X86
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

