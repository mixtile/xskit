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
#include <xs/bon.h>
#include <xs/object.h>
#include <xs/array.h>
#include <xs/memory.h>
#include <xs/value.h>
#include <xs/string.h>
#include <xs/utils.h>
#include <xs/coreapplication.h>
//#include <xs/widget.h>
//#include <xs/remote.h>
//#include <xs/datasource.h>
//#include <xs/widget.h>
//#include <xs/script.h>
//#include <xs/shape.h>
//#include <xs/canvas.h>
//#include <xs/canvascontext.h>

////////////////////////////////////////////////////////
// internal object registry
#define XS_CLS_COUNT		64 // XXX: Need dynamic create
//typedef struct _xsObjectCreatorMap
//{
////	xsS32 classId;
//	xsRuntimeClass *cls;
//} xsClassNameMap;
static xsClass *g_classes[XS_CLS_COUNT] = {0};
static int g_classesCount = 0;


xsBool xsClass::isDerivedFrom(const xsClass *base) const
{
	const xsClass *thiz = this;

	while (thiz != NULL)
	{
		if (thiz == base)
			return XS_TRUE;

		thiz = thiz->baseClass;
	}

	return XS_FALSE;
}

xsClass *xsClass::getBase() const
{
	return baseClass;
}

xsClass *xsClass::forName(const char *name)
{
	int i;
	for (i = 0; i < XS_CLS_COUNT && g_classes[i] != NULL; i++)
	{// 内部对象
		if (xsStrCmp(g_classes[i]->className, name) == 0)
			return g_classes[i];
	}

	XS_TRACE("Cannot find class: %s, using base class Object.", name);

	return XS_CLASS(xsObject);
}

xsObject::xsObject()
{
	id = NULL;
	parent = NULL;
	elements = NULL;
	props = NULL;

	references = 0;

	// read-only for object allocated at stack
	flag = XS_OBJ_RO_DATA;

	// shadow virtual machine object will be lazy created
	vmObject = NULL;
}

xsObject::~xsObject()
{
	if (props != NULL)
	{
		xsIterator iter = NULL;
		xsValue *value;

		// Destroy fields
		for (;;)
		{
			value = (xsValue *)xsHashMapIterate(props, &iter);
			if (iter == NULL)
				break;

			if (value != NULL)
				xsValueDestroy(value, XS_TRUE);
		}

		// Destroy container
		xsHashMapDestroy(props);
	}

	// detach and release VM object from spool
	if (vmObject != NULL)
	{
//		xsScriptReleaseVMObject(xsLauncher::getCurrentApp(), vmObject); // XXX current app maybe not contain objest's stack
		vmObject = NULL;
	}

	// destroy sub objects
	if (elements != NULL)
		delete elements;

	if (id != NULL)
	{
		xsFree(id);
		id = NULL;
	}
}

xsBool xsObject::isTypeOf(const xsClass *cls) const
{
	return getClass()->isDerivedFrom(cls);
}

void xsObject::setId(const char *id)
{
	xsReplaceAny((void **)&this->id, xsStrDup(id));
}

xsBool xsObject::getProperty(int propId, xsValue *value)
{
	xsValue *val;
	XS_ASSERT(value != NULL);

	if (props == NULL)
		return XS_FALSE;

	val = (xsValue *)xsHashMapGet(props, propId);
	if (val == NULL)
		return XS_FALSE;
	else
		*value = *val;
	return XS_TRUE;
}

xsBool xsObject::setProperty(int propId, xsValue *value)
{
	xsValue *oldVal;
	XS_ASSERT(value != NULL);

	switch (propId)
	{
	case XS_PROP_OBJECT_ID:
		if (value->type == XS_VALUE_STRING)
		{
			setId(value->data.s);
			return XS_TRUE;
		}
		return XS_FALSE;
	case XS_PROP_OBJECT_ELEMENTS:
		if (value->type == XS_VALUE_ARRAY)
		{
			if(xsArraySize(value->data.array) > 0)
			{
				elements = new xsElementList();
				elements->add(this, value->data.array);
			}

			xsValueDestroy(value, XS_FALSE);
			return XS_TRUE;
		}
		else
		{
			return XS_FALSE;
		}
		break;
	}

	// Unknown fields
	if (props == NULL)
		props = xsHashMapCreate(2);

	oldVal = (xsValue *)xsHashMapPut(props, propId, xsValueDuplicate(value));
	if (oldVal != NULL)
		xsValueDestroy(oldVal, XS_TRUE);

	return XS_TRUE;
}

