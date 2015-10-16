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
#include <xs/coreapplication.h>
#include <xs/remote.h>
#include <xs/object.h>
#include <xs/memory.h>
#include <xs/container.h>
#include <xs/bon.h>
#include <xs/utils.h>
#include <xs/string.h>

#define XS_DEFAULT_FORM_STACK_NAME		"__default"

xsCoreApplication::xsCoreApplication()
{
	uri = NULL;

	vars = NULL;		// runtime var store
	_res = NULL;
	_index = -1;			// index in application list

//	xsArrayListAdd(g_appList, this);
}

xsCoreApplication::~xsCoreApplication()
{
	if (vars != NULL)
	{
		xsIterator iter = NULL;
		xsValue *val;

		for (;;)
		{
			val = (xsValue *)xsHashMapIterate(vars, &iter);
			if (val == NULL || iter == NULL)
				break;

			xsValueDestroy(val, XS_TRUE);
		}

		xsHashMapDestroy(vars);
	}

//	xsArrayListRemove(g_appList, xsArrayListIndex(g_appList, this));
}

int xsCoreApplication::processEvent(xsEvent *e)
{
	XS_TRACE("xsCoreApplication::processEvent: %d", e->type);
	switch (e->type)
	{
	case XS_EVT_LOAD:
		return load(e);
	case XS_EVT_START:
		return start();
	case XS_EVT_SUSPEND:
		suspend();
		break;
	case XS_EVT_RESUME:
		return resume();
	case XS_EVT_EXIT:
// FIXME		xsScriptContextUninit(this);
//XXYY		xsLauncher::freeApp(this);
		break;
/*	case XS_EVT_MOUSE_DOWN:
	case XS_EVT_MOUSE_UP:
	case XS_EVT_MOUSE_MOVE:
		onMouse((xsMouseEvent *)e);
		break;
*/	}

	return XS_EC_OK;
}

xsBool xsCoreApplication::launchApp(const char *appUri)
{
	// TODO: create loadModule function instead
/*	xsCoreApplication *app = xsLauncher::loadApp(appUri);
	if (app != NULL)
	{
		// set orient
		xsSetScreenOrient(app->getOrient());
		if (xsLauncher::startApp(app) == XS_EC_OK)
			return XS_TRUE;
	}
*/	
	return XS_FALSE;
}

void xsCoreApplication::trace(const char *info)
{
	xsTrace("[Script::Trace] %s", info);
}

int xsCoreApplication::load(xsEvent *e)
{
	_context = (xsRuntimeContext *)e->sys->data.app.instance;
	if (_context == NULL)
	{
		XS_ERROR("Load event miss sys.data.app.instance");
		return XS_EC_ERROR;
	}

	_res = xsResource::createInstance(this);
	if (_res == NULL)
		return XS_EC_ERROR;
	
	xsManifest *manifest = NULL;

	xsValue value = _res->loadValue("app");
	if (value.type == XS_VALUE_OBJECT && value.data.obj->isTypeOf(XS_CLASS(xsManifest)))
		manifest = static_cast<xsManifest *>(value.data.obj);
	else if (value.type != XS_VALUE_NONE)
		xsValueDestroy(&value, XS_FALSE);

	if (manifest == NULL)
		return XS_EC_ERROR;

	_manifest = manifest;

	return XS_EC_OK;
}

int xsCoreApplication::start()
{
/*	xsValue result = {XS_VALUE_NONE};
	if (xsScriptContextInit(this) == XS_EC_OK)
	{
		if (xsScriptExecute(this, "main.js", &result) == XS_EC_OK)
			return xsScriptCall(this, "AppMain", 0, NULL, &result);
	}
	*/
	return XS_EC_ERROR;
}

void xsCoreApplication::suspend()
{

}

int xsCoreApplication::resume()
{
//XXYY	xsSetScreenOrient(_orient);

	return XS_TRUE;
}

void xsCoreApplication::exit()
{
	xsEvent evt;
		
	evt.type = XS_EVT_EXIT;
	xsEvent::post(this, &evt);
}

xsValue *xsCoreApplication::getVar(const char *var)
{
	xsU32 hash;
	if (vars == NULL)
		return NULL;

	hash = xsObject::hashName(var, 0);
	return (xsValue *)xsHashMapGet(vars, hash);
}

xsBool xsCoreApplication::setVar(const char *var, xsValue *value)
{
	xsU32 hash;
	xsValue *oldVal;
	if (var == NULL || value == NULL)
		return XS_FALSE;

	if (vars == NULL)
		vars = xsHashMapCreate(2);
	if (vars == NULL)
		return XS_FALSE;

	hash = xsObject::hashName(var, 0);
	oldVal = (xsValue *)xsHashMapPut(vars, hash, xsValueDuplicate(value));
	if (oldVal != NULL)
		xsValueDestroy(oldVal, XS_TRUE);

	return XS_TRUE;
}

xsBool xsCoreApplication::hasVar(const char *var)
{
	xsU32 hash;
	if (var == NULL || vars == NULL)
		return XS_FALSE;

	hash = xsObject::hashName(var, 0);
	return xsHashMapHas(vars, hash);
}

void xsCoreApplication::removeVar(const char *var)
{
	xsU32 hash;
	if (var == NULL || vars == NULL)
		return;

	hash = xsObject::hashName(var, 0);
	xsHashMapRemove(vars, hash);
}

xsRemote *xsCoreApplication::invoke(const char *uri, const char *cmdCallback)
{
	xsRemote *remote = (xsRemote *)xsRemote::createInstance();

	if (remote == NULL)
		return NULL;

	if (!remote->invoke(uri, this, cmdCallback))
		remote->destroyInstance();

	return remote;
}

int xsCoreApplication::__onTimerCallback(void *param)
{
	xsCallbackData *cbData = (xsCallbackData *)param;
//	xsValue val;

//	xsScriptEvaluate(cbData->app, cbData->cmd, &val);
	xsFree(cbData->cmd);
	xsFree(cbData);

	return 0;
}

int xsCoreApplication::setTimeout(const char *functionName, int millisecond)
{
	xsCallbackData *cbData = (xsCallbackData *)xsMalloc(sizeof(xsCallbackData));

	if (cbData != NULL)
	{
		cbData->app = this;
		cbData->cmd = xsStrDup(functionName);
		return xsStartTimer(millisecond, __onTimerCallback, cbData);
	}

	return XS_EC_ERROR;
}

void xsCoreApplication::clearTimeout(int timerId)
{
	xsStopTimer(timerId);
}
