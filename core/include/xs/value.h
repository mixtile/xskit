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

#ifndef _XS_VALUE_H_
#define _XS_VALUE_H_

#include <xs/pal.h>
#include <xs/datatypes.h>

struct _xsValue
{
	int type;

	union
	{
		xsS32 n;
		float f;
		char *s;
		xsTChar *t;
		void *ptr;
		xsObject *obj;
		xsArray *array;
	} data;
};

enum _xsValueTypes
{
	XS_VALUE_NONE = 0,
	// primitive
	XS_VALUE_NULL,
	XS_VALUE_UNDEFINE,
	XS_VALUE_BOOL,
	XS_VALUE_INT32,
	XS_VALUE_UINT32,
	XS_VALUE_INT64,
	XS_VALUE_UINT64,
	XS_VALUE_FLOAT,
	XS_VALUE_DOUBLE,
	XS_VALUE_STRING,
	XS_VALUE_TEXT,
	XS_VALUE_BINARY, // data.ptr first sizeof(size_t) is binary data's size
	// complex
	XS_VALUE_ARRAY,
	XS_VALUE_OBJECT,
	XS_VALUE_PROTOTYPE_HASH,
	XS_VALUE_PROTOTYPE_STRING,
	XS_VALUE_SALT,
	XS_VALUE_INDEX,
	// script compatible
	XS_VALUE_UTF8
};

xsValue *xsValueCreate(int type);
xsValue *xsValueDuplicate(const xsValue *value);
void xsValueDestroy(xsValue *value, xsBool freeWrap);
int xsValueFindAndGet(xsValue *container, const char *name, xsValue *value);
int xsValueFindAndSet(xsValue *container, const char *name, xsValue *value);
xsTChar *xsValueGetDupText(const xsValue *value);
char *xsValueGetDupString(const xsValue *value);
#ifndef XS_NO_COMPLEX_DATA_TYPE
void xsReplaceObject(xsObject **objOldPtr, xsObject *objNew);
void xsReplaceArray(xsArray **aryOldPtr, xsArray *aryNew);
#endif
void xsReplaceAny(void **anyOld, void *anyNew);

#endif /* _XS_VALUE_H_ */
