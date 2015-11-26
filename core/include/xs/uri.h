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
