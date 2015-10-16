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

#ifndef _XS_COREAPPLICATION_H_
#define _XS_COREAPPLICATION_H_

#include <xs/object.h>
#include <xs/manifest.h>
#include <xs/resource.h>
#include <xs/remote.h>

class xsCoreApplication : public xsObject
{
	XS_OBJECT_DECLARE(CoreApplication, XS_CLS_COREAPPLICATION)
public:
	char *uri;

	static xsBool launchApp(const char *appUri);
	static void trace(const char *info);
	
	xsValue *getVar(const char *var);
	xsBool setVar(const char *var, xsValue *value);
	xsBool hasVar(const char *var);
	void removeVar(const char *var);

	xsRemote *invoke(const char *uri, const char *cmdCallback);

	int setTimeout(const char *functionName, int millisecond);
	void clearTimeout(int timerId);

	virtual int processEvent(xsEvent *e);

	virtual int start();
	virtual void suspend();
	virtual int resume();
	virtual void exit();

	inline int getIndex() { return _index; }
	inline void setIndex(int index) { _index = index; }

	inline const xsRuntimeContext *getContext() const { return _context; }
	inline const char *getUUID() const { return _manifest->getUUID(); };
	inline xsResource *getResource() const { return _res; };
	inline const xsManifest *getManifest() const { return _manifest; };

protected:
	xsCoreApplication();
	~xsCoreApplication();

//private:
	void *scriptEnv;	// Script run environment

private:
	xsRuntimeContext *_context;	// application runtime context
	xsResource *_res;
	xsManifest *_manifest;
	int _index;			// index in application list
	xsHashMap vars;		// runtime var store

	int load(xsEvent *e);

	void onMouse(xsMouseEvent *evt);

	typedef struct _xsCallbackData
	{
		xsCoreApplication *app;

		xsObject *obj;
		char *cmd;

		// TODO: args
	} xsCallbackData;

	static int __onTimerCallback(void *param);
};

#endif /* _XS_APPLICATION_H_ */
