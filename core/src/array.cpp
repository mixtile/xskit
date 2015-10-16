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

#include <xs/pal.h>
#include <xs/array.h>
#include <xs/memory.h>
#include <xs/container.h>
#include <xs/value.h>

xsArray *xsArrayCreate(int size)
{
	xsArray *array = (xsArray *)xsCalloc(sizeof(xsArray));

	if (array != NULL)
	{
		array->data = (xsValue *)xsCalloc(sizeof(xsValue) * size);
		if (array->data == NULL)
		{
			xsFree(array);
			return NULL;
		}

		array->capacity = size;
	}

	return array;
}

void xsArrayDestroy(xsArray *array)
{
	if (array != NULL)
	{
		if (array->references > 0)
		{
			array->references--;
			return;
		}

		xsArrayRemoveAll(array);

		// Destroy data
		if (array->data != NULL)
			xsFree(array->data);

		// Destroy self
		xsFree(array);
	}
}

xsArray *xsArrayRef(xsArray *array)
{
	if (array == NULL)
		return NULL;

	array->references++;
	return array;
}

static int ArrayExpand(xsArray *array)
{
	xsValue *data = (xsValue *)xsCalloc(sizeof(xsValue) * ((array->capacity * 3) / 2 + 2));
	if (data != NULL)
	{
		xsMemCpy(data, array->data, sizeof(xsValue) * array->capacity);
		array->capacity = (array->capacity * 3) / 2 + 2;
		xsFree(array->data);
		array->data = data;
	}
	else
	{
		return XS_EC_NOMEM;
	}

	XS_ASSERT(array->capacity > array->count);

	return XS_EC_OK;
}

int xsArrayAdd(xsArray *array, xsValue *element)
{
	if (array->count == array->capacity)
	{
		if (ArrayExpand(array) != XS_EC_OK)
			return XS_EC_NOMEM;
	}

	array->data[array->count] = *element;
	array->count++;

	return array->count - 1;
}

xsValue *xsArrayGet(xsArray *array, int index)
{
	if (index >= array->count)
		return NULL;

	return &array->data[index];
}

int xsArraySet(xsArray *array, int index, xsValue *element)
{
	if (index >= array->count)
		return XS_EC_ERROR;

	array->data[index] = *element;
	return index;
}

int xsArrayIndex(xsArray *array, xsValue *element)
{
	int i;

	for (i = 0; i < array->count; i++)
	{
		if (array->data[i].type == element->type && array->data[i].data.ptr == element->data.ptr)
			return i;
	}

	return XS_EC_ERROR;
}

void xsArrayRemove(xsArray *array, int index)
{
	int i;

	if (array->data == NULL || array->capacity <= index || array->count <= index)
		return;

	array->count--;
	for (i = index; i < array->count; i++)
	{
		array->data[i] = array->data[i + 1];
	}
}

void xsArrayRemoveAll(xsArray *array)
{
	xsValue *value;
	int i;

	if (array->capacity == 0 || array->count == 0)
		return;

	// Destroy element
	value = array->data;
	for (i = 0; i < array->count; i++, value++)
	{
		xsValueDestroy(value, XS_FALSE);
	}
	array->count = 0;
}

int xsArraySize(xsArray *array)
{
	return array->count;
}

xsValue *xsArrayIterate(xsArray *array, xsIterator *iter)
{
	xsValue *item;
	XS_ASSERT(iter != NULL);

	item = (xsValue *)*iter;
	// �ӵ�һ��element��ʼ
	if (array->count == 0)
	{
		item = NULL;
	}
	else if (item == NULL)
	{
		item = array->data;
	}
	else
	{
		item++; // Next
		if (item - array->data >= array->count)
			item = NULL;
	}

	*iter = item;
	if (item != NULL)
		return item;
	else
		return NULL;
}
