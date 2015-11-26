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

#include <xs/container.h>
#include <xs/memory.h>

typedef struct _xsHashElement
{
	int key;
	int inUse;

	void *data;
} xsHashElement;

typedef struct _xsQueueType
{
	xsArrayType array;	// inherit from xsArrayType

	int front;
	int back;
} xsQueueType;

xsArrayList xsArrayListCreate(int size)
{
	xsArrayType *array = (xsArrayType *)xsCalloc(sizeof(xsArrayType));
	
	if (!xsArrayListInit(array, size))
	{
		xsFree(array);
		return NULL;
	}

	return array;
}

xsBool xsArrayListInit(xsArrayList array, int size)
{
	if (array != NULL)
	{
		if (size < 1)
			size = 1;

		array->elements = xsCalloc(sizeof(void *) * size);
		if (array->elements == NULL)
			return XS_FALSE;

		array->capacity = size;
	}

	return XS_TRUE;
}

void xsArrayListDestroy(xsArrayList array)
{
	if (array != NULL)
	{
		if (((xsArrayType *)array)->elements != NULL)
			xsFree(((xsArrayType *)array)->elements);
		xsFree(array);
	}
}

int xsArrayListExpand(xsArrayList array)
{
	xsArrayType *ar = (xsArrayType *)array;
	void *elements = xsCalloc(sizeof(void *) * ((ar->capacity * 3) / 2 + 2));
	if (elements != NULL)
	{
		xsMemCpy(elements, ar->elements, sizeof(void *) * ar->capacity);
		ar->capacity = (ar->capacity * 3) / 2 + 2;
		xsFree(ar->elements);
		ar->elements = elements;
	}
	else
	{
		return XS_EC_NOMEM;
	}

	XS_ASSERT(ar->capacity > ar->count);

	return XS_EC_OK;
}

int xsArrayListAdd(xsArrayList array, void *element)
{
	xsArrayType *ar = (xsArrayType *)array;
	XS_ASSERT(ar != NULL && ar->elements != NULL);

	if (ar->count == ar->capacity)
	{
		if (xsArrayListExpand(array) != XS_EC_OK)
			return XS_EC_NOMEM;
	}

	((void **)ar->elements)[ar->count] = element;
	ar->count++;

	return ar->count - 1;
}

void *xsArrayListGet(xsArrayList array, int index)
{
	XS_ASSERT(array != NULL && ((xsArrayType *)array)->elements != NULL);

	if (index >= ((xsArrayType *)array)->count)
		return NULL;

	return ((void **)((xsArrayType *)array)->elements)[index];
}

int xsArrayListSet(xsArrayList array, int index, void *element)
{
	if (index >= ((xsArrayType *)array)->count)
		return XS_EC_ERROR;

	((void **)((xsArrayType *)array)->elements)[index] = element;
	return index;
}

int xsArrayListIndex(xsArrayList array, void *element)
{
	int i;
	XS_ASSERT(array != NULL && ((xsArrayType *)array)->elements != NULL);

	for (i = 0; i < ((xsArrayType *)array)->count; i++)
	{
		if (((void **)((xsArrayType *)array)->elements)[i] == element)
			return i;
	}

	return XS_EC_ERROR;
}

void xsArrayListRemove(xsArrayList array, int index)
{
	int i;
	xsArrayType *ar = (xsArrayType *)array;
	XS_ASSERT(array != NULL);

	if (ar->elements == NULL || ar->capacity <= index || ar->count <= index)
		return;

	ar->count--;
	for (i = index; i < ar->count; i++)
	{
		((void **)ar->elements)[i] = ((void **)ar->elements)[i + 1];
	}
}

void xsArrayListRemoveAll(xsArrayList array)
{
	xsArrayType *ar = (xsArrayType *)array;

	if (ar->capacity == 0 || ar->count == 0)
		return;

	xsMemSet(ar->elements, 0, sizeof(void **) * ar->capacity);
	ar->count = 0;
}

int xsArrayListSize(xsArrayList array)
{
	XS_ASSERT(array != NULL);
	return ((xsArrayType *)array)->count;
}

