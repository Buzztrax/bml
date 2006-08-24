/* $Id: bml.h,v 1.9 2006-08-24 19:24:22 ensonic Exp $
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

#ifndef BML_H
#define BML_H

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include <BuzzMachineLoader.h>

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE (!FALSE)
#endif

typedef void BuzzMachine;

//#ifdef __cplusplus
//extern "C" {
//#endif

extern int bml_setup(void (*sighandler)(int,siginfo_t*,void*));
extern void bml_finalize(void);
extern char *bml_convertpath(char *inpath);

// dll passthrough API method pointer types
typedef void (*BMSetMasterInfo)(long bpm, long tpb, long srat);
typedef void * (*BMNew)(char *bm_file_name);
typedef void (*BMInit)(BuzzMachine *bm);
typedef void (*BMFree)(BuzzMachine *bm);

typedef int (*BMGetMachineInfo)(BuzzMachine *bm, BuzzMachineProperty key, void *value);
typedef int (*BMGetGlobalParameterInfo)(BuzzMachine *bm,int index,BuzzMachineParameter key,void *value);
typedef int (*BMGetTrackParameterInfo)(BuzzMachine *bm,int index,BuzzMachineParameter key,void *value);
typedef int (*BMGetAttributeInfo)(BuzzMachine *bm,int index,BuzzMachineAttribute key,void *value);

typedef void * (*BMGetTrackParameterLocation)(BuzzMachine *bm,int track,int index);
typedef int  (*BMGetTrackParameterValue)(BuzzMachine *bm,int track,int index);
typedef void (*BMSetTrackParameterValue)(BuzzMachine *bm,int track,int index,int value);

typedef void * (*BMGetGlobalParameterLocation)(BuzzMachine *bm,int index);
typedef int  (*BMGetGlobalParameterValue)(BuzzMachine *bm,int index);
typedef void (*BMSetGlobalParameterValue)(BuzzMachine *bm,int index,int value);

typedef void * (*BMGetAttributeLocation)(BuzzMachine *bm,int index);
typedef int  (*BMGetAttributeValue)(BuzzMachine *bm,int index);
typedef void (*BMSetAttributeValue)(BuzzMachine *bm,int index,int value);

typedef void (*BMTick)(BuzzMachine *bm);
typedef int  (*BMWork)(BuzzMachine *bm,float *psamples, int numsamples, int const mode);
typedef int  (*BMWorkM2S)(BuzzMachine *bm,float *pin, float *pout, int numsamples, int const mode);
typedef void (*BMStop)(BuzzMachine *bm);

typedef void (*BMSetNumTracks)(BuzzMachine *bm, int num);

typedef const char *(*BMDescribeGlobalValue)(BuzzMachine *bm, int const param,int const value);
typedef const char *(*BMDescribeTrackValue)(BuzzMachine *bm, int const param,int const value);

// dll passthrough API method pointers
/*
extern BMSetMasterInfo bml_set_master_info;
extern BMNew bm_new;
extern BMInit bm_init;
extern BMFree bm_free;

extern BMGetMachineInfo bm_get_machine_info;
extern BMGetGlobalParameterInfo bm_get_global_parameter_info;
extern BMGetTrackParameterInfo bm_get_track_parameter_info;
extern BMGetAttributeInfo bm_get_attribute_info;

extern BMGetTrackParameterLocation bm_get_track_parameter_location;
extern BMGetTrackParameterValue bm_get_track_parameter_value;
extern BMSetTrackParameterValue bm_set_track_parameter_value;

extern BMGetGlobalParameterLocation bm_get_global_parameter_location;
extern BMGetGlobalParameterValue bm_get_global_parameter_value;
extern BMSetGlobalParameterValue bm_set_global_parameter_value;

extern BMGetAttributeLocation bm_get_attribute_location;
extern BMGetAttributeValue bm_get_attribute_value;
extern BMSetAttributeValue bm_set_attribute_value;

extern BMTick bm_tick;
extern BMWork bm_work;
extern BMWorkM2S bm_work_m2s;
extern BMStop bm_stop;

extern BMSetNumTracks bm_set_num_tracks;
*/

// dll wrapper API functions
extern void bml_set_master_info(long bpm, long tpb, long srat);
extern BuzzMachine *bml_new(char *bm_file_name);
extern void bml_init(BuzzMachine *bm);
extern void bml_free(BuzzMachine *bm);

extern int bml_get_machine_info(BuzzMachine *bm, BuzzMachineProperty key, void *value);
extern int bml_get_global_parameter_info(BuzzMachine *bm,int index,BuzzMachineParameter key,void *value);
extern int bml_get_track_parameter_info(BuzzMachine *bm,int index,BuzzMachineParameter key,void *value);
extern int bml_get_attribute_info(BuzzMachine *bm,int index,BuzzMachineAttribute key,void *value);

extern void *bml_get_track_parameter_location(BuzzMachine *bm,int track,int index);
extern int bml_get_track_parameter_value(BuzzMachine *bm,int track,int index);
extern void bml_set_track_parameter_value(BuzzMachine *bm,int track,int index,int value);

extern void *bml_get_global_parameter_location(BuzzMachine *bm,int index);
extern int bml_get_global_parameter_value(BuzzMachine *bm,int index);
extern void bml_set_global_parameter_value(BuzzMachine *bm,int index,int value);

extern void *bml_get_attribute_location(BuzzMachine *bm,int index);
extern int bml_get_attribute_value(BuzzMachine *bm,int index);
extern void bml_set_attribute_value(BuzzMachine *bm,int index,int value);

extern void bml_tick(BuzzMachine *bm);
extern int bml_work(BuzzMachine *bm,float *psamples, int numsamples, int const mode);
extern int bml_work_m2s(BuzzMachine *bm,float *pin, float *pout, int numsamples, int const mode);
extern void bml_stop(BuzzMachine *bm);

extern void bml_set_num_tracks(BuzzMachine *bm, int num);

extern const char *bml_describe_global_value(BuzzMachine *bm, int const param,int const value);
extern const char *bml_describe_track_value(BuzzMachine *bm, int const param,int const value);

//#ifdef __cplusplus
//}
//#endif

#endif // BML_H
