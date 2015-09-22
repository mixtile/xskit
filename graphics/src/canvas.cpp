#include <xs/pal.h>
#include <xs/canvas.h>
#include <xs/memory.h>
#include <xs/coreclassid.h>
#include <xs/string.h>
#include <xs/resource.h>

xsCanvas::xsCanvas()
{
	context = NULL;
	setFlag(0);
	xsGetScreenDimension(&width, &height);
}

xsCanvas::~xsCanvas()
{
	if (context != NULL)
		context->destroyInstance();
}

int xsCanvas::getWidth(void)
{
	return width;
}
int xsCanvas::getHeight(void)
{
	return height;
}

void xsCanvas::getPreferredSize(int proposedWidth, xsMetrics *m)
{
	//m->width = width;
	//m->height = height;
}

void xsCanvas::paint(xsGraphics *gc)
{
	if (context == NULL)
		return;

	//context->paint(gc);
}

xsCanvasContext *xsCanvas::getContext()
{
	if (context != NULL)
		return context;

	context = static_cast<xsCanvasContext *>(xsCanvasContext::createInstance());
	return context;
}

