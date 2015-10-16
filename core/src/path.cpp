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
#include <xs/path.h>
#include <xs/string.h>

/**
 * Assamble stage directory path to 'buf', with endding path delimeter.
 * @param buf buffer with minimal length of XS_MAX_PATH
 * @return address of 'buf', return NULL if assamble failed.
 */
xsTChar *xsGetStagePath(xsTChar *buf)
{
	xsTChar *dir = xsGetSharedDataPath(buf);
	
	if (dir == NULL)
		return NULL; // failed.

	xsTcsCat(buf, xsT("stage"));
	xsTcsAppend(buf, XS_PATH_DELIMITER);
	
	return buf;
}

/**
 * Assamble user data directory path to 'buf', with endding path delimeter.
 * @param buf buffer with minimal length of XS_MAX_PATH
 * @return address of 'buf', return NULL if assamble failed.
 */
xsTChar *xsGetUserDataPath(xsTChar *buf)
{
#ifndef XS_FEATURE_MULTIUSER
	return xsGetSharedDataPath(buf);
#endif

	xsTChar *dir = xsGetPrivateStoragePath(buf);

	if (dir == NULL)
	{
		XS_ERROR("Get user data path failed.");
		return NULL; // failed.
	}

	xsTcsCat(buf, xsT(".xs"));
	xsTcsAppend(buf, XS_PATH_DELIMITER);

	return buf;
}

/**
 * Assamble shared data directory path to 'buf', with endding path delimeter.
 * @param buf buffer with minimal length of XS_MAX_PATH
 * @return address of 'buf', return NULL if assamble failed.
 */
xsTChar *xsGetSharedDataPath(xsTChar *buf)
{
	xsTChar *dir;

	// try external storage
	dir = xsGetExternalStoragePath(buf);

	// try internal public storage
	if (dir == NULL)
		dir = xsGetPublicStoragePath(buf);

	// try local directory
	if (dir == NULL)
		return xsGetLocalDataPath(buf);

	if (dir == NULL)
	{
		XS_ERROR("Get shared data path failed.");
		return NULL; // failed.
	}

	xsTcsCat(buf, xsT(".xs"));
	xsTcsAppend(buf, XS_PATH_DELIMITER);

	return buf;
}

