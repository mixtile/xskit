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

#ifndef _XS_ENTRY_H_
#define _XS_ENTRY_H_

#include <xs/event.h>

#ifdef __cplusplus
extern "C" {
#endif

//#ifdef XS_DYNAMIC_LAUNCHER

typedef int (*xsLibInitFunc)();
typedef void (*xsLibUninitFunc)();
typedef void *(*xsAppCreatorFunc)();
typedef void (*xsAppDestroyerFunc)(void *);
typedef int (*xsAppInvokerFunc)(void *, xsEvent *);

//#else

// library init and uninit
XS_EXPORT int xsLibInit();
XS_EXPORT void xsLibUninit();

// creator and destroyer
XS_EXPORT void *xsCreateApplication();
XS_EXPORT void xsDestroyApplication(void *app);
XS_EXPORT int xsInvokeApplication(void *app, xsEvent *e);

//#endif

#ifdef __cplusplus
}
#endif

#endif /* _XS_ENTRY_H_ */
