#include <xs/pal.h>

#define XS_EVKIT_STORAGE		0
#define XS_EVKIT_STRING		0
#define XS_EVKIT_GRAPHICS		0
#define XS_EVKIT_MEDIA			0
#define XS_EVKIT_MEMORY		0
#define XS_EVKIT_NETWORK		0

#include "evaluate.c"

int main(int argc, char *argv[])
{
	return 0;
}

//////////////////////////////////////////////////////////////////////////////
// PAL interface functions
//////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Debug Utility Functions
///////////////////////////////////////////////////////////////////////////////

void xsTrace(const char *format, ...)
{
}

void xsAssert(const char *expression, const char *file, int line)
{
}
/*
void xsOutputDebugInfo(const char *msg)
{
}

*/


///////////////////////////////////////////////////////////////////////////////
// Memory functions
///////////////////////////////////////////////////////////////////////////////

void *xsMemSet(void *dest, int c, size_t count)
{
	return NULL;
}

void *xsMemCpy(void *dest, const void *src, size_t count)
{
	return NULL;
}

void *xsMemMove(void *dest, const void *src, size_t count)
{
	return NULL;
}

void *xsMallocNative(size_t size)
{
	return NULL;
}

void *xsCallocNative(size_t size)
{
	return NULL;
}

void *xsReAllocNative(void *pointer, size_t size)
{
	return NULL;
}

void xsFreeNative(void *pointer)
{
}


///////////////////////////////////////////////////////////////////////////////
// String Functions
///////////////////////////////////////////////////////////////////////////////


size_t xsStrLen(const char *string)
{
	return -1;
}

char *xsStrCpy(char *strTo, const char *strFrom)
{
	return NULL;
}

char *xsStrCpyN(char *strTo, const char *strFrom, size_t max)
{
	return NULL;
}

int xsStrCmp(const char *string1, const char *string2)
{
	return -1;
}

int xsStrCaseCmp(const char *string1, const char *string2)
{
	return -1;
}

int xsStrCmpN(const char *string1, const char *string2, int max)
{
	return -1;
}

int xsStrCaseCmpN(const char *string1, const char *string2, int max)
{
	return -1;
}

char *xsStrCat(char *strTo, const char *strFrom)
{
	return NULL;
}

char *xsStrCatN(char *strTo, const char *strFrom, int max)
{
	return NULL;
}

const char *xsStrChr(const char *string, int c)
{
	return NULL;
}

const char *xsStrStr(const char *string, const char *substring)
{
	return NULL;
}

int xsStrToInt(const char *string)
{
	return -1;
}

char *xsIntToStr(int value, char *buffer, int radix)
{
	return NULL;
}

char *xsStrLwr(char *string)
{
	return NULL;
}

char *xsStrUpr(char *string)
{
	return NULL;
}

int xsSnprintf(char *buffer, size_t size, const char *format, ...)
{
	return -1;
}

size_t xsWcsLen(const xsWChar *string)
{
	return (size_t)-1;
}

xsWChar *xsWcsCpy(xsWChar *strTo, const xsWChar *strFrom)
{
	return NULL;
}

xsWChar *xsWcsCpyN(xsWChar *strTo, const xsWChar *strFrom, size_t max)
{
	return NULL;
}

int xsWcsCmp(const xsWChar *string1, const xsWChar *string2)
{
	return NULL;
}

int xsWcsCaseCmp(const xsWChar *string1, const xsWChar *string2)
{
	return -1;
}

int xsWcsCmpN(const xsWChar *string1, const xsWChar *string2, int max)
{
	return -1;
}

int xsWcsCaseCmpN(const xsWChar *string1, const xsWChar *string2, int max)
{
	return -1;
}

xsWChar *xsWcsCat(xsWChar *strTo, const xsWChar *strFrom)
{
	return NULL;
}

xsWChar *xsWcsCatN(xsWChar *strTo, const xsWChar *strFrom, int max)
{
	return NULL;
}

