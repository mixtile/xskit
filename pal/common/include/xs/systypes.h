#ifndef _XS_SYSTYPES_H_
#define _XS_SYSTYPES_H_

#include "xsosdep.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef signed int xsS32;
typedef unsigned int xsU32;
typedef signed short xsS16;
typedef unsigned short xsU16;
typedef signed char xsS8;
typedef unsigned char xsU8;

typedef xsUIntPtr xsRes;

#ifndef NULL
#define NULL	0
#endif

typedef char xsBool;
#define XS_TRUE				(1)
#define XS_FALSE			(0)
typedef void *xsAny;

#if defined(UNICODE) || defined(_UNICODE)
#define XS_UNICODE
#endif

#ifdef XS_UNICODE
typedef xsWChar		xsTChar;
#define xsT(x)		(xsWChar *)L##x
#define xsTC(x)		(xsWChar)L##x
#else
typedef char xsTChar;
#define xsT(x)		x
#define xsTC(x)		x
#endif

typedef struct _xsAbstractFileDescriptor
{
	int fileType;
	int srcType;
	union
	{
		xsTChar *filename;
		xsRes res;
		xsFile fd;
		void *ptr;
	} src;
	size_t offset;	// data beginning offset in file/resource
	size_t length;	// data length in bytes
	void *object;	// system awared data object
} xsAFD;

typedef enum _xsAbstractFileDescriptorSourceType
{
	XS_AFD_UNKNOWN	= 0,
	XS_AFD_FILENAME	= 1,	// filename
	XS_AFD_RESOURCE	= 2,	// system resource handle
	XS_AFD_FILE		= 3,	// xskit resource file handle
	XS_AFD_MEMORY	= 4		// date store in memory, address store in src.ptr
} xsAFDSrcType;

typedef enum _xsAbstractFileDescriptorFileType
{
	XS_AFD_FILETYPE_UNKNOWN	= 0
} xsAFDFileType;

typedef struct _xsAppInfo
{
	char uri[260];			// Application URI
	xsTChar name[32];		// Name
	xsAFD icon;				// Icon
} xsAppInfo;

typedef struct _xsTimeType
{
    int sec;         // seconds
    int min;         // minutes
    int hour;        // hours
    int mday;        // day of the month
    int mon;         // month
    int year;        // year
    int wday;        // day of the week
    int yday;        // day in the year
    int isdst;       // daylight saving time
} xsTimeType;

typedef struct _xsSockAddr
{
	xsS16 family;	// Undefined
	xsU16 port;
	union
	{
		xsU8 b[4];
		xsU32 n;
	} addr;
} xsSockAddr;

#ifdef __cplusplus
}
#endif

#endif /* _XS_SYSTYPES_H_ */
