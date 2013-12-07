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

#ifdef USE_DLLWRAPPER
#include "win32.h"
#include "windef.h"
#include "ldt_keeper.h"
#endif  /* USE_DLLWRAPPER */

#include <dlfcn.h>

#define BML_C
#include "bml.h"

// buzz machine loader handle and dll handling
#ifdef USE_DLLWRAPPER
static HINSTANCE emu_dll=0L;
static ldt_fs_t *ldt_fs;
#define LoadDLL(name) LoadLibraryA(name)
#define GetSymbol(dll,name) GetProcAddress(dll,name)
#define FreeDLL(dll) FreeLibrary(dll)
#define BMLX(a) fptr_ ## a

// FIXME: without the mutex and the Check_FS_Segment() things are obviously faster
// it seems to work more or less, but some songs crash when disposing them without
// without the Check_FS_Segment(ldt_fs)
#if 0
static pthread_mutex_t ldt_mutex = PTHREAD_MUTEX_INITIALIZER;
#define win32_prolog(_nop_) \
  pthread_mutex_lock(&ldt_mutex); \
  Check_FS_Segment(ldt_fs)

#define win32_eliplog(_nop_) \
  pthread_mutex_unlock(&ldt_mutex)
#else
#define win32_prolog(_nop_) \
  Check_FS_Segment(ldt_fs)

#define win32_eliplog(_nop_) \
  do {} while(0)
#endif

#endif  /* USE_DLLWRAPPER */
static void *emu_so=NULL;

/* we can configure the packge with --enable-debug to get LOG defined
 * if log is defined logging is compiled in otherwise out
 * if it is in we can still control the amount of logging by setting bits in
 * BML_DEBUG:
 *   1 : only logging from the windows adapter (BuzzMachineLoader.dll)
 *   2 : only logging from bml and dllwrapper
 */
#ifdef LOG
#include <sys/time.h>
#include <time.h>

static double _first_ts = 0.0;

static double
_get_timestamp (void)
{
  struct timespec ts;

  clock_gettime(CLOCK_MONOTONIC, &ts);
  return (((double) ts.tv_sec+ (double) ts.tv_nsec * 1.0e-9) - _first_ts);
}

static void
_log_stdout_printf (const char *file, const int line, const char *fmt, ...)
{
  va_list ap;

  if(file)
    printf ("%10.4lf: %s:%d: ", _get_timestamp(), file, line);
  else
    printf ("%10.4lf: ", _get_timestamp());
  va_start(ap, fmt);
  vprintf (fmt, ap);
  va_end(ap);
}

static void
_log_null_printf (const char *file, const int line, const char *fmt, ...)
{
}

void (*_log_printf)(const char *file, const int line, const char *fmt, ...)=_log_null_printf;


#  define TRACE(...) _log_printf(__FILE__,__LINE__,__VA_ARGS__)
#else
#  define TRACE(...)
#endif

typedef void (*BMLDebugLogger)(char *str);
typedef void (*BMSetLogger)(BMLDebugLogger func);

#ifdef USE_DLLWRAPPER
// windows plugin API method pointers (called through local wrappers)
BMSetLogger BMLX(bmlw_set_logger);
BMSetMasterInfo BMLX(bmlw_set_master_info);

BMOpen BMLX(bmlw_open);
BMClose BMLX(bmlw_close);

BMGetMachineInfo BMLX(bmlw_get_machine_info);
BMGetGlobalParameterInfo BMLX(bmlw_get_global_parameter_info);
BMGetTrackParameterInfo BMLX(bmlw_get_track_parameter_info);
BMGetAttributeInfo BMLX(bmlw_get_attribute_info);

BMNew BMLX(bmlw_new);
BMFree BMLX(bmlw_free);

BMInit BMLX(bmlw_init);

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

#endif /* USE_DLLWRAPPER */

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

#ifdef USE_DLLWRAPPER
// passthrough functions

// global API

void bmlw_set_master_info(long bpm, long tpb, long srat) {
	win32_prolog();
	BMLX(bmlw_set_master_info(bpm,tpb,srat));
	win32_eliplog();
}

