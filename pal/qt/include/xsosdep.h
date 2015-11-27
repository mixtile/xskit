#ifndef _XS_OSDEP_H_
#define _XS_OSDEP_H_

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uintptr_t xsUIntPtr;

typedef unsigned short xsWChar;

typedef FILE * xsFile;

#ifndef XS_SIZE_T
#define XS_SIZE_T
typedef unsigned long size_t;
#endif

#define XS_INTERFACE
#define XS_EXPORT
#define XS_UTF8

#define XS_PATH_DELIMITER       '/'

#define XS_TIMERS_MAX		32

#ifdef __cplusplus
}
#endif



#endif /* _XS_OSDEP_H_ */
