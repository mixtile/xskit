//
//  pal.m
//  paldarwin
//
//  Copyright (c) 2015 XSKit.org. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <xs/pal.h>

////////////////////////////////////////////////////////////////////////////////
// Debug functions
////////////////////////////////////////////////////////////////////////////////

void xsTrace(const char *format, ...)
{
    va_list args;
    
    va_start(args, format);
    NSString *msg = [[NSString alloc] initWithFormat:[NSString stringWithUTF8String:format] arguments:args];
    va_end(args);
    
    NSLog(@"%@", msg);
}

void xsAssert(const char *expression, const char *file, int line)
{
    __assert(expression, file, line);
}

////////////////////////////////////////////////////////////////////////////////
// Memory management functions
////////////////////////////////////////////////////////////////////////////////

void *xsMemSet(void *dest, int c, size_t count)
{
    return memset(dest, c, count);
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

///////////////////////////////////////////////////////////////////////////////
// String Functions
///////////////////////////////////////////////////////////////////////////////

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
    return strcasecmp(string1, string2);
}

int xsStrCmpN(const char *string1, const char *string2, int max)
{
    return strncmp(string1, string2, max);
}

int xsStrCaseCmpN(const char *string1, const char *string2, int max)
{
    return strncasecmp(string1, string2, max);
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
    static const char *digit = "0123456789abcdef";
    char buf[22] =
    {0};
    
    int i = 20;
    for (; value && i; i--)
    {
        buf[i] = digit[value % radix];
        value /= radix;
    }
    
    // copy to buffer
    strcpy(buffer, &buf[i + 1]);
    return buffer;
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

////////////////////////////////////////////////////////////////////////////////
// File system functions
////////////////////////////////////////////////////////////////////////////////

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
    /*
     #include <unistd.h>
     #include <sys/types.h>
     #include <pwd.h>
     
     struct passwd *pw = getpwuid(getuid());
     
     const char *homedir = pw->pw_dir;
     */
    return NULL;
}

int xsPathDelimiter(void)
{
    return '/';
}

xsFile xsOpenFile(const xsTChar *filename, xsU32 style)
{
    char path[XS_MAX_PATH];
    char mode[4] = {0};
    
    if (wcstombs(path, (wchar_t *)filename, XS_MAX_PATH) == -1)
        return 0;
    
    if (style & XS_OF_READONLY)
        strcpy(mode, "r");
    if (style & XS_OF_READWRITE)
    {
        if (style & XS_OF_CREATE)
        {
            if (style & XS_OF_EXCLUSIVE)
                strcpy(mode, "r+");
            else
                strcpy(mode, "w+");
        }
        else
        {
            strcpy(mode, "r+");
        }
    }
    else if (style & XS_OF_WRITEONLY)
    {
        if ((style & XS_OF_CREATE) && !(style & XS_OF_EXCLUSIVE))
            strcpy(mode, "w");
        else
            return 0; // cannot support
    }
    else if (style & XS_OF_READONLY)
    {
        strcpy(mode, "r");
    }
    else
    {
        return 0; // cannot support
    }
    
    
    return fopen(path, mode);
}


xsBool xsFileHandleValid(xsFile handle)
{
    return (handle != (xsFile)0);
}

xsFile xsFileInvalidHandle()
{
    return (xsFile)0;
}

int xsCloseFile(xsFile handle)
{
    return fclose(handle);
}

size_t xsReadFile(xsFile handle, void *buffer, size_t size)
{
    return fread(buffer, 1, size, handle);
}

size_t xsWriteFile(xsFile handle, const void *buffer, size_t size)
{
    return fwrite(buffer, 1, size, handle);
}

long xsSetFilePointer(xsFile handle, long offset, int origin)
{
    int whence;
    
    if (origin == XS_FILE_BEGIN)
        whence = SEEK_SET;
    else if (origin == XS_FILE_CURRENT)
        whence = SEEK_CUR;
    else if (origin == XS_FILE_END)
        whence = SEEK_END;
    else
        return XS_EC_ERROR;
    
    return fseek(handle, offset, whence);
}

long xsGetFilePointer(xsFile handle)
{
    return ftell(handle);
}

int xsFlushFile(xsFile handle)
{
    return fflush(handle);
}

size_t xsGetFileSize(xsFile handle)
{
    long pos = ftell(handle);
    
    // goto end and get size
    fseek(handle, 0, SEEK_END);
    size_t size = ftell(handle);
    
    // back to stored position
    fseek(handle, pos, SEEK_SET);
    
    return size;
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
// Adopt Core Graphics Layer
///////////////////////////////////////////////////////////////////////////////
extern int g_nScreenWidth;
extern int g_nScreenHeight;

static xsGraphics g_sysGc = {0};
static float g_fontSizes[XS_FONT_SIZE_COUNT] = {6, 9, 12, 16, 20, 24, 36};
static xsU16 g_screenLock = 0;

int PalGiInit()
{
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    CGContextRef context = CGBitmapContextCreate(NULL, g_nScreenWidth, g_nScreenHeight, 8,
                                                 g_nScreenWidth * 4, colorSpace,
                                                 kCGBitmapAlphaInfoMask & kCGImageAlphaPremultipliedLast);
    CGColorSpaceRelease(colorSpace);
    
    CGContextTranslateCTM(context, 0, g_nScreenHeight);
    CGContextScaleCTM(context, 1.0, -1.0);
    g_sysGc.surface = context;
    
    return 0;
}

void PalGiUninit(void)
{
    g_sysGc.surface = NULL;
}

xsBool xsSetScreenOrient(int orient)
{
    return XS_FALSE;
}

void xsLockScreen(void)
{
    g_screenLock++;
}

void xsUnlockScreen(void)
{
    g_screenLock--;
}

void xsFlushScreen(int left, int top, int right, int bottom)
{
    // check screen lock
    if (g_screenLock > 0)
        return;
    
    CGRect rect;
    rect.origin.x = left;
    rect.origin.y = top;
    rect.size.width = right - left + 1;
    rect.size.height = bottom - top + 1;
    
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

