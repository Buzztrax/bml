/* $Id: BuzzMachineLoader.cpp,v 1.4 2007-12-03 19:05:05 ensonic Exp $
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

#define BUZZ_MACHINE_LOADER_CPP
#define BUZZ_MACHINE_LOADER

#ifdef WIN32
#include "stdafx.h"
#include <windows.h>
#else
#include "windows_compat.h"
#include <dlfcn.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <exception>
#include "debug.h"

#include "MachineInterface.h"
#include "BuzzMachineCallbacks.h"
#include "BuzzMachineCallbacksPre12.h"
#include "MachineDataImpl.h"
//#include "BuzzMachine.h"
#include "BuzzMachineLoader.h"
#include "dsplib.h"

#ifdef _MSVC_
#define DE __declspec(dllexport)
#else
#define DE 
#endif

// buzz emulation code

typedef CMachineInfo* (*GetInfoPtr)();
typedef CMachineInterface *(*CreateMachinePtr)();

typedef void (*CMIInitPtr)(CMachineInterface *_this, CMachineDataInput * const pi);

// globals
CMasterInfo master_info;

/*
CMachineDataInput mdi;

void CMachineDataInput::Read(void *pbuf, int const numbytes) {
    DBG2("(pbuf=%p,numbytes=%d)\n",pbuf,numbytes);
}
*/

// prototypes

extern "C" DE void bm_set_attribute_value(BuzzMachine *bm,int index,int value);
extern "C" DE void bm_set_global_parameter_value(BuzzMachine *bm,int index,int value);
extern "C" DE void bm_set_track_parameter_value(BuzzMachine *bm,int track,int index,int value);

// helpers

// public API

extern "C" DE void bm_set_logger(DebugLoggerFunc func) {
    debug_log_func=func;
}

extern "C" DE void bm_set_master_info(long bpm, long tpb, long srat) {
  master_info.BeatsPerMin=bpm;        /*   120 */
  master_info.TicksPerBeat=tpb;        /*     4 */
  master_info.SamplesPerSec=srat;    /* 44100 */
  master_info.SamplesPerTick=(int)((60*master_info.SamplesPerSec)/(master_info.BeatsPerMin*master_info.TicksPerBeat));
  master_info.PosInTick=0; /*master_info.SamplesPerTick-1;*/
  master_info.TicksPerSec=(float)master_info.SamplesPerSec/(float)master_info.SamplesPerTick;
#ifdef WIN32
  // todo:: this needs porting
  DSP_Init(master_info.SamplesPerSec);
#endif
}

extern "C" DE void bm_free(BuzzMachine *bm) {
    if(bm) {
        BuzzMachineCallbacks *callbacks = bm->callbacks;
        int version = bm->machine_info->Version;
        
        DBG("freeing\n");
        // FIXME: for Infector this leads to a double free?
        //delete bm->machine_iface;
        
        if(callbacks) {
            if(version & 0xff < 15) {
                delete (BuzzMachineCallbacksPre12 *)callbacks;
                DBG("  freed old callback instance\n");
            }
            else {
                delete callbacks;
                DBG("  freed callback instance\n");
            }         
        }
        if(bm->lib_name) {
            free(bm->lib_name);
        }
        if(bm->h) {
#ifdef WIN32
            FreeLibrary(bm->h);
#else
            dlclose(bm->h);
#endif
            DBG("  dll unloaded\n");
        }
        free(bm);
    }
}

