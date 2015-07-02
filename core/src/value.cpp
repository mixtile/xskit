#include <xs/pal.h>
#include <xs/value.h>
#include <xs/memory.h>
#include <xs/string.h>
#include <xs/array.h>
#include <xs/object.h>
#include <xs/utils.h>

xsValue *xsValueCreate(int type)
{
	xsValue *val = (xsValue *)xsMalloc(sizeof(xsValue));
	if (val == NULL)
		return NULL;

	val->type = type;
	val->data.n = 0;

	return val;
}

xsValue *xsValueDuplicate(const xsValue *value)
{
	xsValue *val;
	if (value == NULL)
		return NULL;

	val = (xsValue *)xsMalloc(sizeof(xsValue));
	if (val == NULL)
		return NULL;

	*val = *value;
	return val;
}

void xsValueDestroy(xsValue *value, xsBool freeWrap)
{
	if (value == NULL)
		return;

	if (value->data.ptr != NULL)
	{
		switch (value->type)
		{
		case XS_VALUE_INT64:
		case XS_VALUE_STRING:
		case XS_VALUE_TEXT:
		case XS_VALUE_BINARY:
		case XS_VALUE_PROTOTYPE_STRING:
			xsFree(value->data.ptr);
			break;
#ifndef XS_NO_COMPLEX_DATA_TYPE
		case XS_VALUE_ARRAY:
			xsArrayDestroy(value->data.array);
			break;
		case XS_VALUE_OBJECT:
			value->data.obj->destroyInstance();
			break;
#endif
		}
	}
	value->type = XS_VALUE_NONE;
	value->data.n = 0;

	if (freeWrap)
		xsFree(value);
}

static int ValueFind(xsValue *container, const char *name, xsValue *value, xsBool set)
{
#ifndef XS_NO_COMPLEX_DATA_TYPE
	const char *split;
	xsU32 hash;
	char idStr[12];
	int id;
	size_t len;
#endif
	xsValue val;
	xsBool hit = XS_FALSE;

	XS_ASSERT(value != NULL);

	val = *container;

	while (*name != '\0')
	{
#ifndef XS_NO_COMPLEX_DATA_TYPE
		if (val.type == XS_VALUE_OBJECT)
		{
			if (*name == '.')
				name++;

			split = xsStrChr(name, '.'); // property split

			// Field name hash
			if (split == NULL)
			{
				hash = val.data.obj->hashName(name);
				hit = XS_TRUE;
			}
			else
			{
				hash = val.data.obj->hashName(name, split - name);
				name = split + 1;
			}

			if (set && hit)
			{
				if (!val.data.obj->setProperty(hash, value))
					return XS_EC_NOT_EXISTS;
			}
			else
			{
				val.type = XS_VALUE_NONE;
				if (!val.data.obj->getProperty(hash, &val))
					return XS_EC_NOT_EXISTS;
			}
		}
		else if (val.type == XS_VALUE_ARRAY)
		{
			name = xsStrChr(name, '['); // element split begin
			if (name == NULL)
				return XS_EC_ERROR; // malformed

			name++;
			split = xsStrChr(name, ']'); // element split end
			// Element id
			if (split == NULL)
				return XS_EC_ERROR; // malformed
			
			len = XS_MIN(split - name, sizeof(idStr) - 1);
			xsStrCpyN(idStr, name, len);
			idStr[len] = '\0';
			name = split + 1;

			if (*name == '\0')
				hit = XS_TRUE;
			
			id = xsStrToInt(idStr);
			if (set && hit)
			{
				if (xsArraySet(val.data.array, id, value) < 0)
					return XS_EC_NOT_EXISTS;
			}
			else
			{
				container = xsArrayGet(val.data.array, id);
				if (container != NULL)
					val = *container;
				else
					return XS_EC_NOT_EXISTS;
			}
		}
		else
#endif
		{
			return XS_EC_ERROR;
		}

		if (hit)
			break; // found
	}

	if (!hit)
		return XS_EC_ERROR;	// wrong path format

	if (!set)
		*value = val;

	return XS_EC_OK;
}

int xsValueFindAndGet(xsValue *container, const char *name, xsValue *value)
{
	return ValueFind(container, name, value, XS_FALSE);
}

int xsValueFindAndSet(xsValue *container, const char *name, xsValue *value)
{
	return ValueFind(container, name, value, XS_TRUE);
}

xsTChar *xsValueGetDupText(const xsValue *value)
{
	switch (value->type)
	{
	case XS_VALUE_STRING:
		return xsStrToTcsDup(value->data.s);
	case XS_VALUE_UTF8:
		return xsUtf8ToTcsDup(value->data.s);
	case XS_VALUE_TEXT:
		return xsTcsDup(value->data.t);
	case XS_VALUE_OBJECT:
		return xsTcsDup(xsT("Object"));
	case XS_VALUE_ARRAY:
		return xsTcsDup(xsT("Array"));
	case XS_VALUE_INT32:
		{
			char buffer[12];
			xsIntToStr(value->data.n, buffer, 10);
			return xsStrToTcsDup(buffer);
		}
	}

	return NULL;
}

char *xsValueGetDupString(const xsValue *value)
{
	switch (value->type)
	{
	case XS_VALUE_STRING:
	case XS_VALUE_UTF8:
		return xsStrDup(value->data.s);
	case XS_VALUE_TEXT:
		return xsTcsToStrDup(value->data.t);
	case XS_VALUE_OBJECT:
		return xsStrDup("Object");
	case XS_VALUE_ARRAY:
		return xsStrDup("Array");
	case XS_VALUE_INT32:
		{
			char buffer[12];
			xsIntToStr(value->data.n, buffer, 10);
			return xsStrDup(buffer);
		}
	}

	return NULL;
}

#ifndef XS_NO_COMPLEX_DATA_TYPE
void xsReplaceObject(xsObject **objOldPtr, xsObject *objNew)
{
	if (*objOldPtr != NULL)
		(*objOldPtr)->destroyInstance();

	*objOldPtr = objNew;
}

void xsReplaceArray(xsArray **aryOldPtr, xsArray *aryNew)
{
	if (*aryOldPtr != NULL)
		xsArrayDestroy(*aryOldPtr);

	*aryOldPtr = aryNew;
}
#endif

void xsReplaceAny(void **anyOld, void *anyNew)
{
	if (*anyOld != NULL)
		xsFree(*anyOld);

	*anyOld = anyNew;
}
