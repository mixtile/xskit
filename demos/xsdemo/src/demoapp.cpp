#include "DemoApp.h"
#include <xs/utils.h>

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
	xsGetScreenDimension(&width, &height);
	xsFillRectangle(xsGetSystemGc(), 0, 0, width, height);//, xsArgbToColor(0xFFFFFFFF));
	xsFlushScreen(0, 0, width - 1, height - 1);

	x = (width - BOX_SIZE) / 2;
	y = (height - BOX_SIZE) / 2;
	img = this->getResource()->loadImage(xsT("logo-48x48.jpg"));
	timer = xsStartTimer(20, _onTimeout, this);

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
	case XS_EVT_MOUSE_DOWN:
		xsStopTimer(timer);
		break;

	case XS_EVT_MOUSE_UP:
		timer = xsStartTimer(20, _onTimeout, this);
		break;

	case XS_EVT_KEY_DOWN:
		if (e->sys->data.key.keyCode == XS_PAD_KEY_CSK)
			xsStopTimer(timer);
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
	xsFillRectangle(gc, x, y, BOX_SIZE, BOX_SIZE);//, xsArgbToColor(0xFFFFFFFF));

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
