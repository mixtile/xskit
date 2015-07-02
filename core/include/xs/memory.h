#ifndef _XS_MEMORY_H_
#define _XS_MEMORY_H_

#include <xs/pal.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * 分配内存，并记录分配调用发生的代码行
 *
 */
void *xsMallocDebug(size_t size, const char *file, int line);


/** 分配内存并清零，同时记录分配调用发生的地址
 *
 */
void *xsCallocDebug(size_t size, const char *file, int line);

/**
 * 重新分配内存
 */
void *xsReAllocDebug(void *pointer, size_t size, const char *file, int line);

void xsFreeDebug(void *pointer, const char *file, int line);

/**
 * 复制内存
 */
void *xsMemDup(const void *pointer, size_t size);

#ifndef XS_MEM_DEBUG

#define xsMalloc(size)				xsMallocNative(size)
#define xsCalloc(size)				xsCallocNative(size)
#define xsReAlloc(pointer, size)	xsReAllocNative(pointer, size)
#define xsFree(pointer)				xsFreeNative(pointer)

#else

#define xsMalloc(size)				xsMallocDebug(size, __FILE__, __LINE__)
#define xsCalloc(size)				xsCallocDebug(size, __FILE__, __LINE__)
#define xsReAlloc(pointer, size)	xsReAllocDebug(pointer, size, __FILE__, __LINE__)
#define xsFree(pointer)				xsFreeDebug(pointer, __FILE__, __LINE__)

#endif

#ifdef __cplusplus
}
#endif

#endif /* _XS_MEMORY_H_ */
