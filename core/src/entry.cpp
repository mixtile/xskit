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

#include "xs/jsapplication.h"
#include "demoapp.h"

#include <xs/entry.h>
#include <xs/core.h>

void *xsCreateApplication()
{
#ifndef  XS_DYNAMIC_APP
	return new DemoApp();
#else
	return new xsJSApplication();
#endif
}

void xsDestroyApplication(void *app)
{
	((xsObject *)app)->destroyInstance();
}

int xsInvokeApplication(void *app, xsEvent *e)
{
	return ((xsObject *)app)->processEvent(e);
}

int xsLibInit()
{
	return xsCoreInit();
}

void xsLibUninit()
{
	xsCoreUninit();
}