// library api

BuzzMachineHandle *bmlw_open(char *bm_file_name) {
	BuzzMachineHandle *bmh;

	win32_prolog();
	bmh=BMLX(bmlw_open(bm_file_name));
	win32_eliplog();
	return(bmh);
}

void bmlw_close(BuzzMachineHandle *bmh) {
	win32_prolog();
	BMLX(bmlw_close(bmh));
	win32_eliplog();
}


int bmlw_get_machine_info(BuzzMachineHandle *bmh, BuzzMachineProperty key, void *value) {
	int ret;

	win32_prolog();
	ret=BMLX(bmlw_get_machine_info(bmh,key,value));
	win32_eliplog();
	return(ret);
}

int bmlw_get_global_parameter_info(BuzzMachineHandle *bmh,int index,BuzzMachineParameter key,void *value) {
	int ret;

	win32_prolog();
	ret=BMLX(bmlw_get_global_parameter_info(bmh,index,key,value));
	win32_eliplog();
	return(ret);
}

int bmlw_get_track_parameter_info(BuzzMachineHandle *bmh,int index,BuzzMachineParameter key,void *value) {
	int ret;

	win32_prolog();
	ret=BMLX(bmlw_get_track_parameter_info(bmh,index,key,value));
	win32_eliplog();
	return(ret);
}

int bmlw_get_attribute_info(BuzzMachineHandle *bmh,int index,BuzzMachineAttribute key,void *value) {
	int ret;

	win32_prolog();
	ret=BMLX(bmlw_get_attribute_info(bmh,index,key,value));
	win32_eliplog();
	return(ret);
}


const char *bmlw_describe_global_value(BuzzMachineHandle *bmh, int const param,int const value) {
	const char *ret;

	win32_prolog();
	ret=BMLX(bmlw_describe_global_value(bmh,param,value));
	win32_eliplog();
	return(ret);
}

const char *bmlw_describe_track_value(BuzzMachineHandle *bmh, int const param,int const value) {
	const char *ret;

	win32_prolog();
	ret=BMLX(bmlw_describe_track_value(bmh,param,value));
	win32_eliplog();
	return(ret);
}


// instance api

BuzzMachine *bmlw_new(BuzzMachineHandle *bmh) {
	BuzzMachine *bm;

	win32_prolog();
	bm=BMLX(bmlw_new(bmh));
	win32_eliplog();
	return(bm);
}

void bmlw_free(BuzzMachine *bm) {
	win32_prolog();
	BMLX(bmlw_free(bm));
	win32_eliplog();
}


void bmlw_init(BuzzMachine *bm, unsigned long blob_size, unsigned char *blob_data) {
	win32_prolog();
	BMLX(bmlw_init(bm,blob_size,blob_data));
	win32_eliplog();
}


void *bmlw_get_track_parameter_location(BuzzMachine *bm,int track,int index) {
	void *ret;

	win32_prolog();
	ret=BMLX(bmlw_get_track_parameter_location(bm,track,index));
	win32_eliplog();
	return(ret);
}

int bmlw_get_track_parameter_value(BuzzMachine *bm,int track,int index) {
	int ret;

	win32_prolog();
	ret=BMLX(bmlw_get_track_parameter_value(bm,track,index));
	win32_eliplog();
	return(ret);
}

void bmlw_set_track_parameter_value(BuzzMachine *bm,int track,int index,int value) {
	win32_prolog();
	BMLX(bmlw_set_track_parameter_value(bm,track,index,value));
	win32_eliplog();
}


void *bmlw_get_global_parameter_location(BuzzMachine *bm,int index) {
	void *ret;

	win32_prolog();
	ret=BMLX(bmlw_get_global_parameter_location(bm,index));
	win32_eliplog();
	return(ret);
}

int bmlw_get_global_parameter_value(BuzzMachine *bm,int index) {
	int ret;

	win32_prolog();
	ret=BMLX(bmlw_get_global_parameter_value(bm,index));
	win32_eliplog();
	return(ret);
}

