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

#ifndef BMLIPC_H
#define BMLIPC_H

// TODO(ensonic): figure max message block size
// buzzmachines use MAX_BUFFER_LENGTH = 256 * sizeof(float) = 1024 bytes for
// wave data
#define IPC_BUF_SIZE 4096

// TODO(ensonic): maybe use this on the stack with a built-in array, we're using
// multiple threads on the sending side and that won't work with a single buffer
typedef struct {
  char *buffer;
  int pos;
  int size;
  int io_error;
} BmlIpcBuf;

BmlIpcBuf *bmlipc_new (void);
void bmlipc_free (BmlIpcBuf *self);

void bmlipc_clear (BmlIpcBuf * self);

int bmlipc_read_int (BmlIpcBuf * self);
char *bmlipc_read_string (BmlIpcBuf * self);

void bmlipc_write_int (BmlIpcBuf * self, int buffer);
void bmlipc_write_string (BmlIpcBuf * self, char *buffer);

#endif // BMLIPC_H