extern "C" DE void bm_init(BuzzMachine *bm, unsigned long blob_size, unsigned char *blob_data) {
    int i,j;
    int wNumberOfTracks=0;

    DBG3("  bm_init(%p,%ld,%p)\n",bm,blob_data, blob_size);

    // initialise attributes
    for(i=0;i<bm->machine_info->numAttributes;i++) {
        bm_set_attribute_value(bm,i,bm->machine_info->Attributes[i]->DefValue);
    }
    DBG("  attributes initialized\n");
#if 1 /* params_first */
    // initialise global parameters (DefValue or NoValue, Buzz seems to use NoValue)
    for(i=0;i<bm->machine_info->numGlobalParameters;i++) {
        if(bm->machine_info->Parameters[i]->Flags&MPF_STATE) {
            bm_set_global_parameter_value(bm,i,bm->machine_info->Parameters[i]->DefValue);
        }
        else {
            bm_set_global_parameter_value(bm,i,bm->machine_info->Parameters[i]->NoValue);
        }
    }
    // initialise track parameters
    if((bm->machine_info->minTracks>0) && (bm->machine_info->maxTracks>0)) {
        wNumberOfTracks=bm->machine_info->minTracks;
        DBG3(" need to initialize %d track params for tracks: %d...%d\n",bm->machine_info->numTrackParameters,bm->machine_info->minTracks,bm->machine_info->maxTracks);
        for(j=0;j<bm->machine_info->maxTracks;j++) {
            for(i=0;i<bm->machine_info->numTrackParameters;i++) {
                if(bm->machine_info->Parameters[bm->machine_info->numGlobalParameters+i]->Flags&MPF_STATE) {
                    bm_set_track_parameter_value(bm,j,i,bm->machine_info->Parameters[bm->machine_info->numGlobalParameters+i]->DefValue);
                }
                else {
                    bm_set_track_parameter_value(bm,j,i,bm->machine_info->Parameters[bm->machine_info->numGlobalParameters+i]->NoValue);
                }
            }
        }
    }
    DBG("  parameters initialized\n");
#endif
    // create the machine data input
    CMachineDataInput * pcmdii = NULL;

    if (blob_size && blob_data) {
      pcmdii = new CMachineDataInputImpl(blob_data, blob_size);
    }
   
    // call Init
    try {
        bm->machine_iface->Init(pcmdii);
    }
    catch (std::exception& e) { DBG1("-> exeption: %s\n",e.what()); }
    catch(...) { DBG(" -> exeption\n"); }
    DBG("  CMachineInterface::Init() called\n");

    // call AttributesChanged
    if(bm->machine_info->numAttributes>0) {
        try {
            bm->machine_iface->AttributesChanged();
        }
        catch (std::exception& e) { DBG1("-> exeption: %s\n",e.what()); }
        catch(...) { DBG(" -> exeption\n"); }
        DBG("  CMachineInterface::AttributesChanged() called\n");
    }

#if 0  /* params_later */
    // initialise global parameters (DefValue or NoValue, Buzz seems to use NoValue)
    for(i=0;i<bm->machine_info->numGlobalParameters;i++) {
        if(bm->machine_info->Parameters[i]->Flags&MPF_STATE) {
            bm_set_global_parameter_value(bm,i,bm->machine_info->Parameters[i]->DefValue);
        }
        else {
            bm_set_global_parameter_value(bm,i,bm->machine_info->Parameters[i]->NoValue);
        }
    }
    DBG("  global parameters initialized\n");
#endif
#if 0 /* params_later */
    // initialise track parameters
    if((bm->machine_info->minTracks>0) && (bm->machine_info->maxTracks>0)) {
        wNumberOfTracks=bm->machine_info->minTracks;
#endif
        DBG3(" need to initialize %d track params for tracks: %d...%d\n",bm->machine_info->numTrackParameters,bm->machine_info->minTracks,bm->machine_info->maxTracks);
        if(wNumberOfTracks) {
            // call SetNumTracks
            DBG1("  CMachineInterface::SetNumTracks(%d)\n",wNumberOfTracks);
            try {
                // calling this without the '-1' crashes: Automaton Parametric EQ.dll
                //bm->machine_iface->SetNumTracks(wNumberOfTracks-1);
                bm->machine_iface->SetNumTracks(wNumberOfTracks);
            }
            catch (std::exception& e) { DBG1("-> exeption: %s\n",e.what()); }
            catch(...) { DBG(" -> exeption\n"); }
            DBG("  CMachineInterface::SetNumTracks() called\n");
        }

#if 0
        for(j=0;j<bm->machine_info->maxTracks;j++) {
            for(i=0;i<bm->machine_info->numTrackParameters;i++) {
                if(bm->machine_info->Parameters[bm->machine_info->numGlobalParameters+i]->Flags&MPF_STATE) {
                    bm_set_track_parameter_value(bm,j,i,bm->machine_info->Parameters[bm->machine_info->numGlobalParameters+i]->DefValue);
                }
                else {
                    bm_set_track_parameter_value(bm,j,i,bm->machine_info->Parameters[bm->machine_info->numGlobalParameters+i]->NoValue);
                }
            }
        }
    }
    DBG("  track parameters initialized\n");
#endif

    // now that we've given the machine the initial global- and track-parameters,
    // and the attributes, give it a tick
    // (NOTE - must tick HERE rather than later, to give (for example) the Master machine
    // a chance to update its state before any other machines need to rely on it ...)
    // (NOTE - must tick AFTER AttributesChanged, and after we've set initial track and global
    // data for machine)
    try {
        bm->machine_iface->Tick();
    }
    catch (std::exception& e) { DBG1("-> exeption: %s\n",e.what()); }
    catch(...) { DBG(" -> exeption\n"); }
    DBG("  CMachineInterface::Tick() called\n");

    if(bm->machine_info->Flags&MIF_USES_LIB_INTERFACE) {
        DBG(" MIF_USES_LIB_INTERFACE");
        FIXME;
    }
}

