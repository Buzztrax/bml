#ifndef __bml_bml_h__
#define __bml_bml_h__

#include <setjmp.h>
#include <signal.h>
#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}
#endif

extern int bml_init(void (*sighandler)(int,siginfo_t*,void*));
extern void bml_done(void);

// dll passthrough API method pointer types
typedef void *(*BMInitPtr)(char *bm_file_name);
typedef void (*BMFreePtr)(void *bm);
// dll passthrough API method pointers
extern BMInitPtr bm_init;
extern BMFreePtr bm_free;

#endif /* __bml_bml_h__ */
