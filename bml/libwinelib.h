
/*
 * header file for winelib
 * simple dlopen like interface for dlls
 *
 * This file is released under the LGPL.
 * Author: Torben Hohn <torbenh@informatik.uni-bremen.de>
 */

#ifndef WINELIB_H
#define WINELIB_H

#include <signal.h>
#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

extern HANDLE gui_thread;
extern DWORD  gui_thread_id;

void * WineLoadLibrary(const char *dll);
void WineFreeLibrary(void *handle);
void * WineGetProcAddress(void *handle, const char *function);
int SharedWineInit(void (*sighandler)(int, siginfo_t*, void*));
int WineAdoptThread(void);

#ifdef __cplusplus
}
#endif

#endif