extern "C" DE BuzzMachine *bm_new(char *bm_file_name) {
    BuzzMachine *bm=(BuzzMachine *)calloc(sizeof(BuzzMachine),1);
    GetInfoPtr GetInfo=NULL;
    CreateMachinePtr CreateMachine=NULL;

#ifdef WIN32
    bm->h=LoadLibraryA(bm_file_name);
#else
    bm->h=dlopen(bm_file_name,RTLD_LAZY);
#endif
    if(!bm->h) {
#ifdef WIN32
        DBG1("  failed to load machine dll from \"%s\"\n",bm_file_name);
#else
        DBG2("  failed to load machine dll from \"%s\": %s\n",bm_file_name,dlerror());
#endif
        bm_free(bm);
        return(NULL);
    }
    DBG1("  dll %s loaded\n",bm_file_name);
    bm->lib_name=strdup(bm_file_name);

    //-- get the two dll entries
#ifdef WIN32
    GetInfo      =(GetInfoPtr      )GetProcAddress(bm->h,"GetInfo");
    CreateMachine=(CreateMachinePtr)GetProcAddress(bm->h,"CreateMachine");
#else
    GetInfo      =(GetInfoPtr      )dlsym(bm->h,"GetInfo");
    CreateMachine=(CreateMachinePtr)dlsym(bm->h,"CreateMachine");
#endif
    if(!GetInfo) {
        DBG("  failed to connect to GetInfo method\n");
        bm_free(bm);
        return(NULL);
    }
    if(!CreateMachine) {
        DBG("  failed to connect to GreateMachine method\n");
        bm_free(bm);
        return(NULL);
    }
    DBG("  symbols connected\n");

    //-- call GetInfo
    bm->machine_info=GetInfo();
    DBG("  GetInfo() called\n");
    /* DEBUG
    {
        char *machine_info_types[]={"MT_MASTER","MT_GENERATOR","MT_EFFECT" };
        printf("BML::"__FUNCTION__"  machine_info is %p\n",bm->machine_info);
        printf("BML::"__FUNCTION__"    Type: %i -> %s\n",bm->machine_info->Type,((bm->machine_info->Type<3)?machine_info_types[bm->machine_info->Type]:"unknown"));
        printf("BML::"__FUNCTION__"    Version: %i\n",bm->machine_info->Version);
        printf("BML::"__FUNCTION__"    Flags: %i\n",bm->machine_info->Flags);
        if(bm->machine_info->Flags&MIF_MONO_TO_STEREO)     puts("BML::"__FUNCTION__"      MIF_MONO_TO_STEREO");
        if(bm->machine_info->Flags&MIF_PLAYS_WAVES)        puts("BML::"__FUNCTION__"      MIF_PLAYS_WAVES");
        if(bm->machine_info->Flags&MIF_USES_LIB_INTERFACE) puts("BML::"__FUNCTION__"      MIF_USES_LIB_INTERFACE");
        if(bm->machine_info->Flags&MIF_USES_INSTRUMENTS)   puts("BML::"__FUNCTION__"      MIF_USES_INSTRUMENTS");
        if(bm->machine_info->Flags&MIF_DOES_INPUT_MIXING)  puts("BML::"__FUNCTION__"      MIF_DOES_INPUT_MIXING");
        if(bm->machine_info->Flags&MIF_NO_OUTPUT)          puts("BML::"__FUNCTION__"      MIF_NO_OUTPUT");
        if(bm->machine_info->Flags&MIF_CONTROL_MACHINE)    puts("BML::"__FUNCTION__"      MIF_CONTROL_MACHINE");
        if(bm->machine_info->Flags&MIF_INTERNAL_AUX)       puts("BML::"__FUNCTION__"      MIF_INTERNAL_AUX");
        //if(bm->machine_info->Flags&) puts("BML::"__FUNCTION__"      ");
        printf("BML::"__FUNCTION__"    minTracks: %i\n",bm->machine_info->minTracks);
        printf("BML::"__FUNCTION__"    maxTracks: %i\n",bm->machine_info->maxTracks);
        printf("BML::"__FUNCTION__"    numGlobalParameters: %i\n",bm->machine_info->numGlobalParameters);
        printf("BML::"__FUNCTION__"    numTrackParameters: %i\n",bm->machine_info->numTrackParameters);
        printf("BML::"__FUNCTION__"    numAttributes: %i\n",bm->machine_info->numAttributes);
        printf("BML::"__FUNCTION__"    Name: %s\n",bm->machine_info->Name);
        printf("BML::"__FUNCTION__"    ShortName: %s\n",bm->machine_info->ShortName);
        printf("BML::"__FUNCTION__"    Author: %s\n",bm->machine_info->Author);
        printf("BML::"__FUNCTION__"    Commands: %s\n",bm->machine_info->Commands);
        fflush(stdout);
    }
    */

    // call CreateMachine
    bm->machine_iface=CreateMachine();
    DBG("  CreateMachine() called\n");

    /* @todo: we need to create a CMachine object
     * bm->machine=new BuzzMachineXXX(bm->machine_iface,bm->machine_info)
     */
    bm->machine=NULL;

    if(bm->machine_info->Version & 0xff < 15) {
      // @todo: we need to pass a CMachine as first arg
      bm->callbacks=(BuzzMachineCallbacks *)new BuzzMachineCallbacksPre12(bm->machine,bm->machine_iface,bm->machine_info);
      DBG("  need to create old callback instance\n");
    }
    else {
      // @todo: we need to pass a CMachine as first arg
      bm->callbacks=new BuzzMachineCallbacks(bm->machine,bm->machine_iface,bm->machine_info);
      DBG("  callback instance created\n");
    }

    bm->machine_iface->pMasterInfo=&master_info;
    bm->machine_iface->pCB=bm->callbacks;

    //bm_init(bm,0,NULL);

    return(bm);
}

