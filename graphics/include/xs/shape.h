#ifndef _XS_SHAPE_H_
#define _XS_SHAPE_H_

#include <xs/object.h>

class xsShape : public xsObject
{
	XS_OBJECT_DECLARE(Shape, XS_CLS_SHAPE)
public:
	virtual void paint(xsGraphics *gc);
};

#endif /* _XS_SHAPE_H_ */
