#ifndef _XS_LAUNCHEREXPORT_H_
#define _XS_LAUNCHEREXPORT_H_

#include <xs/connection.h>
// Define export functions
#ifdef __cplusplus
extern "C" {
#endif

#ifdef WIN32
#undef XS_INTERFACE
#define XS_INTERFACE	__declspec(dllexport)
#endif

XS_INTERFACE int xsConnCheckin(xsConnection *conn);
XS_INTERFACE void xsConnCheckout(xsConnection * conn);

#ifdef __cplusplus
}
#endif

#ifdef WIN32
#undef XS_INTERFACE
#define XS_INTERFACE
#endif

#endif /* _XS_LAUNCHEREXPORT_H_ */