extern "C" DE int bm_get_machine_info(BuzzMachine *bm,BuzzMachineProperty key,void *value) {
    int ret=TRUE;
    int *ival;
    const char **sval;

    if(!value) return(FALSE);

    ival=(int *)value;
    sval=(const char **)value;
    switch(key) {
        case BM_PROP_TYPE:                *ival=bm->machine_info->Type;break;
        case BM_PROP_VERSION:            *ival=bm->machine_info->Version;break;
        case BM_PROP_FLAGS:                *ival=bm->machine_info->Flags;break;
        case BM_PROP_MIN_TRACKS:        *ival=bm->machine_info->minTracks;break;
        case BM_PROP_MAX_TRACKS:        *ival=bm->machine_info->maxTracks;break;
        case BM_PROP_NUM_GLOBAL_PARAMS:    *ival=bm->machine_info->numGlobalParameters;break;
        case BM_PROP_NUM_TRACK_PARAMS:    *ival=bm->machine_info->numTrackParameters;break;
        case BM_PROP_NUM_ATTRIBUTES:    *ival=bm->machine_info->numAttributes;break;
        case BM_PROP_NAME:                *sval=bm->machine_info->Name;break;
        case BM_PROP_SHORT_NAME:        *sval=bm->machine_info->ShortName;break;
        case BM_PROP_AUTHOR:            *sval=bm->machine_info->Author;break;
        case BM_PROP_COMMANDS:            *sval=bm->machine_info->Commands;break;
        case BM_PROP_DLL_NAME:          *sval=bm->lib_name;break;
        default: ret=FALSE;
    }
    return(ret);
}

