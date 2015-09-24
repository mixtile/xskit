
#include <windows.h>
#include <stdio.h>
#include <direct.h>
#include <time.h>
#include <assert.h>
#include <tchar.h>
#ifdef _MSC_VER
#include <crtdbg.h>
#endif

#include "xs/pal.h"

extern xsBool xsUnitTest(const char *unitName);
HWND g_hMainWnd;


xsTChar *xsGetExternalStoragePath(xsTChar *buf)
{
	return NULL;
}

xsTChar *xsGetPublicStoragePath(xsTChar *buf)
{
	// TODO: user's home
	return NULL;
}

xsTChar *xsGetPrivateStoragePath(xsTChar *buf)
{
	return NULL;
}

xsTChar *xsGetLocalDataPath(xsTChar *buf)
{
	xsTChar *dir = _tgetcwd(buf, XS_MAX_PATH);
	if (dir != NULL)
		xsTcsCat(buf, xsT("\\"));

	return dir;
}

xsFile xsOpenFile(const xsTChar *filename, xsU32 style)
{
	DWORD dwDisposition = 0;
	DWORD dwAccess = 0;
	HANDLE file;

	if (style & XS_OF_READWRITE)
		dwAccess = GENERIC_READ | GENERIC_WRITE;
	else if (style & XS_OF_WRITEONLY)
		dwAccess = GENERIC_WRITE;
  	else if (style & XS_OF_READONLY)
  		dwAccess = GENERIC_READ;
	
	if (style & XS_OF_CREATE)
	{
		if (style & XS_OF_EXCLUSIVE)
			dwDisposition = OPEN_ALWAYS;
		else
			dwDisposition = CREATE_ALWAYS;
	}
	else
	{
		dwDisposition = OPEN_EXISTING;
	}

	file = CreateFile((const TCHAR *)filename, dwAccess, 0, NULL, dwDisposition, FILE_ATTRIBUTE_NORMAL, NULL);

	if (file == INVALID_HANDLE_VALUE)
	{//TODO: transform error code
		DWORD dwError = GetLastError();
		XS_ERROR("Open file failed. code: %d", dwError);
	}
	
	return (xsFile)file;
}

int xsCloseFile(xsFile handle)
{
	return CloseHandle((HANDLE)handle);
}

xsBool xsFileHandleValid(xsFile handle)
{
	return ((HANDLE)handle != INVALID_HANDLE_VALUE);
}

xsFile xsFileInvalidHandle()
{
	return (xsFile)INVALID_HANDLE_VALUE;
}

size_t xsReadFile(xsFile handle, void *buffer, size_t size)
{
	DWORD read = 0;
	BOOL ret;

	ret = ReadFile((HANDLE)handle, buffer, size, &read, NULL);
	if (ret)
		return (size_t)read;
	else
		return (size_t)(XS_EC_ERROR);
}

size_t xsWriteFile(xsFile handle, const void *buffer, size_t size)
{
	DWORD written = 0;
	BOOL ret;

	ret = WriteFile((HANDLE)handle, buffer, size, &written, NULL);
	if (ret)
		return (size_t)written;
	else
		return (size_t)(-1);
}

long xsSetFilePointer(xsFile handle, long offset, int origin)
{
	DWORD ret;
	DWORD whence;

	if (origin == XS_FILE_BEGIN)
		whence = FILE_BEGIN;
	else if (origin == XS_FILE_CURRENT)
		whence = FILE_CURRENT;
	else if (origin == XS_FILE_END)
		whence = FILE_END;
	else
		return XS_EC_ERROR;

	ret = SetFilePointer((HANDLE)handle, offset, 0, whence);
	return (long)ret;
}

long xsGetFilePointer(xsFile handle)
{
	DWORD ret;

	ret = SetFilePointer((HANDLE)handle, 0, 0, XS_FILE_CURRENT);
	return ret;
}

int xsFlushFile(xsFile handle)
{
	if (FlushFileBuffers((HANDLE)handle))
		return XS_EC_OK;
	else
		return XS_EC_ERROR;
}

size_t xsGetFileSize(xsFile handle)
{
	return (size_t)GetFileSize((HANDLE)handle, NULL);
}

xsBool xsFileExists(const xsTChar *path)
{
	WIN32_FIND_DATA FindFileData;

	HANDLE handle = FindFirstFile(path, &FindFileData);
	if (handle != INVALID_HANDLE_VALUE) 
	{
		FindClose(handle);
		return XS_FALSE;
	}

	return XS_TRUE;
}

