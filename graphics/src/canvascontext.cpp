#include <xs/canvascontext.h>
#include <xs/utils.h>
#include <xs/shape.h>
#include <xs/line.h>
#include <xs/value.h>

const float PI  = 3.1415926;//PI

xsCanvasContext::xsCanvasContext()
{
	lineWidth = 1.0;
	beginFlag = XS_FALSE;
	strokeColor = XS_COLOR_BLACK;
	fillColor = XS_COLOR_BLACK;
//	shadowColor = XS_COLOR_BLACK;
	textAlign = XS_TEXT_ALIGN_START;
	textBaseline = XS_BASELINE_MIDDLE;

	font.size = 12;
	font.style = XS_FONT_NORMAL;

	graphicsGroups = NULL;
	currentgraphics = NULL;
	rects = xsArrayListCreate(2);
	arcs = xsArrayListCreate(2);
	curves = xsArrayListCreate(2);
	statusArchive = xsStackCreate(2);
}

xsCanvasContext::~xsCanvasContext()
{
	clear();

	while(graphicsGroups != NULL)
	{
		xsGraphicsGroup * tmp = graphicsGroups ->next;
		xsArrayListDestroy(graphicsGroups ->lines);
		xsFree(graphicsGroups);
		graphicsGroups = tmp;
	}
	if (rects != NULL)
		xsArrayListDestroy(rects);
	if (arcs != NULL)
		xsArrayListDestroy(arcs);
	if (curves != NULL)
		xsArrayListDestroy(curves);
	if (statusArchive != NULL)
		xsStackDestroy(statusArchive);

	graphicsGroups = NULL;
	currentgraphics = NULL;
	rects = NULL;
	statusArchive = NULL;
}

//xsBool xsCanvasContext::getProperty(int propId, xsValue *value)
//{
//	switch (propId)
//	{
//	case XS_PROP_CANVASCONTEXT_LINEWIDTH:
//		value->type = XS_VALUE_INT32;
//		value->data.n = lineWidth;
//		break;
//	case XS_PROP_CANVASCONTEXT_STROKESTYLE:
//		value->type = XS_VALUE_INT32;
//		value->data.n = xsColorToArgb(strokeColor);
//		break;
//	default:
//		return xsObject::getProperty(propId, value);
//	}
//
//	return XS_TRUE;
//}

//xsBool xsCanvasContext::setProperty(int propId, xsValue *value)
//{
//	switch (propId)
//	{
//	case XS_PROP_CANVASCONTEXT_LINEWIDTH:
//		if (value->type == XS_VALUE_INT32)
//			lineWidth = value->data.n;
//		else
//			return XS_FALSE;
//		break;
//	case XS_PROP_CANVASCONTEXT_STROKESTYLE:
//		if (value->type == XS_VALUE_INT32)
//			xsArgbToColor(&strokeColor, value->data.n);
//		else
//			return XS_FALSE;
//		break;
//	default:
//		return xsObject::setProperty(propId, value);
//	}
//
//	return XS_TRUE;
//}

