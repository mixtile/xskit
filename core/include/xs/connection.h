#ifndef _XS_CONNECTION_H_
#define _XS_CONNECTION_H_

#include <xs/network.h>
#include <xs/uri.h>

typedef struct _xsConnResult
{
	int code;
	xsBuffer *buffer;
} xsConnResult;

enum _xsConnState
{
	XS_CONN_READY		= 0,
	XS_CONN_RESOLVING,
	XS_CONN_CONNECTING,
	XS_CONN_ESTABLISHED,
	XS_CONN_CLOSING,
	XS_CONN_END
};

class xsConnection : public xsListener
{
public:
	static xsConnection *createInstance(const char *url);
	void destroyInstance();

	int connect();
	int close();
	int send(xsBuffer *buffer);
	int receive(xsBuffer *buffer);

	virtual int processEvent(xsEvent *e);

	inline int getSock() { return sock; }

	void setProxy(xsProxy *proxy);
	inline void setListener(xsListener *listener) { this->listener = listener; }
	inline void setCallback(xsCallbackFunc cb) { callback.setCallback(cb); listener = &callback; }

protected:
	xsConnection(const char *url);
	virtual ~xsConnection();

private:
	void initial(const char *url, xsProxy *proxy);

	int doConnect();
	int doSend();
	int doRecv();

protected:
	xsU32 hash;	// string hash of uri.base
	int id;
	int state;

	char *uriStr;
	xsUri *uri;

	char *hostname;
	xsU16 port;

	xsProxy proxy;

	xsListener *listener;
	xsCallbackListener callback;

	xsSockAddr addr;
	int sock;

	xsQueue sendQ;
	xsQueue recvQ;
};

#endif /* _XS_CONNECTION_H_ */
