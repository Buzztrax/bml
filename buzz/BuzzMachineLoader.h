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
	BMP_TYPE=0,
	BMP_VERSION,
	BMP_FLAGS,
	BMP_MIN_TRACKS,
	BMP_MAX_TRACKS,
	BMP_NUM_GLOBAL_PARAMS,
	BMP_NUM_TRACK_PARAMS,
	BMP_NUM_ATTRIBUTES,
	BMP_NAME,
	BMP_SHORT_NAME,
	BMP_AUTHOR,
	BMP_COMMANDS
} BuzzMachineProperty;

#ifndef BUZZ_MACHINE_LOADER_CPP
//extern void bm_set_master_info(long bpm, long tpb, long srat);
//extern BuzzMachine *bm_init(char *bm_file_name);
//extern void bm_free(BuzzMachine *bm);
//extern int bm_get_property(BuzzMachine *bm, BuzzMachineProperty key, void *value);
#endif

#ifdef __cplusplus
}
#endif

#endif // BUZZ_MACHINE_LOADER_H
