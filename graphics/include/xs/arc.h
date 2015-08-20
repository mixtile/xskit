/*
 * arc.h
 *
 *  Created on: 2015-7-17
 *      Author: lewis
 */

#ifndef ARC_H_
#define ARC_H_

#include <xs/object.h>
#include <xs/shape.h>

class xsArc: public xsShape
{
	XS_OBJECT_DECLARE(Arc, XS_CLS_ARC)
public:
	float x;
	float y;
	float radius;
	float startAngle;
	float endAngle;
	float lineWidth;
	xsBool anticlockwise;

	xsColor strokeColor;
	xsColor fillColor;

	virtual void paint(xsGraphics *gc);
	void fill(xsGraphics *gc);
	void stroke(xsGraphics *gc);
};


#endif /* ARC_H_ */
