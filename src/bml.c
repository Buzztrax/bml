/* $Id: bml.c,v 1.2 2005-05-18 17:24:39 ensonic Exp $
 */

#include "config.h"
#include "bml.h"

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

// buzz machine loader handle
#ifdef USE_DLLWRAPPER1
static HINSTANCE h=NULL;
static ldt_fs_t *ldt_fs;
#endif
#ifdef USE_DLLWRAPPER2
static void *h=NULL;
#endif



// API method pointers
BMSetMasterInfo _bm_set_master_info;
BMNew _bm_new;
BMInit _bm_init;
BMFree _bm_free;

BMGetMachineInfo _bm_get_machine_info;
BMGetGlobalParameterInfo _bm_get_global_parameter_info;
BMGetTrackParameterInfo _bm_get_track_parameter_info;
BMGetAttributeInfo _bm_get_attribute_info;

BMGetTrackParameterLocation _bm_get_track_parameter_location;
BMGetTrackParameterValue _bm_get_track_parameter_value;
BMSetTrackParameterValue _bm_set_track_parameter_value;

BMGetGlobalParameterLocation _bm_get_global_parameter_location;
BMGetGlobalParameterValue _bm_get_global_parameter_value;
BMSetGlobalParameterValue _bm_set_global_parameter_value;

BMGetAttributeLocation _bm_get_attribute_location;
BMGetAttributeValue _bm_get_attribute_value;
BMSetAttributeValue _bm_set_attribute_value;

BMTick _bm_tick;
BMWork _bm_work;
BMWorkM2S _bm_work_m2s;
BMStop _bm_stop;

BMSetNumTracks _bm_set_num_tracks;

// setup API wrapper

#ifdef USE_DLLWRAPPER1
#define LoadDLL(name) LoadLibraryA(name)
#define GetSymbol(dll,name) GetProcAddress(dll,name)
#define FreeDLL(dll) FreeLibrary(dll)
#endif
#ifdef USE_DLLWRAPPER2
#define LoadDLL(name) (void *)WineLoadLibrary(name)
#define GetSymbol(dll,name) WineGetProcAddress(dll,name)
#define FreeDLL(dll) WineFreeLibrary(dll)
#endif

void bm_set_master_info(long bpm, long tpb, long srat) {
	Check_FS_Segment();
	_bm_set_master_info(bpm,tpb,srat);
}

BuzzMachine *bm_new(char *bm_file_name) {
	Check_FS_Segment();
	return(_bm_new(bm_file_name));
}

void bm_init(BuzzMachine *bm) {
	Check_FS_Segment();
	_bm_init(bm);
}

void bm_free(BuzzMachine *bm) {
	Check_FS_Segment();
	_bm_free(bm);
}


int bm_get_machine_info(BuzzMachine *bm, BuzzMachineProperty key, void *value) {
	Check_FS_Segment();
	return(_bm_get_machine_info(bm,key,value));
}

int bm_get_global_parameter_info(BuzzMachine *bm,int index,BuzzMachineParameter key,void *value) {
	Check_FS_Segment();
	return(_bm_get_global_parameter_info(bm,index,key,value));
}

int bm_get_track_parameter_info(BuzzMachine *bm,int index,BuzzMachineParameter key,void *value) {
	Check_FS_Segment();
	return(_bm_get_track_parameter_info(bm,index,key,value));
}

int bm_get_attribute_info(BuzzMachine *bm,int index,BuzzMachineAttribute key,void *value) {
	Check_FS_Segment();
	return(_bm_get_attribute_info(bm,index,key,value));
}


void *bm_get_track_parameter_location(BuzzMachine *bm,int track,int index) {
	Check_FS_Segment();
	return(_bm_get_track_parameter_location(bm,track,index));
}

int bm_get_track_parameter_value(BuzzMachine *bm,int track,int index) {
	Check_FS_Segment();
	return(_bm_get_track_parameter_value(bm,track,index));
}

