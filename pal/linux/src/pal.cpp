#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <stdarg.h>
#include <assert.h>
#include <syslog.h>

#include <xs/pal.h>

//////////////////////////////////////////////////////////////////////////////
// Debug functions
//////////////////////////////////////////////////////////////////////////////
void xsTrace(const char *format, ...)
{
	static xsBool opened = XS_FALSE;

	if (!opened)
	{
		openlog("PAL", LOG_PID, 0);
	}

	va_list args;

	va_start(args, format);
	vsyslog(LOG_DEBUG, format, args);
	va_end(args);
}

void xsAssert(const char *expression, const char *file, int line)
{
	__assert(expression, file, line);
}

///////////////////////////////////////////////////////////////////////////////
// Memory functions
///////////////////////////////////////////////////////////////////////////////

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

xsFile xsOpenFile(const char *filename, xsU32 style)
{
	char mode[8] =	{0};

	if ((style & (XS_OF_READWRITE | XS_OF_CREATE)) == (XS_OF_READWRITE
		| XS_OF_CREATE))
		xsStrCpy(mode, "wb+");
	else if (style & XS_OF_READWRITE)
		xsStrCpy(mode, "rb+");
	else if (style & XS_OF_WRITEONLY)
		xsStrCpy(mode, "wb");
	else
		xsStrCpy(mode, "rb");

	return (xsFile)fopen(filename, mode);
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


/*为了编译通过，复制win32下的pal.c的部分函数*/
xsU32 xsStartTimer(xsU32 elapseInMs, xsCallbackFunc cb, void *userdata)
{}

void xsStopTimer(xsU32 id)
{}

void xsQuit(void)
{}

size_t xsWcsLen(const xsWChar *string)
{}

xsWChar *xsWcsCpy(xsWChar *strTo, const xsWChar *strFrom)
{}

xsWChar *xsWcsCpyN(xsWChar *strTo, const xsWChar *strFrom, size_t max)
{}
xsBool xsResHandleValid(xsRes handle)
{}
xsRes xsResInvalidHandle()
{}

xsRes xsOpenRes(const xsTChar *name)
{}

void xsCloseRes(xsRes handle)
{}

size_t xsReadRes(xsRes handle, void *buffer, size_t begin, size_t size)
{}

int xsGetAddrByName(const char *hostname, xsSockAddr *addr, int tag)
{}
