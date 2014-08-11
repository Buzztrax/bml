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
#include <sys/stat.h>
#include <dlfcn.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef USE_DLLWRAPPER_IPC
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#define BML_C
#include "bml.h"
#include "bmllog.h"
#ifdef USE_DLLWRAPPER
#include "bmlw.h"
#endif
#ifdef USE_DLLWRAPPER_IPC
#include "strpool.h"
#include "bmlipc.h"
#endif

// wrapped(ipc)
#ifdef USE_DLLWRAPPER_IPC
static int server_socket;
static BmlIpcBuf *buf;
static StrPool *sp;
#endif
// native
static void *emu_so=NULL;

// native plugin API method pointers
BMSetLogger bmln_set_logger;
BMSetMasterInfo bmln_set_master_info;

BMOpen bmln_open;
BMClose bmln_close;

BMGetMachineInfo bmln_get_machine_info;
BMGetGlobalParameterInfo bmln_get_global_parameter_info;
BMGetTrackParameterInfo bmln_get_track_parameter_info;
BMGetAttributeInfo bmln_get_attribute_info;

BMNew bmln_new;
BMFree bmln_free;

BMInit bmln_init;

BMGetTrackParameterLocation bmln_get_track_parameter_location;
BMGetTrackParameterValue bmln_get_track_parameter_value;
BMSetTrackParameterValue bmln_set_track_parameter_value;

BMGetGlobalParameterLocation bmln_get_global_parameter_location;
BMGetGlobalParameterValue bmln_get_global_parameter_value;
BMSetGlobalParameterValue bmln_set_global_parameter_value;

BMGetAttributeLocation bmln_get_attribute_location;
BMGetAttributeValue bmln_get_attribute_value;
BMSetAttributeValue bmln_set_attribute_value;

BMTick bmln_tick;
BMWork bmln_work;
BMWorkM2S bmln_work_m2s;
BMStop bmln_stop;

BMAttributesChanged bmln_attributes_changed;

BMSetNumTracks bmln_set_num_tracks;

BMDescribeGlobalValue bmln_describe_global_value;
BMDescribeTrackValue bmln_describe_track_value;

BMSetCallbacks bmln_set_callbacks;



#ifdef USE_DLLWRAPPER_IPC
// ipc wrapper functions

// global API

void bmlw_set_master_info(long bpm, long tpb, long srat) {
  bmlipc_clear(buf);
  bmlipc_write_int(buf, BM_SET_MASTER_INFO);
  bmlipc_write_int(buf, bpm);
  bmlipc_write_int(buf, tpb);
  bmlipc_write_int(buf, srat);
  send(server_socket, buf->buffer, buf->size, 0);
}

// library api

BuzzMachineHandle *bmlw_open(char *bm_file_name) {
  TRACE("bmlw_open('%s')...\n", bm_file_name);
  bmlipc_clear(buf);
  bmlipc_write_int(buf, BM_OPEN);
  bmlipc_write_string(buf, bm_file_name);
  send(server_socket, buf->buffer, buf->size, 0);
  bmlipc_clear(buf);
  buf->size = (int) recv(server_socket, buf->buffer, IPC_BUF_SIZE, 0);
  BuzzMachineHandle *bmh = (BuzzMachineHandle *)((long)bmlipc_read_int(buf));
  TRACE("bmlw_open('%s')=%p\n", bm_file_name, bmh);
  return bmh;
	//return (BuzzMachineHandle *)((long)bmlipc_read_int(buf));
}

void bmlw_close(BuzzMachineHandle *bmh) {
  bmlipc_clear(buf);
  bmlipc_write_int(buf, BM_CLOSE);
  bmlipc_write_int(buf, (int)((long)bmh));
  send(server_socket, buf->buffer, buf->size, 0);
}


int bmlw_get_machine_info(BuzzMachineHandle *bmh, BuzzMachineProperty key, void *value) {
  int ret;
  int *ival=(int *)value;
  const char **sval=(const char **)value;

  bmlipc_clear(buf);
  bmlipc_write_int(buf, BM_GET_MACHINE_INFO);
  bmlipc_write_int(buf, (int)((long)bmh));
  bmlipc_write_int(buf, key);
  send(server_socket, buf->buffer, buf->size, 0);
  bmlipc_clear(buf);
  buf->size = (int) recv(server_socket, buf->buffer, IPC_BUF_SIZE, 0);
  ret = bmlipc_read_int(buf);
  // function writes result into value which is either an int or string
  switch(ret) {
    case 0: break;
    case 1:
      *ival = bmlipc_read_int(buf);
      break;
    case 2:
      // this is a pointer to the receive buffer, we intern the received string
      *sval = sp_intern(sp, bmlipc_read_string(buf));
      break;
    default:
      TRACE("unhandled value type: %d", ret);
  }
	return (ret ? 1 : 0);
}

