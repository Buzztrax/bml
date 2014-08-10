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

#include <stdlib.h>
#include <string.h>

#include "bmlipc.h"

BmlIpcBuf *bmlipc_new (void)
{
  BmlIpcBuf *self = calloc(1, sizeof(BmlIpcBuf));
  self->buffer = malloc(IPC_BUF_SIZE);  
  return self;
}

void bmlipc_free (BmlIpcBuf *self)
{
  free (self->buffer);
  free (self);
}

void bmlipc_clear (BmlIpcBuf * self)
{
  self->size = self->pos = self->io_error = 0;
}

// reader

static int mem_read (BmlIpcBuf * self, void *ptr, int size, int n_items)
{
  int bytes = size * n_items;

  if (self->pos + bytes <= self->size) {
    memcpy (ptr, &self->buffer[self->pos], bytes);
    self->pos += bytes;
    return n_items;
  }
  return 0;
}

int bmlipc_read_int (BmlIpcBuf * self)
{
  int buffer = 0;

  if (mem_read (self, &buffer, sizeof (buffer), 1) != 1) {
    self->io_error = 1;
  }
  return buffer;
}

char *bmlipc_read_string (BmlIpcBuf * self)
{
  char *buffer = &self->buffer[self->pos];
  char *c = buffer;
  int p = self->pos;

  while (*c && p < self->size) {
    c++;p++;
  }
  if (*c) {
    self->io_error = 1;
    return NULL;
  }
  self->pos = p;
  return buffer;
}

char *bmlipc_read_data (BmlIpcBuf * self, int size)
{
  char *buffer = &self->buffer[self->pos];
  self->pos += size;
  return buffer;
}

// writer

static int mem_write (BmlIpcBuf * self, void *ptr, int size, int n_items)
{
  int bytes = size * n_items;

  if (self->pos + bytes <= IPC_BUF_SIZE) {
    memcpy (&self->buffer[self->pos], ptr, bytes);
    self->pos += bytes;
    self->size += bytes;
    return n_items;
  }
  return 0;
}

void bmlipc_write_int (BmlIpcBuf * self, int buffer)
{
  if (mem_write (self, &buffer, sizeof (buffer), 1) != 1) {
    self->io_error = 1;
  }
}

void bmlipc_write_string (BmlIpcBuf * self, char *buffer) {
  if (mem_write (self, buffer, strlen (buffer) + 1, 1) != 1) {
    self->io_error = 1;
  }
}

void bmlipc_write_data (BmlIpcBuf * self, int size, char *buffer) {
  if (mem_write (self, buffer, size, 1) != 1) {
    self->io_error = 1;
  }
}