void bmlw_set_global_parameter_value(BuzzMachine *bm,int index,int value) {
	win32_prolog();
	BMLX(bmlw_set_global_parameter_value(bm,index,value));
	win32_eliplog();
}


void *bmlw_get_attribute_location(BuzzMachine *bm,int index) {
	void *ret;

	win32_prolog();
	ret=BMLX(bmlw_get_attribute_location(bm,index));
	win32_eliplog();
	return(ret);
}

int bmlw_get_attribute_value(BuzzMachine *bm,int index) {
	int ret;

	win32_prolog();
	ret=BMLX(bmlw_get_attribute_value(bm,index));
	win32_eliplog();
	return(ret);
}

void bmlw_set_attribute_value(BuzzMachine *bm,int index,int value) {
	win32_prolog();
	BMLX(bmlw_set_attribute_value(bm,index,value));
	win32_eliplog();
}


void bmlw_tick(BuzzMachine *bm) {
	win32_prolog();
	BMLX(bmlw_tick(bm));
	win32_eliplog();
}

int bmlw_work(BuzzMachine *bm,float *psamples, int numsamples, int const mode) {
	int ret;

	win32_prolog();
	ret=BMLX(bmlw_work(bm,psamples,numsamples,mode));
	win32_eliplog();
	return(ret);
}

int bmlw_work_m2s(BuzzMachine *bm,float *pin, float *pout, int numsamples, int const mode) {
	int ret;

	win32_prolog();
	ret=BMLX(bmlw_work_m2s(bm,pin,pout,numsamples,mode));
	win32_eliplog();
	return(ret);
}

void bmlw_stop(BuzzMachine *bm) {
	win32_prolog();
	BMLX(bmlw_stop(bm));
	win32_eliplog();
}


void bmlw_attributes_changed(BuzzMachine *bm) {
	win32_prolog();
	BMLX(bmlw_attributes_changed(bm));
	win32_eliplog();
}


void bmlw_set_num_tracks(BuzzMachine *bm, int num) {
	win32_prolog();
	BMLX(bmlw_set_num_tracks(bm,num));
	win32_eliplog();
}


void bmlw_set_callbacks(BuzzMachine *bm, CHostCallbacks *callbacks) {
    win32_prolog();
    // @todo: remove after rebuild
    if(BMLX(bmlw_set_callbacks)!=NULL)
      BMLX(bmlw_set_callbacks(bm,callbacks));
    win32_eliplog();
}

#endif /* USE_DLLWRAPPER */

// wrapper management

static void bml_stdout_logger(char *str) {
#ifdef LOG
  static char lbuf[1000];
  static int p=0;
  int i=0;

  if(!str)
    return;

  while((str[i]!='\0') && (str[i]!='\n')) {
    if(p<1000) lbuf[p++]=str[i++];
  }
  if(str[i]=='\n') {
    lbuf[p]='\0';
    _log_stdout_printf(NULL,0,"%s\n",lbuf);
    p=0;
  }
#endif
}

static void bml_null_logger(char *str) {
}

