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

#include "config.h"

#ifdef HAVE_X86
#ifdef USE_DLLWRAPPER1
#include "win32.h"
#include "windef.h"
#include "ldt_keeper.h"
#endif
#ifdef USE_DLLWRAPPER2
#include <libwinelib.h>
#include <windows.h>
#include <winnt.h>
#endif
#endif  /* HAVE_X86 */

#include <dlfcn.h>

#define BML_C
#include "bml.h"

// buzz machine loader handle and dll handling
#ifdef HAVE_X86
#ifdef USE_DLLWRAPPER1
static HINSTANCE emu_dll=0L;
static ldt_fs_t *ldt_fs;
#define LoadDLL(name) LoadLibraryA(name)
#define GetSymbol(dll,name) GetProcAddress(dll,name)
#define FreeDLL(dll) FreeLibrary(dll)
#endif
#ifdef USE_DLLWRAPPER2
static void *emu_dll=NULL;
#define LoadDLL(name) (void *)WineLoadLibrary(name)
#define GetSymbol(dll,name) WineGetProcAddress(dll,name)
#define FreeDLL(dll) WineFreeLibrary(dll)
#endif
#define BMLX(a) fptr_ ## a
pthread_mutex_t ldt_mutex = PTHREAD_MUTEX_INITIALIZER;
#endif  /* HAVE_X86 */
static void *emu_so=NULL;

#ifdef LOG
#  define TRACE printf
#else
#  define TRACE(...)
#endif

typedef void (*BMLDebugLogger)(char *str);
typedef void (*BMSetLogger)(BMLDebugLogger func);

#ifdef HAVE_X86
// windows plugin API method pointers
BMSetLogger BMLX(bmlw_set_logger);
BMSetMasterInfo BMLX(bmlw_set_master_info);
BMNew BMLX(bmlw_new);
BMInit BMLX(bmlw_init);
BMFree BMLX(bmlw_free);

BMGetMachineInfo BMLX(bmlw_get_machine_info);
BMGetGlobalParameterInfo BMLX(bmlw_get_global_parameter_info);
BMGetTrackParameterInfo BMLX(bmlw_get_track_parameter_info);
BMGetAttributeInfo BMLX(bmlw_get_attribute_info);

BMGetTrackParameterLocation BMLX(bmlw_get_track_parameter_location);
BMGetTrackParameterValue BMLX(bmlw_get_track_parameter_value);
BMSetTrackParameterValue BMLX(bmlw_set_track_parameter_value);

BMGetGlobalParameterLocation BMLX(bmlw_get_global_parameter_location);
BMGetGlobalParameterValue BMLX(bmlw_get_global_parameter_value);
BMSetGlobalParameterValue BMLX(bmlw_set_global_parameter_value);

BMGetAttributeLocation BMLX(bmlw_get_attribute_location);
BMGetAttributeValue BMLX(bmlw_get_attribute_value);
BMSetAttributeValue BMLX(bmlw_set_attribute_value);

BMTick BMLX(bmlw_tick);
BMWork BMLX(bmlw_work);
BMWorkM2S BMLX(bmlw_work_m2s);
BMStop BMLX(bmlw_stop);

BMAttributesChanged BMLX(bmlw_attributes_changed);

BMSetNumTracks BMLX(bmlw_set_num_tracks);

BMDescribeGlobalValue BMLX(bmlw_describe_global_value);
BMDescribeTrackValue BMLX(bmlw_describe_track_value);

BMSetCallbacks BMLX(bmlw_set_callbacks);

#endif /* HAVE_X86 */

// native plugin API method pointers
BMSetLogger bmln_set_logger;
BMSetMasterInfo bmln_set_master_info;
BMNew bmln_new;
BMInit bmln_init;
BMFree bmln_free;

BMGetMachineInfo bmln_get_machine_info;
BMGetGlobalParameterInfo bmln_get_global_parameter_info;
BMGetTrackParameterInfo bmln_get_track_parameter_info;
BMGetAttributeInfo bmln_get_attribute_info;

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

#ifdef HAVE_X86
// passthrough functions

void bmlw_set_master_info(long bpm, long tpb, long srat) {
	pthread_mutex_lock(&ldt_mutex);
	Check_FS_Segment(ldt_fs);
	BMLX(bmlw_set_master_info(bpm,tpb,srat));
	pthread_mutex_unlock(&ldt_mutex);
}

BuzzMachine *bmlw_new(char *bm_file_name) {
	BuzzMachine *bm;

	pthread_mutex_lock(&ldt_mutex);
	Check_FS_Segment(ldt_fs);
	bm=BMLX(bmlw_new(bm_file_name));
	pthread_mutex_unlock(&ldt_mutex);
	return(bm);
}

