#include <xs/pal.h>
#include <xs/launcher.h>
#include <xs/appcontext.h>
#include <xs/memory.h>
#include <xs/container.h>
#include <xs/bon.h>
#include <xs/utils.h>
#include <xs/string.h>
#include <xs/uri.h>
#include <xs/path.h>

#define XS_CONTEXT_LIST_INIT	(2)

xsArrayList xsLauncher::_contextList = NULL;
xsAppContext *xsLauncher::_theContext = NULL;
xsBool xsLauncher::_xsBackgound = XS_FALSE;
xsRes xsLauncher::_resAppList = 0;

int xsLauncher::init(void)
{
	XS_TRACE("xsLauncher::Init");

	if (_contextList != NULL)
		return XS_EC_OK;

	_resAppList = xsResInvalidHandle();

	// prepare application list
	_contextList = xsArrayListCreate(XS_CONTEXT_LIST_INIT);
	if (_contextList == NULL)
		return XS_EC_ERROR;

	// create necessary directory
	xsTChar path[XS_MAX_PATH];
#ifdef XS_FEATURE_MULTIUSER
	xsCreateDir(xsGetSharedDataPath(path));
#endif
	xsCreateDir(xsGetUserDataPath(path));

	return XS_EC_OK;
}

void xsLauncher::uninit(void)
{
	xsAppContext *context;
	xsIterator iter = NULL;
	XS_TRACE("[APP]xsLauncher::Uninit");
	
	// destroy contexts.
	if (_contextList != NULL)
	{
		for (;;)
		{
			context = (xsAppContext *)xsArrayListIterate(_contextList, &iter);
			if (context == NULL || iter == NULL)
				break;

			context->exit();
			context->destroyInstance();
		}
		_theContext = NULL;

		xsArrayListDestroy(_contextList);
		_contextList = NULL;
	}

	if (xsResHandleValid(_resAppList))
		xsCloseRes(_resAppList);
	_resAppList = xsResInvalidHandle();
}

/**
 * Query installed application
 */
int xsLauncher::query(int index, xsAppInfo *appInfo)
{
	XS_ASSERT(appInfo != NULL);

	int cnt = count();
	if (cnt < 0)
	{
		XS_ERROR("Query installed count failed.");
		return XS_EC_ERROR;
	}

	if (index >= cnt)
	{
		XS_ERROR("Query installed application out of index.");
		return XS_EC_ERROR;
	}

	// read name, uri and icon from application list
	xsValue name, uri, icon;
	char path[XS_MAX_PATH];

	xsSnprintf(path, sizeof(path), "%d/name", index);
	if (xsBonReadValueFromRes(_resAppList, path, &name) != XS_EC_OK)
		return XS_EC_ERROR;

	xsSnprintf(path, sizeof(path), "%d/uri", index);
	if (xsBonReadValueFromRes(_resAppList, path, &uri) != XS_EC_OK)
		return XS_EC_ERROR;

	xsSnprintf(path, sizeof(path), "%d/icon", index);
	xsBonReadValueFromRes(_resAppList, path, &icon);	// icon is optional

	// feed to AppInfo
	xsTcsCpyN(appInfo->name, name.data.t, sizeof(appInfo->name) / sizeof(xsTChar));
	xsStrCpyN(appInfo->uri, uri.data.s, sizeof(appInfo->uri) / sizeof(char));

	xsMemSet(&appInfo->icon, 0, sizeof(appInfo->icon));
	appInfo->icon.srcType = XS_AFD_UNKNOWN;
	if (icon.type == XS_VALUE_STRING && icon.data.s != NULL)
	{// extract uri and feed AFD
		xsUri *uri = xsUriParse(icon.data.s);
		if (uri != NULL)
		{
			xsTChar iconName[XS_MAX_PATH];
#ifdef XS_UNICODE
			xsMbsToWcs(iconName, uri->base, XS_MAX_PATH - 1);
#else
			xsStrCpyN(iconName, uri->base, XS_MAX_PATH - 1);
#endif

			switch (uri->schema)
			{
			case XS_URI_RES:
				appInfo->icon.srcType = XS_AFD_RESOURCE;
				appInfo->icon.src.res = xsOpenRes(iconName);
				break;
			case XS_URI_FILE:
				appInfo->icon.srcType = XS_AFD_FILENAME;
				appInfo->icon.src.filename = xsTcsDup(iconName); // free at PAL. it's no problem for Launcher is always link with PAL statically.
				break;
			}
				
			xsUriDestroy(uri);
		}
	}

	xsValueDestroy(&name, XS_FALSE);
	xsValueDestroy(&uri, XS_FALSE);
	xsValueDestroy(&icon, XS_FALSE);

	return XS_EC_OK;
}

