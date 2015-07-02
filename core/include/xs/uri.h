#ifndef _XS_URI_H_
#define _XS_URI_H_

typedef struct _xsUri
{
	int schema;
	char *base;
	char *path;
	char *query;
	char *fragment;
} xsUri;

enum _xsUriSchema
{
	XS_URI_UNKNOWN		= 0,
	XS_URI_FILE,		// as a global file
	XS_URI_HTTP,		// HTTP
	XS_URI_LOCAL,		// as a local file
	XS_URI_RES,			// as a node in resource
	XS_URI_ROI			// Remote Object Invocation
};

xsUri *xsUriParse(const char *uri);
void xsUriDestroy(xsUri *uri);

#endif /* _XS_URI_H_ */
