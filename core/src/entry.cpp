#include <xs/pal.h>
#include <xs/launcher.h>
#include <xs/appcontext.h>
#include <xs/connmanager.h>

//extern void *xsUnitTestEventHandler(xsSysEvent *event);

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
		if (xsLauncher::init() == XS_EC_OK && xsConnManager::init() == XS_EC_OK)
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
