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
#include <xs/httpconnection.h>
#include <xs/utils.h>
#include <xs/memory.h>
#include <xs/object.h>
#include <xs/string.h>

static int g_xsHttpGetHeaderSize = 0;
static const char *g_xsHttpGetHeader = 
	"GET %s HTTP/1.1\r\n"
	"Host: %s:%d\r\n"
	"\r\n";
#define XS_HTTP_RESPONSE_BUF_SIZE		8192

xsHttpConnection::xsHttpConnection(const char *url) : xsConnection(url)
{
	if (g_xsHttpGetHeaderSize == 0)
	{// calculate
		g_xsHttpGetHeaderSize = xsStrLen(g_xsHttpGetHeader);
	}
	port = 80;
	_saveFile = xsFileInvalidHandle();
}

xsHttpConnection::~xsHttpConnection()
{
	if (_recvBuf != NULL)
		xsFree(_recvBuf);

	// close opened file
	if (xsFileHandleValid(_saveFile))
		xsCloseFile(_saveFile);

	// notice failed download progress
	if (_autoSave && listener && _receivedLength <= 0)
	{
		xsCommonEvent evt;
		evt.type = XS_EVT_HTTP_PROGRESS;
		evt.param = -1;

		// Notice listener
		xsEvent::send(listener, &evt);
	}
}

xsHttpConnection *xsHttpConnection::createInstance(const char *url)
{
	xsHttpConnection *conn;

	conn = new xsHttpConnection(url);
	if (conn == NULL)
		return NULL;

	if (conn->id < 0)
	{
		conn->destroyInstance();
		return NULL;
	}

	return conn;
}

xsHttpConnection *xsHttpConnection::createDownload(const char *url, const xsTChar *savePath)
{
	xsHttpConnection *conn;

	conn = xsHttpConnection::createInstance(url);
	if (conn == NULL)
	{
		XS_ERROR("xsHttpGet: wrong URL");
		return NULL;
	}

	if (conn->setAutoSave(savePath) != XS_EC_OK)
	{// open file failed.
		conn->destroyInstance();
		return NULL;
	}

	conn->setSingleBuffer(XS_FALSE);

	return conn;
}

int xsHttpConnection::processEvent(xsEvent *e)
{
	xsConnEvent *evt = (xsConnEvent *)e;

	switch (e->type)
	{
	case XS_EVT_CONN_ESTABLISHED:
		sendGetRequest();
		break;
	case XS_EVT_CONN_SENT:
		xsFree(((xsConnResult *)evt->result)->buffer);
		((xsConnResult *)evt->result)->buffer= NULL;
		break;
	case XS_EVT_CONN_RECEIVED:
		recvResponse((xsConnResult *)evt->result);
		break;
	default:
		return xsConnection::processEvent(e);
	}

	return XS_EC_OK;
}

const char *xsHttpConnection::getHeader(xsArrayList headers, const char *name)
{
	int i;
	int count = xsArrayListSize(headers);
	char *header;

	for (i = 0; i < count - 1; i += 2)
	{
		header = (char *)xsArrayListGet(headers, i);
		if (xsStrCmp(header, name) == 0)
			return (char *)xsArrayListGet(headers, i + 1);
	}

	return NULL;
}

void xsHttpConnection::sendGetRequest()
{
	xsBuffer *request;
	size_t size, baseSize;
	const char *path;

	size = xsStrLen(uriStr);
	baseSize = xsStrLen(uri->base);
	
	// qualify web path
	if (size > baseSize + 7) // 7 is lengthOf("http://")
		path = uriStr + baseSize + 7;
	else
		path = "/"; // web root, has no path

	size += g_xsHttpGetHeaderSize;
	size += baseSize;
	request = xsBufferCreate(size);
	if (request == NULL)
		return;

	// build request string
	if (proxy.hostname != NULL)
	{// use proxy
		xsSnprintf(request->data, size, g_xsHttpGetHeader, uriStr, proxy.hostname, proxy.port);
	}
	else
	{
		xsSnprintf(request->data, size, g_xsHttpGetHeader, path, hostname, port);
	}

	request->len = xsStrLen(request->data);
	send(request);
	_transferState = XS_HTTP_REQUEST;

	doReceive();
}

void xsHttpConnection::doReceive()
{
	if (_recvBuf == NULL)
		_recvBuf = xsBufferCreate(XS_HTTP_RESPONSE_BUF_SIZE);
	if (_recvBuf == NULL)
	{
		XS_ERROR("Create buffer failed.");
		return;
	}

	receive(_recvBuf);
}