int bmlw_get_global_parameter_info(BuzzMachineHandle *bmh,int index,BuzzMachineParameter key,void *value) {
	int ret;
  int *ival=(int *)value;
  const char **sval=(const char **)value;

  bmlipc_clear(buf);
  bmlipc_write_int(buf, BM_GET_GLOBAL_PARAMETER_INFO);
  bmlipc_write_int(buf, (int)((long)bmh));
  bmlipc_write_int(buf, key);
  send(server_socket, buf->buffer, buf->size, 0);
  bmlipc_clear(buf);
  buf->size = (int) recv(server_socket, buf->buffer, IPC_BUF_SIZE, 0);
  ret = bmlipc_read_int(buf);
  // function writes result into value which is either an int or string
  switch(ret) {
    case 0: break;
    case 1:
      *ival = bmlipc_read_int(buf);
      break;
    case 2:
      // this is a pointer to the receive buffer, we intern the received string
      *sval = sp_intern(sp, bmlipc_read_string(buf));
      break;
    default:
      TRACE("unhandled value type: %d", ret);
  }
	return (ret ? 1 : 0);
}

int bmlw_get_track_parameter_info(BuzzMachineHandle *bmh,int index,BuzzMachineParameter key,void *value) {
	int ret;
  int *ival=(int *)value;
  const char **sval=(const char **)value;

  bmlipc_clear(buf);
  bmlipc_write_int(buf, BM_GET_TRACK_PARAMETER_INFO);
  bmlipc_write_int(buf, (int)((long)bmh));
  bmlipc_write_int(buf, index);
  bmlipc_write_int(buf, key);
  send(server_socket, buf->buffer, buf->size, 0);
  bmlipc_clear(buf);
  buf->size = (int) recv(server_socket, buf->buffer, IPC_BUF_SIZE, 0);
  ret = bmlipc_read_int(buf);
  // function writes result into value which is either an int or string
  switch(ret) {
    case 0: break;
    case 1:
      *ival = bmlipc_read_int(buf);
      break;
    case 2:
      // this is a pointer to the receive buffer, we intern the received string
      *sval = sp_intern(sp, bmlipc_read_string(buf));
      break;
    default:
      TRACE("unhandled value type: %d", ret);
  }
	return (ret ? 1 : 0);
}

int bmlw_get_attribute_info(BuzzMachineHandle *bmh,int index,BuzzMachineAttribute key,void *value) {
	int ret;
  int *ival=(int *)value;
  const char **sval=(const char **)value;

  bmlipc_clear(buf);
  bmlipc_write_int(buf, BM_GET_ATTRIBUTE_INFO);
  bmlipc_write_int(buf, (int)((long)bmh));
  bmlipc_write_int(buf, index);
  bmlipc_write_int(buf, key);
  send(server_socket, buf->buffer, buf->size, 0);
  bmlipc_clear(buf);
  buf->size = (int) recv(server_socket, buf->buffer, IPC_BUF_SIZE, 0);
  ret = bmlipc_read_int(buf);
  // function writes result into value which is either an int or string
  switch(ret) {
    case 0: break;
    case 1:
      *ival = bmlipc_read_int(buf);
      break;
    case 2:
      // this is a pointer to the receive buffer, we intern the received string
      *sval = sp_intern(sp, bmlipc_read_string(buf));
      break;
    default:
      TRACE("unhandled value type: %d", ret);
  }
	return (ret ? 1 : 0);
}


const char *bmlw_describe_global_value(BuzzMachineHandle *bmh, int const param,int const value) {
  int ret;

  bmlipc_clear(buf);
  bmlipc_write_int(buf, BM_DESCRIBE_GLOBAL_VALUE);
  bmlipc_write_int(buf, (int)((long)bmh));
  bmlipc_write_int(buf, param);
  bmlipc_write_int(buf, value);
  send(server_socket, buf->buffer, buf->size, 0);
  bmlipc_clear(buf);
  buf->size = (int) recv(server_socket, buf->buffer, IPC_BUF_SIZE, 0);
  ret = bmlipc_read_int(buf);

  return ret ? bmlipc_read_string(buf): NULL;
}

