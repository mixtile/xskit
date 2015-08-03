#ifndef _XS_CANVAS_H_
#define _XS_CANVAS_H_

#include "xs/object.h"
#include "canvascontext.h"

class xsCanvas: public xsObject
{
XS_OBJECT_DECLARE(Canvas, XS_CLS_CANVAS)

public:
	xsCanvas();
	virtual ~xsCanvas();

public:
	xsCanvasContext *getContext();

	void getPreferredSize(int proposedWidth, xsMetrics *m);
	void paint(xsGraphics *gc);

private:
	xsCanvasContext *context;
};

#endif /* _XS_CANVAS_H_ */
