/* $Id: bml.h,v 1.7 2004-09-09 16:54:48 ensonic Exp $
 */

#ifndef BML_H
#define BML_H

#include <setjmp.h>
#include <signal.h>
#include <pthread.h>

#undef BUZZ_MACHINE_LOADER_CPP
#include <BuzzMachineLoader.h>

//#ifdef __cplusplus
//extern "C" {
//#endif

/**
 * Display BML error message.
 *
 * Set via bml_set_error_function(), otherwise a BML-provided
 * default will print @a msg (plus a newline) to stderr.
 *
 * @param msg error message text (no newline at end).
 */
extern void (*bml_error_callback)(const char *msg);

/**
 * Set the @ref bml_error_callback for error message display.
 *
 * The BML library provides two built-in callbacks for this purpose:
 * default_bml_error_callback() and silent_bml_error_callback().
 */
void bml_set_error_function (void (*func)(const char *));

extern int bml_init(void (*sighandler)(int,siginfo_t*,void*));
extern void bml_done(void);
extern char *bml_convertpath(char *inpath);

// dll passthrough API method pointer types
typedef void (*BMSetMasterInfoPtr)(long bpm, long tpb, long srat);
typedef BuzzMachine *(*BMInitPtr)(char *bm_file_name);
typedef void (*BMFreePtr)(BuzzMachine *bm);

typedef int (*BMGetPropertyPtr)(BuzzMachine *bm, BuzzMachineProperty key, void *value);
typedef int (*BMGetGlobalParameter)(BuzzMachine *bm,int index,BuzzMachineParameter key,void *value);
typedef int (*BMGetTrackParameter)(BuzzMachine *bm,int index,BuzzMachineParameter key,void *value);
typedef int (*BMGetAttribute)(BuzzMachine *bm,int index,BuzzMachineAttribute key,void *value);

typedef void (*BMTick)(BuzzMachine *bm);
typedef int (*BMWork)(BuzzMachine *bm,float *psamples, int numsamples, int const mode);
typedef void (*BMStop)(BuzzMachine *bm);

// dll passthrough API method pointers
extern BMSetMasterInfoPtr bm_set_master_info;
extern BMInitPtr bm_init;
extern BMFreePtr bm_free;

extern BMGetPropertyPtr bm_get_property;
extern BMGetGlobalParameter bm_get_global_parameter;
extern BMGetTrackParameter bm_get_track_parameter;
extern BMGetAttribute bm_get_attribute;

extern BMTick bm_tick;
extern BMWork bm_work;
extern BMStop bm_stop;

//#ifdef __cplusplus
//}
//#endif

#endif // BML_H

