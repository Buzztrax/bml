/* $Id: bml.h,v 1.5 2004-09-02 07:49:11 ensonic Exp $
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

// dll passthrough API method pointer types
typedef void (*BMSetMasterInfoPtr)(long bpm, long tpb, long srat);
typedef BuzzMachine *(*BMInitPtr)(char *bm_file_name);
typedef void (*BMFreePtr)(BuzzMachine *bm);
typedef int (*BMGetPropertyPtr)(BuzzMachine *bm, BuzzMachineProperty key, void *value);
// dll passthrough API method pointers
extern BMSetMasterInfoPtr bm_set_master_info;
extern BMInitPtr bm_init;
extern BMFreePtr bm_free;
extern BMGetPropertyPtr bm_get_property;

//#ifdef __cplusplus
//}
//#endif

#endif // BML_H