void bmlw_init(BuzzMachine *bm, unsigned long blob_size, unsigned char *blob_data) {
	pthread_mutex_lock(&ldt_mutex);
	Check_FS_Segment(ldt_fs);
	BMLX(bmlw_init(bm,blob_size,blob_data));
	pthread_mutex_unlock(&ldt_mutex);
}

void bmlw_free(BuzzMachine *bm) {
	pthread_mutex_lock(&ldt_mutex);
	Check_FS_Segment(ldt_fs);
	BMLX(bmlw_free(bm));
	pthread_mutex_unlock(&ldt_mutex);
}


int bmlw_get_machine_info(BuzzMachine *bm, BuzzMachineProperty key, void *value) {
	int ret;

	pthread_mutex_lock(&ldt_mutex);
	Check_FS_Segment(ldt_fs);
	ret=BMLX(bmlw_get_machine_info(bm,key,value));
	pthread_mutex_unlock(&ldt_mutex);
	return(ret);
}

int bmlw_get_global_parameter_info(BuzzMachine *bm,int index,BuzzMachineParameter key,void *value) {
	int ret;

	pthread_mutex_lock(&ldt_mutex);
	Check_FS_Segment(ldt_fs);
	ret=BMLX(bmlw_get_global_parameter_info(bm,index,key,value));
	pthread_mutex_unlock(&ldt_mutex);
	return(ret);
}

int bmlw_get_track_parameter_info(BuzzMachine *bm,int index,BuzzMachineParameter key,void *value) {
	int ret;

	pthread_mutex_lock(&ldt_mutex);
	Check_FS_Segment(ldt_fs);
	ret=BMLX(bmlw_get_track_parameter_info(bm,index,key,value));
	pthread_mutex_unlock(&ldt_mutex);
	return(ret);
}

int bmlw_get_attribute_info(BuzzMachine *bm,int index,BuzzMachineAttribute key,void *value) {
	int ret;

	pthread_mutex_lock(&ldt_mutex);
	Check_FS_Segment(ldt_fs);
	ret=BMLX(bmlw_get_attribute_info(bm,index,key,value));
	pthread_mutex_unlock(&ldt_mutex);
	return(ret);
}


void *bmlw_get_track_parameter_location(BuzzMachine *bm,int track,int index) {
	void *ret;

	pthread_mutex_lock(&ldt_mutex);
	Check_FS_Segment(ldt_fs);
	ret=BMLX(bmlw_get_track_parameter_location(bm,track,index));
	pthread_mutex_unlock(&ldt_mutex);
	return(ret);
}

int bmlw_get_track_parameter_value(BuzzMachine *bm,int track,int index) {
	int ret;

	pthread_mutex_lock(&ldt_mutex);
	Check_FS_Segment(ldt_fs);
	ret=BMLX(bmlw_get_track_parameter_value(bm,track,index));
	pthread_mutex_unlock(&ldt_mutex);
	return(ret);
}

void bmlw_set_track_parameter_value(BuzzMachine *bm,int track,int index,int value) {
	pthread_mutex_lock(&ldt_mutex);
	Check_FS_Segment(ldt_fs);
	BMLX(bmlw_set_track_parameter_value(bm,track,index,value));
	pthread_mutex_unlock(&ldt_mutex);
}


void *bmlw_get_global_parameter_location(BuzzMachine *bm,int index) {
	void *ret;

	pthread_mutex_lock(&ldt_mutex);
	Check_FS_Segment(ldt_fs);
	ret=BMLX(bmlw_get_global_parameter_location(bm,index));
	pthread_mutex_unlock(&ldt_mutex);
	return(ret);
}

int bmlw_get_global_parameter_value(BuzzMachine *bm,int index) {
	int ret;

	pthread_mutex_lock(&ldt_mutex);
	Check_FS_Segment(ldt_fs);
	ret=BMLX(bmlw_get_global_parameter_value(bm,index));
	pthread_mutex_unlock(&ldt_mutex);
	return(ret);
}

void bmlw_set_global_parameter_value(BuzzMachine *bm,int index,int value) {
	pthread_mutex_lock(&ldt_mutex);
	Check_FS_Segment(ldt_fs);
	BMLX(bmlw_set_global_parameter_value(bm,index,value));
	pthread_mutex_unlock(&ldt_mutex);
}


void *bmlw_get_attribute_location(BuzzMachine *bm,int index) {
	void *ret;

	pthread_mutex_lock(&ldt_mutex);
	Check_FS_Segment(ldt_fs);
	ret=BMLX(bmlw_get_attribute_location(bm,index));
	pthread_mutex_unlock(&ldt_mutex);
	return(ret);
}

