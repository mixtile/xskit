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

#ifndef _XS_OSDEP_H_
#define _XS_OSDEP_H_

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

#include <gdk/gdk.h>

typedef uintptr_t xsUIntPtr;

typedef unsigned short xsWChar;

typedef FILE * xsFile;

//typedef cairo_pattern_t xsGradientPattern;

int PalGiInit(GdkWindow *window);
void PalGiUninit(void);

// for GDK and Cairo
#define XS_CAIRO(g)	((cairo_t *)g->device)
#define XS_SURFACE(g)	((cairo_surface_t *)g->surface)

#ifdef __cplusplus
extern "C" {
#define XS_EXPORT
#define XS_PATH_DELIMITER		'\\'
#define XS_UTF8
}
#endif

#define XS_INTERFACE
#define XS_TIMERS_MAX		32

#endif /* _XS_OSDEP_H_ */
