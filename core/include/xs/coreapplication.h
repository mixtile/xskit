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
