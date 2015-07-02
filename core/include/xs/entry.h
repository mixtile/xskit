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