void xsHttpConnection::recvResponse(xsConnResult *result)
{
	char *response = result->buffer->head;

	if (result->code == XS_CONN_RES_OK || result->code == XS_CONN_RES_CLOSED)
	{
		if (_transferState == XS_HTTP_REQUEST)
		{// response begin, process status line and header
			_transferState = XS_HTTP_STATUS;
			const char *statusLine = response;
			char statusStr[4] = {0};

			if (result->buffer->len < 17 || xsStrCmpN(statusLine, "HTTP/1.", 7) != 0) // 17 is HTTP/1.X 2NN OK\r\n
				goto final;// Malformed http response

			// version
			_responseVersion = statusLine[7] - '0';

			// status code
			xsStrCpyN(statusStr, statusLine + 9, 3);
			_responseStatus = (short)xsStrToInt(statusStr);

			// prepare to process header
			statusLine = xsStrStr(statusLine, "\r\n");
			if (statusLine == NULL)
				goto final; // status line has no end

			statusLine += 2;
			size_t length = statusLine - response;
			if (length > result->buffer->len)
				goto final; // out of buffer,  status line has no end

			// continue process header
			_transferState = XS_HTTP_HEADER;
			statusLine = parseHeaders(statusLine, result->buffer->len - length);

			// free status and header data from buffer
			xsBufferConsume(result->buffer, statusLine - response);
		}
		else if (_transferState == XS_HTTP_HEADER)
		{// continue process header
			const char *header = response;
			header = parseHeaders(header, result->buffer->len);

			// free header data from buffer
			xsBufferConsume(result->buffer, header - response);
		}

		if (_transferState == XS_HTTP_CONTENT)
		{
			// get length
			_receivedLength += result->buffer->len;
			
			// is transfer completed?
			if (_contentLength > 0 && _receivedLength >= _contentLength)
				_transferState = XS_HTTP_DONE;
			else if (result->code == XS_CONN_RES_CLOSED)
				_transferState = XS_HTTP_DONE;

			if (_singleBuffer && _transferState != XS_HTTP_DONE)
			{// prepare to receive more data
				size_t size = xsBufferBlockSize(_recvBuf);

				// extend receive buffer if needed
				if ((_contentLength < 0 && size < XS_HTTP_RESPONSE_BUF_SIZE / 2)
						|| ((int)size < _contentLength - _receivedLength))
					_recvBuf = xsBufferExtend(_recvBuf);
			}
			else
			{// notice listener only when transfer completed or not in single buffer mode
				if (_autoSave)
				{// save to file
					if (xsFileHandleValid(_saveFile))
					{
						xsWriteFile(_saveFile, _recvBuf->head, _recvBuf->len);

						if (_contentLength > 0)
						{// notice download progress
							if (_contentLength == _receivedLength)
							{// close file before notice listener when download finished
								xsCloseFile(_saveFile);
								_saveFile = xsFileInvalidHandle();
							}

							xsCommonEvent evt;
							evt.type = XS_EVT_HTTP_PROGRESS;
							evt.param = (_receivedLength * 10000) / _contentLength;	// param is 0~10000

							// Notice listener
							xsEvent::send(listener, &evt);
						}
					}
				}
				else
				{// notice receive data
					// construct event
					xsHttpConnEvent evt;
					evt.type = XS_EVT_HTTP_RESPONSE;
					evt.content = _recvBuf->head;
					evt.len = _contentLength;
					evt.headers = _headers;
					evt.status = _responseStatus;

					// Notice listener
					xsEvent::send(listener, &evt);
				}

				xsBufferClear(_recvBuf);
			}

		}

		if (result->code == XS_CONN_RES_OK)
			doReceive();
	}

final:
	result->buffer = NULL;
}

const char *xsHttpConnection::parseHeaders(const char *header, size_t len)
{
	if (_headers == NULL)
		_headers = xsArrayListCreate(2);

	const char *split, *end;
	char *item;

	for (;;)
	{
		end = xsStrStr(header, "\r\n");
		if (end == NULL || end - header > (int)len)
			return header; // no valid header line end

		if (end == header)
		{// empty header line indicates end of headers
			_transferState = XS_HTTP_CONTENT;
			header = end + 2; // skip \r\n to content

			// header ended, parser concerned header items
			const char *item;

			item = getHeader(_headers, "Content-Length");
			if (item == NULL)
				_contentLength = -1;
			else
				_contentLength = xsStrToInt(item);

			// Connection: keep-alive?
			item = getHeader(_headers, "Connection");
			if (item != NULL && xsStrCmp(item, "keep-alive") == 0)
				_keepalive = XS_TRUE;
			else
				_keepalive = XS_FALSE;

			// end parse
			break;
		}

		split = xsStrChr(header, ':');
		if (split == NULL || split > end)
		{// malformed header line
			header = end + 2;
			break;
		}
		
		// key
		item = xsStrDupN(header, split - header);
		xsArrayListAdd(_headers, item);

		// value
		item = xsStrDupN(split + 2/*: */, end - split - 2);
		xsArrayListAdd(_headers, item);

		// next
		header = end + 2;
	}

	// add end
	xsArrayListAdd(_headers, NULL);

	return header;
}

void xsHttpConnection::freeHeaders(xsArrayList headers)
{
	xsIterator iter = NULL;
	char *item;

	for (;;)
	{
		item = (char *)xsArrayListIterate(headers, &iter);
		if (item == NULL || iter == NULL)
			break;

		xsFree(item);
	}

	xsArrayListDestroy(headers);
}

int xsHttpConnection::setAutoSave(const xsTChar *filename)
{
	// close opened file
	if (xsFileHandleValid(_saveFile))
		xsCloseFile(_saveFile);

	if (filename != NULL)
	{
		_autoSave = XS_TRUE;

		xsReplaceAny((void **)&_saveFilename, xsTcsDup(filename));
		_saveFile = xsOpenFile(filename, XS_OF_CREATE | XS_OF_WRITEONLY);
		if (!xsFileHandleValid(_saveFile))
			return XS_EC_ERROR;
	}
	else
	{
		_autoSave = XS_FALSE;
		xsReplaceAny((void **)&_saveFilename, NULL);
	}

	return XS_EC_OK;
}