static int bm_get_parameter_info(BuzzMachine *bm,int index,BuzzMachineParameter key,void *value) {
    int ret=TRUE;
    int *ival;
    const char **sval;

    ival=(int *)value;
    sval=(const char **)value;
    switch(key) {
        case BM_PARA_TYPE:            *ival=bm->machine_info->Parameters[index]->Type;break;
        case BM_PARA_NAME:            *sval=bm->machine_info->Parameters[index]->Name;break;
        case BM_PARA_DESCRIPTION:    *sval=bm->machine_info->Parameters[index]->Description;break;
        case BM_PARA_MIN_VALUE:        *ival=bm->machine_info->Parameters[index]->MinValue;break;
        case BM_PARA_MAX_VALUE:        *ival=bm->machine_info->Parameters[index]->MaxValue;break;
        case BM_PARA_NO_VALUE:        *ival=bm->machine_info->Parameters[index]->NoValue;break;
        case BM_PARA_FLAGS:            *ival=bm->machine_info->Parameters[index]->Flags;break;
        case BM_PARA_DEF_VALUE:        *ival=bm->machine_info->Parameters[index]->DefValue;break;
        default: ret=FALSE;
    }
    return(ret);
}

extern "C" DE int bm_get_global_parameter_info(BuzzMachine *bm,int index,BuzzMachineParameter key,void *value) {
    if(!value) return(FALSE);
    if(!(index<bm->machine_info->numGlobalParameters)) return(FALSE);

    return(bm_get_parameter_info(bm,index,key,value));
}

extern "C" DE int bm_get_track_parameter_info(BuzzMachine *bm,int index,BuzzMachineParameter key,void *value) {
    if(!value) return(FALSE);
    if(!(index<bm->machine_info->numTrackParameters)) return(FALSE);

    return(bm_get_parameter_info(bm,(bm->machine_info->numGlobalParameters+index),key,value));
}

extern "C" DE int bm_get_attribute_info(BuzzMachine *bm,int index,BuzzMachineAttribute key,void *value) {
    int ret=TRUE;
    int *ival;
    const char **sval;

    if(!value) return(FALSE);
    if(!(index<bm->machine_info->numAttributes)) return(FALSE);

    ival=(int *)value;
    sval=(const char **)value;
    switch(key) {
        case BM_ATTR_NAME:        *sval=bm->machine_info->Attributes[index]->Name;break;
        case BM_ATTR_MIN_VALUE:    *ival=bm->machine_info->Attributes[index]->MinValue;break;
        case BM_ATTR_MAX_VALUE:    *ival=bm->machine_info->Attributes[index]->MaxValue;break;
        case BM_ATTR_DEF_VALUE:    *ival=bm->machine_info->Attributes[index]->DefValue;break;
        default: ret=FALSE;
    }
    return(ret);
}

extern "C" DE void * bm_get_track_parameter_location(BuzzMachine *bm,int track,int index) {
    void *res=NULL;

    if(!(track<bm->machine_info->maxTracks)) return(0);
    if(!(index<bm->machine_info->numTrackParameters)) return(0);
    if(!(bm->machine_iface->TrackVals)) return(0);

    byte *ptr=(byte *)bm->machine_iface->TrackVals;
    if(!ptr) DBG(" -> machine->TrackVals is NULL!\n");

    // @todo prepare pointer array in bm_init
    for(int j=0;j<=track;j++) {
        for(int i=0;i<bm->machine_info->numTrackParameters;i++) {
            switch(bm->machine_info->Parameters[bm->machine_info->numGlobalParameters+i]->Type) {
                case pt_note:
                case pt_switch:
                case pt_byte:
                    if((j==track) && (i==index)) res=(void *)ptr;
                    else ptr++;
                    break;
                case pt_word:
                    if((j==track) && (i==index)) res=(void *)ptr;
                    else ptr+=2;
                    break;
            }
        }
    }
    return(res);
}

