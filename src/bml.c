/* $Id: bml.c,v 1.4 2005-05-18 22:31:00 ensonic Exp $
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

#define BMLX(a) fptr_ ## a
//#define BMLX(a) a

// API method pointers
BMSetMasterInfo BMLX(bml_set_master_info);
BMNew BMLX(bml_new);
BMInit BMLX(bml_init);
BMFree BMLX(bml_free);

BMGetMachineInfo BMLX(bml_get_machine_info);
BMGetGlobalParameterInfo BMLX(bml_get_global_parameter_info);
BMGetTrackParameterInfo BMLX(bml_get_track_parameter_info);
BMGetAttributeInfo BMLX(bml_get_attribute_info);

BMGetTrackParameterLocation BMLX(bml_get_track_parameter_location);
BMGetTrackParameterValue BMLX(bml_get_track_parameter_value);
BMSetTrackParameterValue BMLX(bml_set_track_parameter_value);

BMGetGlobalParameterLocation BMLX(bml_get_global_parameter_location);
BMGetGlobalParameterValue BMLX(bml_get_global_parameter_value);
BMSetGlobalParameterValue BMLX(bml_set_global_parameter_value);

BMGetAttributeLocation BMLX(bml_get_attribute_location);
BMGetAttributeValue BMLX(bml_get_attribute_value);
BMSetAttributeValue BMLX(bml_set_attribute_value);

BMTick BMLX(bml_tick);
BMWork BMLX(bml_work);
BMWorkM2S BMLX(bml_work_m2s);
BMStop BMLX(bml_stop);

BMSetNumTracks BMLX(bml_set_num_tracks);

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

void bml_set_master_info(long bpm, long tpb, long srat) {
	Check_FS_Segment();
	BMLX(bml_set_master_info(bpm,tpb,srat));
}

BuzzMachine *bml_new(char *bm_file_name) {
	Check_FS_Segment();
	return(BMLX(bml_new(bm_file_name)));
}

void bml_init(BuzzMachine *bm) {
	Check_FS_Segment();
	BMLX(bml_init(bm));
}

void bml_free(BuzzMachine *bm) {
	Check_FS_Segment();
	BMLX(bml_free(bm));
}


int bml_get_machine_info(BuzzMachine *bm, BuzzMachineProperty key, void *value) {
	Check_FS_Segment();
	return(BMLX(bml_get_machine_info(bm,key,value)));
}

int bml_get_global_parameter_info(BuzzMachine *bm,int index,BuzzMachineParameter key,void *value) {
	Check_FS_Segment();
	return(BMLX(bml_get_global_parameter_info(bm,index,key,value)));
}

int bml_get_track_parameter_info(BuzzMachine *bm,int index,BuzzMachineParameter key,void *value) {
	Check_FS_Segment();
	return(BMLX(bml_get_track_parameter_info(bm,index,key,value)));
}

int bml_get_attribute_info(BuzzMachine *bm,int index,BuzzMachineAttribute key,void *value) {
	Check_FS_Segment();
	return(BMLX(bml_get_attribute_info(bm,index,key,value)));
}


void *bml_get_track_parameter_location(BuzzMachine *bm,int track,int index) {
	Check_FS_Segment();
	return(BMLX(bml_get_track_parameter_location(bm,track,index)));
}

int bml_get_track_parameter_value(BuzzMachine *bm,int track,int index) {
	Check_FS_Segment();
	return(BMLX(bml_get_track_parameter_value(bm,track,index)));
}

void bml_set_track_parameter_value(BuzzMachine *bm,int track,int index,int value) {
	Check_FS_Segment();
	return(BMLX(bml_set_track_parameter_value(bm,track,index,value)));
}


void *bml_get_global_parameter_location(BuzzMachine *bm,int index) {
	Check_FS_Segment();
	return(BMLX(bml_get_global_parameter_location(bm,index)));
}

int bml_get_global_parameter_value(BuzzMachine *bm,int index) {
	Check_FS_Segment();
	return(BMLX(bml_get_global_parameter_value(bm,index)));
}

void bml_set_global_parameter_value(BuzzMachine *bm,int index,int value) {
	Check_FS_Segment();
	return(BMLX(bml_set_global_parameter_value(bm,index,value)));
}


void *bml_get_attribute_location(BuzzMachine *bm,int index) {
	Check_FS_Segment();
	return(BMLX(bml_get_attribute_location(bm,index)));
}

int bml_get_attribute_value(BuzzMachine *bm,int index) {
	Check_FS_Segment();
	return(BMLX(bml_get_attribute_value(bm,index)));
}

void bml_set_attribute_value(BuzzMachine *bm,int index,int value) {
	Check_FS_Segment();
	return(BMLX(bml_set_attribute_value(bm,index,value)));
}


void bml_tick(BuzzMachine *bm) {
	Check_FS_Segment();
	BMLX(bml_tick(bm));
}

int bml_work(BuzzMachine *bm,float *psamples, int numsamples, int const mode) {
	Check_FS_Segment();
	return(BMLX(bml_work(bm,psamples,numsamples,mode)));
}

int bml_work_m2s(BuzzMachine *bm,float *pin, float *pout, int numsamples, int const mode) {
	Check_FS_Segment();
	return(BMLX(bml_work_m2s(bm,pin,pout,numsamples,mode)));
}

void bml_stop(BuzzMachine *bm) {
	Check_FS_Segment();
	BMLX(bml_stop(bm));
}


void bml_set_num_tracks(BuzzMachine *bm, int num) {
	Check_FS_Segment();
	BMLX(bml_set_num_tracks(bm,num));
}


int bml_setup(void (*sighandler)(int,siginfo_t*,void*)) {
  printf("%s: bml_init\n",__FUNCTION__);
  
#ifdef USE_DLLWRAPPER1
  ldt_fs=Setup_LDT_Keeper();
  printf("%s:   wrapper initialized: 0x%p\n",__FUNCTION__,ldt_fs);
  //Check_FS_Segment();
#endif
#ifdef USE_DLLWRAPPER2
  SharedWineInit(sighandler);
#endif

  if(!(h=LoadDLL("BuzzMachineLoader.dll"))) {
	printf("%s:   failed to load bml\n",__FUNCTION__);
	return(FALSE);
  }
  printf("%s:   bml loaded: 0x%p\n",__FUNCTION__,h);

  if(!(BMLX(bml_set_master_info)=(BMSetMasterInfo)GetSymbol(h,"bm_set_master_info"))) { puts("bm_set_master_info is missing");return(FALSE);}
  if(!(BMLX(bml_new)=(BMNew)GetSymbol(h,"bm_new"))) { puts("bm_new is missing");return(FALSE);}
  if(!(BMLX(bml_init)=(BMInit)GetSymbol(h,"bm_init"))) { puts("bm_init is missing");return(FALSE);}
  if(!(BMLX(bml_free)=(BMFree)GetSymbol(h,"bm_free"))) { puts("bm_free is missing");return(FALSE);}

  if(!(BMLX(bml_get_machine_info)=(BMGetMachineInfo)GetSymbol(h,"bm_get_machine_info"))) { puts("bm_get_machine_info is missing");return(FALSE);}
  if(!(BMLX(bml_get_global_parameter_info)=(BMGetGlobalParameterInfo)GetSymbol(h,"bm_get_global_parameter_info"))) { puts("bm_get_global_parameter_info is missing");return(FALSE);}
  if(!(BMLX(bml_get_track_parameter_info)=(BMGetTrackParameterInfo)GetSymbol(h,"bm_get_track_parameter_info"))) { puts("bm_get_track_parameter_info is missing");return(FALSE);}
  if(!(BMLX(bml_get_attribute_info)=(BMGetAttributeInfo)GetSymbol(h,"bm_get_attribute_info"))) { puts("bm_get_attribute_info is missing");return(FALSE);}

  if(!(BMLX(bml_get_track_parameter_location)=(BMGetTrackParameterLocation)GetSymbol(h,"bm_get_track_parameter_location"))) { puts("bm_get_track_parameter_location is missing");return(FALSE);}
  if(!(BMLX(bml_get_track_parameter_value)=(BMGetTrackParameterValue)GetSymbol(h,"bm_get_track_parameter_value"))) { puts("bm_get_track_parameter_value is missing");return(FALSE);}
  if(!(BMLX(bml_set_track_parameter_value)=(BMSetTrackParameterValue)GetSymbol(h,"bm_set_track_parameter_value"))) { puts("bm_set_track_parameter_value is missing");return(FALSE);}

  if(!(BMLX(bml_get_global_parameter_location)=(BMGetGlobalParameterLocation)GetSymbol(h,"bm_get_global_parameter_location"))) { puts("bm_get_global_parameter_location is missing");return(FALSE);}
  if(!(BMLX(bml_get_global_parameter_value)=(BMGetGlobalParameterValue)GetSymbol(h,"bm_get_global_parameter_value"))) { puts("bm_get_global_parameter_value is missing");return(FALSE);}
  if(!(BMLX(bml_set_global_parameter_value)=(BMSetGlobalParameterValue)GetSymbol(h,"bm_set_global_parameter_value"))) { puts("bm_set_global_parameter_value is missing");return(FALSE);}

  if(!(BMLX(bml_get_attribute_location)=(BMGetAttributeLocation)GetSymbol(h,"bm_get_attribute_location"))) { puts("bm_get_attribute_location is missing");return(FALSE);}
  if(!(BMLX(bml_get_attribute_value)=(BMGetAttributeValue)GetSymbol(h,"bm_get_attribute_value"))) { puts("bm_get_attribute_value is missing");return(FALSE);}
  if(!(BMLX(bml_set_attribute_value)=(BMSetAttributeValue)GetSymbol(h,"bm_set_attribute_value"))) { puts("bm_set_attribute_value is missing");return(FALSE);}

  if(!(BMLX(bml_tick)=(BMTick)GetSymbol(h,"bm_tick"))) { puts("bm_tick is missing");return(FALSE);}
  if(!(BMLX(bml_work)=(BMWork)GetSymbol(h,"bm_work"))) { puts("bm_work is missing");return(FALSE);}
  if(!(BMLX(bml_work_m2s)=(BMWorkM2S)GetSymbol(h,"bm_work_m2s"))) { puts("bm_work_m2s is missing");return(FALSE);}
  if(!(BMLX(bml_stop)=(BMStop)GetSymbol(h,"bm_stop"))) { puts("bm_stop is missing");return(FALSE);}

  if(!(BMLX(bml_set_num_tracks)=(BMSetNumTracks)GetSymbol(h,"bm_set_num_tracks"))) { puts("bm_set_num_tracks is missing");return(FALSE);}

  // @todo more API entries
  printf("%s:   symbols connected\n",__FUNCTION__);
  
  return(TRUE);
}

void bml_finalize(void) {
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