const char *bmlw_describe_track_value(BuzzMachineHandle *bmh, int const param,int const value) {
  int ret;

  bmlipc_clear(buf);
  bmlipc_write_int(buf, BM_DESCRIBE_TRACK_VALUE);
  bmlipc_write_int(buf, (int)((long)bmh));
  bmlipc_write_int(buf, param);
  bmlipc_write_int(buf, value);
  send(server_socket, buf->buffer, buf->size, 0);
  bmlipc_clear(buf);
  buf->size = (int) recv(server_socket, buf->buffer, IPC_BUF_SIZE, 0);
  ret = bmlipc_read_int(buf);

  return ret ? bmlipc_read_string(buf): NULL;
}

// instance api

BuzzMachine *bmlw_new(BuzzMachineHandle *bmh) {
  bmlipc_clear(buf);
  bmlipc_write_int(buf, BM_NEW);
  bmlipc_write_int(buf, (int)((long)bmh));
  send(server_socket, buf->buffer, buf->size, 0);
  bmlipc_clear(buf);
  buf->size = (int) recv(server_socket, buf->buffer, IPC_BUF_SIZE, 0);
	return (BuzzMachine *)((long)bmlipc_read_int(buf));
}

void bmlw_free(BuzzMachine *bm) {
  bmlipc_clear(buf);
  bmlipc_write_int(buf, BM_FREE);
  bmlipc_write_int(buf, (int)((long)bm));
  send(server_socket, buf->buffer, buf->size, 0);
}


void bmlw_init(BuzzMachine *bm, unsigned long blob_size, unsigned char *blob_data) {
  bmlipc_clear(buf);
  bmlipc_write_int(buf, BM_INIT);
  bmlipc_write_int(buf, (int)((long)bm));
  bmlipc_write_int(buf, (int)blob_size);
  bmlipc_write_data(buf, (int)blob_size, (char *)blob_data);
  send(server_socket, buf->buffer, buf->size, 0);
}


void *bmlw_get_track_parameter_location(BuzzMachine *bm,int track,int index) {
  // TODO(ensonic): use a local address and swap the content before tick()
	return NULL;
}

int bmlw_get_track_parameter_value(BuzzMachine *bm,int track,int index) {
  bmlipc_clear(buf);
  bmlipc_write_int(buf, BM_GET_TRACK_PARAMETER_VALUE);
  bmlipc_write_int(buf, (int)((long)bm));
  bmlipc_write_int(buf, track);
  bmlipc_write_int(buf, index);
  send(server_socket, buf->buffer, buf->size, 0);
	bmlipc_clear(buf);
  buf->size = (int) recv(server_socket, buf->buffer, IPC_BUF_SIZE, 0);
	return bmlipc_read_int(buf);
}

void bmlw_set_track_parameter_value(BuzzMachine *bm,int track,int index,int value) {
  bmlipc_clear(buf);
  bmlipc_write_int(buf, BM_GET_TRACK_PARAMETER_VALUE);
  bmlipc_write_int(buf, (int)((long)bm));
  bmlipc_write_int(buf, track);
  bmlipc_write_int(buf, index);
  bmlipc_write_int(buf, value);
  send(server_socket, buf->buffer, buf->size, 0);
}


void *bmlw_get_global_parameter_location(BuzzMachine *bm,int index) {
  // TODO(ensonic): use a local address and swap the content before tick()
	return NULL;
}

int bmlw_get_global_parameter_value(BuzzMachine *bm,int index) {
  bmlipc_clear(buf);
  bmlipc_write_int(buf, BM_GET_GLOBAL_PARAMETER_VALUE);
  bmlipc_write_int(buf, (int)((long)bm));
  bmlipc_write_int(buf, index);
  send(server_socket, buf->buffer, buf->size, 0);
	bmlipc_clear(buf);
  buf->size = (int) recv(server_socket, buf->buffer, IPC_BUF_SIZE, 0);
	return bmlipc_read_int(buf);
}

void bmlw_set_global_parameter_value(BuzzMachine *bm,int index,int value) {
  bmlipc_clear(buf);
  bmlipc_write_int(buf, BM_GET_GLOBAL_PARAMETER_VALUE);
  bmlipc_write_int(buf, (int)((long)bm));
  bmlipc_write_int(buf, index);
  bmlipc_write_int(buf, value);
  send(server_socket, buf->buffer, buf->size, 0);
}


