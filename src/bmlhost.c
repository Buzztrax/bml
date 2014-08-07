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

#include "config.h"

#include <stdio.h>
#include <stdlib.h>

#include "bmllog.h"

int _bmlw_setup(BMLDebugLogger logger);
void _bmlw_finalize(void);

int main( int argc, char **argv ) {
  const char *debug_log_flag_str=getenv("BML_DEBUG");
  const int debug_log_flags=debug_log_flag_str?atoi(debug_log_flag_str):0;
  BMLDebugLogger logger;

  logger = TRACE_INIT(debug_log_flags);
  TRACE("beg\n");

  if (!_bmlw_setup(logger)) {
    return FALSE;
  }
  // TODO(ensonic): more code here

  _bmlw_finalize();
  TRACE("end\n");
  return 0;
}
