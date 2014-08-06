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

#define BML_C
#include "bml.h"
#include "bmllog.h"

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
}

// library api

BuzzMachineHandle *bmlw_open(char *bm_file_name) {
	BuzzMachineHandle *bmh = NULL;

	return(bmh);
}

void bmlw_close(BuzzMachineHandle *bmh) {
}


int bmlw_get_machine_info(BuzzMachineHandle *bmh, BuzzMachineProperty key, void *value) {
	return(0);
}

int bmlw_get_global_parameter_info(BuzzMachineHandle *bmh,int index,BuzzMachineParameter key,void *value) {
	return(0);
}

int bmlw_get_track_parameter_info(BuzzMachineHandle *bmh,int index,BuzzMachineParameter key,void *value) {
	return(0);
}

int bmlw_get_attribute_info(BuzzMachineHandle *bmh,int index,BuzzMachineAttribute key,void *value) {
	return(0);
}


const char *bmlw_describe_global_value(BuzzMachineHandle *bmh, int const param,int const value) {
	const char *ret = "";

	return(ret);
}

const char *bmlw_describe_track_value(BuzzMachineHandle *bmh, int const param,int const value) {
	const char *ret = "";

	return(ret);
}


// instance api

BuzzMachine *bmlw_new(BuzzMachineHandle *bmh) {
	BuzzMachine *bm = NULL;

	return(bm);
}

void bmlw_free(BuzzMachine *bm) {
}


void bmlw_init(BuzzMachine *bm, unsigned long blob_size, unsigned char *blob_data) {
}


void *bmlw_get_track_parameter_location(BuzzMachine *bm,int track,int index) {
	return(NULL);
}

int bmlw_get_track_parameter_value(BuzzMachine *bm,int track,int index) {
	return(0);
}

void bmlw_set_track_parameter_value(BuzzMachine *bm,int track,int index,int value) {
}


void *bmlw_get_global_parameter_location(BuzzMachine *bm,int index) {
	return(NULL);
}

int bmlw_get_global_parameter_value(BuzzMachine *bm,int index) {
	return(0);
}

void bmlw_set_global_parameter_value(BuzzMachine *bm,int index,int value) {
}


void *bmlw_get_attribute_location(BuzzMachine *bm,int index) {
	return(NULL);
}

int bmlw_get_attribute_value(BuzzMachine *bm,int index) {
	return(0);
}

void bmlw_set_attribute_value(BuzzMachine *bm,int index,int value) {
}


void bmlw_tick(BuzzMachine *bm) {
}

int bmlw_work(BuzzMachine *bm,float *psamples, int numsamples, int const mode) {
	return(0);
}

int bmlw_work_m2s(BuzzMachine *bm,float *pin, float *pout, int numsamples, int const mode) {
	return(0);
}

void bmlw_stop(BuzzMachine *bm) {
}

void bmlw_attributes_changed(BuzzMachine *bm) {
}

void bmlw_set_num_tracks(BuzzMachine *bm, int num) {
}

void bmlw_set_callbacks(BuzzMachine *bm, CHostCallbacks *callbacks) {
}

#endif /* USE_DLLWRAPPER_IPC */


