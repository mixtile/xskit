/*
 * beziercurve.h
 *
 *  Created on: 2015-7-20
 *      Author: lewis
 */

#ifndef BEZIERCURVE_H_
#define BEZIERCURVE_H_

#include <xs/object.h>
#include <xs/shape.h>

class xsBezierCurve: public xsShape
{
	XS_OBJECT_DECLARE(BezierCurve, XS_CLS_BEZIERCURVE)
public:
	float x1;
	float y1;
	float x2;
	float y2;
	float x3;
	float y3;
	float lineWidth;
	xsBool isCubic;

	xsColor strokeColor;
	xsColor fillColor;


	virtual void paint(xsGraphics *gc);
	void fill(xsGraphics *gc);
	void stroke(xsGraphics *gc);
};

#endif /* BEZIERCURVE_H_ */
