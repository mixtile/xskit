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

//#define XS_EVKIT_EVALUATE		1

#define XS_EVKIT_STORAGE		1
#define XS_EVKIT_STRING		0
#define XS_EVKIT_GRAPHICS		1
#define XS_EVKIT_MEDIA		0
#define XS_EVKIT_MEMORY		0
#define XS_EVKIT_NETWORK		0

#ifdef XS_EVKIT_EVALUATE

#include "../../common/evkit/evaluate.c"

#endif
