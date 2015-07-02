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

