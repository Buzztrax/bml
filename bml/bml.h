#ifndef __bml_bml_h__
#define __bml_bml_h__

#include <setjmp.h>
#include <signal.h>
#include <pthread.h>

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

//@todo include buzz stuff here

typedef struct {
	int Type;                                             // MT_GENERATOR or MT_EFFECT,
	int Version;                                          // MI_VERSION
	// v1.2: high word = internal machine version
	// higher version wins if two copies of machine found
	int Flags;
	int minTracks;                                        // [0..256] must be >= 1 if numTrackParameters > 0
	int maxTracks;                                        // [minTracks..256]
	int numGlobalParameters;
	int numTrackParameters;
	void /*CMachineParameter*/ const **Parameters;
	int numAttributes;
	void /*CMachineAttribute*/ const **Attributes;
	char const *Name;                                     // "Jeskola Reverb"
	char const *ShortName;                                // "Reverb"
	char const *Author;                                   // "Oskari Tammelin"
	char const *Commands;                                 // "Command1\nCommand2\nCommand3..."
	void /*CLibInterface*/ *pLI;                                   // ignored if MIF_USES_LIB_INTERFACE is not set
} CMachineInfo;

typedef CMachineInfo* (*GetInfoFunctionType)();

#ifdef __cplusplus
}
#endif

#endif /* __bml_bml_h__ */
