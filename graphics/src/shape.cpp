#include <xs/pal.h>
#include <xs/shape.h>
#include <xs/line.h>
#include <xs/rectangle.h>
#include "xs/arc.h"
#include "xs/beziercurve.h"

void xsShape::paint(xsGraphics *gc)
{

}

void xsLine::paint(xsGraphics *gc)
{
	int loop;
	float offset = -width / 2;

	xsSetColor(gc, color);

	if (x2 - x1 > y2 - y1 || y1 == y2)
	{
		for (loop = 0; loop < width; loop++)
		{
			xsDrawLine(gc, x1, y1 + offset, x2, y2 + offset);
			offset++;
		}
	}
	else
	{
		for (loop = 0; loop < width; loop++)
		{
			xsDrawLine(gc, x1 + offset, y1, x2 + offset, y2);
			offset++;
		}
	}
}

void xsRectangle::paint(xsGraphics *gc)
{

}

void xsRectangle::fill(xsGraphics *gc)
{
	xsSetColor(gc, fillColor);
	xsFillRectangle(gc, x, y, width, height);
}

void xsRectangle::stroke(xsGraphics *gc)
{
	int i = 0;
	xsSetColor(gc, strokeColor);
	for(i = 0; i < lineWidth; i++)
	{
		xsDrawRectangle(gc, x - i, y - i, width + 2*i, height + 2*i);
	}
}

void xsArc::paint(xsGraphics *gc)
{

}

void xsArc::fill(xsGraphics *gc)
{
	xsSetColor(gc, fillColor);
	if(XS_FALSE == anticlockwise)
	{
		xsFillArc(gc, x, y, radius, startAngle, endAngle);
	}
	else
	{
		xsFillArc(gc, x, y, radius, endAngle, 360.0);
		xsFillArc(gc, x, y, radius, 0.0, startAngle);
	}
}

void xsArc::stroke(xsGraphics *gc)
{
	int i = 0;

	xsSetColor(gc, strokeColor);
	if(XS_FALSE == anticlockwise)
	{
		for(i = 0; i < lineWidth; i++)
		{
			xsDrawArc(gc, x, y, radius + i, startAngle, endAngle);
			xsDrawArc(gc, x, y, radius + i, endAngle , 360.0);
			xsDrawArc(gc, x, y, radius + i, 0, startAngle);
		}
	}
	else
	{
		for(i = 0; i < lineWidth; i++)
		{
			xsDrawArc(gc, x, y, radius + i, endAngle , 360.0);
			xsDrawArc(gc, x, y, radius + i, 0, startAngle);
		}
	}
}

void xsBezierCurve::paint(xsGraphics *gc)
{

}

void xsBezierCurve::fill(xsGraphics *gc)
{
	xsSetColor(gc, fillColor);
	if(XS_FALSE == isCubic)
	{
		xsFillQuadraticBezierCurve(gc, x1, y1, x2, y2, x3, y3);
	}
	else
	{
		xsFillCubicBezierCurve(gc, x1, y1, x2, y2, x3, y3);
	}
}

void xsBezierCurve::stroke(xsGraphics *gc)
{
	int i = 0;

	xsSetColor(gc, strokeColor);
	if(XS_FALSE == isCubic)
	{
		for(i = 0; i < lineWidth; i++)
		{
			xsDrawQuadraticBezierCurve(gc, x1, y1, x2, y2, x3, y3);
		}
	}
	else
	{
		for(i = 0; i < lineWidth; i++)
		{
			xsDrawCubicBezierCurve(gc, x1, y1, x2, y2, x3, y3);
		}
	}
}
