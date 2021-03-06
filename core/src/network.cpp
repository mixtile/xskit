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

#include <xs/network.h>
#include <xs/bon.h>
#include <xs/httpconnection.h>

xsU16 xsHtons(xsU16 hostshort)
{
	if (xsIsBigEndian())
	{
		return hostshort;
	}
	else
	{
		xsU16 shortle;

		shortle = (hostshort & 0xFF) << 8;
		shortle |= (hostshort & 0xFF00) >> 8;

		return shortle;
	}
}

int xsStrToAddr(const char *ipStr, xsSockAddr *addr)
{
	int sect = 0, val, sectVal = 0;

	while (*ipStr)
	{
		if (*ipStr == '.')
		{
			if (sectVal < 0 || sectVal > 255)
				return XS_EC_ERROR;

			addr->addr.b[sect] = (xsU8)sectVal;
			sectVal = 0;

			sect++;
			if (sect > 3)
				return XS_EC_ERROR;
		}
		else
		{
			val = *ipStr - '0';
			if (val < 0 || val > 9)
				return XS_EC_ERROR;
			sectVal = sectVal * 10;
			sectVal += val;
		}

		ipStr++;
	}

	if (*ipStr == 0)
	{
		if (sectVal < 0 || sectVal > 255)
			return XS_EC_ERROR;

		addr->addr.b[sect] = (xsU8)sectVal;
	}

	if (sect < 3)
		return XS_EC_ERROR;

	return XS_EC_OK;
}

xsHandle xsHttpGet(const char *url, xsBool singleBuffer, xsCallbackFunc cb)
{
	xsHttpConnection *conn;
	int ret;

	conn = xsHttpConnection::createInstance(url);
	if (conn == NULL)
		return NULL;

	conn->setSingleBuffer(singleBuffer);
	conn->setCallback(cb);

	ret = conn->connect();

	return conn;
}

xsHandle xsHttpDownload(const char *url, const xsTChar *savePath, xsCallbackFunc cb)
{
	int ret;
	xsHttpConnection *conn = xsHttpConnection::createDownload(url, savePath);
	if (conn == NULL)
		return NULL;

	conn->setCallback(cb);
	ret = conn->connect();

	return conn;
}

int xsHttpCancel(xsHandle *handle)
{
	return ((xsConnection *)handle)->close();
}
