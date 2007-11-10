/* $Id: BuzzMachineLoader.h,v 1.2 2007-11-10 19:06:37 ensonic Exp $
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

#ifndef BUZZ_MACHINE_LOADER_H
#define BUZZ_MACHINE_LOADER_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef WIN32
#include <windows.h>
#else
#include "windows_compat.h"
#endif


// our high level instance handle
#ifdef BUZZ_MACHINE_LOADER
class BuzzMachineCallbacks;

class BuzzMachine {
public:
	// library handle
	HMODULE h;
	char *lib_name;
	// callback instance
	BuzzMachineCallbacks *callbacks;
	// classes
	CMachineInfo *machine_info;
	CMachineInterface *machine_iface;
	CMachine *machine;
	//callbacks->machine_ex;
	//CMachineInterfaceEx *machine_ex;
};
#else
typedef void BuzzMachine;
#endif

typedef enum {
	BM_PROP_TYPE=0,
	BM_PROP_VERSION,
	BM_PROP_FLAGS,
	BM_PROP_MIN_TRACKS,
	BM_PROP_MAX_TRACKS,
	BM_PROP_NUM_GLOBAL_PARAMS,
	BM_PROP_NUM_TRACK_PARAMS,
	BM_PROP_NUM_ATTRIBUTES,
	BM_PROP_NAME,
	BM_PROP_SHORT_NAME,
	BM_PROP_AUTHOR,
	BM_PROP_COMMANDS,
	BM_PROP_DLL_NAME
} BuzzMachineProperty;

typedef enum {
	BM_PARA_TYPE=0,
	BM_PARA_NAME,
	BM_PARA_DESCRIPTION,
	BM_PARA_MIN_VALUE,
	BM_PARA_MAX_VALUE,
	BM_PARA_NO_VALUE,
	BM_PARA_FLAGS,
	BM_PARA_DEF_VALUE
} BuzzMachineParameter;

typedef enum {
	BM_ATTR_NAME=0,
	BM_ATTR_MIN_VALUE,
	BM_ATTR_MAX_VALUE,
	BM_ATTR_DEF_VALUE
} BuzzMachineAttribute;

#ifndef BUZZ_MACHINE_LOADER_CPP
/*
extern void   bm_set_master_info(long bpm, long tpb, long srat);
extern BuzzMachine *bm_new(char *bm_file_name);
extern void   bm_free(BuzzMachine *bm);
extern void   bm_init(BuzzMachine *bm, unsigned long blob_size, unsigned char *blob_data);

extern int    bm_get_machine_info(BuzzMachine *bm, BuzzMachineProperty key, void *value);
extern int    bm_get_global_parameter_info(BuzzMachine *bm,int index,BuzzMachineParameter key,void *value);
extern int    bm_get_track_parameter_info(BuzzMachine *bm,int index,BuzzMachineParameter key,void *value);
extern int    bm_get_attribute_info(BuzzMachine *bm,int index,BuzzMachineAttribute key,void *value);

extern void * bm_get_track_parameter_location(BuzzMachine *bm,int track,int index);
extern int    bm_get_track_parameter_value(BuzzMachine *bm,int track,int index);
extern void   bm_set_track_parameter_value(BuzzMachine *bm,int track,int index,int value);

extern void * bm_get_global_parameter_location(BuzzMachine *bm,int index);
extern int    bm_get_global_parameter_value(BuzzMachine *bm,int index);
extern void   bm_set_global_parameter_value(BuzzMachine *bm,int index,int value);

extern void * bm_get_attribute_location(BuzzMachine *bm,int index);
extern int    bm_get_attribute_value(BuzzMachine *bm,int index);
extern void   bm_set_attribute_value(BuzzMachine *bm,int index,int value);
extern void   bm_attributes_changed(BuzzMachine *bm);

extern void   bm_tick(BuzzMachine *bm);
extern bool   bm_work(BuzzMachine *bm,float *psamples, int numsamples, int const mode);
extern bool   bm_work_m2s(BuzzMachine *bm,float *pin, float *pout, int numsamples, int const mode);
extern void   bm_stop(BuzzMachine *bm);

extern void   bm_set_num_tracks(BuzzMachine *bm, int num);
*/
#endif

#ifdef __cplusplus
}
#endif

#endif // BUZZ_MACHINE_LOADER_H