void bm_set_track_parameter_value(BuzzMachine *bm,int track,int index,int value) {
	Check_FS_Segment();
	return(_bm_set_track_parameter_value(bm,track,index,value));
}


void *bm_get_global_parameter_location(BuzzMachine *bm,int index) {
	Check_FS_Segment();
	return(_bm_get_global_parameter_location(bm,index));
}

int bm_get_global_parameter_value(BuzzMachine *bm,int index) {
	Check_FS_Segment();
	return(_bm_get_global_parameter_value(bm,index));
}

void bm_set_global_parameter_value(BuzzMachine *bm,int index,int value) {
	Check_FS_Segment();
	return(_bm_set_global_parameter_value(bm,index,value));
}


void *bm_get_attribute_location(BuzzMachine *bm,int index) {
	Check_FS_Segment();
	return(_bm_get_attribute_location(bm,index));
}

int bm_get_attribute_value(BuzzMachine *bm,int index) {
	Check_FS_Segment();
	return(_bm_get_attribute_value(bm,index));
}

void bm_set_attribute_value(BuzzMachine *bm,int index,int value) {
	Check_FS_Segment();
	return(_bm_set_attribute_value(bm,index,value));
}


void bm_tick(BuzzMachine *bm) {
	Check_FS_Segment();
	_bm_tick(bm);
}

int bm_work(BuzzMachine *bm,float *psamples, int numsamples, int const mode) {
	Check_FS_Segment();
	return(_bm_work(bm,psamples,numsamples,mode));
}

int bm_work_m2s(BuzzMachine *bm,float *pin, float *pout, int numsamples, int const mode) {
	Check_FS_Segment();
	return(_bm_work_m2s(bm,pin,pout,numsamples,mode));
}

void bm_stop(BuzzMachine *bm) {
	Check_FS_Segment();
	_bm_stop(bm);
}


void bm_set_num_tracks(BuzzMachine *bm, int num) {
	Check_FS_Segment();
	_bm_set_num_tracks(bm,num);
}