int bmlw_get_attribute_value(BuzzMachine *bm,int index) {
	int ret;

	pthread_mutex_lock(&ldt_mutex);
	Check_FS_Segment(ldt_fs);
	ret=BMLX(bmlw_get_attribute_value(bm,index));
	pthread_mutex_unlock(&ldt_mutex);
	return(ret);
}

void bmlw_set_attribute_value(BuzzMachine *bm,int index,int value) {
	pthread_mutex_lock(&ldt_mutex);
	Check_FS_Segment(ldt_fs);
	BMLX(bmlw_set_attribute_value(bm,index,value));
	pthread_mutex_unlock(&ldt_mutex);
}


void bmlw_tick(BuzzMachine *bm) {
	pthread_mutex_lock(&ldt_mutex);
	Check_FS_Segment(ldt_fs);
	BMLX(bmlw_tick(bm));
	pthread_mutex_unlock(&ldt_mutex);
}

int bmlw_work(BuzzMachine *bm,float *psamples, int numsamples, int const mode) {
	int ret;

	pthread_mutex_lock(&ldt_mutex);
	Check_FS_Segment(ldt_fs);
	ret=BMLX(bmlw_work(bm,psamples,numsamples,mode));
	pthread_mutex_unlock(&ldt_mutex);
	return(ret);
}

int bmlw_work_m2s(BuzzMachine *bm,float *pin, float *pout, int numsamples, int const mode) {
	int ret;
	
	pthread_mutex_lock(&ldt_mutex);
	Check_FS_Segment(ldt_fs);
	ret=BMLX(bmlw_work_m2s(bm,pin,pout,numsamples,mode));
	pthread_mutex_unlock(&ldt_mutex);
	return(ret);
}

void bmlw_stop(BuzzMachine *bm) {
	pthread_mutex_lock(&ldt_mutex);
	Check_FS_Segment(ldt_fs);
	BMLX(bmlw_stop(bm));
	pthread_mutex_unlock(&ldt_mutex);
}


void bmlw_attributes_changed(BuzzMachine *bm) {
	pthread_mutex_lock(&ldt_mutex);
	Check_FS_Segment(ldt_fs);
	BMLX(bmlw_attributes_changed(bm));
	pthread_mutex_unlock(&ldt_mutex);
}


void bmlw_set_num_tracks(BuzzMachine *bm, int num) {
	pthread_mutex_lock(&ldt_mutex);
	Check_FS_Segment(ldt_fs);
	BMLX(bmlw_set_num_tracks(bm,num));
	pthread_mutex_unlock(&ldt_mutex);
}


const char *bmlw_describe_global_value(BuzzMachine *bm, int const param,int const value) {
	const char *ret;
	
	pthread_mutex_lock(&ldt_mutex);
	Check_FS_Segment(ldt_fs);
	ret=BMLX(bmlw_describe_global_value(bm,param,value));
	pthread_mutex_unlock(&ldt_mutex);
	return(ret);
}

const char *bmlw_describe_track_value(BuzzMachine *bm, int const param,int const value) {
	const char *ret;
	
	pthread_mutex_lock(&ldt_mutex);
	Check_FS_Segment(ldt_fs);
	ret=BMLX(bmlw_describe_track_value(bm,param,value));
	pthread_mutex_unlock(&ldt_mutex);
	return(ret);
}


void bmlw_set_callbacks(BuzzMachine *bm, CHostCallbacks *callbacks) {
	pthread_mutex_lock(&ldt_mutex);
	Check_FS_Segment(ldt_fs);
    // @todo: remove after rebuild
    if(BMLX(bmlw_set_callbacks)!=NULL)
      BMLX(bmlw_set_callbacks(bm,callbacks));
	pthread_mutex_unlock(&ldt_mutex);  
}

#endif /* HAVE_X86 */

// wrapper management

void bml_logger(char *str) {
  TRACE(str);
}