void xsCanvasContext::paintStroke(xsGraphics *gc)
{
	xsIterator iter = NULL;
	xsArrayList lines = NULL;
	xsLine *line = NULL;
	xsRectangle *rect = NULL;
	xsArc *arc = NULL;
	xsBezierCurve* curve = NULL;

	xsGraphicsGroup* groupTmp = graphicsGroups;
	while(groupTmp != NULL)
	{
		lines = groupTmp ->lines;
		for (;;)
		{
			line = static_cast<xsLine *>(xsArrayListIterate(lines, &iter));
			if (iter == NULL || line == NULL)
				break;

			line->paint(gc);
		}
		groupTmp = groupTmp ->next;
	}

	iter = NULL;
	for (;;)
	{
		rect = static_cast<xsRectangle *>(xsArrayListIterate(rects, &iter));
		if (iter == NULL || rect == NULL)
			break;

		rect->stroke(gc);
	}

	iter = NULL;
	for (;;)
	{
		arc = static_cast<xsArc *>(xsArrayListIterate(arcs, &iter));
		if (iter == NULL || arc == NULL)
			break;

		arc->stroke(gc);
	}

	iter = NULL;
	for (;;)
	{
		curve = static_cast<xsBezierCurve *>(xsArrayListIterate(curves, &iter));
		if (iter == NULL || curve == NULL)
			break;

		curve->stroke(gc);
	}
}
void xsCanvasContext::paintFill(xsGraphics *gc)
{
	xsIterator iter = NULL;
	xsArrayList lines = NULL;
	xsLine *line = NULL;
	xsRectangle *rect = NULL;
	xsArc *arc = NULL;
	xsBezierCurve* curve = NULL;

	for (;;)
	{
		arc = static_cast<xsArc *>(xsArrayListIterate(arcs, &iter));
		if (iter == NULL || arc == NULL)
			break;

		arc->fill(gc);
	}

	iter = NULL;
	for (;;)
	{
		rect = static_cast<xsRectangle *>(xsArrayListIterate(rects, &iter));
		if (iter == NULL || rect == NULL)
			break;

		rect->fill(gc);
	}

	iter = NULL;
	for (;;)
	{
		curve = static_cast<xsBezierCurve *>(xsArrayListIterate(curves, &iter));
		if (iter == NULL || curve == NULL)
			break;

		curve->fill(gc);
	}

	xsGraphicsGroup* groupTmp = graphicsGroups;
	while(groupTmp != NULL)
	{
		lines = groupTmp ->lines;
		int n = xsArrayListSize(lines);

		if(n < 2)
			return;
		line = static_cast<xsLine *>(xsArrayListGet(lines, 0));
		xsPoint *polygen = (xsPoint*)xsCalloc(sizeof(xsPoint)*(n + 1));
		polygen[0].x = line ->x1;
		polygen[0].y = line ->y1;
		int i = 1;
		iter = NULL;
		for (;;)
		{
			line = static_cast<xsLine *>(xsArrayListIterate(lines, &iter));
			if (iter == NULL || line == NULL)
				break;

			polygen[i].x = line ->x2;
			polygen[i].y = line ->y2;
			i++;
		}

		xsSetColor(gc, fillColor);
		xsFillPolygon(gc, polygen, n + 1);
		if(polygen != NULL)
		{
			xsFree(polygen);
			polygen = NULL;
		}
		groupTmp = groupTmp ->next;
	}

}

void xsCanvasContext::clear()
{
	xsIterator iter = NULL;
	xsArrayList lines;
	xsLine *line;
	xsRectangle *rect;
	xsArc *arc;

	while(graphicsGroups != NULL)
	{
		lines = graphicsGroups ->lines;
		for (;;)
		{
			line = static_cast<xsLine *>(xsArrayListIterate(lines, &iter));
			if (iter == NULL || line == NULL)
				break;

			line->destroyInstance();
		}
		graphicsGroups = graphicsGroups ->next;
	}

	iter = NULL;
	for (;;)
	{
		rect = static_cast<xsRectangle *>(xsArrayListIterate(rects, &iter));
		if (iter == NULL || rect == NULL)
			break;

		rect->destroyInstance();
	}

	xsArrayListRemoveAll(rects);

	iter = NULL;
	for (;;)
	{
		arc = static_cast<xsArc *>(xsArrayListIterate(arcs, &iter));
		if (iter == NULL || rect == NULL)
			break;

		arc->destroyInstance();
	}

	xsArrayListRemoveAll(arcs);
}

void xsCanvasContext::createGraphics()
{
	if(NULL == graphicsGroups)
	{
		currentgraphics = (xsGraphicsGroup *)xsCalloc(sizeof(xsGraphicsGroup));
		currentgraphics ->lines = xsArrayListCreate(2);
		currentgraphics ->next = NULL;
		graphicsGroups = currentgraphics;

	}
	else
	{
		xsGraphicsGroup *newgraphics =  (xsGraphicsGroup *)xsCalloc(sizeof(xsGraphicsGroup));
		newgraphics ->lines = xsArrayListCreate(2);
		newgraphics ->next = NULL;
		currentgraphics ->next = newgraphics;
		currentgraphics = newgraphics;
	}
}