#ifdef USE_DLLWRAPPER_DIRECT
int _bmlw_setup(BMLDebugLogger logger);
#endif

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

  if(!(emu_so=dlopen(NATIVE_BML_DIR "/libbuzzmachineloader.so",RTLD_LAZY))) {
    TRACE("   failed to load native bml : %s\n",dlerror());
    return(FALSE);
  }
  TRACE("   native bml loaded\n");

  if(!(bmln_set_logger=(BMSetLogger)dlsym(emu_so,"bm_set_logger"))) { puts("bm_set_logger is missing");return(FALSE);}

  if(!(bmln_set_master_info=(BMSetMasterInfo)dlsym(emu_so,"bm_set_master_info"))) { puts("bm_set_master_info is missing");return(FALSE);}


  if(!(bmln_open=(BMOpen)dlsym(emu_so,"bm_open"))) { puts("bm_open is missing");return(FALSE);}
  if(!(bmln_close=(BMClose)dlsym(emu_so,"bm_close"))) { puts("bm_close is missing");return(FALSE);}

  if(!(bmln_get_machine_info=(BMGetMachineInfo)dlsym(emu_so,"bm_get_machine_info"))) { puts("bm_get_machine_info is missing");return(FALSE);}
  if(!(bmln_get_global_parameter_info=(BMGetGlobalParameterInfo)dlsym(emu_so,"bm_get_global_parameter_info"))) { puts("bm_get_global_parameter_info is missing");return(FALSE);}
  if(!(bmln_get_track_parameter_info=(BMGetTrackParameterInfo)dlsym(emu_so,"bm_get_track_parameter_info"))) { puts("bm_get_track_parameter_info is missing");return(FALSE);}
  if(!(bmln_get_attribute_info=(BMGetAttributeInfo)dlsym(emu_so,"bm_get_attribute_info"))) { puts("bm_get_attribute_info is missing");return(FALSE);}

  if(!(bmln_describe_global_value=(BMDescribeGlobalValue)dlsym(emu_so,"bm_describe_global_value"))) { puts("bm_describe_global_value is missing");return(FALSE);}
  if(!(bmln_describe_track_value=(BMDescribeTrackValue)dlsym(emu_so,"bm_describe_track_value"))) { puts("bm_describe_track_value is missing");return(FALSE);}


  if(!(bmln_new=(BMNew)dlsym(emu_so,"bm_new"))) { puts("bm_new is missing");return(FALSE);}
  if(!(bmln_free=(BMFree)dlsym(emu_so,"bm_free"))) { puts("bm_free is missing");return(FALSE);}

  if(!(bmln_init=(BMInit)dlsym(emu_so,"bm_init"))) { puts("bm_init is missing");return(FALSE);}

  if(!(bmln_get_track_parameter_location=(BMGetTrackParameterLocation)dlsym(emu_so,"bm_get_track_parameter_location"))) { puts("bm_get_track_parameter_location is missing");return(FALSE);}
  if(!(bmln_get_track_parameter_value=(BMGetTrackParameterValue)dlsym(emu_so,"bm_get_track_parameter_value"))) { puts("bm_get_track_parameter_value is missing");return(FALSE);}
  if(!(bmln_set_track_parameter_value=(BMSetTrackParameterValue)dlsym(emu_so,"bm_set_track_parameter_value"))) { puts("bm_set_track_parameter_value is missing");return(FALSE);}

  if(!(bmln_get_global_parameter_location=(BMGetGlobalParameterLocation)dlsym(emu_so,"bm_get_global_parameter_location"))) { puts("bm_get_global_parameter_location is missing");return(FALSE);}
  if(!(bmln_get_global_parameter_value=(BMGetGlobalParameterValue)dlsym(emu_so,"bm_get_global_parameter_value"))) { puts("bm_get_global_parameter_value is missing");return(FALSE);}
  if(!(bmln_set_global_parameter_value=(BMSetGlobalParameterValue)dlsym(emu_so,"bm_set_global_parameter_value"))) { puts("bm_set_global_parameter_value is missing");return(FALSE);}

  if(!(bmln_get_attribute_location=(BMGetAttributeLocation)dlsym(emu_so,"bm_get_attribute_location"))) { puts("bm_get_attribute_location is missing");return(FALSE);}
  if(!(bmln_get_attribute_value=(BMGetAttributeValue)dlsym(emu_so,"bm_get_attribute_value"))) { puts("bm_get_attribute_value is missing");return(FALSE);}
  if(!(bmln_set_attribute_value=(BMSetAttributeValue)dlsym(emu_so,"bm_set_attribute_value"))) { puts("bm_set_attribute_value is missing");return(FALSE);}

  if(!(bmln_tick=(BMTick)dlsym(emu_so,"bm_tick"))) { puts("bm_tick is missing");return(FALSE);}
  if(!(bmln_work=(BMWork)dlsym(emu_so,"bm_work"))) { puts("bm_work is missing");return(FALSE);}
  if(!(bmln_work_m2s=(BMWorkM2S)dlsym(emu_so,"bm_work_m2s"))) { puts("bm_work_m2s is missing");return(FALSE);}
  if(!(bmln_stop=(BMStop)dlsym(emu_so,"bm_stop"))) { puts("bm_stop is missing");return(FALSE);}

  if(!(bmln_attributes_changed=(BMAttributesChanged)dlsym(emu_so,"bm_attributes_changed"))) { puts("bm_attributes_changed is missing");return(FALSE);}

  if(!(bmln_set_num_tracks=(BMSetNumTracks)dlsym(emu_so,"bm_set_num_tracks"))) { puts("bm_set_num_tracks is missing");return(FALSE);}

  if(!(bmln_set_callbacks=(BMSetCallbacks)dlsym(emu_so,"bm_set_callbacks"))) { puts("bm_set_callbacks is missing");return(FALSE);}

  TRACE("   symbols connected\n");
  bmln_set_logger(logger);

  return TRUE;
}

void bml_finalize(void) {
#ifdef USE_DLLWRAPPER_DIRECT
  FreeDLL(emu_dll);
  Restore_LDT_Keeper(ldt_fs);
#endif /* USE_DLLWRAPPER_DIRECT */
  dlclose(emu_so);
  TRACE("   bml unloaded\n");
}
