#ifndef _XS_LAUNCHER_H_
#define _XS_LAUNCHER_H_

#include <xs/container.h>

#define XS_DYNAMIC_LAUNCHER		// for entry.h typedef selection

class xsAppContext;

class xsLauncher
{
public:
	static int init(void);
	static void uninit(void);

	static int query(int index, xsAppInfo *appInfo);
	static int count(void);

	static xsAppContext *load(const char *appUri);
	static xsAppContext *load(xsAppContext *context);
	static void free(xsAppContext *context);
	
	static xsBool isBackgound(void);
	static xsAppContext *getCurrentContext(void);

	static int onStart(xsAppContext *context);
	static int onResume(void);
	static void onSuspend(void);

private:
	static xsArrayList _contextList;
	static xsAppContext *_theContext;
	static xsRes _resAppList;
	static xsBool _xsBackgound;
};

#endif /* _XS_LAUNCHER_H_ */