extern "C" DE int bm_get_track_parameter_value(BuzzMachine *bm,int track,int index) {
    int value=0;
    if(!(index<bm->machine_info->numTrackParameters)) return(0);
    if(!(bm->machine_iface->TrackVals)) return(0);

    void *ptr=bm_get_track_parameter_location(bm,track,index);
    switch(bm->machine_info->Parameters[bm->machine_info->numGlobalParameters+index]->Type) {
        case pt_note:
        case pt_switch:
        case pt_byte:
            value=(int)(*((byte *)ptr));
            break;
        case pt_word:
            value=(int)(*((word *)ptr));
            break;
    }
    return(value);
}

extern "C" DE void bm_set_track_parameter_value(BuzzMachine *bm,int track,int index,int value) {
    if(!(index<bm->machine_info->numTrackParameters)) return;
    if(!(bm->machine_iface->TrackVals)) return;

    void *ptr=bm_get_track_parameter_location(bm,track,index);
    switch(bm->machine_info->Parameters[bm->machine_info->numGlobalParameters+index]->Type) {
        case pt_note:
        case pt_switch:
        case pt_byte:
            (*((byte *)ptr))=(byte)value;
            break;
        case pt_word:
            (*((word *)ptr))=(word)value;
            break;
    }
}

extern "C" DE void * bm_get_global_parameter_location(BuzzMachine *bm,int index) {
    void *res=NULL;

    if(!(index<bm->machine_info->numGlobalParameters)) return(0);
    if(!(bm->machine_iface->GlobalVals)) return(0);

    byte *ptr=(byte *)bm->machine_iface->GlobalVals;
    if(!ptr) DBG(" -> machine->GlobalVals is NULL!\n");

    // @todo prepare pointer array in bm_init
    for(int i=0;i<=index;i++) {
        switch(bm->machine_info->Parameters[i]->Type) {
            case pt_note:
            case pt_switch:
            case pt_byte:
                if(i==index) res=(void *)ptr;
                else ptr++;
                break;
            case pt_word:
                if(i==index) res=(void *)ptr;
                else ptr+=2;
                break;
        }
    }
    return(res);
}

extern "C" DE int bm_get_global_parameter_value(BuzzMachine *bm,int index) {
    int value=0;
    if(!(index<bm->machine_info->numGlobalParameters)) return(0);
    if(!(bm->machine_iface->GlobalVals)) return(0);

    void *ptr=bm_get_global_parameter_location(bm,index);
    switch(bm->machine_info->Parameters[index]->Type) {
        case pt_note:
        case pt_switch:
        case pt_byte:
            value=(int)(*((byte *)ptr));
            break;
        case pt_word:
            value=(int)(*((word *)ptr));
            break;
    }
    return(value);
}

extern "C" DE void bm_set_global_parameter_value(BuzzMachine *bm,int index,int value) {
    if(!(index<bm->machine_info->numGlobalParameters)) return;
    if(!(bm->machine_iface->GlobalVals)) return;

    void *ptr=bm_get_global_parameter_location(bm,index);
    printf("%s: index=%d, GlobalVals :%p, %p\n",__PRETTY_FUNCTION__,index,bm->machine_iface->GlobalVals,ptr);
    switch(bm->machine_info->Parameters[index]->Type) {
        case pt_note:
        case pt_switch:
        case pt_byte:
            (*((byte *)ptr))=(byte)value;
            break;
        case pt_word:
            (*((word *)ptr))=(word)value;
            break;
    }
}

extern "C" DE void * bm_get_attribute_location(BuzzMachine *bm,int index) {
    if(!(index<bm->machine_info->numAttributes)) return(0);
    if(!(bm->machine_iface->AttrVals)) return(0);

    return((void *)&bm->machine_iface->AttrVals[index]);
}

