#ifndef _XS_CONTAINER_H_
#define _XS_CONTAINER_H_

#include <xs/pal.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _xsArrayType
{
	int capacity; /// Number of slots
	int count; /// Number of items

	void *elements;
} xsArrayType;

typedef xsAny xsIterator;
typedef xsArrayType *xsArrayList;
typedef xsAny xsHashMap;
typedef xsAny xsStack;
typedef xsAny xsQueue;

xsArrayList xsArrayListCreate(int size);
xsBool xsArrayListInit(xsArrayList array, int size);
void xsArrayListDestroy(xsArrayList array);
int xsArrayListAdd(xsArrayList array, void *element);
void *xsArrayListGet(xsArrayList array, int index);
int xsArrayListSet(xsArrayList array, int index, void *element);
int xsArrayListIndex(xsArrayList array, void *element);
void xsArrayListRemove(xsArrayList array, int index);
void xsArrayListRemoveAll(xsArrayList array);
int xsArrayListSize(xsArrayList array);
void *xsArrayListIterate(xsArrayList array, xsIterator *iterator);

xsHashMap xsHashMapCreate(int size);
void xsHashMapDestroy(xsHashMap map);
void *xsHashMapPut(xsHashMap map, int key, void *value);
void *xsHashMapGet(xsHashMap map, int key);
xsBool xsHashMapHas(xsHashMap map, int key);
int xsHashMapRemove(xsHashMap map, int key);
int xsHashMapSize(xsHashMap map);
void *xsHashMapIterate(xsHashMap map, xsIterator *iterator);
xsBool xsHashMapIterateEntry(xsHashMap map, xsIterator *iterator, int *key, void **value);

xsStack xsStackCreate(int size);
void xsStackDestroy(xsStack stack);
int xsStackPush(xsStack stack, void *value);
void *xsStackPop(xsStack stack);
int xsStackSize(xsStack stack);
xsBool xsStackEmpty(xsStack stack);

xsQueue xsQueueCreate(int size);
void xsQueueDestroy(xsQueue queue);
int xsQueuePush(xsQueue queue, void *value);
void *xsQueuePop(xsQueue queue);
void *xsQueueFront(xsQueue queue);
int xsQueueSize(xsQueue queue);
xsBool xsQueueEmpty(xsQueue queue);

#ifdef __cplusplus
}
#endif

#endif /* _XS_CONTAINER_H_ */
