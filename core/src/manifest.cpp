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

#include <xs/manifest.h>

xsManifest::~xsManifest()
{
	if (_uuid != NULL)
		xsFree(_uuid);

	if (_preload != NULL)
		xsFree(_preload);

	if (_startup != NULL)
		xsFree(_startup);
}

xsBool xsManifest::setProperty(int propId, xsValue *value)
{
	switch (propId)
	{
	case XS_PROP_MANIFEST_NAME:
		if (value->type == XS_VALUE_TEXT)
			xsReplaceAny((void **)&_name, value->data.t);
		else
			return XS_FALSE;
		break;
	case XS_PROP_MANIFEST_ORIENT:
		if (value->type == XS_VALUE_INT32)
			_orient = value->data.n;
		break;
	case XS_PROP_MANIFEST_UUID:
		if (value->type == XS_VALUE_STRING)
			xsReplaceAny((void **)&_uuid, value->data.t);
		else
			return XS_FALSE;
		break;
	case XS_PROP_MANIFEST_PRELOAD:
		if (value->type == XS_VALUE_STRING)
			xsReplaceAny((void **)&_preload, value->data.t);
		else
			return XS_FALSE;
		break;
	case XS_PROP_MANIFEST_STARTUP:
		if (value->type == XS_VALUE_STRING)
			xsReplaceAny((void **)&_startup, value->data.t);
		else
			return XS_FALSE;
		break;
	default:
		return xsObject::setProperty(propId, value);
	}

	return XS_TRUE;
}