extern "C" DE int bm_get_attribute_value(BuzzMachine *bm,int index) {
    if(!(index<bm->machine_info->numAttributes)) return(0);
    if(!(bm->machine_iface->AttrVals)) return(0);

    return(bm->machine_iface->AttrVals[index]);
}

extern "C" DE void bm_set_attribute_value(BuzzMachine *bm,int index,int value) {
    if(!(index<bm->machine_info->numAttributes)) return;
    if(!(bm->machine_iface->AttrVals)) return;

    bm->machine_iface->AttrVals[index]=value;
}

extern "C" DE void bm_attributes_changed(BuzzMachine *bm) {
    // call AttributesChanged
    if(bm->machine_info->numAttributes>0) {
        try {
            bm->machine_iface->AttributesChanged();
        }
        catch (std::exception& e) { DBG1("-> exeption: %s\n",e.what()); }
        catch(...) { DBG(" -> exeption\n"); }
        DBG("  CMachineInterface::AttributesChanged() called\n");
    }
}

extern "C" DE void bm_tick(BuzzMachine *bm) {
    try{
        bm->machine_iface->Tick();
    }
    catch (std::exception& e) { DBG1("-> exeption: %s\n",e.what()); }
    catch(...) { DBG(" -> exeption\n"); }
    // FIXME: do we need to bump that from somewhere
    //DBG1("master_info.PosInTick = %d\n", master_info.PosInTick);
}

extern "C" DE bool bm_work(BuzzMachine *bm,float *psamples, int numsamples, int const mode) {
    bool res=0;

    try {
        res=bm->machine_iface->Work(psamples,numsamples,mode);
    }
    catch (std::exception& e) { DBG1("-> exeption: %s\n",e.what()); }
    catch(...) { DBG(" -> exeption\n"); }
    return(res);
}

extern "C" DE bool bm_work_m2s(BuzzMachine *bm, float *pin, float *pout, int numsamples, int const mode) {
    bool res=0;

    try {
        res=bm->machine_iface->WorkMonoToStereo(pin,pout,numsamples,mode);
    }
    catch (std::exception& e) { DBG1("-> exeption: %s\n",e.what()); }
    catch(...) { DBG(" -> exeption\n"); }
    return(res);
}

extern "C" DE void bm_stop(BuzzMachine *bm) {
    try {
        bm->machine_iface->Stop();
    }
    catch (std::exception& e) { DBG1("-> exeption: %s\n",e.what()); }
    catch(...) { DBG(" -> exeption\n"); }
}

/*
virtual void AttributesChanged() {}
virtual void Command(int const i) {}
*/
extern "C" DE void bm_set_num_tracks(BuzzMachine *bm, int num) {
    try {
        DBG1("(num=%d)\n",num);
        bm->machine_iface->SetNumTracks(num);
        // we don't need to initialize the track params, as the max-num of tracks is already initialized in bm_init()
    }
    catch (std::exception& e) { DBG1("-> exeption: %s\n",e.what()); }
    catch(...) { DBG(" -> exeption\n"); }
}

/*
virtual void MuteTrack(int const i) {}
virtual bool IsTrackMuted(int const i) const { return false; }
*/

extern "C" DE char const *bm_describe_global_value(BuzzMachine *bm, int const param,int const value) {
    static const char *empty="";

    if(!(param<bm->machine_info->numGlobalParameters)) return(empty);

    try {
        DBG2("(param=%d,value=%d)\n",param,value);
        return(bm->machine_iface->DescribeValue(param,value));
    }
    catch (std::exception& e) { DBG1("-> exeption: %s\n",e.what()); }
    catch(...) { DBG(" -> exeption\n"); }
    return(empty);
}

extern "C" DE char const *bm_describe_track_value(BuzzMachine *bm, int const param,int const value) {
    static const char *empty="";

    if(!(param<bm->machine_info->numTrackParameters)) return(empty);

    try {
        DBG2("(param=%d,value=%d)\n",param,value);
        return(bm->machine_iface->DescribeValue(bm->machine_info->numGlobalParameters+param,value));
    }
    catch (std::exception& e) { DBG1("-> exeption: %s\n",e.what()); }
    catch(...) { DBG(" -> exeption\n"); }
    return(empty);
}
