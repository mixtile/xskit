#include "demoapp.h"
#include <xs/utils.h>
#include "canvasinterface.h"

#define BOX_SIZE	50

DemoApp::DemoApp(void)
{
	rateX = 1;
	rateY = 2;
}

DemoApp::~DemoApp(void)
{
}

int DemoApp::start()
{
	xsColor color = {255,255,255,255};
	xsGetScreenDimension(&width, &height);
	xsFillRectangle(xsGetSystemGc(), 0, 0, width, height, color);//, xsArgbToColor(0xFFFFFFFF));
	printf("width = %d, height = %d\n", width, height);
//	xsColor color1 = {255,255,0,0};
//	xsColor color2 = {255,0,255,0};
//	xsDrawBorderText(xsGetSystemGc(), "d d d d d d", 150, 150, 3, color1, color2, false);

	//xsGcTranslate(xsGetSystemGc(), 50, 50);
	xsFlushScreen(0, 0, width - 1, height - 1);

//	xs_canvas_demo();

	duktape_test();

	x = (width - BOX_SIZE) / 2;
	y = (height - BOX_SIZE) / 2;
	//img = this->getResource()->loadImage(xsT("logo-48x48.jpg"));
	//timer = xsStartTimer(20, _onTimeout, this);

	return XS_EC_OK;
}

void DemoApp::suspend()
{
}

int DemoApp::resume()
{
	return XS_EC_OK;
}

void DemoApp::exit()
{
	this->getResource()->freeImage(img);
}

int DemoApp::processEvent(xsEvent *e)
{
	switch (e->type)
	{
		case XS_EVT_START:
			return start();
			
		case XS_EVT_SUSPEND:
			suspend();
			break;
			
		case XS_EVT_RESUME:
			return resume();
			
		case XS_EVT_EXIT:
			exit();
			break;
			
		case XS_EVT_MOUSE_DOWN:
			xsStopTimer(timer);
			break;

		case XS_EVT_MOUSE_UP:
			timer = xsStartTimer(20, _onTimeout, this);
			break;

		case XS_EVT_KEY_DOWN:
			if (e->sys->data.key.keyCode == XS_PAD_KEY_CSK)
				xsStopTimer(timer);
			if(e ->sys ->data.key.keyCode == XS_PAD_KEY_LEFT_ARROW  || e ->sys ->data.key.keyCode == XS_PAD_KEY_UP_ARROW || e ->sys ->data.key.keyCode == XS_PAD_KEY_RIGHT_ARROW || e ->sys ->data.key.keyCode == XS_PAD_KEY_DOWN_ARROW)
				xsArrowKeysHandler(e);
			break;

		case XS_EVT_KEY_UP:
			if (e->sys->data.key.keyCode == XS_PAD_KEY_CSK)
				timer = xsStartTimer(20, _onTimeout, this);
			break;

		default:
			return xsCoreApplication::processEvent(e);
	}

	return XS_EC_OK;
}

int DemoApp::_onTimeout(void *userdata)
{
	return ((DemoApp *)userdata)->onTimeout();
}

int DemoApp::onTimeout()
{
	xsGraphics *gc = xsGetSystemGc();

	// clear
	xsColor color = {255, 255, 255, 255};
	xsFillRectangle(gc, x, y, BOX_SIZE, BOX_SIZE, color);//, xsArgbToColor(0xFFFFFFFF));

	// draw new box
	x += rateX;
	y += rateY;

	if (x < 0 || x + BOX_SIZE > width)
		rateX = -rateX;
	if (y < 0 || y + BOX_SIZE > height)
		rateY = -rateY;

	xsDrawImage(gc, img, x, y);
	
	xsFlushScreen(0, 0, width - 1, height - 1);

	return 0;
}
