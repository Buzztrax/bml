// BuzzMachineLoader.h : defines the public API of the library
//

#ifndef BUZZ_MACHINE_LOADER_H
#define BUZZ_MACHINE_LOADER_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef BUZZ_MACHINE_LOADER_CPP
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
	BM_PROP_COMMANDS
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
extern void bm_set_master_info(long bpm, long tpb, long srat);
extern BuzzMachine *bm_init(char *bm_file_name);
extern void bm_free(BuzzMachine *bm);

extern int bm_get_machine_info(BuzzMachine *bm, BuzzMachineProperty key, void *value);
extern int bm_get_global_parameter_info(BuzzMachine *bm,int index,BuzzMachineParameter key,void *value);
extern int bm_get_track_parameter_info(BuzzMachine *bm,int index,BuzzMachineParameter key,void *value);
extern int bm_get_attribute_info(BuzzMachine *bm,int index,BuzzMachineAttribute key,void *value);

extern int  bm_get_track_parameter_value(BuzzMachine *bm,int index);
extern void bm_set_track_parameter_value(BuzzMachine *bm,int index,int value);
extern int  bm_get_global_parameter_value(BuzzMachine *bm,int index);
extern void bm_set_global_parameter_value(BuzzMachine *bm,int index,int value);
extern int  bm_get_attribute_value(BuzzMachine *bm,int index);
extern void bm_set_attribute_value(BuzzMachine *bm,int index,int value);

extern void bm_tick(BuzzMachine *bm);
extern bool bm_work(BuzzMachine *bm,float *psamples, int numsamples, int const mode);
extern void bm_stop(BuzzMachine *bm);
*/
#endif

#ifdef __cplusplus
}
#endif

#endif // BUZZ_MACHINE_LOADER_H
