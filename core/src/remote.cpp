#include <xs/pal.h>
#include <xs/remote.h>
#include <xs/object.h>
//#include <xs/datasource.h>
//#include <xs/script.h>
#include <xs/string.h>
#include <xs/memory.h>

xsRemote::~xsRemote()
{
	if (cmd != NULL)
		xsFree(cmd);

//	if (ds != NULL)
//		ds->destroyInstance();
}

int xsRemote::processEvent(xsEvent *e)
{
	switch (e->type)
	{
	case XS_EVT_DATA_UPDATE:
		__onData((xsDataEvent *)e);
		break;
	}

	return 0;
}

xsBool xsRemote::invoke(const char *uri, xsCoreApplication *app, const char *cmdCallback)
{
	this->app = app;
	this->cmd = xsStrDup(cmdCallback);

//	ds = xsDataSource::alloc(uri, this);
	if (ds == NULL)
		return XS_FALSE;

	return XS_TRUE;
}

void xsRemote::__onData(xsDataEvent *e)
{
//	xsValue args[1];
	
//	args[0] = ((xsDataEvent *)e)->ds->get(NULL);

//	xsScriptCall(app, cmd, 1, args, NULL);
}
