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

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "bmllog.h"
#include "bmlipc.h"
#include "bmlw.h"

static void _bmlw_set_master_info(BmlIpcBuf *buf)
{
  long bpm = bmlipc_read_int(buf);
  long tpb = bmlipc_read_int(buf);
  long srat = bmlipc_read_int(buf);
  TRACE("bmlw_set_master_info(%ld,%ld,%ld)\n",bpm,tpb,srat);
  bmlw_set_master_info(bpm,tpb,srat);
  bmlipc_clear(buf);
}

static void _bmlw_open(BmlIpcBuf *buf)
{
  char *bm_file_name = bmlipc_read_string(buf);
  TRACE("bmlw_open('%s')\n", bm_file_name);
  BuzzMachineHandle *bmh = bmlw_open(bm_file_name);
  TRACE("bmlw_open('%s')=%p\n", bm_file_name, bmh);
  bmlipc_clear(buf);
  bmlipc_write_int(buf, (int)bmh);
}

static void _bmlw_close(BmlIpcBuf *buf)
{
  BuzzMachineHandle *bmh = (BuzzMachineHandle *)bmlipc_read_int(buf);
  bmlw_close(bmh);
  bmlipc_clear(buf);
}

static void _bmlw_get_machine_info(BmlIpcBuf *buf)
{
  BuzzMachineHandle *bmh = (BuzzMachineHandle *)bmlipc_read_int(buf);
  BuzzMachineProperty key = bmlipc_read_int(buf);
  int ival,ret;
  char *sval;
  switch(key) {
    case BM_PROP_TYPE:
    case BM_PROP_VERSION:
    case BM_PROP_FLAGS:
    case BM_PROP_MIN_TRACKS:
    case BM_PROP_MAX_TRACKS:
    case BM_PROP_NUM_GLOBAL_PARAMS:
    case BM_PROP_NUM_TRACK_PARAMS:
    case BM_PROP_NUM_ATTRIBUTES:
    case BM_PROP_NUM_INPUT_CHANNELS:
    case BM_PROP_NUM_OUTPUT_CHANNELS:
      ret=bmlw_get_machine_info(bmh, key, &ival);
      bmlipc_clear(buf);
      bmlipc_write_int(buf, (ret ? 1 : 0));
      bmlipc_write_int(buf, ival);
      break;
    case BM_PROP_NAME:
    case BM_PROP_SHORT_NAME:
    case BM_PROP_AUTHOR:
    case BM_PROP_COMMANDS:
    case BM_PROP_DLL_NAME:
      ret=bmlw_get_machine_info(bmh, key, &sval);
      bmlipc_clear(buf);
      bmlipc_write_int(buf, (ret ? 2 : 0));
      if (sval) {
        bmlipc_write_string(buf, sval);
      }
      break;
  }
}

static void _bmlw_get_global_parameter_info(BmlIpcBuf *buf)
{
}

static void _bmlw_get_track_parameter_info(BmlIpcBuf *buf)
{
}

static void _bmlw_get_attribute_info(BmlIpcBuf *buf)
{
}

static void _bmlw_new(BmlIpcBuf *buf)
{
  BuzzMachineHandle *bmh = (BuzzMachineHandle *)bmlipc_read_int(buf);
  BuzzMachine *bm = bmlw_new(bmh);
  bmlipc_clear(buf);
  bmlipc_write_int(buf, (int)bm);
}

static void _bmlw_free(BmlIpcBuf *buf)
{
  BuzzMachine *bm = (BuzzMachine *)bmlipc_read_int(buf);
  bmlw_free(bm);
  bmlipc_clear(buf);
}

static void _bmlw_init(BmlIpcBuf *buf)
{
}

