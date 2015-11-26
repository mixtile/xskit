#include <xs/network.h>
#include <xs/connection.h>
#include <xs/connmanager.h>
#include <xs/launcherexport.h>

xsArrayList xsConnManager::_connList = NULL;
xsStack xsConnManager::_connUnused = NULL;

int xsConnManager::init()
{
	XS_TRACE("[NET]xsConnManInit");

	if (_connList != NULL)
		return XS_EC_OK;

	xsSocketInitialize();

	_connList = xsArrayListCreate(2);
	_connUnused = xsStackCreate(2);

	return XS_EC_OK;
}

void xsConnManager::uninit()
{
	XS_TRACE("[NET]xsConnManUninit");
	if (_connList != NULL)
	{// remove all
		xsIterator iter = NULL;
		xsConnection *conn;

		for (;;)
		{
			conn = (xsConnection *)xsArrayListIterate(_connList, &iter);
			if (iter == NULL)
				break;

			conn->destroyInstance();
		}

		xsArrayListDestroy(_connList);
		_connList = NULL;
	}

	if (_connUnused != NULL)
	{
		xsStackDestroy(_connUnused);
		_connUnused = NULL;
	}

	xsSocketUninitialize();
}

int xsConnManager::processEvent(xsEvent *e)
{
	xsConnection *conn = NULL;

	if (e->type == XS_EVT_RESOLVE)
	{
		conn = getById(e->sys->data.resolve.tag);
		if (conn == NULL)
		{
			XS_ERROR("[NET]xsConnResolveHandler: wrong tag id %d",
					e->sys->data.resolve.tag);
			return XS_EC_ERROR;
		}
	}
	else
	{
		conn = getBySock(e->sys->data.socket.sock);
		if (conn == NULL)
		{
			XS_ERROR("[NET]xsConnHandler: wrong sock %d",
					e->sys->data.socket.sock);
			return XS_EC_ERROR;
		}
	}

	return conn->processEvent(e);
}

int xsConnManager::checkin(xsConnection *conn)
{
	int unused = (int)xsStackPop(_connUnused);
	int index;

	XS_TRACE("[NET]xsConnManAdd");
	if (unused == 0)
		index = xsArrayListAdd(_connList, conn);
	else
		index = xsArrayListSet(_connList, unused - 1, conn);

	return index;
}

void xsConnManager::checkout(xsConnection *conn)
{
	int index;

	XS_TRACE("[NET]xsConnManRemove");
	index = xsArrayListIndex(_connList, conn);
	if (index >= 0)
	{
		xsArrayListSet(_connList, index, NULL); // keep order
		xsStackPush(_connUnused, (void *)index);
	}
}

xsConnection *xsConnManager::getById(int id)
{
	return (xsConnection *)xsArrayListGet(_connList, id);
}

xsConnection *xsConnManager::getBySock(int sock)
{
	xsIterator iter = NULL;
	xsConnection *conn;

	for (;;)
	{
		conn = (xsConnection *)xsArrayListIterate(_connList, &iter);
		if (iter == NULL)
			break;

		if (conn != NULL && conn->getSock() == sock)
			return conn;
	}

	return NULL; // Not found
}

int xsConnCheckin(xsConnection *conn)
{
	return xsConnManager::checkin(conn);
}

void xsConnCheckout(xsConnection *conn)
{
	xsConnManager::checkout(conn);
}