int xsCreateDir(const xsTChar *path)
{
	if (CreateDirectory(path, NULL))
		return XS_EC_OK;

	return XS_EC_ERROR;
}

int xsRemoveDir(const xsTChar *path)
{
	if (RemoveDirectory(path))
		return XS_EC_OK;

	return XS_EC_ERROR;
}

xsRes xsOpenRes(const xsTChar *name)
{
	return (xsRes)xsOpenFile(name, XS_OF_READONLY);
}

xsBool xsResHandleValid(xsRes handle)
{
	return xsFileHandleValid(handle);
}

xsRes xsResInvalidHandle()
{
	return (xsRes)xsFileInvalidHandle();
}

void xsCloseRes(xsRes handle)
{
	xsCloseFile((xsFile)handle);
}

size_t xsReadRes(xsRes handle, void *buffer, size_t begin, size_t size)
{
	xsSetFilePointer(handle, begin, XS_FILE_BEGIN);
	return xsReadFile(handle, buffer, size);
}

size_t xsStrLen(const char *string)
{
	return strlen(string);
}

char *xsStrCpy(char *strTo, const char *strFrom)
{
	return strcpy(strTo, strFrom);
}

char *xsStrCpyN(char *strTo, const char *strFrom, size_t max)
{
	return strncpy(strTo, strFrom, max);
}

int xsStrCmp(const char *string1, const char *string2)
{
	return strcmp(string1, string2);
}

int xsStrCaseCmp(const char *string1, const char *string2)
{
	//XXX: perhaps no this function
	return stricmp(string1, string2);
}

int xsStrCmpN(const char *string1, const char *string2, int max)
{
	return strncmp(string1, string2, max);
}

int xsStrCaseCmpN(const char *string1, const char *string2, int max)
{
	return strnicmp(string1, string2, max);
}

char *xsStrCat(char *strTo, const char *strFrom)
{
	return strcat(strTo, strFrom);
}

char *xsStrCatN(char *strTo, const char *strFrom, int max)
{
	return strncat(strTo, strFrom, max);
}

const char *xsStrChr(const char *string, int c)
{
	return strchr(string, c);
}

const char *xsStrStr(const char *string, const char *substring)
{
	return strstr(string, substring);
}

int xsStrToInt(const char *string)
{
	return atoi(string);
}

char *xsIntToStr(int value, char *buffer, int radix)
{
	return _itoa(value, buffer, radix);
}

char *xsStrLwr(char *string)
{
	int i = 0;

	if (string == NULL)
		return string;

	for (i = 0; string[i] != 0; i++)
	{
		if (string[i] >= 'A' && string[i] <= 'Z')
			string[i] += 32;
	}

	return string;
}

char *xsStrUpr(char *string)
{
	int i = 0;

	if (string == NULL)
		return string;

	for (i = 0; string[i] != 0; i++)
	{
		if (string[i] >= 'a' && string[i] <= 'z')
			string[i] -= 32;
	}

	return string;
}

int xsSnprintf(char *buffer, size_t size, const char *format, ...)
{
	va_list args;
	int ret;

	va_start(args, format);
	ret = vsnprintf(buffer, size, format, args);
	va_end(args);

	return ret;
}

size_t xsWcsLen(const xsWChar *string)
{
	return wcslen(string);
}

xsWChar *xsWcsCpy(xsWChar *strTo, const xsWChar *strFrom)
{
	return (xsWChar *)wcscpy(strTo, strFrom);
}

xsWChar *xsWcsCpyN(xsWChar *strTo, const xsWChar *strFrom, size_t max)
{
	return (xsWChar *)wcsncpy(strTo, strFrom, max);
}

int xsWcsCmp(const xsWChar *string1, const xsWChar *string2)
{
	return wcscmp(string1, string2);
}

int xsWcsCaseCmp(const xsWChar *string1, const xsWChar *string2)
{
	return wcsicmp(string1, string2);
}

int xsWcsCmpN(const xsWChar *string1, const xsWChar *string2, int max)
{
	return wcsncmp(string1, string2, max);
}

int xsWcsCaseCmpN(const xsWChar *string1, const xsWChar *string2, int max)
{
	return wcsnicmp(string1, string2, max);
}

