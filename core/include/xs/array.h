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

#ifndef _XS_ARRAY_H_
#define _XS_ARRAY_H_

#include <xs/datatypes.h>
#include <xs/container.h>

struct _xsArray
{
	// privates
	xsU16 references;
	xsU16 flag;			// not defined yet

	int capacity; /// Number of slots
	int count; /// Number of elements
	xsValue *data;
};

xsArray *xsArrayCreate(int size);
void xsArrayDestroy(xsArray *array);
xsArray *xsArrayRef(xsArray *array);
int xsArrayAdd(xsArray *array, xsValue *element);
xsValue *xsArrayGet(xsArray *array, int index);
int xsArraySet(xsArray *array, int index, xsValue *element);
int xsArrayIndex(xsArray *array, xsValue *element);
void xsArrayRemove(xsArray *array, int index);
void xsArrayRemoveAll(xsArray *array);
int xsArraySize(xsArray *array);
xsValue *xsArrayIterate(xsArray *array, xsIterator *iter);

#endif /* _XS_ARRAY_H_ */
