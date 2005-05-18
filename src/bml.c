/* $Id: bml.c,v 1.1 2005-05-18 12:47:25 ensonic Exp $
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
HINSTANCE h;
ldt_fs_t *ldt_fs;
#endif
#ifdef USE_DLLWRAPPER2
void *h;
#endif

// API method pointers
BMSetMasterInfo bm_set_master_info;
BMNew bm_new;
BMInit bm_init;
BMFree bm_free;

BMGetMachineInfo bm_get_machine_info;
BMGetGlobalParameterInfo bm_get_global_parameter_info;
BMGetTrackParameterInfo bm_get_track_parameter_info;
BMGetAttributeInfo bm_get_attribute_info;

BMGetTrackParameterLocation bm_get_track_parameter_location;
BMGetTrackParameterValue bm_get_track_parameter_value;
BMSetTrackParameterValue bm_set_track_parameter_value;

BMGetGlobalParameterLocation bm_get_global_parameter_location;
BMGetGlobalParameterValue bm_get_global_parameter_value;
BMSetGlobalParameterValue bm_set_global_parameter_value;

BMGetAttributeLocation bm_get_attribute_location;
BMGetAttributeValue bm_get_attribute_value;
BMSetAttributeValue bm_set_attribute_value;

BMTick bm_tick;
BMWork bm_work;
BMWorkM2S bm_work_m2s;
BMStop bm_stop;

BMSetNumTracks bm_set_num_tracks;

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

int bml_init(void (*sighandler)(int,siginfo_t*,void*)) {
  printf("%s: bml_init\n",__FUNCTION__);
  
#ifdef USE_DLLWRAPPER1
 ldt_fs=Setup_LDT_Keeper();
#endif
#ifdef USE_DLLWRAPPER2
  SharedWineInit(sighandler);
#endif

  if(!(h=LoadDLL("BuzzMachineLoader.dll"))) return(FALSE);
  printf("%s:   bml loaded\n",__FUNCTION__);

  if(!(bm_set_master_info=(BMSetMasterInfo)GetSymbol(h,"bm_set_master_info"))) { puts("bm_set_master_info is missing");return(FALSE);}
  if(!(bm_new=(BMNew)GetSymbol(h,"bm_new"))) { puts("bm_new is missing");return(FALSE);}
  if(!(bm_init=(BMInit)GetSymbol(h,"bm_init"))) { puts("bm_init is missing");return(FALSE);}
  if(!(bm_free=(BMFree)GetSymbol(h,"bm_free"))) { puts("bm_free is missing");return(FALSE);}

  if(!(bm_get_machine_info=(BMGetMachineInfo)GetSymbol(h,"bm_get_machine_info"))) { puts("bm_get_machine_info is missing");return(FALSE);}
  if(!(bm_get_global_parameter_info=(BMGetGlobalParameterInfo)GetSymbol(h,"bm_get_global_parameter_info"))) { puts("bm_get_global_parameter_info is missing");return(FALSE);}
  if(!(bm_get_track_parameter_info=(BMGetTrackParameterInfo)GetSymbol(h,"bm_get_track_parameter_info"))) { puts("bm_get_track_parameter_info is missing");return(FALSE);}
  if(!(bm_get_attribute_info=(BMGetAttributeInfo)GetSymbol(h,"bm_get_attribute_info"))) { puts("bm_get_attribute_info is missing");return(FALSE);}

  if(!(bm_get_track_parameter_location=(BMGetTrackParameterLocation)GetSymbol(h,"bm_get_track_parameter_location"))) { puts("bm_get_track_parameter_location is missing");return(FALSE);}
  if(!(bm_get_track_parameter_value=(BMGetTrackParameterValue)GetSymbol(h,"bm_get_track_parameter_value"))) { puts("bm_get_track_parameter_value is missing");return(FALSE);}
  if(!(bm_set_track_parameter_value=(BMSetTrackParameterValue)GetSymbol(h,"bm_set_track_parameter_value"))) { puts("bm_set_track_parameter_value is missing");return(FALSE);}

  if(!(bm_get_global_parameter_location=(BMGetGlobalParameterLocation)GetSymbol(h,"bm_get_global_parameter_location"))) { puts("bm_get_global_parameter_location is missing");return(FALSE);}
  if(!(bm_get_global_parameter_value=(BMGetGlobalParameterValue)GetSymbol(h,"bm_get_global_parameter_value"))) { puts("bm_get_global_parameter_value is missing");return(FALSE);}
  if(!(bm_set_global_parameter_value=(BMSetGlobalParameterValue)GetSymbol(h,"bm_set_global_parameter_value"))) { puts("bm_set_global_parameter_value is missing");return(FALSE);}

  if(!(bm_get_attribute_location=(BMGetAttributeLocation)GetSymbol(h,"bm_get_attribute_location"))) { puts("bm_get_attribute_location is missing");return(FALSE);}
  if(!(bm_get_attribute_value=(BMGetAttributeValue)GetSymbol(h,"bm_get_attribute_value"))) { puts("bm_get_attribute_value is missing");return(FALSE);}
  if(!(bm_set_attribute_value=(BMSetAttributeValue)GetSymbol(h,"bm_set_attribute_value"))) { puts("bm_set_attribute_value is missing");return(FALSE);}

  if(!(bm_tick=(BMTick)GetSymbol(h,"bm_tick"))) { puts("bm_tick is missing");return(FALSE);}
  if(!(bm_work=(BMWork)GetSymbol(h,"bm_work"))) { puts("bm_work is missing");return(FALSE);}
  if(!(bm_work_m2s=(BMWorkM2S)GetSymbol(h,"bm_work_m2s"))) { puts("bm_work_m2s is missing");return(FALSE);}
  if(!(bm_stop=(BMStop)GetSymbol(h,"bm_stop"))) { puts("bm_stop is missing");return(FALSE);}

  if(!(bm_set_num_tracks=(BMSetNumTracks)GetSymbol(h,"bm_set_num_tracks"))) { puts("bm_set_num_tracks is missing");return(FALSE);}

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
