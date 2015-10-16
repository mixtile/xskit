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

#ifndef _XS_HTTPCONNECTION_H_
#define _XS_HTTPCONNECTION_H_

#include <xs/connection.h>

class xsHttpConnection : public xsConnection
{
public:
	static xsHttpConnection *createInstance(const char *url);
	static xsHttpConnection *createDownload(const char *url, const xsTChar *savePath);

	virtual int processEvent(xsEvent *e);

	int setAutoSave(const xsTChar *filename);
	inline void setSingleBuffer(xsBool singleBuffer) { this->_singleBuffer = singleBuffer; }

protected:
	xsHttpConnection(const char *url);
	virtual ~xsHttpConnection();

private:
	enum _xsTransferState
	{
		XS_HTTP_READY		= 0,
		XS_HTTP_REQUEST,
		XS_HTTP_STATUS,
		XS_HTTP_HEADER,
		XS_HTTP_CONTENT,
		XS_HTTP_DONE
	};

	xsBool _keepalive;
	xsBool _intrans;
	int _transferState;
	xsBool _singleBuffer;

	xsBool _autoSave;
	xsTChar *_saveFilename;
	xsFile _saveFile;

	// current response
	xsU8 _responseVersion;
	short _responseStatus;
	xsArrayList _headers;
	int _contentLength;		// total content length
	int _headerLength;
	int _receivedLength;	// received content length
	xsBuffer *_recvBuf;

	void initial();

	void sendGetRequest();
	void doReceive();
	void recvResponse(xsConnResult *result);

	const char *getHeader(xsArrayList headers, const char *name);

	const char *parseHeaders(const char *header, size_t len);
	void freeHeaders(xsArrayList headers);
};


#endif /* _XS_HTTPCONNECTION_H_ */