xsWChar *xsWcsCat(xsWChar *strTo, const xsWChar *strFrom)
{
	return (xsWChar *)wcscat(strTo, strFrom);
}

xsWChar *xsWcsCatN(xsWChar *strTo, const xsWChar *strFrom, int max)
{
	return (xsWChar *)wcsncat(strTo, strFrom, max);
}

xsWChar *xsWcsChr(const xsWChar *string, int c)
{
	return (xsWChar *)wcschr(string, c);
}

xsWChar *xsWcsStr(const xsWChar *string, const xsWChar *substring)
{
	return (xsWChar *)wcsstr(string, substring);
}

int xsWcsToInt(const xsWChar *string)
{
	return _wtoi(string);
}

xsWChar *xsWcsLwr(xsWChar *string)
{
	return (xsWChar *)wcslwr(string);
}

xsWChar *xsWcsUpr(xsWChar *string)
{
	return (xsWChar *)wcsupr(string);
}

/*
int xsWSprintf(xsWChar *buffer, xsWChar *format, ...)
{
	return 0;
}
*/

size_t xsMbsToWcs(xsWChar *dest, const char *src, size_t count)
{
	return mbstowcs(dest, src, count);
}

size_t xsWcsToMbs(char *dest, const xsWChar *src, size_t count)
{
	return wcstombs(dest, src, count);
}

void xsSetBacklight(int mode)
{
}

void *xsMemSet(void *dest, int c, size_t count)
{
	return memset(dest, c, count);
}

int xsMemCmp(const void *pointer1, const void *pointer2, size_t size)
{
	return memcmp(pointer1, pointer2, size);
}

void *xsMemCpy(void *dest, const void *src, size_t count)
{
	return memcpy(dest, src, count);
}

void *xsMemMove(void *dest, const void *src, size_t count)
{
	return memmove(dest, src, count);
}

void *xsMallocNative(size_t size)
{
	return malloc(size);
}

void *xsCallocNative(size_t size)
{
	return calloc(1, size);
}

void *xsReAllocNative(void *pointer, size_t size)
{
	return realloc(pointer, size);
}

void xsFreeNative(void *pointer)
{
	free(pointer);
}

void xsQuit(void)
{
	PostMessage(g_hMainWnd, WM_CLOSE, 0, 0);
}

void xsSeed(int seed)
{
	srand(seed);
}

int xsRandom(void)
{
	return rand();
}

xsU32 xsTime(void)
{
	return (xsU32)time(NULL);
}

xsTimeType *xsGetLocalTime(xsTimeType *tmt)
{
	if (tmt != NULL)
	{
		struct tm *dt;
		time_t t;

		t = time(NULL);
		dt = localtime(&t);

		tmt->hour = dt->tm_hour;
		tmt->isdst = dt->tm_isdst;
		tmt->mday = dt->tm_mday;
		tmt->min = dt->tm_min;
		tmt->mon = dt->tm_mon;
		tmt->sec = dt->tm_sec;
		tmt->wday = dt->tm_wday;
		tmt->yday = dt->tm_yday;
		tmt->year = dt->tm_year;
	}

	return tmt;
}

typedef struct _xsTimerItem
{
	UINT idEvent;
	xsCallbackFunc cb;
	void *userdata;
} xsTimerItem;

static xsTimerItem g_xsTimers[XS_TIMERS_MAX] = {{0}};
static int g_xsValidTimerIds[XS_TIMERS_MAX] = {0};
static int g_xsTimerIdHead = -1;
static int g_xsTimerIdTail = -1;
static int g_xsTimerCount = 0;

static xsU32 xsGetValidTimerId(void)
{
	int id;

	if (g_xsTimerCount >= XS_TIMERS_MAX)
		return 0;

	g_xsTimerIdHead++;
	g_xsTimerCount++;
	if (g_xsTimerIdHead >= XS_TIMERS_MAX)
		g_xsTimerIdHead = 0;

	id = g_xsValidTimerIds[g_xsTimerIdHead];
	if (id == 0)
		return g_xsTimerIdHead + 1; // 1 based

	return id;
}

static void xsReturnValidTimerId(xsU32 id)
{
	g_xsTimerIdTail++;
	g_xsTimerCount--;
	if (g_xsTimerIdTail >= XS_TIMERS_MAX)
		g_xsTimerIdTail = 0;

	g_xsValidTimerIds[g_xsTimerIdTail] = id;
}