int bml_setup(void) {
  const char *debug_log_flag_str=getenv("BML_DEBUG");
  const int debug_log_flags=debug_log_flag_str?atoi(debug_log_flag_str):0;

#ifdef LOG
  _first_ts = _get_timestamp();
  if (debug_log_flags&0x2) {
    _log_printf=_log_stdout_printf;
  }
#endif

  TRACE("%s\n",__FUNCTION__);

#ifdef USE_DLLWRAPPER
  ldt_fs=Setup_LDT_Keeper();
  TRACE("%s:   wrapper initialized: 0x%p\n",__FUNCTION__,ldt_fs);
  //Check_FS_Segment(ldt_fs);

  if(!(emu_dll=LoadDLL("BuzzMachineLoader.dll"))) {
	TRACE("%s:   failed to load window bml\n",__FUNCTION__);
	return(FALSE);
  }
  TRACE("%s:   windows bml loaded\n",__FUNCTION__);

  if(!(BMLX(bmlw_set_logger)=(BMSetLogger)GetSymbol(emu_dll,"bm_set_logger"))) { puts("bm_set_logger is missing");return(FALSE);}

  if(!(BMLX(bmlw_set_master_info)=(BMSetMasterInfo)GetSymbol(emu_dll,"bm_set_master_info"))) { puts("bm_set_master_info is missing");return(FALSE);}


  if(!(BMLX(bmlw_open)=(BMOpen)GetSymbol(emu_dll,"bm_open"))) { puts("bm_open is missing");return(FALSE);}
  if(!(BMLX(bmlw_close)=(BMClose)GetSymbol(emu_dll,"bm_close"))) { puts("bm_close is missing");return(FALSE);}

  if(!(BMLX(bmlw_get_machine_info)=(BMGetMachineInfo)GetSymbol(emu_dll,"bm_get_machine_info"))) { puts("bm_get_machine_info is missing");return(FALSE);}
  if(!(BMLX(bmlw_get_global_parameter_info)=(BMGetGlobalParameterInfo)GetSymbol(emu_dll,"bm_get_global_parameter_info"))) { puts("bm_get_global_parameter_info is missing");return(FALSE);}
  if(!(BMLX(bmlw_get_track_parameter_info)=(BMGetTrackParameterInfo)GetSymbol(emu_dll,"bm_get_track_parameter_info"))) { puts("bm_get_track_parameter_info is missing");return(FALSE);}
  if(!(BMLX(bmlw_get_attribute_info)=(BMGetAttributeInfo)GetSymbol(emu_dll,"bm_get_attribute_info"))) { puts("bm_get_attribute_info is missing");return(FALSE);}

  if(!(BMLX(bmlw_describe_global_value)=(BMDescribeGlobalValue)GetSymbol(emu_dll,"bm_describe_global_value"))) { puts("bm_describe_global_value is missing");return(FALSE);}
  if(!(BMLX(bmlw_describe_track_value)=(BMDescribeTrackValue)GetSymbol(emu_dll,"bm_describe_track_value"))) { puts("bm_describe_track_value is missing");return(FALSE);}


  if(!(BMLX(bmlw_new)=(BMNew)GetSymbol(emu_dll,"bm_new"))) { puts("bm_new is missing");return(FALSE);}
  if(!(BMLX(bmlw_free)=(BMFree)GetSymbol(emu_dll,"bm_free"))) { puts("bm_free is missing");return(FALSE);}

  if(!(BMLX(bmlw_init)=(BMInit)GetSymbol(emu_dll,"bm_init"))) { puts("bm_init is missing");return(FALSE);}

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

  if(!(BMLX(bmlw_set_callbacks)=(BMSetCallbacks)GetSymbol(emu_dll,"bm_set_callbacks"))) { puts("bm_set_callbacks is missing");return(FALSE);}

  TRACE("%s:   symbols connected\n",__FUNCTION__);
  BMLX(bmlw_set_logger((debug_log_flags&0x1)?bml_stdout_logger:bml_null_logger));
#endif /* USE_DLLWRAPPER */

  if(!(emu_so=dlopen(NATIVE_BML_DIR "/libbuzzmachineloader.so",RTLD_LAZY))) {
	TRACE("%s:   failed to load native bml : %s\n",__FUNCTION__,dlerror());
	return(FALSE);
  }
  TRACE("%s:   native bml loaded\n",__FUNCTION__);

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

  TRACE("%s:   symbols connected\n",__FUNCTION__);
  bmln_set_logger((debug_log_flags&0x1)?bml_stdout_logger:bml_null_logger);

  return(TRUE);
}

void bml_finalize(void) {
#ifdef USE_DLLWRAPPER
  FreeDLL(emu_dll);
  Restore_LDT_Keeper(ldt_fs);
#endif /* USE_DLLWRAPPER */
  dlclose(emu_so);
  TRACE("%s:   bml unloaded\n",__FUNCTION__);
}
