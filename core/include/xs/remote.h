#ifndef _XS_REMOTE_H_
#define _XS_REMOTE_H_

#include <xs/object.h>
#include <xs/uri.h>
#include <xs/value.h>

class xsCoreApplication;
class xsDataSource;

class xsRemote : public xsObject
{
	XS_OBJECT_DECLARE(Remote, XS_CLS_REMOTE)
public:
	xsBool invoke(const char *uri, xsCoreApplication *app, const char *cmdCallback);

protected:
	~xsRemote();

	int processEvent(xsEvent *e);

private:
	xsCoreApplication *app;
	xsDataSource *ds;
	char *cmd;

	void __onData(xsDataEvent *e);
};

#endif /* _XS_REMOTE_H_ */
