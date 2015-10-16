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

#ifndef _XS_RESOURCE_H_
#define _XS_RESOURCE_H_

#ifdef __cplusplus

class xsManifest;
class xsCoreApplication;
class xsRuntimeContext;
class xsRandomAccessStream;

class xsResource : public xsBase
{
public:
	static xsResource *createInstance(const xsCoreApplication *app);


	xsValue loadValue(const char *name);
//	xsWidget *loadWidget(const char *name);

	void *loadData(const char *name, size_t *size);
	char *loadScript(const char *name, size_t *size);

	xsImage *loadImage(const xsTChar *src);
	void freeImage(xsImage *img);

	xsMedia *loadMedia(const xsTChar *src);
	void freeMedia(xsMedia *media);

private:

	xsAFD *createAFD(const xsTChar *src, const xsTChar *path);
	void freeAFD(xsAFD *afd);

	void *loadDataFromFile(const xsTChar *filename, size_t *size);

private:
	const xsCoreApplication *_app;
	const xsRuntimeContext *_context;

	// clone of xsRuntimeContext
	int _loadMode;
	const xsTChar *_base;
	xsRes _res;
	xsFile _fd;
};

#endif /* __cplusplus */

#ifdef __cplusplus
extern "C" {
#endif

xsImage *xsLoadImage(void *context, const xsTChar *src);
void xsFreeImage(void *context, xsImage *img);

xsMedia *xsLoadMedia(void *context, const xsTChar *src);
void xsFreeMedia(void *context, xsMedia *media);

/**
 * Get application base directory path, with endding path delimeter.
 * @param buf Buffer to store directory, at least XS_MAX_PATH xsTChar length
 * @return Return path if successful, or NULL if failed.
 */
xsTChar *xsGetBasePath(void *context, xsTChar *buf);

#ifdef __cplusplus
}
#endif

#endif /* _XS_RESOURCE_H_ */