static void CALLBACK TimerProc(HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	xsTimerItem *item;

	item = &g_xsTimers[idEvent - 1];
//	KillTimer(g_hMainWnd, item->idEvent);

	item->cb(item->userdata);
//	xsReturnValidTimerId(idEvent);
}

xsU32 xsStartTimer(xsU32 elapseInMs, xsCallbackFunc cb, void *userdata)
{
	xsTimerItem *item;
	int id = xsGetValidTimerId();
	if (id == 0)
	{
		XS_ERROR("[PAL]No more timer slot");
		return 0; // Failed
	}

	item = &g_xsTimers[id - 1];
	item->cb = cb;
	item->userdata = userdata;
	item->idEvent = SetTimer(g_hMainWnd, id, elapseInMs, TimerProc);

	return id;
}

void xsStopTimer(xsU32 id)
{
	xsTimerItem *item;

	item = &g_xsTimers[id - 1];
	KillTimer(g_hMainWnd, item->idEvent);
	xsReturnValidTimerId(id);
}

static char g_xsTraceMsg[XS_CFG_MAX_TRACE_LEN] = {0};
void xsTrace(const char *format, ...)
{
	va_list args;
	xsTimeType tm;

	// add 14-chars date time
	xsGetLocalTime(&tm);
	_snprintf(g_xsTraceMsg, sizeof(g_xsTraceMsg), "%02d-%02d %02d:%02d:%02d",
		tm.mon + 1, tm.mday + 1, tm.hour, tm.min, tm.sec);
	va_start(args, format);
	vsnprintf(g_xsTraceMsg + 14, sizeof(g_xsTraceMsg) - 15, format, args);
	va_end(args);
	OutputDebugStringA(g_xsTraceMsg);
#ifdef _MSC_VER
	OutputDebugStringA("\n");
#endif
	fprintf(stdout, "%s\n", g_xsTraceMsg);
	fflush(stdout);
}

void xsAssert(const char *expression, const char *file, int line)
{
#ifdef _MSC_VER
#ifdef _DEBUG
	_CrtDbgReport(_CRT_ASSERT, file, line, NULL, expression);
#endif
	_CrtDbgBreak();
#else
	_assert(expression, file, line);
#endif
}

void xsOutputDebugInfo(const char *msg)
{
	OutputDebugStringA(msg);
	OutputDebugStringA("\n");
}

void xsBootUp(int appId)
{

}

int xsMediaOpen(xsMedia *media)
{
	return -1;
}

void xsMediaClose(xsMedia *media)
{
}

int xsMediaPlay(xsMedia *media)
{
	return -1;
}

void xsMediaSetLooping(xsMedia *media, xsBool looping)
{
}

void xsMediaSetVolume(xsMedia *media, int volume)
{
}

int xsMediaPause(xsMedia *media)
{
	return -1;
}

int xsMediaStop(xsMedia *media)
{
	return -1;
}

int xsSocketInitialize(void)
{
	WSADATA wsaData;
	int ret;

	ret = WSAStartup(MAKEWORD(1, 1), &wsaData);
	if (ret == 0)
		return XS_EC_OK;
	else
		return XS_EC_ERROR;
}

int xsSocketUninitialize(void)
{
	WSACleanup();
	return XS_EC_OK;
}