void xsCanvasContext::addLine(xsLine *line)
{
	XS_ASSERT(line != NULL);

	if (currentgraphics != NULL && currentgraphics ->lines != NULL)
	{
		xsArrayListAdd(currentgraphics ->lines, line);
	}
}

void xsCanvasContext::addRect(xsRectangle *rect)
{
	XS_ASSERT(rect != NULL);

	if (rects == NULL)
		rects = xsArrayListCreate(2);

	xsArrayListAdd(rects, rect);
}

void xsCanvasContext::addArc(xsArc *arc)
{
	XS_ASSERT(arc != NULL);

	if (arcs == NULL)
		arcs = xsArrayListCreate(2);

	xsArrayListAdd(arcs, arc);
}

void xsCanvasContext::addCurve(xsBezierCurve *curve)
{
	XS_ASSERT(curve != NULL);

	if (curves == NULL)
		curves = xsArrayListCreate(2);

	xsArrayListAdd(curves, curve);
}

void xsCanvasContext::addStatus(xsCanvasAttribute* status)
{
	XS_ASSERT(status != NULL);

	if (statusArchive == NULL)
		statusArchive = xsStackCreate(2);

	xsStackPush(statusArchive, status);
}

void xsCanvasContext::moveTo(float x, float y)
{
	beginFlag = XS_TRUE;;
	lastX = x;
	lastY = y;
	beginX = x;
	beginY = y;

	createGraphics();
}

void xsCanvasContext::lineTo(float x, float y)
{
	if(XS_FALSE == beginFlag)
	{
		beginFlag = XS_TRUE;
		lastX = x;
		lastY = y;
		beginX = x;
		beginY = y;

		createGraphics();

		return;
	}
	xsLine *line = static_cast<xsLine *>(xsLine::createInstance());
	line->x1 = lastX;
	line->y1 = lastY;
	line->x2 = x;
	line->y2 = y;
	line->color = strokeColor;
	line->width = lineWidth > 1.0 ? lineWidth : 1.0;
	addLine(line);

	lastX = x;
	lastY = y;
}

void xsCanvasContext::stroke()
{
	xsGraphics *gc = xsGetSystemGc();
	paintStroke(gc);
	xsRect 	clientRect;
	if(xsGetClientRect(&clientRect))
	{
		xsFlushScreen(clientRect.left, clientRect.top, clientRect.right, clientRect.bottom);
	}
}

void xsCanvasContext::fill()
{
	xsGraphics *gc = xsGetSystemGc();
	paintFill(gc);
	xsRect 	clientRect;
	if(xsGetClientRect(&clientRect))
	{
		xsFlushScreen(clientRect.left, clientRect.top, clientRect.right, clientRect.bottom);
	}
}

void xsCanvasContext::rect(float x, float y, float width, float height)
{
	beginFlag = XS_TRUE;
	lastX = x;
	lastY = y;
	beginX = x;
	beginY = y;

	createGraphics();

	xsRectangle *rect = static_cast<xsRectangle *>(xsRectangle::createInstance());
	rect ->x = x;
	rect ->y = y;
	rect ->width = width;
	rect ->height = height;
	rect ->lineWidth = lineWidth > 1.0 ? lineWidth : 1.0;
	rect ->fillColor = fillColor;
	rect ->strokeColor = strokeColor;
	addRect(rect);

}

void xsCanvasContext::fillRect(float x, float y, float width, float height)
{
	xsGraphics *gc = xsGetSystemGc();
	xsSetColor(gc, fillColor);
	xsFillRectangle(gc, x, y, width, height);
	xsFlushScreen(x, y, x + width, y + height);
}

void xsCanvasContext::strokeRect(float x, float y, float width, float height)
{
	xsGraphics *gc = xsGetSystemGc();
	int i;
	lineWidth = lineWidth > 1.0 ? lineWidth : 1.0;
//another way to draw rectangle with specified linwWidth.
//	xsFillRectangle(gc, x - lineWidth + 1, y - lineWidth + 1, width + 2 * (lineWidth - 1), height + 2 * (lineWidth - 1), strokeColor);
//	xsFillRectangle(gc, x, y, width, height, XS_COLOR_WHITE);
	xsSetColor(gc, strokeColor);

	for(i = 0; i < lineWidth; i++)
	{
		xsDrawRectangle(gc, x - i, y - i, width + 2*i, height + 2*i);
	}
	xsFlushScreen(x - lineWidth, y - lineWidth, x + width + 2*lineWidth, y + height + 2*lineWidth);
}

