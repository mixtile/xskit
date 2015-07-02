#ifndef _XS_ELEMENTLIST_H_
#define _XS_ELEMENTLIST_H_

#include <xs/base.h>

class xsObject;

class xsElementList : public xsBase
{
public:
	xsElementList();
	~xsElementList();

	void add(xsObject *parent, xsArray *items);
	void add(xsObject *parent, xsObject *sub);
	xsObject *get(const char *id);
	xsObject *iterate(xsIterator *iter);
	xsObject *riterate(xsIterator *iter);
	void adopt(xsObject *parent);
	int count();
	void remove(const char *widId);
	void removeAll(xsBool destroy);

private:
	xsArrayList items;
};

#endif /* _XS_ELEMENTLIST_H_ */