void *xsArrayListIterate(xsArrayList array, xsIterator *iterator)
{
	void **item;
	XS_ASSERT(iterator != NULL);

	item = (void **)*iterator;
	// �ӵ�һ��element��ʼ
	if (((xsArrayType *)array)->count == 0)
	{
		item = NULL;
	}
	else if (item == NULL)
	{
		item = (void **)((xsArrayType *)array)->elements;
	}
	else
	{
		item++; // Next
		if (item - (void **)((xsArrayType *)array)->elements >= ((xsArrayType *)array)->count)
			item = NULL;
	}

	*iterator = item;
	if (item != NULL)
		return *item;
	else
		return NULL;
}

xsHashMap xsHashMapCreate(int size)
{
	xsArrayType *map = (xsArrayType *)xsCalloc(sizeof(xsArrayType));
	if (map != NULL)
	{
		map->elements = xsCalloc(sizeof(xsHashElement) * size);
		if (map->elements == NULL)
		{
			xsFree(map);
			return NULL;
		}

		map->capacity = size;
	}

	return map;
}

void xsHashMapDestroy(xsHashMap map)
{
	xsArrayListDestroy((xsArrayList)map);
}

void *xsHashMapPut(xsHashMap map, int key, void *value)
{
	int i;
	xsHashElement *el;
	xsArrayType * ar = (xsArrayType *)map;
	void *oldVal;

	// TODO: use real hash
	for (i = 0; i < ((xsArrayType *)map)->capacity; i++)
	{
		el = (xsHashElement *)((xsArrayType *)map)->elements + i;
		if (el->inUse && el->key == key)
		{
			oldVal = el->data;
			el->data = value;
			return oldVal;
		}
	}

	if (ar->count == ar->capacity)
	{
		void *elements = xsCalloc(sizeof(xsHashElement) * ((ar->capacity * 3) / 2 + 2));
		if (elements != NULL)
		{
			xsMemCpy(elements, ar->elements, sizeof(xsHashElement) * ar->capacity);
			ar->capacity = (ar->capacity * 3) / 2 + 2;
			xsFree(ar->elements);
			ar->elements = elements;
		}
		else
		{
			return NULL;
		}
	}

	XS_ASSERT(ar->capacity > ar->count);

	// TODO: use real hash
	for (i = 0; i < ar->capacity; i++)
	{
		el = (xsHashElement *)ar->elements + i;
		if (!el->inUse)
		{
			el->inUse = XS_TRUE;
			el->key = key;
			el->data = value;
			ar->count++;
			return NULL;
		}
	}

	// Map full
	XS_ASSERT(0);

	return NULL;
}

void *xsHashMapGet(xsHashMap map, int key)
{
	int i;
	xsHashElement *el;

	// TODO: use real hash
	for (i = 0; i < ((xsArrayType *)map)->capacity; i++)
	{
		el = (xsHashElement *)((xsArrayType *)map)->elements + i;
		if (el->inUse && el->key == key)
			return el->data;
	}

	return NULL;
}

xsBool xsHashMapHas(xsHashMap map, int key)
{
	int i;
	xsHashElement *el;

	// TODO: use real hash
	for (i = 0; i < ((xsArrayType *)map)->capacity; i++)
	{
		el = (xsHashElement *)((xsArrayType *)map)->elements + i;
		if (el->inUse && el->key == key)
			return XS_TRUE;
	}

	return XS_FALSE;
}

int xsHashMapRemove(xsHashMap map, int key)
{
	return XS_EC_ERROR;
}

int xsHashMapSize(xsHashMap map)
{
	XS_ASSERT(map != NULL);
	return ((xsArrayType *)map)->count;
}

void *xsHashMapIterate(xsHashMap map, xsIterator *iterator)
{
	int i;
	xsHashElement *el;

	i = *(int *)iterator;

	for (; i < ((xsArrayType *)map)->capacity; i++)
	{
		el = (xsHashElement *)((xsArrayType *)map)->elements + i;
		if (el->inUse)
		{
			*(int *)iterator = i + 1;
			return el->data;
		}
	}

	if (i >= ((xsArrayType *)map)->capacity)
		*iterator = 0;
	
	return NULL;
}

