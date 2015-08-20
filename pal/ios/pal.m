/*
 *  pal.m
 *  palios
 *
 *  Created on 11-7-26.
 *  Copyright 2011 XSKit.org. All rights reserved.
 *
 */

#import <xs/pal.h>

////////////////////////////////////////////////////////////////////////////////
// File system functions
////////////////////////////////////////////////////////////////////////////////

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

	
	return (xsUIntPtr)fopen(path, mode);
}

////////////////////////////////////////////////////////////////////////////////
// String functions
////////////////////////////////////////////////////////////////////////////////

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
// Memory management functions
////////////////////////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////////////////////////
// Debug functions
////////////////////////////////////////////////////////////////////////////////

void xsTrace(const char *format, ...)
{
	va_list args;
	
	va_start(args, format);
	NSString *msg = [[[NSString alloc] initWithFormat:[NSString stringWithUTF8String:format] arguments:args] autorelease];
	va_end(args);
	
	NSLog(@"%@", msg);
}