void xsCanvasContext::clearRect(float x, float y, float width, float height)
{
	xsGraphics *gc = xsGetSystemGc();
	xsSetColor(gc, XS_COLOR_WHITE);
	xsFillRectangle(gc, x, y, width, height);
	xsFlushScreen(x, y, x + width, y + height);
}
void xsCanvasContext::arc(float x, float y, float radius, float startAngle, float endAngle, xsBool anticlockwise)
{
	//MTK support only 0-360 angle.
	xsArc *arc = static_cast<xsArc *>(xsArc::createInstance());
	arc ->x = x;
	arc ->y = y;
	arc ->radius = radius;
	arc ->startAngle = startAngle;
	arc ->endAngle = endAngle;
	arc ->anticlockwise = anticlockwise;
	arc ->lineWidth = lineWidth;
	arc ->strokeColor = strokeColor;
	arc ->fillColor = fillColor;
	addArc(arc);
}
void xsCanvasContext::arcTo(float x1, float y1, float x2, float y2, float radius)
{
//	float k = (y2 - y1)/(x2 - x1);
//	float centreB = (y1 + y2)/2 + (x1 + x2)/(2*k);
//	float centreX = (y2 * y2 + x2 * x2 - y1 * y1 - x1 * x1)/(2 * (x2 - x1 - centreB/k * (y2 - y1)));
//	float centreY = - (centreX/k) + centreB;
//	float alpha;
}

void xsCanvasContext::beginPath()
{
	beginFlag = XS_FALSE;
	clear();
}

void xsCanvasContext::closePath()
{
	lineTo(beginX, beginY);
	lastX = beginX;
	lastY = beginY;
}

void xsCanvasContext::bezierCurveTo(float cp1x, float cp1y, float cp2x, float cp2y, float x, float y)
{
	xsBezierCurve *curve = static_cast<xsBezierCurve *>(xsBezierCurve::createInstance());
	curve ->x1 = lastX;
	curve ->y1 = lastY;
	curve ->x2 = cp1x;
	curve ->y2 = cp1y;
	curve ->x3 = cp2x;
	curve ->y3 = cp2y;
	curve ->x4 = x;
	curve ->y4 = y;
	curve ->fillColor = fillColor;
	curve ->strokeColor = strokeColor;
	curve ->lineWidth = lineWidth;
	curve ->isCubic = XS_TRUE;
	addCurve(curve);
}

void xsCanvasContext::quadraticCurveTo(float qcpx, float qcpy, float qx, float qy)
{
	xsBezierCurve *curve = static_cast<xsBezierCurve *>(xsBezierCurve::createInstance());
	curve ->x1 = lastX;
	curve ->y1 = lastY;
	curve ->x2 = qcpx;
	curve ->y2 = qcpy;
	curve ->x3 = qx;
	curve ->y3 = qy;
	curve ->fillColor = fillColor;
	curve ->strokeColor = strokeColor;
	curve ->lineWidth = lineWidth;
	curve ->isCubic = XS_FALSE;
	addCurve(curve);
}

void xsCanvasContext::translate(float x, float y)
{
	xsGraphics *gc = xsGetSystemGc();
	xsGcTranslate(gc, x, y);
	xsRect 	clientRect;
	if(xsGetClientRect(&clientRect))
	{
		xsFlushScreen(clientRect.left, clientRect.top, clientRect.right, clientRect.bottom);
	}
}
void xsCanvasContext::scale(float scalewidth, float scaleheight)
{
	xsGcScale(xsGetSystemGc(), scalewidth, scaleheight);
}

void xsCanvasContext::rotate(float angle)
{
	xsGcRotate(xsGetSystemGc(), angle);
}

void	xsCanvasContext::transform(float xx, float yx, float xy, float yy, float x0, float y0)
{
	xsGcTransform(xsGetSystemGc(), xx, yx, xy, yy, x0, y0);
}

