#ifndef _XS_OSDEP_H_
#define _XS_OSDEP_H_

#include <windows.h>
#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#define XS_EXPORT				__declspec(dllexport)
#define XS_PATH_DELIMITER		'\\'

#ifdef _MSC_VER
#pragma warning(disable : 4100 4819 4512)       // 4100 for unimplemented functions, 4819 for UTF-8
#endif

#ifdef _DEBUG
#define XS_MEM_DEBUG
#endif

#define XS_HAS_INPUT_CONTROL
#define XS_CONFIG_FILE_IOBUFFER_SIZE		4096

#define XS_FEATURE_PRELOAD // XXX: REMOVE IT WHEN SIMULATOR ABSENCED
#define XS_FEATURE_MULTIUSER

#define XS_INTERFACE			__declspec(dllexport)

typedef uintptr_t xsUIntPtr;
typedef FILE *xsFile;
typedef wchar_t xsWChar;

#define XS_UNICODE

#define XS_CFG_MAX_TRACE_LEN	1024
#define XS_CFG_MAX_RESOLVE_MSG	128

#define XS_MARGIN		(5)

#define WM_XS_SOCKET_MSG		(WM_USER + 101)
#define WM_XS_RESOLVE_MSG		(WM_USER + 110)
#define WM_XS_RESOLVE_MSG_END	(WM_USER + XS_CFG_MAX_RESOLVE_MSG)

int xsPalGuiInit(void);
void xsPalGuiUninit(void);
int xsUpdateScreen(int left, int top, int right, int bottom);


extern HWND g_hMainWnd;
extern HINSTANCE g_hInst;
void xsPalResetFrameWnd();
int xsPalHandleGetAddrByName(UINT message, WPARAM wParam, LPARAM lParam);
int xsPalHandleSocketEvent(UINT message, WPARAM wParam, LPARAM lParam);
int MainEntry(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine,
		int nCmdShow);
#ifdef __cplusplus
}
#endif

#ifdef _MSC_VER
#pragma warning(disable : 4996)	// for deprecated POSIX and ANSI functions
#endif

#define XS_TIMERS_MAX		32

#endif /* _XS_OSDEP_H_ */