xsBool xsObject::getProperty(const char *propName, xsValue *value)
{
	int id = getPropertyId(propName);
	if (id == 0)
		id = hashName(propName); // TODO: separate native props and dynamics.

	return getProperty(id, value);
}

xsBool xsObject::setProperty(const char *propName, xsValue *value)
{
	int id = getPropertyId(propName);
	if (id == 0)
		id = hashName(propName); // XXX: Maybe dynamic prop has a same hash with native id

	return setProperty(id, value);
}

int xsObject::getPropertyId(const char *propName)
{
	xsClass *cls = getClass();
	const xsPropertyMap *map = cls->properties;
	
	for (;;)
	{
		if (map->name != NULL)
		{
			if (xsStrCmp(map->name, propName) == 0)
				return map->id;
			map++;
		}
		else
		{
			cls = cls->getBase();
			if (cls == NULL)
				return 0;

			map = cls->properties;
		}
	}
}

int xsObject::getPropertyId(int index)
{
	return getClass()->classId | index; 
}

xsObject *xsObject::getElementById(const char *id)
{
	if (elements == NULL)
		return NULL;

	return elements->get(id);
}


void xsObject::add(xsObject *sub)
{
	XS_ASSERT(sub != NULL);

	if (elements == NULL)
		elements = new xsElementList();

	if (elements != NULL)
		elements->add(this, sub);
}

void xsObject::removeAll(xsBool destroy)
{
	if (elements == NULL)
		return;

	// Avoid shared sub-widget free twice
	elements->removeAll(destroy);

	if (destroy)
	{
		delete elements;
		elements = NULL;
	}
}

int xsObject::count()
{
	if (elements == NULL)
		return 0;

	return elements->count();
}

int xsObject::processEvent(xsEvent *e)
{
	return XS_EC_OK;
}

///////////////////////////////////////////////////////////////////////////////


int xsObjInit(void)
{
//	int i = 0;

	XS_TRACE("[CORE]xsObjInit");

	// register inner object classes
	xsObjectRegister(XS_CLASS(xsObject));
	xsObjectRegister(XS_CLASS(xsCoreApplication));
	xsObjectRegister(XS_CLASS(xsManifest));
//	xsObjectRegister(XS_CLASS(xsDataSource));
//	xsObjectRegister(XS_CLASS(xsRemote));

	//xsObjectRegister(XS_CLASS(xsCanvas));
	//xsObjectRegister(XS_CLASS(xsCanvasContext));
	//xsObjectRegister(XS_CLASS(xsShape));
	//xsObjectRegister(XS_CLASS(xsLine));
	//xsObjectRegister(XS_CLASS(xsRectangle));
	//xsObjectRegister(XS_CLASS(xsArc));
	//xsObjectRegister(XS_CLASS(xsBezierCurve));

	return XS_EC_OK;
}

void xsObjUninit(void)
{

	// release object class list

}

xsObject *xsObjectCreate(const char *name)
{
	xsClass *cls = xsClass::forName(name);
	xsObject *obj;

	if (cls == NULL)
		return NULL;

	obj = cls->create();
	obj->setFlag(XS_OBJ_NORMAL); // XXX for dynamic

	return obj;
}

void xsObjectRegister(xsClass *cls)
{
	if (g_classesCount >= XS_CLS_COUNT)
		return;

	g_classes[g_classesCount] = cls;

	g_classesCount++;
}

xsObject *xsObject::reference()
{
	if ((flag & XS_OBJ_RO_DATA) == XS_OBJ_RO_DATA)
		return this; // need not inc ref

	references++;
	return this;
}

xsObject *xsObject::clone()
{
	xsObject *ret = getClass()->create();
	if (ret != NULL)
		*ret = *this; // assignee

	return ret;
}

void xsObject::destroyInstance()
{
	if ((flag & XS_OBJ_RO_DATA) == XS_OBJ_RO_DATA)
		return; // need not destroy

	if (references > 0)
	{
		references--;
		return;
	}

	delete this;
}

int xsObject::allocFields(int count)
{
	// TODO: free fields element
	if (props != NULL)
		xsHashMapDestroy(props);

	props = xsHashMapCreate(count);
	if (props == NULL)
		return XS_EC_NOMEM;

	return XS_EC_OK;
}

xsU32 xsObject::hashName(const char *name, size_t len)
{
	return xsBonHashNameN(name, len, 0);
}

xsU32 xsObject::hashName(const char *name)
{
	return xsBonHashNameN(name, (size_t)-1, 0);
}