bool xsCanvasContext::isPointInPath(float x, float y)
{
	return XS_FALSE;
}

void xsCanvasContext::drawImage(xsImage* image,float x, float y)
{
	xsGraphics *gc = xsGetSystemGc();
	float originW,  originH;
	xsGetImageDimension(image, &originW, &originH);
	xsDrawImage(gc, image, x, y, 0, 0);
	xsRect clientRect;
	xsFlushScreen(x, y, originW, originH);
}

void xsCanvasContext::drawImage(xsImage* image,float x, float y, float width, float height)
{
	xsGraphics *gc = xsGetSystemGc();
	float originW,  originH;
	xsGetImageDimension(image, &originW, &originH);
	xsDrawImage(gc, image, x, y, width, height);
	xsFlushScreen(x, y, x + width, y + height);
}

void xsCanvasContext::drawImage(xsImage* image,float sx, float sy, float swidth, float sheight, float x, float y, float width, float height)
{
	xsGraphics *gc = xsGetSystemGc();
	xsRect clipRect;
	clipRect.left = x;
	clipRect.top = y;
	clipRect.right = x + width;
	clipRect.bottom = y + height;
	xsSetClipRect(gc, &clipRect);
	float originW,  originH;
	xsGetImageDimension(image, &originW, &originH);
	xsDrawImage(gc, image, x - sx, y - sy, originW * width/swidth, originH*height/sheight);
	xsResetClipRect(gc);
	xsFlushScreen(clipRect.left, clipRect.top, clipRect.right, clipRect.bottom);
}

void xsCanvasContext::clip()
{

}

void xsCanvasContext::drawWithBaseline(const xsTChar* text, int count, float x, float y, float maxWidth, int drawFlag)
{
	xsGraphics *gc = xsGetSystemGc();
	float width, height;
	xsMeasureText(gc, text, count, &font, &width, &height);
	if(drawFlag == 0)//stroke
	{
		switch(textBaseline)
		{
		case XS_BASELINE_ALPHABETIC:
			break;
		case XS_BASELINE_TOP:
			xsDrawBorderText(gc, text, count, x, y, maxWidth, XS_COLOR_WHITE,strokeColor, XS_TRUE);
			break;
		case XS_BASELINE_HANGING:
			xsDrawBorderText(gc, text, count, x, y, maxWidth, XS_COLOR_WHITE,strokeColor, XS_TRUE);
			break;
		case XS_BASELINE_MIDDLE:
			xsDrawBorderText(gc, text, count, x, y + height/2, maxWidth, XS_COLOR_WHITE,strokeColor, XS_TRUE);
			break;
		case XS_BASELINE_IDEOGRAPHIC:
			break;
		case XS_BASELINE_BOTTOM:
			xsDrawBorderText(gc, text, count, x, y - height/2, maxWidth, XS_COLOR_WHITE,strokeColor, XS_TRUE);
			break;
		}
	}
	else if(drawFlag == 1)//fill
	{
		switch(textBaseline)
		{
		case XS_BASELINE_ALPHABETIC:
			break;
		case XS_BASELINE_TOP:
			xsDrawBorderText(gc, text, count, x, y, maxWidth, fillColor, XS_COLOR_WHITE, XS_FALSE);
			break;
		case XS_BASELINE_HANGING:
			break;
		case XS_BASELINE_MIDDLE:
			xsDrawBorderText(gc, text, count, x, y + height/2, maxWidth, fillColor, XS_COLOR_WHITE, XS_FALSE);
			break;
		case XS_BASELINE_IDEOGRAPHIC:
			break;
		case XS_BASELINE_BOTTOM:
			xsDrawBorderText(gc, text, count, x, y - height/2, maxWidth, fillColor, XS_COLOR_WHITE, XS_FALSE);
			break;
		}
	}
	int screenWidth, screenHeight;
	xsGetScreenDimension(&screenWidth, &screenHeight);
	xsFlushScreen(0, 0, screenWidth - 1, screenHeight - 1);
}