int xsSocketCreate(void)
{
	SOCKET sock;
	int ret;

	sock = socket(PF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
		return XS_EC_ERROR;

	ret = WSAAsyncSelect(sock, g_hMainWnd, WM_XS_SOCKET_MSG, FD_READ | FD_WRITE | FD_CONNECT | FD_CLOSE);
	if (ret != 0)
	{
		closesocket(sock);
		return XS_EC_ERROR;
	}

	return sock;
}

int xsSocketClose(int socket)
{
	return closesocket(socket);
}

int xsSocketConnect(int socket, const xsSockAddr *addr)
{
	struct sockaddr_in sockAddr = {0};
	int ret;

	sockAddr.sin_family = AF_INET;
	memcpy(&sockAddr.sin_addr, addr->addr.b, 4);
	sockAddr.sin_port = addr->port;

	ret = connect(socket, (struct sockaddr *)&sockAddr, sizeof(struct sockaddr_in));
	if (ret != 0)
	{
		int error = WSAGetLastError();
		if (error == WSAEWOULDBLOCK)
			return XS_EC_IN_PROGRESS;

		XS_ERROR("[PAL]xsSocketConnect: code:%d", error);
		return XS_EC_ERROR;
	}

	return ret;
}

int xsSocketShutdown(int socket, int how)
{
	return shutdown(socket, how);
}

int xsSocketSend(int socket, const void *buffer, int length)
{
	int ret;

	ret = send(socket, (const char *)buffer, length, 0);
	if (ret == SOCKET_ERROR)
	{
		int error = WSAGetLastError();
		if (error == WSAEWOULDBLOCK || error == WSAEINPROGRESS)
			return XS_EC_IN_PROGRESS;

		return XS_EC_ERROR;
	}

	return ret;
}

int xsSocketRecv(int socket, void *buffer, int length)
{
	int ret;

	ret = recv(socket, (char *)buffer, length, 0);
	if (ret == SOCKET_ERROR)
	{
		int error = WSAGetLastError();
		if (error == WSAEWOULDBLOCK || error == WSAEINPROGRESS)
			return XS_EC_IN_PROGRESS;

		return XS_EC_ERROR;
	}

	return ret;
}

int xsPalHandleSocketEvent(UINT message, WPARAM wParam, LPARAM lParam)
{
	DWORD dwError, dwEvent;
	xsSysEvent evt = {0};

	dwError = WSAGETSELECTERROR(lParam);
	dwEvent = WSAGETSELECTEVENT(lParam);

	evt.data.socket.sock = wParam;

	if (dwError == 0)
		evt.data.socket.status = XS_EC_OK;
	else
		evt.data.socket.status = XS_EC_ERROR;	// TODO: translate error code

	switch (dwEvent)
	{
	case FD_CONNECT:
		evt.type = XS_EVT_SOCK_CONNECT;
		break;
	case FD_READ:
		evt.type = XS_EVT_SOCK_READ;
		break;
	case FD_WRITE:
		evt.type = XS_EVT_SOCK_WRITE;
		break;
	case FD_CLOSE:
		evt.type = XS_EVT_SOCK_CLOSE;
		break;
	default:	// ignore other event
		return 0;
	}

	xsSysEventHandler(&evt);
	return 0;
}

typedef struct _xsResolveTag
{
	HANDLE handle;
	char *buf;
} xsResolveTag;

static xsResolveTag g_xsResolveTags[XS_CFG_MAX_RESOLVE_MSG] = {{0}};

int xsGetAddrByName(const char *hostname, xsSockAddr *addr, int tag)
{
	char *buf;
	UINT message = WM_XS_RESOLVE_MSG + tag;

	if (message > WM_XS_RESOLVE_MSG_END)
		return XS_EC_NOMEM;

	buf = (char *)xsMallocNative(MAXGETHOSTSTRUCT);
	if (buf == NULL)
		return XS_EC_NOMEM;

	g_xsResolveTags[tag].buf = buf;
	g_xsResolveTags[tag].handle = WSAAsyncGetHostByName(g_hMainWnd, message, hostname, buf, MAXGETHOSTSTRUCT);
	if (g_xsResolveTags[tag].handle == NULL)
	{
		int error = WSAGetLastError();
		XS_ERROR("[PAL]xsGetAddrByName: code:%d", error);
		return XS_EC_ERROR;
	}
	return XS_EC_IN_PROGRESS;
}

int xsPalHandleGetAddrByName(UINT message, WPARAM wParam, LPARAM lParam)
{
	int tagId = message - WM_XS_RESOLVE_MSG;
	DWORD dwError, dwBufLen;
	xsResolveTag *tag;
	struct hostent *he;
	xsSysEvent evt;

	dwError = WSAGETASYNCERROR(lParam);
	dwBufLen = WSAGETASYNCBUFLEN(lParam);

	tag = &g_xsResolveTags[tagId];
	he = (struct hostent *)tag->buf;

	if (dwError == 0)
	{
		memcpy(evt.data.resolve.addr.addr.b, he->h_addr_list[0], 4);
		evt.data.resolve.success = XS_TRUE;
	}
	else
	{
		evt.data.resolve.addr.addr.n = -1;
		evt.data.resolve.success = XS_FALSE;
	}

	xsFreeNative(tag->buf);

	evt.type = XS_EVT_RESOLVE;
	evt.data.resolve.tag = tagId;
	xsSysEventHandler(&evt);

 	return 0;
}
