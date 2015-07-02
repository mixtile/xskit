#include <xs/object.h>
#include <xs/array.h>
#include <xs/elementlist.h>
#include <xs/memory.h>

xsElementList::xsElementList()
{
	items = xsArrayListCreate(2);
}

xsElementList::~xsElementList()
{
	removeAll(XS_TRUE);

	if (items != NULL)
		xsArrayListDestroy(items);
}

int xsElementList::count()
{
	if (items == NULL)
		return 0;
	return xsArrayListSize(items);
}

xsObject *xsElementList::iterate(xsIterator *iter)
{
	if (items == NULL)
		return NULL;

	return (xsObject *)xsArrayListIterate(items, iter);
}

xsObject *xsElementList::riterate(xsIterator *iter)
{
	int index;
	xsObject *sub;
	if (items == NULL)
		return NULL;

	// Reverse order
	index = xsArrayListSize(items) - 1;
	index -= *(int *)iter;
	if (index < 0)
	{
		*(int *)iter = 0;
		return NULL;
	}

	sub = (xsObject *)xsArrayListGet(items, index);
	*(int *)iter += 1;

	return sub;
}

xsObject *xsElementList::get(const char *id)
{
	xsIterator iter = NULL;
	xsObject *obj;
	const char *subId;
	if (items == NULL)
		return NULL;

	for (;;)
	{
		obj = (xsObject *)xsArrayListIterate(items, &iter);
		if (obj == NULL || iter == NULL)
			break;

		subId = obj->getId();
		if (subId != NULL && xsStrCmp(subId, id) == 0)
			return obj;
	}

	return NULL;
}

void xsElementList::adopt(xsObject *parent)
{
	xsIterator iter = NULL;
	xsObject *sub;
	XS_ASSERT(parent != NULL);

	for (;;)
	{
		sub = (xsObject *)xsArrayListIterate(items, &iter);
		if (iter == NULL || sub == NULL)
			break;

		sub->setParent(parent);
	}
}

void xsElementList::add(xsObject *parent, xsObject *sub)
{
	XS_ASSERT(sub != NULL);

	if (items == NULL)
		items = xsArrayListCreate(2);

	xsArrayListAdd(items, sub);
	sub->setParent(parent);
}

void xsElementList::add(xsObject *parent, xsArray *elements)
{
	xsIterator iter = NULL;
	xsValue *val;

	for (;;)
	{
		val = xsArrayIterate(elements, &iter);
		if (val == NULL || iter == NULL)
			break;

		if (val->type == XS_VALUE_OBJECT) // XXX: xsObject hasn't a parent field
			add(parent, val->data.obj->reference());
	}
}

void xsElementList::remove(const char *id)
{
	xsIterator iter = NULL;
	xsObject *sub;
	int i;

	for (i = 0;; i++)
	{
		sub = (xsObject *)xsArrayListIterate(items, &iter);
		if (iter == NULL || sub == NULL)
			break;

		if (xsStrCmp(sub->getId(), id) == 0)
		{
			sub->setParent(NULL);
			xsArrayListRemove(items, i);
			break;
		}
	}
}

void xsElementList::removeAll(xsBool destroy)
{
	if (destroy)
	{
		xsIterator iter = NULL;
		xsObject *sub;

		for (;;)
		{
			sub = (xsObject *)xsArrayListIterate(items, &iter);
			if (iter == NULL || sub == NULL)
				break;

			sub->destroyInstance();
		}
	}

	xsArrayListRemoveAll(items);
}