void xsCanvasContext::fillText(const xsTChar *text , float x, float y, xsU32 maxWidth)
{
	xsGraphics *gc = xsGetSystemGc();
	xsSetFont(gc, &font);
	float width, height;
	int count = xsTcsLen(text);
	xsMeasureText(gc, text, count, &font, &width, &height);
	maxWidth = maxWidth > width ? width : maxWidth;
	switch(textAlign)
	{
	case XS_TEXT_ALIGN_START:
		drawWithBaseline(text, count, x, y, maxWidth, 1);
		break;
	case XS_TEXT_ALIGN_END:
		drawWithBaseline(text, count, x - width, y, maxWidth, 1);
		break;
	case XS_TEXT_ALIGN_LEFT:
		drawWithBaseline(text, count, x, y, maxWidth, 1);
		break;
	case XS_TEXT_ALIGN_CENTER:
		drawWithBaseline(text, count, x - width/2, y, maxWidth, 1);
		break;
	case XS_TEXT_ALIGN_RIGHT:
		drawWithBaseline(text, count, x - width, y, maxWidth, 1);
		break;
	}
}

void xsCanvasContext::strokeText(const xsTChar *text , float x, float y, xsU32 maxWidth)
{
	xsGraphics *gc = xsGetSystemGc();
	xsSetFont(gc, &font);
	float width, height;
	int count = xsTcsLen(text);
	xsMeasureText(gc, text, count, &font, &width, &height);
	maxWidth = maxWidth > width ? width : maxWidth;
	switch(textAlign)
	{
	case XS_TEXT_ALIGN_START:
		drawWithBaseline(text, count, x, y, maxWidth,0);
		break;
	case XS_TEXT_ALIGN_END:
		drawWithBaseline(text, count, x - width, y, maxWidth, 0);
		break;
	case XS_TEXT_ALIGN_LEFT:
		drawWithBaseline(text, count, x, y, maxWidth, 0);
		break;
	case XS_TEXT_ALIGN_CENTER:
		drawWithBaseline(text, count, x - width/2, y, maxWidth, 0);
		break;
	case XS_TEXT_ALIGN_RIGHT:
		drawWithBaseline(text, count, x - width, y, maxWidth, 0);
		break;
	}
}

xsTextSize xsCanvasContext::measureText(const xsTChar *text)
{
	xsGraphics *gc = xsGetSystemGc();
	xsTextSize size;
	xsMeasureText(gc, text, xsTcsLen(text), &font, &size.width, &size.height);
	return size;
}

void xsCanvasContext::save()
{
	xsCanvasAttribute* newStatus = (xsCanvasAttribute*)xsCalloc(sizeof(xsCanvasAttribute));
	newStatus ->fillColor = fillColor;
	newStatus ->strokeColor = strokeColor;
//	newStatus ->shadowColor = shadowColor;
//	newStatus ->shadowBlur = shadowBlur;
//	newStatus ->shadowOffsetX = shadowOffsetX;
//	newStatus ->shadowOffsetY = shadowOffsetY;
	newStatus ->lineWidth = lineWidth;
//	newStatus ->globalCompositeOperation = globalCompositeOperation;
//	newStatus ->globalAlpha = globalAlpha;
	newStatus ->font = font;
	newStatus ->textAlign = textAlign;
	newStatus ->textBaseline = textBaseline;

	addStatus(newStatus);

}

void xsCanvasContext::restore()
{
	xsCanvasAttribute* oldStatus = static_cast<xsCanvasAttribute*>(xsStackPop(statusArchive));
	if(NULL != oldStatus)
	{
		fillColor = oldStatus ->fillColor;
		strokeColor = oldStatus ->strokeColor;
//		shadowColor = oldStatus ->shadowColor;
//		shadowBlur = oldStatus ->shadowBlur;
//		shadowOffsetX = oldStatus ->shadowOffsetX;
//		shadowOffsetY = oldStatus ->shadowOffsetY;
		lineWidth = oldStatus ->lineWidth;
//		globalCompositeOperation = oldStatus ->globalCompositeOperation;
//		globalAlpha = oldStatus ->globalAlpha;
		font = oldStatus ->font;
		textAlign = oldStatus ->textAlign;
		textBaseline = oldStatus ->textBaseline;
		xsFree(oldStatus);
	}
}