int bml_init(void (*sighandler)(int,siginfo_t*,void*)) {
  printf("%s: bml_init\n",__FUNCTION__);
  
#ifdef USE_DLLWRAPPER1
 ldt_fs=Setup_LDT_Keeper();
 printf("%s:   wrapper initialized: 0x%p\n",__FUNCTION__,ldt_fs);
#endif
#ifdef USE_DLLWRAPPER2
  SharedWineInit(sighandler);
#endif

  if(!(h=LoadDLL("BuzzMachineLoader.dll"))) {
	printf("%s:   failed to load bml\n",__FUNCTION__);
	return(FALSE);
  }
  printf("%s:   bml loaded: 0x%p\n",__FUNCTION__,h);

  if(!(_bm_set_master_info=(BMSetMasterInfo)GetSymbol(h,"bm_set_master_info"))) { puts("bm_set_master_info is missing");return(FALSE);}
  if(!(_bm_new=(BMNew)GetSymbol(h,"bm_new"))) { puts("bm_new is missing");return(FALSE);}
  if(!(_bm_init=(BMInit)GetSymbol(h,"bm_init"))) { puts("bm_init is missing");return(FALSE);}
  if(!(_bm_free=(BMFree)GetSymbol(h,"bm_free"))) { puts("bm_free is missing");return(FALSE);}

  if(!(_bm_get_machine_info=(BMGetMachineInfo)GetSymbol(h,"bm_get_machine_info"))) { puts("bm_get_machine_info is missing");return(FALSE);}
  if(!(_bm_get_global_parameter_info=(BMGetGlobalParameterInfo)GetSymbol(h,"bm_get_global_parameter_info"))) { puts("bm_get_global_parameter_info is missing");return(FALSE);}
  if(!(_bm_get_track_parameter_info=(BMGetTrackParameterInfo)GetSymbol(h,"bm_get_track_parameter_info"))) { puts("bm_get_track_parameter_info is missing");return(FALSE);}
  if(!(_bm_get_attribute_info=(BMGetAttributeInfo)GetSymbol(h,"bm_get_attribute_info"))) { puts("bm_get_attribute_info is missing");return(FALSE);}

  if(!(_bm_get_track_parameter_location=(BMGetTrackParameterLocation)GetSymbol(h,"bm_get_track_parameter_location"))) { puts("bm_get_track_parameter_location is missing");return(FALSE);}
  if(!(_bm_get_track_parameter_value=(BMGetTrackParameterValue)GetSymbol(h,"bm_get_track_parameter_value"))) { puts("bm_get_track_parameter_value is missing");return(FALSE);}
  if(!(_bm_set_track_parameter_value=(BMSetTrackParameterValue)GetSymbol(h,"bm_set_track_parameter_value"))) { puts("bm_set_track_parameter_value is missing");return(FALSE);}

  if(!(_bm_get_global_parameter_location=(BMGetGlobalParameterLocation)GetSymbol(h,"bm_get_global_parameter_location"))) { puts("bm_get_global_parameter_location is missing");return(FALSE);}
  if(!(_bm_get_global_parameter_value=(BMGetGlobalParameterValue)GetSymbol(h,"bm_get_global_parameter_value"))) { puts("bm_get_global_parameter_value is missing");return(FALSE);}
  if(!(_bm_set_global_parameter_value=(BMSetGlobalParameterValue)GetSymbol(h,"bm_set_global_parameter_value"))) { puts("bm_set_global_parameter_value is missing");return(FALSE);}

  if(!(_bm_get_attribute_location=(BMGetAttributeLocation)GetSymbol(h,"bm_get_attribute_location"))) { puts("bm_get_attribute_location is missing");return(FALSE);}
  if(!(_bm_get_attribute_value=(BMGetAttributeValue)GetSymbol(h,"bm_get_attribute_value"))) { puts("bm_get_attribute_value is missing");return(FALSE);}
  if(!(_bm_set_attribute_value=(BMSetAttributeValue)GetSymbol(h,"bm_set_attribute_value"))) { puts("bm_set_attribute_value is missing");return(FALSE);}

  if(!(_bm_tick=(BMTick)GetSymbol(h,"bm_tick"))) { puts("bm_tick is missing");return(FALSE);}
  if(!(_bm_work=(BMWork)GetSymbol(h,"bm_work"))) { puts("bm_work is missing");return(FALSE);}
  if(!(_bm_work_m2s=(BMWorkM2S)GetSymbol(h,"bm_work_m2s"))) { puts("bm_work_m2s is missing");return(FALSE);}
  if(!(_bm_stop=(BMStop)GetSymbol(h,"bm_stop"))) { puts("bm_stop is missing");return(FALSE);}

  if(!(_bm_set_num_tracks=(BMSetNumTracks)GetSymbol(h,"bm_set_num_tracks"))) { puts("bm_set_num_tracks is missing");return(FALSE);}

  // @todo more API entries
  printf("%s:   symbols connected\n",__FUNCTION__);
  
  return(TRUE);
}

void bml_done(void) {
  FreeDLL(h);
#ifdef USE_DLLWRAPPER1
  Restore_LDT_Keeper(ldt_fs);
#endif
  printf("%s:   bml unloaded\n",__FUNCTION__);
}

char *bml_convertpath(char *inpath) {
#ifdef USE_DLLWRAPPER1
    return(inpath);
#endif
#ifdef USE_DLLWRAPPER2
    static char outpath[2048],*str;
    
    GetFullPathName(inpath,2048,outpath,&str);
    // !!! this is a temporary hack,
    // as my outdated version of wine always maps to 'Y:\'
    if(inpath[0]=='/') outpath[0]='Z';
    return(outpath);
#endif
}