void *bmlw_get_attribute_location(BuzzMachine *bm,int index) {
  // TODO(ensonic): use a local address and swap the content before tick()
	return NULL;
}

int bmlw_get_attribute_value(BuzzMachine *bm,int index) {
  bmlipc_clear(buf);
  bmlipc_write_int(buf, BM_GET_ATTRIBUTE_VALUE);
  bmlipc_write_int(buf, (int)((long)bm));
  bmlipc_write_int(buf, index);
  send(server_socket, buf->buffer, buf->size, 0);
	bmlipc_clear(buf);
  buf->size = (int) recv(server_socket, buf->buffer, IPC_BUF_SIZE, 0);
	return bmlipc_read_int(buf);
}

void bmlw_set_attribute_value(BuzzMachine *bm,int index,int value) {
  bmlipc_clear(buf);
  bmlipc_write_int(buf, BM_SET_ATTRIBUTE_VALUE);
  bmlipc_write_int(buf, (int)((long)bm));
  bmlipc_write_int(buf, index);
  bmlipc_write_int(buf, value);
  send(server_socket, buf->buffer, buf->size, 0);
}


void bmlw_tick(BuzzMachine *bm) {
  bmlipc_clear(buf);
  bmlipc_write_int(buf, BM_TICK);
  bmlipc_write_int(buf, (int)((long)bm));
  send(server_socket, buf->buffer, buf->size, 0);
}

int bmlw_work(BuzzMachine *bm,float *psamples, int numsamples, int const mode) {
  int size = numsamples * sizeof(float);
  int ret;

  bmlipc_clear(buf);
  bmlipc_write_int(buf, BM_WORK);
  bmlipc_write_int(buf, (int)((long)bm));
  bmlipc_write_int(buf, size);
  bmlipc_write_data(buf, size, (char *)psamples);
  bmlipc_write_int(buf, mode);
  send(server_socket, buf->buffer, buf->size, 0);
  bmlipc_clear(buf);
  buf->size = (int) recv(server_socket, buf->buffer, IPC_BUF_SIZE, 0);
  ret = bmlipc_read_int(buf);
  size = bmlipc_read_int(buf);
  memcpy(psamples, bmlipc_read_data(buf, size), size);
	return ret;
}

int bmlw_work_m2s(BuzzMachine *bm,float *pin, float *pout, int numsamples, int const mode) {
  int size = numsamples * sizeof(float);
  int ret;

  bmlipc_clear(buf);
  bmlipc_write_int(buf, BM_WORK_M2S);
  bmlipc_write_int(buf, (int)((long)bm));
  bmlipc_write_int(buf, size);
  bmlipc_write_data(buf, size, (char *)pin);
  bmlipc_write_int(buf, mode);
  send(server_socket, buf->buffer, buf->size, 0);
  bmlipc_clear(buf);
  buf->size = (int) recv(server_socket, buf->buffer, IPC_BUF_SIZE, 0);
  ret = bmlipc_read_int(buf);
  size = bmlipc_read_int(buf);
  memcpy(pout, bmlipc_read_data(buf, size), size);
	return ret;
}

void bmlw_stop(BuzzMachine *bm) {
  bmlipc_clear(buf);
  bmlipc_write_int(buf, BM_STOP);
  bmlipc_write_int(buf, (int)((long)bm));
  send(server_socket, buf->buffer, buf->size, 0);
}

void bmlw_attributes_changed(BuzzMachine *bm) {
  bmlipc_clear(buf);
  bmlipc_write_int(buf, BM_ATTRIBUTES_CHANGED);
  bmlipc_write_int(buf, (int)((long)bm));
  send(server_socket, buf->buffer, buf->size, 0);
}

void bmlw_set_num_tracks(BuzzMachine *bm, int num) {
  bmlipc_clear(buf);
  bmlipc_write_int(buf, BM_SET_NUM_TRACKS);
  bmlipc_write_int(buf, (int)((long)bm));
  bmlipc_write_int(buf, num);
  send(server_socket, buf->buffer, buf->size, 0);
}

void bmlw_set_callbacks(BuzzMachine *bm, CHostCallbacks *callbacks) {
  // TODO(ensonic): maybe a proxy
}

#endif /* USE_DLLWRAPPER_IPC */

