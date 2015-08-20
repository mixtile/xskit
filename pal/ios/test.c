/*
 *  test.c
 *  palios
 *
 *  Created by Martin on 11-7-26.
 *  Copyright 2011 XSKit.org. All rights reserved.
 *
 */

#include <xs/pal.h>

#define XS_EVKIT_EVALUATE


#define XS_EVKIT_STORAGE		0
#define XS_EVKIT_STRING			0
#define XS_EVKIT_GRAPHICS		0
#define XS_EVKIT_MEDIA			0
#define XS_EVKIT_MEMORY			0
#define XS_EVKIT_NETWORK		0

#ifdef XS_EVKIT_EVALUATE

#include "../common/evkit/evaluate.c"

#endif