xsWChar *xsWcsChr(const xsWChar *string, int c)
{
	return NULL;
}

xsWChar *xsWcsStr(const xsWChar *string, const xsWChar *substring)
{
	return NULL;
}

int xsWcsToInt(const xsWChar *string)
{
	return -1;
}

xsWChar *xsWcsLwr(xsWChar *string)
{
	return NULL;
}

xsWChar *xsWcsUpr(xsWChar *string)
{
	return NULL;
}

/*
int xsWSprintf(xsWChar *buffer, xsWChar *format, ...)
{
	return 0;
}
*/

size_t xsMbsToWcs(xsWChar *dest, const char *src, size_t count)
{
	return (size_t)-1;
}

size_t xsWcsToMbs(char *dest, const xsWChar *src, size_t count)
{
	return (size_t)-1;
}


///////////////////////////////////////////////////////////////////////////////
// File System Functions
///////////////////////////////////////////////////////////////////////////////

xsTChar *xsGetExternalStoragePath(xsTChar *buf)
{
	return NULL;
}

xsTChar *xsGetPublicStoragePath(xsTChar *buf)
{
	return NULL;
}

xsTChar *xsGetPrivateStoragePath(xsTChar *buf)
{
	return NULL;
}

xsTChar *xsGetLocalDataPath(xsTChar *buf)
{
	return NULL;
}

xsFile xsOpenFile(const xsTChar *filename, xsU32 style)
{
	return 0;
}

xsBool xsFileHandleValid(xsFile handle)
{
	return XS_FALSE;
}

int xsCloseFile(xsFile handle)
{
	return -1;
}

size_t xsReadFile(xsFile handle, void *buffer, size_t size)
{
	return (size_t)(-1);
}

size_t xsWriteFile(xsFile handle, const void *buffer, size_t size)
{
	return (size_t)(-1);
}

long xsSetFilePointer(xsFile handle, long offset, int origin)
{
	return -1;
}

long xsGetFilePointer(xsFile handle)
{
	return -1;
}

int xsFlashFile(xsFile handle)
{
	return -1;
}

size_t xsGetFileSize(xsFile handle)
{
	return -1;
}

xsBool xsFileExists(const xsTChar *path)
{
	return XS_FALSE;
}

int xsCreateDir(const xsTChar *path)
{
	return -1;
}

int xsRemoveDir(const xsTChar *path)
{
	return -1;
}

///////////////////////////////////////////////////////////////////////////////
// Resource Functions
///////////////////////////////////////////////////////////////////////////////

xsRes xsOpenRes(const xsTChar *name)
{
    return 0;
}

xsBool xsResHandleValid(xsRes handle)
{
    return XS_FALSE;
}

xsRes xsResInvalidHandle(void)
{
    return 0;
}

void xsCloseRes(xsRes handle)
{
}

size_t xsReadRes(xsRes handle, void *buffer, size_t begin, size_t size)
{
    return -1;
}


///////////////////////////////////////////////////////////////////////////////
// Screen Operation Functions
///////////////////////////////////////////////////////////////////////////////

xsBool xsSetScreenOrient(int orient)
{
    return XS_FALSE;
}

void xsLockScreen(void)
{
}

void xsUnlockScreen(void)
{
}

void xsFlushScreen(int left, int top, int right, int bottom)
{
}

xsGraphics *xsGetSystemGc(void)
{
    return NULL;
}

void *xsLockPixelBuffer(xsGraphics *gc, int pixelDepth)
{
    return NULL;
}

void xsUnlockPixelBuffer(xsGraphics *gc)
{
}

void xsGcTranslate(xsGraphics *gc, int xoffset, int yoffset)
{
}

xsBool xsGetScreenDimension(int *width, int *height)
{
    return XS_FALSE;
}

xsBool xsGetClientRect(xsRect *rect)
{
    return XS_FALSE;
}

void xsGetClipRect(xsGraphics *gc, xsRect *rect)
{
}