int bml_setup(void) {
  const char *debug_log_flag_str=getenv("BML_DEBUG");
  const int debug_log_flags=debug_log_flag_str?atoi(debug_log_flag_str):0;
  BMLDebugLogger logger;

  logger = TRACE_INIT(debug_log_flags);
  TRACE("beg\n");

#ifdef USE_DLLWRAPPER_DIRECT
  if (!_bmlw_setup(logger)) {
    return FALSE;
  }
#endif /* USE_DLLWRAPPER_DIRECT */
#ifdef USE_DLLWRAPPER_IPC
  struct sockaddr_un address;
  char *socket_file = malloc(16 + 20);
  pid_t child_pid;
  int retries = 0;

  if (getenv("BMLIPC_DEBUG")) {
    snprintf(socket_file, 16 + 20, "/tmp/bml.sock");
  } else {
    snprintf(socket_file, 16 + 20, "/tmp/bml.%d.XXXXXX", (int)getpid());
    socket_file = mktemp(socket_file);
    // spawn the server
    child_pid = fork ();
    if (child_pid == 0) {
      char *args[] = { "bmlhost", socket_file, NULL };
      int res = execvp("bmlhost", args);
      TRACE("an error occurred in execvp\n", strerror(res));
      return FALSE;
    } else if (child_pid < 0) {
      TRACE("fork failed: %s\n", strerror(child_pid));
      return FALSE;
    } else {
      sleep(1);
    }
  }

  if ((server_socket=socket(PF_LOCAL, SOCK_STREAM, 0)) > 0) {
    TRACE("server socket created\n");
  }
  address.sun_family = AF_LOCAL;
  strcpy(address.sun_path, socket_file);
  while (retries < 3) {
    int res;
    if ((res = connect(server_socket, (struct sockaddr *) &address, sizeof (address))) == 0) {
      TRACE("server connected\n");
      break;
    } else {
      TRACE("connection failed: %s\n", strerror(res));
      retries++;
      sleep(1);
    }
  }
  buf = bmlipc_new();
  sp = sp_new(25);
#endif /* USE_DLLWRAPPER_IPC */

  if(!(emu_so=dlopen(NATIVE_BML_DIR "/libbuzzmachineloader.so",RTLD_LAZY))) {
    TRACE("   failed to load native bml : %s\n",dlerror());
    return(FALSE);
  }
  TRACE("   native bml loaded\n");

  if(!(bmln_set_logger=(BMSetLogger)dlsym(emu_so,"bm_set_logger"))) { TRACE("bm_set_logger is missing\n");return(FALSE);}

  if(!(bmln_set_master_info=(BMSetMasterInfo)dlsym(emu_so,"bm_set_master_info"))) { TRACE("bm_set_master_info is missing\n");return(FALSE);}


  if(!(bmln_open=(BMOpen)dlsym(emu_so,"bm_open"))) { TRACE("bm_open is missing\n");return(FALSE);}
  if(!(bmln_close=(BMClose)dlsym(emu_so,"bm_close"))) { TRACE("bm_close is missing\n");return(FALSE);}

  if(!(bmln_get_machine_info=(BMGetMachineInfo)dlsym(emu_so,"bm_get_machine_info"))) { TRACE("bm_get_machine_info is missing\n");return(FALSE);}
  if(!(bmln_get_global_parameter_info=(BMGetGlobalParameterInfo)dlsym(emu_so,"bm_get_global_parameter_info"))) { TRACE("bm_get_global_parameter_info is missing\n");return(FALSE);}
  if(!(bmln_get_track_parameter_info=(BMGetTrackParameterInfo)dlsym(emu_so,"bm_get_track_parameter_info"))) { TRACE("bm_get_track_parameter_info is missing\n");return(FALSE);}
  if(!(bmln_get_attribute_info=(BMGetAttributeInfo)dlsym(emu_so,"bm_get_attribute_info"))) { TRACE("bm_get_attribute_info is missing\n");return(FALSE);}

  if(!(bmln_describe_global_value=(BMDescribeGlobalValue)dlsym(emu_so,"bm_describe_global_value"))) { TRACE("bm_describe_global_value is missing\n");return(FALSE);}
  if(!(bmln_describe_track_value=(BMDescribeTrackValue)dlsym(emu_so,"bm_describe_track_value"))) { TRACE("bm_describe_track_value is missing\n");return(FALSE);}


  if(!(bmln_new=(BMNew)dlsym(emu_so,"bm_new"))) { TRACE("bm_new is missing\n");return(FALSE);}
  if(!(bmln_free=(BMFree)dlsym(emu_so,"bm_free"))) { TRACE("bm_free is missing\n");return(FALSE);}

  if(!(bmln_init=(BMInit)dlsym(emu_so,"bm_init"))) { TRACE("bm_init is missing\n");return(FALSE);}

  if(!(bmln_get_track_parameter_location=(BMGetTrackParameterLocation)dlsym(emu_so,"bm_get_track_parameter_location"))) { TRACE("bm_get_track_parameter_location is missing\n");return(FALSE);}
  if(!(bmln_get_track_parameter_value=(BMGetTrackParameterValue)dlsym(emu_so,"bm_get_track_parameter_value"))) { TRACE("bm_get_track_parameter_value is missing\n");return(FALSE);}
  if(!(bmln_set_track_parameter_value=(BMSetTrackParameterValue)dlsym(emu_so,"bm_set_track_parameter_value"))) { TRACE("bm_set_track_parameter_value is missing\n");return(FALSE);}

  if(!(bmln_get_global_parameter_location=(BMGetGlobalParameterLocation)dlsym(emu_so,"bm_get_global_parameter_location"))) { TRACE("bm_get_global_parameter_location is missing\n");return(FALSE);}
  if(!(bmln_get_global_parameter_value=(BMGetGlobalParameterValue)dlsym(emu_so,"bm_get_global_parameter_value"))) { TRACE("bm_get_global_parameter_value is missing\n");return(FALSE);}
  if(!(bmln_set_global_parameter_value=(BMSetGlobalParameterValue)dlsym(emu_so,"bm_set_global_parameter_value"))) { TRACE("bm_set_global_parameter_value is missing\n");return(FALSE);}

  if(!(bmln_get_attribute_location=(BMGetAttributeLocation)dlsym(emu_so,"bm_get_attribute_location"))) { TRACE("bm_get_attribute_location is missing\n");return(FALSE);}
  if(!(bmln_get_attribute_value=(BMGetAttributeValue)dlsym(emu_so,"bm_get_attribute_value"))) { TRACE("bm_get_attribute_value is missing\n");return(FALSE);}
  if(!(bmln_set_attribute_value=(BMSetAttributeValue)dlsym(emu_so,"bm_set_attribute_value"))) { TRACE("bm_set_attribute_value is missing\n");return(FALSE);}

  if(!(bmln_tick=(BMTick)dlsym(emu_so,"bm_tick"))) { TRACE("bm_tick is missing\n");return(FALSE);}
  if(!(bmln_work=(BMWork)dlsym(emu_so,"bm_work"))) { TRACE("bm_work is missing\n");return(FALSE);}
  if(!(bmln_work_m2s=(BMWorkM2S)dlsym(emu_so,"bm_work_m2s"))) { TRACE("bm_work_m2s is missing\n");return(FALSE);}
  if(!(bmln_stop=(BMStop)dlsym(emu_so,"bm_stop"))) { TRACE("bm_stop is missing\n");return(FALSE);}

  if(!(bmln_attributes_changed=(BMAttributesChanged)dlsym(emu_so,"bm_attributes_changed"))) { TRACE("bm_attributes_changed is missing\n");return(FALSE);}

  if(!(bmln_set_num_tracks=(BMSetNumTracks)dlsym(emu_so,"bm_set_num_tracks"))) { TRACE("bm_set_num_tracks is missing\n");return(FALSE);}

  if(!(bmln_set_callbacks=(BMSetCallbacks)dlsym(emu_so,"bm_set_callbacks"))) { TRACE("bm_set_callbacks is missing\n");return(FALSE);}

  TRACE("   symbols connected\n");
  bmln_set_logger(logger);

  return TRUE;
}

void bml_finalize(void) {
#ifdef USE_DLLWRAPPER_DIRECT
  _bmlw_finalize();
#endif /* USE_DLLWRAPPER_DIRECT */
#ifdef USE_DLLWRAPPER_IPC
  TRACE("string pool size: %d\n", sp_get_count(sp));
  sp_delete(sp);
  TRACE("closing socket\n");
  bmlipc_free(buf);
  //shutdown(server_socket,SHUT_RDWR);
  close(server_socket);
#endif /* USE_DLLWRAPPER_IPC */
  dlclose(emu_so);
  TRACE("bml unloaded\n");
}
