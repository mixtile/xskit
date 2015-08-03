#ifndef _XS_LINE_H_
#define _XS_LINE_H_

#include <xs/object.h>
#include <xs/shape.h>

class xsLine : public xsShape
{
	XS_OBJECT_DECLARE(Line, XS_CLS_LINE)
public:
	float x1;
	float y1;
	float x2;
	float y2;
	float width;
	xsColor color;

	virtual void paint(xsGraphics *gc);
};

#endif /* _XS_LINE_H_ */