void xsSetClipRect(xsGraphics *gc, xsRect *rect)
{
}

void xsResetClipRect(xsGraphics *gc)
{
}

void xsDrawLine(xsGraphics *gc, float x1, float y1, float x2, float y2)
{
}

void xsDrawRectangle(xsGraphics *gc, float x, float y, float width, float height)
{
}

void xsFillRectangle(xsGraphics *gc, float x, float y, float width, float height)
{
}

void xsFillTriangle(xsGraphics *gc, float x1, float y1, float x2, float y2, float x3, float y3)
{
}

void xsDrawCircle(xsGraphics *gc, float x, float y, float r)
{
}

void xsFillCircle(xsGraphics *gc, float x, float y, float r)
{
}

void xsSetFont(xsGraphics *gc, xsFontType *font)
{
}

float xsGetFontHeight(xsGraphics *gc, xsFontType *font)
{
    return -1;
}

void xsDrawText(xsGraphics *gc, const xsTChar *text, int count, float x, float y)
{
}

void xsMeasureText(xsGraphics *gc, const xsTChar *text, int count, xsFontType *font, float *width, float *height)
{
}

int xsGetImageType(xsImage *img)
{
    return -1;
}

int xsGetImageDimension(xsImage *img, float *width, float *height)
{
    return -1;
}

void xsFreeImageObject(xsImage *img)
{
}

void xsDrawImage(xsGraphics *gc, xsImage *img, float x, float y)
{
}


///////////////////////////////////////////////////////////////////////////////
// UI Component Functions
///////////////////////////////////////////////////////////////////////////////

void xsInputDialog(const xsTChar *prompt, xsTChar *buffer, size_t len,
                   int type, xsCallbackFunc cb, void *userdata)
{
}

void xsInputCtrlShow(int x, int y, int width, int height,
                     const xsFontType *font, xsTChar *buffer, size_t len,
                     int type, xsCallbackFunc cbUpdate, void *userdata)
{
}

void xsInputCtrlHide(void)
{
}

xsBool xsGetCaretPos(int *x, int *y)
{
    return XS_FALSE;
}


///////////////////////////////////////////////////////////////////////////////
// Device Control Functions
///////////////////////////////////////////////////////////////////////////////

void xsSetBacklight(int mode)
{
}


///////////////////////////////////////////////////////////////////////////////
// System Interface Functions
///////////////////////////////////////////////////////////////////////////////

void xsQuit(void)
{
}

void xsSeed(int seed)
{
}

int xsRandom(void)
{
    return -1;
}

xsU32 xsTime(void)
{
    return -1;
}

xsTimeType *xsGetLocalTime(xsTimeType *tmt)
{
    return NULL;
}

xsU32 xsStartTimer(xsU32 elapseInMs, xsCallbackFunc cb, void *userdata)
{
    return -1;
}

void xsStopTimer(xsU32 id)
{
}

char *xsGetDeviceInfo(size_t *size)
{
    return NULL;
}


///////////////////////////////////////////////////////////////////////////////
// Media Functions
///////////////////////////////////////////////////////////////////////////////

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


///////////////////////////////////////////////////////////////////////////////
// Socket Functions
///////////////////////////////////////////////////////////////////////////////

int xsSocketInitialize(void)
{
    return -1;
}

int xsSocketUninitialize(void)
{
    return -1;
}

int xsSocketCreate(void)
{
    return -1;
}

int xsSocketClose(int socket)
{
    return -1;
}

int xsSocketConnect(int socket, const xsSockAddr *addr)
{
    return -1;
}

int xsSocketShutdown(int socket, int how)
{
    return -1;
}

int xsSocketSend(int socket, const void *buffer, int length)
{
    return -1;
}

int xsSocketRecv(int socket, void *buffer, int length)
{
    return -1;
}


int xsGetAddrByName(const char *hostname, xsSockAddr *addr, int tag)
{
    return -1;
}
