#ifndef _XS_PATH_H_
#define _XS_PATH_H_

#ifdef __cplusplus
extern "C" {
#endif

xsTChar *xsGetUserDataPath(xsTChar *buf);
xsTChar *xsGetSharedDataPath(xsTChar *buf);

xsTChar *xsGetStagePath(xsTChar *buf);

#ifdef __cplusplus
}
#endif

#endif /* _XS_PATH_H_ */
