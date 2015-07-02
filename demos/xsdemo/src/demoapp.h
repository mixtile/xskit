#ifndef _DEMOAPP_H_
#define _DEMOAPP_H_

#include <xs/coreapplication.h>

class DemoApp : public xsCoreApplication
{
public:
	DemoApp(void);
	~DemoApp(void);

	int start();
	void suspend();
	int resume();
	void exit();

	int processEvent(xsEvent *e);

private:
	int width, height;
	xsU32 timer;
	int x, y;
	int rateX, rateY;
	xsImage *img;

	static int _onTimeout(void *userdata);
	int onTimeout();
};

#endif /* _DEMOAPP_H_ */
