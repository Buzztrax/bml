#include "bml.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include <libwinelib.h>
#include <windows.h>
#include <winnt.h>
#include <wine/exception.h>

// error logging

void bml_error(const char *fmt, ...) {
	va_list ap;
	char buffer[512];

	va_start (ap, fmt);
	vsnprintf (buffer, sizeof(buffer), fmt, ap);
	bml_error_callback (buffer);
	va_end (ap);
}

void default_bml_error_callback(const char *desc) {
	fprintf(stderr, "%s\n", desc);
}

void (*bml_error_callback)(const char *desc) = &default_bml_error_callback;

// wine stuff

int wine_shared_premain () {
	//WNDCLASSA wc;
	HMODULE hInst;

	if ((hInst = GetModuleHandleA (NULL)) == NULL) {
		bml_error ("can't get module handle");
		return -1;
	}
  /*
	wc.style = 0;
	wc.lpfnWndProc = my_window_proc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInst;
	wc.hIcon = LoadIconA( hInst, "BML");
	wc.hCursor = LoadCursorA( NULL, IDI_APPLICATION );
	wc.hbrBackground = GetStockObject( BLACK_BRUSH );
	wc.lpszMenuName = "MENU_BML";
	wc.lpszClassName = "BML";

	if (!RegisterClassA(&wc)){
		return 1;
	}

	if (CreateThread (NULL, 0, gui_event_loop, NULL, 0, NULL) == NULL) {
		bml_error ("could not create new thread proxy");
		return -1;
	}
  */
	return 0;
}

