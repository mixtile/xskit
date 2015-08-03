
#include "xsdemo.h"
#include "demoapp.h"

#include <xs/entry.h>
#include <xs/core.h>

void *xsCreateApplication()
{
	return new DemoApp();
}

void xsDestroyApplication(void *app)
{
	((xsObject *)app)->destroyInstance();
}

int xsInvokeApplication(void *app, xsEvent *e)
{
	return ((xsObject *)app)->processEvent(e);
}

int xsLibInit()
{
	return xsCoreInit();
}

void xsLibUninit()
{
	xsCoreUninit();
}
