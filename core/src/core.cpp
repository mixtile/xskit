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
#include <xs/launcher.h>
#include <xs/appcontext.h>
#include <xs/connmanager.h>

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
#else
int xsSysEventHandler(xsSysEvent *event)
{
	xsAppContext *app;

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

			app = xsLauncher::getCurrentContext();
			if (app != NULL)
				app->processEvent(&evt);
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

			app = xsLauncher::getCurrentContext();
			if (app != NULL)
				app->processEvent(&evt);
		}
		break;
	case XS_EVT_RESOLVE:
	case XS_EVT_SOCK_CONNECT:
	case XS_EVT_SOCK_READ:
	case XS_EVT_SOCK_WRITE:
	case XS_EVT_SOCK_CLOSE:
		{
			xsEvent evt;

			evt.type = event->type;
			evt.sys = event;

			return xsConnManager::processEvent(&evt);
		}
	case XS_EVT_APP_QUERY:
		return xsLauncher::query(event->data.common.param, (xsAppInfo *)event->data.common.ptr);
	case XS_EVT_APP_COUNT:
		event->data.common.param = xsLauncher::count();
		break;
	case XS_EVT_LOAD:
		if(xsCoreInit() == XS_EC_OK && xsLauncher::init() == XS_EC_OK && xsConnManager::init() == XS_EC_OK)
		{
			xsAppContext *context = (xsAppContext *)event->data.app.instance;
			if (context == NULL)
				context = xsLauncher::load(event->data.app.uri);
			else
				context = xsLauncher::load(context);

			event->data.app.instance = context; // return context to PAL

			if (context != NULL)
			{
				event->data.app.orient = context->getOrient();
				event->data.app.name = context->getName();
				event->data.app.loaded = context->getLoaded();
			}
			else
			{
				XS_ERROR("XS_EVT_LOAD failed.");
				return XS_EC_ERROR;
			}
		}
		else
		{
			XS_ERROR("xsLauncher::init failed.");
			return XS_EC_ERROR;
		}
		break;
	case XS_EVT_START:
		return xsLauncher::onStart((xsAppContext *)(event->data.app.instance));
	case XS_EVT_SUSPEND:
		xsLauncher::onSuspend();
		break;
	case XS_EVT_RESUME:
		return xsLauncher::onResume();
	case XS_EVT_EXIT:
		xsLauncher::uninit();
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
