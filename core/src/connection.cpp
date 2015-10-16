/*
 * Copyright (C) 2015 Focalcrest, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <xs/pal.h>
#include <xs/network.h>
#include <xs/connection.h>
#include <xs/memory.h>
#include <xs/string.h>
#include <xs/error.h>
#include <xs/value.h>
#include <xs/buffer.h>
#include <xs/uri.h>
#include <xs/bon.h>

extern "C" {
int xsConnCheckin(xsAny conn);
void xsConnCheckout(xsAny conn);
}

xsConnection::xsConnection(const char *url)
{
	XS_TRACE("URL:%s", url);

	//id = xsConnCheckin(this);
	sock = -1;

	xsUri *uri = xsUriParse(url);

	XS_TRACE("[NET]xsConnCreate");
	if (uri == NULL || uri->base == NULL)
		return;

	this->uri = uri;
	uriStr = xsStrDup(url);
	hash = xsBonHashName(uri->base, 0); // TODO: do not use special purpose hash

	// 默认使用直接连接
	const char *portStr;

	// parse hostname
	portStr = xsStrChr(uri->base, ':');
	if (portStr == NULL)
	{
		port = 0; // wait for initialize
		hostname = xsStrDup(uri->base);
	}
	else
	{
		port = (xsU16)xsStrToInt(portStr + 1);
		hostname = xsStrDupN(uri->base, portStr - uri->base);
	}
}

xsConnection::~xsConnection()
{
	if (sock >= 0)
		//xsSocketClose(sock);

	if (listener)
	{
		xsConnEvent evt;

		// construct event
		evt.type = XS_EVT_CONN_CLOSED;
		evt.result = 0; // TODO: last error code

		// Notice listener
		xsEvent::send(listener, &evt);
	}
	
	if (this->uriStr != NULL)
		xsFree(this->uriStr);

	if (this->uri != NULL)
		xsUriDestroy(this->uri);
	
	if (this->hostname)
		xsFree(this->hostname);

	if (this->sendQ != NULL)
	{
		xsConnEvent evt;
		xsConnResult result;
		xsBuffer *buf;

		evt.type = XS_EVT_CONN_SENT;
		evt.result = &result;
		result.code = XS_CONN_RES_CLOSED;

		while ((buf = (xsBuffer *)xsQueuePop(this->sendQ)) != NULL)
		{
			result.buffer = buf;
			xsEvent::send(this, &evt);
		}

		xsQueueDestroy(this->sendQ);
	}

	if (this->recvQ != NULL)
	{
		xsQueueDestroy(this->recvQ);
	}

	//xsConnCheckout(this);
}

void xsConnection::setProxy(xsProxy *proxy)
{
	this->proxy = *proxy;
	this->proxy.hostname = xsStrDup(proxy->hostname);

	// free old hostname (maybe proxy.hostname)
	xsReplaceAny((void **)&hostname, this->proxy.hostname);
	port = proxy->port;
}

xsConnection *xsConnection::createInstance(const char *url)
{
	xsConnection *conn;

	conn = new xsConnection(url);
	if (conn == NULL)
		return NULL;

	if (conn->id < 0)
	{
		delete conn;
		return NULL;
	}

	return conn;
}

void xsConnection::destroyInstance()
{
	XS_TRACE("Destroy xsConnection");
	delete this;
}

int xsConnection::connect()
{
	XS_TRACE("[NET]xsConnEstablish");
	if (xsStrToAddr(hostname, &addr) != XS_EC_OK)
	{// need resolv domain name
		int err = xsGetAddrByName(hostname, &addr, id);
		if (err == XS_EC_IN_PROGRESS)
		{
			state = XS_CONN_RESOLVING;
			return XS_EC_IN_PROGRESS;
		}
		else if (err < 0)
		{
			XS_ERROR("[NET]xsConnEstablish: Resolve hostname failed. ret:%d", err);
			return err;
		}
	}

	XS_TRACE("[NET]xsConnEstablish: Hostname is in cache");
	return doConnect();
}

int xsConnection::doConnect()
{
	int ret = XS_EC_ERROR;

	XS_TRACE("[NET]DoConnect");
	// Has address, connect
	addr.port = xsHtons(port);
	//sock = xsSocketCreate();

	if (sock < 0)
	{
		XS_ERROR("Create socket failed. ret:%d", sock);

		// destroy self
		destroyInstance();
		return sock;
	}

	XS_TRACE("connecting");
	state = XS_CONN_CONNECTING;
	//ret = xsSocketConnect(sock, &addr);
	if (ret < 0)
	{
		if (ret == XS_EC_IN_PROGRESS)
			return ret;

		// other error code, indicate failure
		XS_ERROR("Connect failed. ret:%d", ret);
		// destroy self
		destroyInstance();
		return ret;
	}

	return ret;
}

int xsConnection::close()
{
	XS_TRACE("[NET]xsConnShutdown");
	return xsSocketClose(sock);
}

int xsConnection::send(xsBuffer *buffer)
{
	XS_TRACE("[NET]xsConnSend");
	if (sendQ == NULL)
		sendQ = xsQueueCreate(2);

	if (sendQ == NULL)
		return XS_EC_NOMEM;

	xsQueuePush(sendQ, buffer);

	return doSend();
}

int xsConnection::receive(xsBuffer *buffer)
{
	XS_TRACE("[NET]xsConnReceive");
	if (recvQ == NULL)
		recvQ = xsQueueCreate(2);

	if (recvQ == NULL)
		return XS_EC_NOMEM;

	xsQueuePush(recvQ, buffer);

	return XS_EC_IN_PROGRESS;
}


int xsConnection::doSend()
{
	xsBuffer *buf;
	int sent;
	xsConnEvent evt;
	xsConnResult result;

	XS_TRACE("[NET]ConnDoSend");
	evt.type = XS_EVT_CONN_SENT;
	evt.result = &result;

	for (;;)
	{
		buf = (xsBuffer *)xsQueueFront(sendQ);
		if (buf == NULL)
			break;

		sent = xsSocketSend(sock, buf->head, buf->len);
		if (sent > 0)
		{
			xsBufferConsume(buf, sent);

			if (buf->len == 0)
			{
				xsQueuePop(sendQ);

				result.code = XS_CONN_RES_OK;
				result.buffer = buf;
				xsEvent::send(this, &evt);
			}
		}
		else if (sent == 0)
		{
			result.code = XS_CONN_RES_CLOSED;
			result.buffer = buf;
			xsEvent::send(this, &evt);
			break;
		}
		else
		{
			if (sent != XS_EC_IN_PROGRESS)
			{
				XS_ERROR("[NET]ConnDoSend: err:%d", sent);
				return XS_EC_ERROR; // Error occured
			}
			break;
		}
	}

	return XS_EC_OK;
}

int xsConnection::doRecv()
{
	int read;
	xsBuffer *buf;
	size_t size;
	xsConnEvent evt;
	xsConnResult result;

	XS_TRACE("[NET]ConnDoRecv");
	evt.type = XS_EVT_CONN_RECEIVED;
	evt.result = &result;

	for (;;)
	{
		buf = (xsBuffer *)xsQueueFront(recvQ);
		if (buf == NULL)
			break;

		size = xsBufferBlockSize(buf);
		read = xsSocketRecv(sock, buf->tail, size);
		if (read > 0)
		{
			xsBufferProduce(buf, read);

			if (buf->len == buf->size || read < (int)size)
			{// readed, notice
				xsQueuePop(recvQ);

				result.buffer = buf;
				result.code = XS_CONN_RES_OK;
				xsEvent::send(this, &evt);
			}
		}
		else if (read == 0)
		{// connection closed
			result.buffer = buf;
			result.code = XS_CONN_RES_CLOSED;
			xsEvent::send(this, &evt);
			break;
		}
		else
		{
			if (buf->len > 0)
			{// notice last received data
				xsQueuePop(recvQ);

				result.buffer = buf;
				result.code = XS_CONN_RES_OK;
				xsEvent::send(this, &evt);
			}

			if (read != XS_EC_IN_PROGRESS)
			{
				XS_ERROR("[NET]ConnDoRecv: err:%d", read);
				return XS_EC_ERROR;
			}

			break;
		}
	}

	return XS_EC_OK;
}

int xsConnection::processEvent(xsEvent *e)
{
//	xsConnection *conn;
	xsConnEvent evt;

	// handle resolve event
	if (e->type == XS_EVT_RESOLVE)
	{
		XS_TRACE("[NET]xsConnResolveHandler: state:%d", state);
		if (state == XS_CONN_RESOLVING)
		{// should connect
			if (e->sys->data.resolve.success)
			{
				addr.addr.n = e->sys->data.resolve.addr.addr.n;
				return doConnect();
			}
			XS_ERROR("[NET]xsConnResolveHandler: Resolve failed.");
		}
		else
		{
			XS_ERROR("[NET]xsConnResolveHandler: Invoke XS_EVT_RESOLVE on wrong state: %d", state);
		}

		return XS_EC_ERROR;
	}

	// Cannot handle other event except socket event.
	if (e->type != XS_EVT_SOCK_READ && e->type != XS_EVT_SOCK_WRITE
		&& e->type != XS_EVT_SOCK_CONNECT && e->type != XS_EVT_SOCK_CLOSE)
		return XS_EC_OK;

	// handle socket event
	if (e->sys->data.socket.status != XS_EC_OK)
	{
		XS_ERROR("socket error. %d", e->sys->data.socket.status);
		destroyInstance();
		return XS_EC_OK;
	}

	switch (e->type)
	{
	case XS_EVT_SOCK_READ:
		XS_TRACE("[NET]xsConnHandler: OnRead");
		if (state == XS_CONN_ESTABLISHED)
			return doRecv();
		break;
	case XS_EVT_SOCK_WRITE:
		XS_TRACE("[NET]xsConnHandler: OnWrite");
		if (state == XS_CONN_ESTABLISHED)
			return doSend();
		break;
	case XS_EVT_SOCK_CONNECT:
		XS_TRACE("[NET]xsConnHandler: OnConnect");
		if (state == XS_CONN_CONNECTING)
		{
			state = XS_CONN_ESTABLISHED;
			evt.type = XS_EVT_CONN_ESTABLISHED;
			xsEvent::send(this, &evt);
		}
		else
		{
			XS_ERROR("[NET]xsConnHandler: Invoke XS_EVT_SOCK_CONNECT on wrong state: %d", state);
		}
		break;
	case XS_EVT_SOCK_CLOSE:
		XS_TRACE("[NET]xsConnHandler: OnClose");
		destroyInstance();
		break;
	}

	return XS_EC_OK;
}