int main( int argc, char **argv ) {
  char *socket_file=NULL;
  const char *debug_log_flag_str=getenv("BML_DEBUG");
  const int debug_log_flags=debug_log_flag_str?atoi(debug_log_flag_str):0;
  BMLDebugLogger logger;
  int server_socket, client_socket;
  socklen_t addrlen;
  ssize_t size;
  struct sockaddr_un address;
  int running = TRUE;
  BmlIpcBuf *buf;
  BmAPI id;

  logger = TRACE_INIT(debug_log_flags);
  TRACE("beg\n");
  
  if (argc > 1) {
    socket_file=argv[1];
  } else {
    fprintf (stderr, "Usage: bmlhost <socket file>\n");
    return EXIT_FAILURE;
  }
  TRACE("socket file: '%s'\n", socket_file);

  if (!_bmlw_setup(logger)) {
    TRACE("bmlw setup failed\n");
    return EXIT_FAILURE;
  }
  
  if ((server_socket=socket (AF_LOCAL, SOCK_STREAM, 0)) > 0) {
    TRACE("server socket created\n");
  }
  
  unlink(socket_file);
  address.sun_family = AF_LOCAL;
  strcpy(address.sun_path, socket_file);
  if (bind(server_socket, (struct sockaddr *) &address, sizeof(address)) != 0) {
    TRACE("socket path already in use!\n");
  }
  listen(server_socket, /* backlog of pending connections */ 5);
  addrlen = sizeof(struct sockaddr_in);
  client_socket = accept(server_socket, (struct sockaddr *) &address, &addrlen);
  if (client_socket > 0) {
    TRACE("client connected\n");
  }
  buf = bmlipc_new();
  while (running) {
    TRACE("waiting for command\n");
    bmlipc_clear(buf);
    size = recv(client_socket, buf->buffer, IPC_BUF_SIZE, 0);
    if (size < 0) {
      TRACE("recv failed: %s\n", strerror(size));
      continue;
    }
    buf->size = (int)size;
    TRACE("got %d bytes\n", buf->size);
    // parse message
    id = bmlipc_read_int(buf);
    if (buf->io_error) {
      TRACE("message should be atleast 4 bytes");
      continue;
    }
    TRACE("command: %d", id);
    switch (id) {
      case 0:                                running = FALSE;break;
      case BM_SET_MASTER_INFO:               _bmlw_set_master_info(buf);break;
      case BM_OPEN:                          _bmlw_open(buf);break;
      case BM_CLOSE:                         _bmlw_close(buf);break;
      case BM_GET_MACHINE_INFO:              _bmlw_get_machine_info(buf);break;
      case BM_GET_GLOBAL_PARAMETER_INFO:     _bmlw_get_global_parameter_info(buf);break;
      case BM_GET_TRACK_PARAMETER_INFO:      _bmlw_get_track_parameter_info(buf);break;
      case BM_GET_ATTRIBUTE_INFO:            _bmlw_get_attribute_info(buf);break;
      case BM_NEW:                           _bmlw_new(buf);break;
      case BM_FREE:                          _bmlw_free(buf);break;
      case BM_INIT:                          _bmlw_init(buf);break;
      case BM_GET_TRACK_PARAMETER_LOCATION:  break;
      case BM_GET_TRACK_PARAMETER_VALUE:     break;
      case BM_SET_TRACK_PARAMETER_VALUE:     break;
      case BM_GET_GLOBAL_PARAMETER_LOCATION: break;
      case BM_GET_GLOBAL_PARAMETER_VALUE:    break;
      case BM_SET_GLOBAL_PARAMETER_VALUE:    break;
      case BM_GET_ATTRIBUTE_LOCATION:        break;
      case BM_GET_ATTRIBUTE_VALUE:           break;
      case BM_SET_ATTRIBUTE_VALUE:           break;
      case BM_TICK:                          break;
      case BM_WORK:                          break;
      case BM__WORK_M2S:                     break;
      case BM_STOP:                          break;
      case BM_ATTRIBUTES_CHANGED:            break;
      case BM_SET_NUM_TRACKS:                break;
      case BM_DESCRIBE_GLOBAL_VALUE:         break;
      case BM_DESCRIBE_TRACK_VALUE:          break;
      case BM_SET_CALLBACKS:                 break;
      default:
        break;
    }   
    if (buf->size) {
      send(client_socket, buf->buffer, buf->size, 0);
    }
  }
  bmlipc_free(buf);
  close(client_socket);
  close(server_socket);

  _bmlw_finalize();
  TRACE("end\n");
  return EXIT_SUCCESS;
}
