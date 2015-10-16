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

#ifndef _XS_OBJECT_H_
#define _XS_OBJECT_H_

#include <xs/pal.h>
#include <xs/coreclassid.h>
#include <xs/value.h>
#include <xs/event.h>
#include <xs/elementlist.h>
#include <xs/container.h>
#include <xs/datatypes.h>
#include <xs/base.h>
#include <xs/memory.h>

typedef struct _xsPropertyMap
{
	const char *name;
	int id;
} xsPropertyMap;

/// Object dynamic create function.
typedef xsObject *(*xsObjectCreateFunc)();

struct xsClass
{
	xsU32 classId;
	const char *className;
	xsObjectCreateFunc create;
	xsClass *baseClass;

	const xsPropertyMap *properties;

	xsBool isDerivedFrom(const xsClass *base) const;
	xsClass *getBase() const;
	static xsClass *forName(const char *name);
};

#define XS_CLASS(name)	((xsClass *)(&name::name##Class))

#define XS_OBJECT_DECLARE(name, id) \
public: \
	static const xsClass xs##name##Class; \
	virtual xsClass *getClass() const; \
	static xsObject *createInstance(); \

#define _XS_OBJECT_IMPLEMENT(name, id, baseclass) \
	const xsClass xs##name::xs##name##Class = { id, #name, xs##name::createInstance, \
		baseclass, xs##name##Properties}; \
	xsClass *xs##name::getClass() const { return XS_CLASS(xs##name); } \
	xsObject *xs##name::createInstance() { return new xs##name(); } \

#define XS_OBJECT_IMPLEMENT(name, id, base) \
	_XS_OBJECT_IMPLEMENT(name, id, XS_CLASS(xs##base)) \

#define XS_OBJECT_IMPLEMENT_BASE(name, id) \
	_XS_OBJECT_IMPLEMENT(name, id, NULL) \

class xsObject : public xsListener
{
	XS_OBJECT_DECLARE(Object, XS_CLS_OBJECT)
public:
	virtual xsBool getProperty(int propId, xsValue *value);
	virtual xsBool setProperty(int propId, xsValue *value);

	virtual xsBool getProperty(const char *propName, xsValue *value);
	virtual xsBool setProperty(const char *propName, xsValue *value);

	virtual xsObject *getElementById(const char *id);
	inline xsElementList *getElements() const { return elements; }
	inline void setElements(xsElementList *elements) { this->elements = elements; }
	void add(xsObject *sub);
	void removeAll(xsBool destroy);
	int count();

	int getPropertyId(const char *propName);
	int getPropertyId(int index);

	virtual int processEvent(xsEvent *e);

	xsBool isTypeOf(const xsClass *cls) const;
	xsObject *reference();
	virtual xsObject *clone();

	void setId(const char *id);
	inline const char *getId() const { return id; }
	inline void setParent(xsObject *parent) { this->parent = parent; }
	inline xsObject *getParent() const { return parent; }
	inline xsHashMap getProperties() const { return props; }

public: // Static functions
	void destroyInstance();

	xsU32 hashName(const char *key);
	xsU32 hashName(const char *name, size_t len);
	static int sendEvent(xsObject *obj, xsEvent *evt);
	static void postEvent(xsObject *obj, xsEvent *evt);

	int allocFields(int count);

	inline void setFlag(xsU16 flag) { this->flag = flag; }
	inline void *getVMObject() { return this->vmObject; }
	inline void setVMObject(void *vmObject) { this->vmObject = vmObject; }

protected:
	char *id;
	void *vmObject;
	xsObject *parent;
	xsElementList *elements;

	xsObject();
	virtual ~xsObject();

private:
	xsU16 references;
	xsU16 flag;
	xsHashMap props;		// unclassified fields
};

enum
{
	XS_OBJ_NORMAL		= 0,
	XS_OBJ_RO_DATA		= 1		// Read-only data
};

#define XS_OBJ_CAST(obj)		((xsObject *)obj)
#define XS_OBJP_CAST(objPtr)	((xsObject **)objPtr)

xsObject *xsObjectCreate(const char *name);
void xsObjectRegister(xsClass *cls);

#endif /* _XS_OBJECT_H_ */