xsBool xsHashMapIterateEntry(xsHashMap map, xsIterator *iterator, int *key, void **value)
{
		int i;
	xsHashElement *el;

	i = *(int *)iterator;

	for (; i < ((xsArrayType *)map)->capacity; i++)
	{
		el = (xsHashElement *)((xsArrayType *)map)->elements + i;
		if (el->inUse)
		{
			*(int *)iterator = i + 1;
			*key = el->key;
			*value = el->data;
			return XS_TRUE;
		}
	}

	if (i >= ((xsArrayType *)map)->capacity)
		*iterator = 0;
	
	return XS_FALSE;
}

xsStack xsStackCreate(int size)
{
	return xsArrayListCreate(size);
}

void xsStackDestroy(xsStack stack)
{
	xsArrayListDestroy((xsArrayList)stack);
}

int xsStackPush(xsStack stack, void *value)
{
	return xsArrayListAdd((xsArrayList)stack, value);
}

void *xsStackPop(xsStack stack)
{
	void *el;
	xsArrayType *self = (xsArrayType *)stack;
	XS_ASSERT(stack != NULL);

	if (self->count == 0)
		return NULL;

	self->count--;
	el = ((void **)self->elements)[self->count];

	return el;
}

int xsStackSize(xsStack stack)
{
	XS_ASSERT(stack != NULL);
	return ((xsArrayType *)stack)->count;
}

xsBool xsStackEmpty(xsStack stack)
{
	XS_ASSERT(stack != NULL);
	return ((xsArrayType *)stack)->count == 0;
}

xsQueue xsQueueCreate(int size)
{
	xsQueueType *queue = (xsQueueType *)xsCalloc(sizeof(xsQueueType));

	if (queue != NULL)
	{
		if (size < 1)
			size = 1;

		queue->array.elements = xsCalloc(sizeof(void *) * size);
		if (queue->array.elements == NULL)
		{
			xsFree(queue);
			return NULL;
		}

		queue->array.capacity = size;
		queue->back = -1;
		queue->front = 0;
	}

	return queue;
}

void xsQueueDestroy(xsQueue queue)
{
	xsArrayListDestroy((xsArrayList)queue);
}

int xsQueuePush(xsQueue queue, void *value)
{
	xsQueueType *self = (xsQueueType *)queue;

	if (self->array.count == self->array.capacity)
	{// full, expand
		if (xsArrayListExpand(&self->array) != XS_EC_OK)
			return XS_EC_NOMEM;

		// resort
		if (self->front > self->back)
		{
			int i, j;
			for (i = self->array.count - 1, j = self->array.capacity - 1; i > self->front; i--, j--)
			{
				((void **)self->array.elements)[j] = ((void **)self->array.elements)[i];
				((void **)self->array.elements)[i] = NULL;
			}
			self->front += self->array.capacity - self->array.count;
		}
	}

	// push back
	self->back++;
	if (self->back >= self->array.capacity)
		self->back = 0;
	
	((void **)self->array.elements)[self->back] = value;
	self->array.count++;

	return self->back;
}

void *xsQueuePop(xsQueue queue)
{
	void *el;
	xsQueueType *self = (xsQueueType *)queue;
	XS_ASSERT(queue != NULL);

	if (self->array.count == 0)
		return NULL;

	el = ((void **)self->array.elements)[self->front];
	((void **)self->array.elements)[self->front] = NULL;
	self->front++;
	if (self->front >= self->array.capacity)
		self->front = 0;
	self->array.count--;

	return el;
}

void *xsQueueFront(xsQueue queue)
{
	xsQueueType *self = (xsQueueType *)queue;
	XS_ASSERT(queue != NULL);

	if (self->array.count == 0)
		return NULL;

	return ((void **)self->array.elements)[self->front];
}

int xsQueueSize(xsQueue queue)
{
	XS_ASSERT(queue != NULL);
	return ((xsArrayType *)queue)->count;
}

xsBool xsQueueEmpty(xsQueue queue)
{
	XS_ASSERT(queue != NULL);
	return ((xsArrayType *)queue)->count == 0;
}
