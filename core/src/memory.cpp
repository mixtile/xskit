/*
 * Copyright (C) 2015 Focalcrest, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <xs/pal.h>
#include <xs/memory.h>
#include <xs/utils.h>

void *xsMemDup(const void *pointer, size_t size)
{
	void *ret;
	
	if (pointer == NULL || size == 0)
		return NULL;
		
	ret = xsMalloc(size);
	if (ret == NULL)
		return NULL;

	xsMemCpy(ret, pointer, size);

	return ret;

}

#ifdef XS_MEM_DEBUG

typedef struct _xsMemDebugInfoItem
{// 44bytes per item
	void *ptr;		// NULL is available
	size_t size;
	char file[31];	// store filename's last 30 chars
	int line;
} xsMemDebugInfoItem;

#define XS_MEM_DEBUG_ITEM_MAX		20480
static xsMemDebugInfoItem g_xsMemDebugInfo[XS_MEM_DEBUG_ITEM_MAX] = {0};	// XS_MEM_DEBUG_ITEM_MAX * 44bytes
static size_t g_xsMemAllocSize = 0;
static size_t g_xsMemAllocPeak = 0;

static void AddMemInfoItem(void *ptr, size_t size, const char *file, int line);
static xsBool RemoveMemInfoItem(void *pointer);

/**
 * 分配内存，并记录分配调用发生的代码行
 */
void *xsMallocDebug(size_t size, const char *file, int line)
{
	void *p = xsMallocNative(size);

	if (p == NULL)
	{
		xsTrace("Malloc failed. size: %d, file: %s, line: %d", size, file, line);
		xsAssert("Malloc failed", file, line);
	}

	AddMemInfoItem(p, size, file, line);

	return p;
}


/**
 * 分配内存并清零，同时记录分配调用发生的地址
 */
void *xsCallocDebug(size_t size, const char *file, int line)
{
	void *p = xsCallocNative(size);

	if (p == NULL)
	{
		xsTrace("Calloc failed. size: %d, file: %s, line: %d", size, file, line);
		xsAssert("Calloc failed", file, line);
	}

	AddMemInfoItem(p, size, file, line);

	return p;
}

/**
 * 重内存并清除原分配地址，记录新分配调用发生的地址
 */
void *xsReAllocDebug(void *pointer, size_t size, const char *file, int line)
{
	void *p = xsReAllocNative(pointer, size);

	if (p == NULL)
	{
		xsTrace("Calloc failed. size: %d, file: %s, line: %d", size, file, line);
		xsAssert("Calloc failed", file, line);
	}

	AddMemInfoItem(p, size, file, line);

	return p;
}

void xsFreeDebug(void *pointer, const char *file, int line)
{
	if (RemoveMemInfoItem(pointer))
		return;

	xsAssert("Free memory twice occurred", file, line);
}

static void AddMemInfoItem(void *ptr, size_t size, const char *file, int line)
{
	int i;

	for (i = 0; i < XS_MEM_DEBUG_ITEM_MAX; i++)
	{
		if (g_xsMemDebugInfo[i].ptr == NULL || g_xsMemDebugInfo[i].ptr == ptr)
		{
			size_t len = xsStrLen(file);

			g_xsMemAllocSize += size;
			g_xsMemAllocPeak = XS_MAX(g_xsMemAllocPeak, g_xsMemAllocSize);

			g_xsMemDebugInfo[i].ptr = ptr;
			g_xsMemDebugInfo[i].size = size;

			if (len > sizeof(g_xsMemDebugInfo[i].file) - 1)
				file = file + (len - sizeof(g_xsMemDebugInfo[i].file)) + 1;
			
			xsStrCpy(g_xsMemDebugInfo[i].file, file);
			g_xsMemDebugInfo[i].line = line;

			return;
		}
	}

	xsAssert("Memory debug info FULL", file, line);
}

static xsBool RemoveMemInfoItem(void *pointer)
{
	int i;

	for (i = 0; i < XS_MEM_DEBUG_ITEM_MAX; i++)
	{
		if (g_xsMemDebugInfo[i].ptr == pointer)
		{
			g_xsMemAllocSize -= g_xsMemDebugInfo[i].size;

			g_xsMemDebugInfo[i].ptr = NULL;
			g_xsMemDebugInfo[i].size = 0;
			xsMemSet(g_xsMemDebugInfo[i].file, 0, sizeof(g_xsMemDebugInfo[i].file));

			xsFreeNative(pointer);
			return XS_TRUE;
		}
	}

	return XS_FALSE;
}

#endif
