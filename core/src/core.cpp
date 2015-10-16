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

#include <xs/entry.h>
#include <xs/coreapplication.h>
#include <xs/path.h>
#include <xs/core.h>

extern int xsObjInit(void);
extern void xsObjUninit(void);
int xsCoreInit()
{
	if (xsObjInit() != XS_EC_OK)
		return XS_EC_ERROR;

	return XS_EC_OK;
}

void xsCoreUninit()
{
	xsObjUninit();
}

#ifndef XS_DYNAMIC_APP
static void *theApp = NULL;

int xsSysEventHandler(xsSysEvent *event)
{
	XS_TRACE("xsSysEventHandler: type:%d", event->type);

	switch (event->type)
	{
	case XS_EVT_MOUSE_DOWN:
	case XS_EVT_MOUSE_UP:
	case XS_EVT_MOUSE_MOVE:
		{
			xsMouseEvent evt;
			evt.sys = event;
			evt.type = event->type;
			evt.x = event->data.mouse.x;
			evt.y = event->data.mouse.y;
			evt.button = event->data.mouse.button;

			if (theApp != NULL)
				xsInvokeApplication(theApp, &evt);
		}
		break;
	case XS_EVT_KEY_DOWN:
	case XS_EVT_KEY_UP:
	case XS_EVT_KEY_HOLD:
		{
			xsKeyEvent evt;
			evt.sys = event;
			evt.type = event->type;
			evt.keyCode = event->data.key.keyCode;

			xsInvokeApplication(theApp, &evt);
		}
		break;
	case XS_EVT_RESOLVE:
	case XS_EVT_SOCK_CONNECT:
	case XS_EVT_SOCK_READ:
	case XS_EVT_SOCK_WRITE:
	case XS_EVT_SOCK_CLOSE:
		break;
	case XS_EVT_LOAD:
		XS_TRACE("Create application object");
		theApp = xsCreateApplication();
		if (theApp == NULL)
		{
			XS_WARN("Application create NULL object. Try to continue.");
			return XS_EC_ERROR;
		}
		break;
	case XS_EVT_START:
		{
			xsCommonEvent evt;
			evt.sys = event;
			evt.type = event->type;
			xsInvokeApplication(theApp, &evt);
		}
		break;
	case XS_EVT_SUSPEND:
		break;
	case XS_EVT_RESUME:
		break;
	case XS_EVT_EXIT:
		if (theApp != NULL)
			xsDestroyApplication(theApp);
		break;
	}

	return XS_EC_OK;
}
#endif // XS_DYNAMIC_APP

#ifdef XS_CORE_STANDALONE
xsListener *xsCreateApplication()
{
	return xsCoreApplication::createInstance();
}

int xsDestroyApplication(xsListener *app)
{
	static_cast<xsObject *>(app)->destroyInstance();
	return XS_EC_OK;
}

int xsLibInit()
{
	return xsCoreInit();
}

void xsLibUninit()
{
	xsCoreUninit();
}
#endif /* XS_CORE_STANDALONE */