int bml_setup(void (*sighandler)(int,siginfo_t*,void*)) {
  TRACE("%s\n",__FUNCTION__);
  
#ifdef HAVE_X86
#ifdef USE_DLLWRAPPER1
  ldt_fs=Setup_LDT_Keeper();
  TRACE("%s:   wrapper initialized: 0x%p\n",__FUNCTION__,ldt_fs);
  //Check_FS_Segment(ldt_fs);
#endif
#ifdef USE_DLLWRAPPER2
  SharedWineInit(sighandler);
#endif

  if(!(emu_dll=LoadDLL("BuzzMachineLoader.dll"))) {
	TRACE("%s:   failed to load window bml\n",__FUNCTION__);
	return(FALSE);
  }
  TRACE("%s:   windows bml loaded\n",__FUNCTION__);

  if(!(BMLX(bmlw_set_logger)=(BMSetLogger)GetSymbol(emu_dll,"bm_set_logger"))) { puts("bm_set_logger is missing");return(FALSE);}

  if(!(BMLX(bmlw_set_master_info)=(BMSetMasterInfo)GetSymbol(emu_dll,"bm_set_master_info"))) { puts("bm_set_master_info is missing");return(FALSE);}
  if(!(BMLX(bmlw_new)=(BMNew)GetSymbol(emu_dll,"bm_new"))) { puts("bm_new is missing");return(FALSE);}
  if(!(BMLX(bmlw_init)=(BMInit)GetSymbol(emu_dll,"bm_init"))) { puts("bm_init is missing");return(FALSE);}
  if(!(BMLX(bmlw_free)=(BMFree)GetSymbol(emu_dll,"bm_free"))) { puts("bm_free is missing");return(FALSE);}

  if(!(BMLX(bmlw_get_machine_info)=(BMGetMachineInfo)GetSymbol(emu_dll,"bm_get_machine_info"))) { puts("bm_get_machine_info is missing");return(FALSE);}
  if(!(BMLX(bmlw_get_global_parameter_info)=(BMGetGlobalParameterInfo)GetSymbol(emu_dll,"bm_get_global_parameter_info"))) { puts("bm_get_global_parameter_info is missing");return(FALSE);}
  if(!(BMLX(bmlw_get_track_parameter_info)=(BMGetTrackParameterInfo)GetSymbol(emu_dll,"bm_get_track_parameter_info"))) { puts("bm_get_track_parameter_info is missing");return(FALSE);}
  if(!(BMLX(bmlw_get_attribute_info)=(BMGetAttributeInfo)GetSymbol(emu_dll,"bm_get_attribute_info"))) { puts("bm_get_attribute_info is missing");return(FALSE);}

  if(!(BMLX(bmlw_get_track_parameter_location)=(BMGetTrackParameterLocation)GetSymbol(emu_dll,"bm_get_track_parameter_location"))) { puts("bm_get_track_parameter_location is missing");return(FALSE);}
  if(!(BMLX(bmlw_get_track_parameter_value)=(BMGetTrackParameterValue)GetSymbol(emu_dll,"bm_get_track_parameter_value"))) { puts("bm_get_track_parameter_value is missing");return(FALSE);}
  if(!(BMLX(bmlw_set_track_parameter_value)=(BMSetTrackParameterValue)GetSymbol(emu_dll,"bm_set_track_parameter_value"))) { puts("bm_set_track_parameter_value is missing");return(FALSE);}

  if(!(BMLX(bmlw_get_global_parameter_location)=(BMGetGlobalParameterLocation)GetSymbol(emu_dll,"bm_get_global_parameter_location"))) { puts("bm_get_global_parameter_location is missing");return(FALSE);}
  if(!(BMLX(bmlw_get_global_parameter_value)=(BMGetGlobalParameterValue)GetSymbol(emu_dll,"bm_get_global_parameter_value"))) { puts("bm_get_global_parameter_value is missing");return(FALSE);}
  if(!(BMLX(bmlw_set_global_parameter_value)=(BMSetGlobalParameterValue)GetSymbol(emu_dll,"bm_set_global_parameter_value"))) { puts("bm_set_global_parameter_value is missing");return(FALSE);}

  if(!(BMLX(bmlw_get_attribute_location)=(BMGetAttributeLocation)GetSymbol(emu_dll,"bm_get_attribute_location"))) { puts("bm_get_attribute_location is missing");return(FALSE);}
  if(!(BMLX(bmlw_get_attribute_value)=(BMGetAttributeValue)GetSymbol(emu_dll,"bm_get_attribute_value"))) { puts("bm_get_attribute_value is missing");return(FALSE);}
  if(!(BMLX(bmlw_set_attribute_value)=(BMSetAttributeValue)GetSymbol(emu_dll,"bm_set_attribute_value"))) { puts("bm_set_attribute_value is missing");return(FALSE);}

  if(!(BMLX(bmlw_tick)=(BMTick)GetSymbol(emu_dll,"bm_tick"))) { puts("bm_tick is missing");return(FALSE);}
  if(!(BMLX(bmlw_work)=(BMWork)GetSymbol(emu_dll,"bm_work"))) { puts("bm_work is missing");return(FALSE);}
  if(!(BMLX(bmlw_work_m2s)=(BMWorkM2S)GetSymbol(emu_dll,"bm_work_m2s"))) { puts("bm_work_m2s is missing");return(FALSE);}
  if(!(BMLX(bmlw_stop)=(BMStop)GetSymbol(emu_dll,"bm_stop"))) { puts("bm_stop is missing");return(FALSE);}

  if(!(BMLX(bmlw_attributes_changed)=(BMAttributesChanged)GetSymbol(emu_dll,"bm_attributes_changed"))) { puts("bm_attributes_changed is missing");return(FALSE);}

  if(!(BMLX(bmlw_set_num_tracks)=(BMSetNumTracks)GetSymbol(emu_dll,"bm_set_num_tracks"))) { puts("bm_set_num_tracks is missing");return(FALSE);}

  if(!(BMLX(bmlw_describe_global_value)=(BMDescribeGlobalValue)GetSymbol(emu_dll,"bm_describe_global_value"))) { puts("bm_describe_global_value is missing");return(FALSE);}
  if(!(BMLX(bmlw_describe_track_value)=(BMDescribeTrackValue)GetSymbol(emu_dll,"bm_describe_track_value"))) { puts("bm_describe_track_value is missing");return(FALSE);}

  // @todo needs rebuild
  if(!(BMLX(bmlw_set_callbacks)=(BMSetCallbacks)GetSymbol(emu_dll,"bm_set_callbacks"))) { puts("bm_set_callbacks is missing");/*return(FALSE);*/}

  TRACE("%s:   symbols connected\n",__FUNCTION__);
  BMLX(bmlw_set_logger(bml_logger));
#endif /* HAVE_X86 */

  if(!(emu_so=dlopen("libbuzzmachineloader.so",RTLD_LAZY))) {
	TRACE("%s:   failed to load native bml : %s\n",__FUNCTION__,dlerror());
	return(FALSE);
  }
  TRACE("%s:   native bml loaded\n",__FUNCTION__);

  if(!(bmln_set_logger=(BMSetLogger)dlsym(emu_so,"bm_set_logger"))) { puts("bm_set_logger is missing");return(FALSE);}

  if(!(bmln_set_master_info=(BMSetMasterInfo)dlsym(emu_so,"bm_set_master_info"))) { puts("bm_set_master_info is missing");return(FALSE);}
  if(!(bmln_new=(BMNew)dlsym(emu_so,"bm_new"))) { puts("bm_new is missing");return(FALSE);}
  if(!(bmln_init=(BMInit)dlsym(emu_so,"bm_init"))) { puts("bm_init is missing");return(FALSE);}
  if(!(bmln_free=(BMFree)dlsym(emu_so,"bm_free"))) { puts("bm_free is missing");return(FALSE);}

  if(!(bmln_get_machine_info=(BMGetMachineInfo)dlsym(emu_so,"bm_get_machine_info"))) { puts("bm_get_machine_info is missing");return(FALSE);}
  if(!(bmln_get_global_parameter_info=(BMGetGlobalParameterInfo)dlsym(emu_so,"bm_get_global_parameter_info"))) { puts("bm_get_global_parameter_info is missing");return(FALSE);}
  if(!(bmln_get_track_parameter_info=(BMGetTrackParameterInfo)dlsym(emu_so,"bm_get_track_parameter_info"))) { puts("bm_get_track_parameter_info is missing");return(FALSE);}
  if(!(bmln_get_attribute_info=(BMGetAttributeInfo)dlsym(emu_so,"bm_get_attribute_info"))) { puts("bm_get_attribute_info is missing");return(FALSE);}

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

  if(!(bmln_describe_global_value=(BMDescribeGlobalValue)dlsym(emu_so,"bm_describe_global_value"))) { puts("bm_describe_global_value is missing");return(FALSE);}
  if(!(bmln_describe_track_value=(BMDescribeTrackValue)dlsym(emu_so,"bm_describe_track_value"))) { puts("bm_describe_track_value is missing");return(FALSE);}

  if(!(bmln_set_callbacks=(BMSetCallbacks)dlsym(emu_so,"bm_set_callbacks"))) { puts("bm_set_callbacks is missing");return(FALSE);}

  TRACE("%s:   symbols connected\n",__FUNCTION__);
  bmln_set_logger(bml_logger);
  
  return(TRUE);
}

void bml_finalize(void) {
#ifdef HAVE_X86
  FreeDLL(emu_dll);
#ifdef USE_DLLWRAPPER1
  Restore_LDT_Keeper(ldt_fs);
#endif
#endif /* HAVE_X86 */
  dlclose(emu_so);
  TRACE("%s:   bml unloaded\n",__FUNCTION__);
}