/**
 * Count installed applications
 */
int xsLauncher::count(void)
{
	if (!xsResHandleValid(_resAppList))
		_resAppList = xsOpenRes(xsT("_appList"));

	if (!xsResHandleValid(_resAppList))
	{
		XS_ERROR("Cannot open _appList resource.");
		return XS_EC_ERROR;
	}

	return xsBonCountItemInRes(_resAppList, NULL);
}

/**
 * Load application context and library if needed
 * @param appUri Application URI
 * @return loaded xsAppContext, or NULL if failed.
 */
xsAppContext *xsLauncher::load(const char *appUri)
{
	if (appUri == NULL || *appUri == '\0')
		appUri = "default"; // staging default

	XS_TRACE("[APP]Load application from '%s'", appUri);
	_xsBackgound = XS_FALSE;

	return xsAppContext::createInstance(appUri);
}

/**
 * Continue load application context
 * @param context created application context
 * @return loaded xsAppContext, or NULL if failed.
 */
xsAppContext *xsLauncher::load(xsAppContext *context)
{
	if (context != NULL)
	{
		if (context->getLoaded() == xsAppContext::LOAD_FAILED)
		{// kill load failed context
			context->destroyInstance();
			context = NULL;
		}
	}

	return context;
}

void xsLauncher::free(xsAppContext *context)
{
	if (context == NULL)
		context = _theContext;

	if (context == NULL)
	{
		XS_ERROR("[APP]Free NULL app.");
		return;
	}
		
	xsArrayListRemove(_contextList, context->getIndex());
	context->exit();
	context->destroyInstance();

	int count = xsArrayListSize(_contextList);
	if (count > 0)
	{// resume to last context
		_theContext = (xsAppContext *)(xsArrayListGet(_contextList, count - 1));
		if (_theContext != NULL)
			_theContext->resume(); // TODO: change diretory back to this context
		else
			XS_ERROR("[APP]Free NULL app.");
	}
	else
	{// There no other context, quit
		_theContext = NULL;
		xsQuit();
	}
}

xsBool xsLauncher::isBackgound(void)
{
	return _xsBackgound;
}

int xsLauncher::onStart(xsAppContext *context)
{
	if (context == NULL)
	{
		XS_ERROR("Run NULL application.");
		return XS_EC_ERROR;
	}

	if (context->getLoaded() != xsAppContext::LOAD_COMPLETE)
	{
		XS_WARN("Run unloaded application.");
		return XS_EC_IN_PROGRESS;
	}

	// run context
	if (_theContext != NULL)
		_theContext->suspend();

	_theContext = context;

	int index = xsArrayListAdd(_contextList, context);
	context->setIndex(index);

	// Execute AppMain
	return context->start();
}

void xsLauncher::onSuspend(void)
{
	xsAppContext *context = getCurrentContext();
	if (context == NULL)
		return;

	_xsBackgound = XS_TRUE;
	context->suspend();
}

int xsLauncher::onResume(void)
{
	xsAppContext *context = getCurrentContext();
	if (context == NULL)
		return XS_EC_ERROR;

	_xsBackgound = XS_FALSE;
	return context->resume();
}

xsAppContext *xsLauncher::getCurrentContext(void)
{
	return _theContext;
}
