#ifndef _XS_CONNMANAGER_H_
#define _XS_CONNMANAGER_H_

#include <xs/container.h>
#include <xs/event.h>

class xsConnection;

class xsConnManager
{
public:
	static int init(void);
	static void uninit(void);

	static int checkin(xsConnection *conn);
	static void checkout(xsConnection *conn);

	static int processEvent(xsEvent *e);

private:

	static xsConnection *getById(int id);
	static xsConnection *getBySock(int sock);

	static xsArrayList _connList;
	static xsStack _connUnused;
};

#endif /* _XS_CONNMANAGER_H_ */
