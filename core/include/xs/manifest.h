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

#ifndef _XS_MANIFEST_H_
#define _XS_MANIFEST_H_

#include <xs/object.h>
#include <xs/resource.h>

class xsManifest : public xsObject
{
	XS_OBJECT_DECLARE(Manifest, XS_CLS_MANIFEST)
public:
	xsBool setProperty(int propId, xsValue *value);

	inline int getOrient() { return _orient; }
	inline xsTChar *getName() { return _name; }
	inline const char *getUUID() { return _uuid; };
	inline const char *getPreload() { return _preload; };
	inline const char *getStartup() { return _startup; };

protected:
	~xsManifest();

private:
	int _orient;
	xsTChar *_name;		// application's name
	char *_uuid;		// application's UUID
	char *_preload;		// preload library module
	char *_startup;		// startup script
};

#endif /* _XS_MANIFEST_H_ */
