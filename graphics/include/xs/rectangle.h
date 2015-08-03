/*
 * rectangle.h
 *
 *  Created on: 2015-6-12
 *      Author: lewis
 */

#ifndef _XS_RECTANGLE_H_
#define _XS_RECTANGLE_H_

#include <xs/object.h>
#include <xs/shape.h>

class xsRectangle : public xsShape
{
	XS_OBJECT_DECLARE(Rectangle, XS_CLS_RECTANGLE)
public:
	float x;
	float y;
	float width;
	float height;
	float lineWidth;
	xsColor strokeColor;
	xsColor fillColor;

	virtual void paint(xsGraphics *gc);
	void fill(xsGraphics *gc);
	void stroke(xsGraphics *gc);
};


#endif /* _XS_RECTANGLE_H_ */
