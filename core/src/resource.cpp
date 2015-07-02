#include <xs/pal.h>
#include <xs/memory.h>
#include <xs/object.h>
#include <xs/runtimecontext.h>
#include <xs/coreapplication.h>
#include <xs/utils.h>
#include <xs/resource.h>
#include <xs/string.h>
#include <xs/error.h>
#include <xs/bon.h>
#include <xs/path.h>

xsResource *xsResource::createInstance(const xsCoreApplication *app)
{
	const xsRuntimeContext *context = app->getContext();

	XS_TRACE("xsResource::createInstance: Load application '%s' resource", context->getId());

	xsResource *res = new xsResource();

	// clone runtime context
	res->_loadMode = context->getLoadMode();
	res->_base = context->getBasePath();
	res->_res = context->getRes();
	res->_fd = context->getFd();

	// remember where i am
	res->_app = app;
	res->_context = context;

	return res;
}

xsValue xsResource::loadValue(const char *name)
{
	xsValue value;
	int ret;

	if (_loadMode == xsRuntimeContext::MODE_FILE)
	{
		ret = xsBonReadValueFromFd(_fd, name, &value);
	}
	else if (_loadMode == xsRuntimeContext::MODE_RESOURCE)
	{
		ret = xsBonReadValueFromRes(_res, name, &value);
	}
	else if (_loadMode == xsRuntimeContext::MODE_STAGING)
	{
		// build path
		xsTChar path[XS_MAX_PATH];
		xsTcsCpyN(path, _base, XS_MAX_PATH);
		xsTcsCat(path, xsT("build"));
		xsTcsAppend(path, XS_PATH_DELIMITER);
		xsTcsCatStrS(path, XS_MAX_PATH - 1, name);
		xsTcsCat(path, xsT(".bon"));

		// read NAME.bon from file
		ret = xsBonReadValueFromFile(path, NULL, &value);
	}
	else
	{
		ret = XS_EC_ERROR;
	}

	if (ret != XS_EC_OK)
		value.type = XS_VALUE_NONE;

	return value;
}

void *xsResource::loadData(const char *name, size_t *size)
{
	if (_loadMode == xsRuntimeContext::MODE_STAGING)
	{
		xsTChar path[XS_MAX_PATH];
		xsTcsCpy(path, _context->getBasePath());
		xsTcsCatStrS(path, XS_MAX_PATH - 1, name);
		// read data from file
		return loadDataFromFile(path, size);
	}
	
	xsValue value;
	int ret;
	if (_loadMode == xsRuntimeContext::MODE_RESOURCE)
		ret = xsBonReadValueFromRes(_res, name, &value);
	else if (_loadMode == xsRuntimeContext::MODE_FILE)
		ret = xsBonReadValueFromFd(_fd, name, &value);
	else
		return NULL;

	if (ret != XS_EC_OK || value.type != XS_VALUE_BINARY)
	{
		xsValueDestroy(&value, XS_FALSE);
		return NULL;
	}

	*size = *(size_t *)value.data.ptr;
	return (char *)value.data.ptr + sizeof(void *); // FIXME: Who will free these memory?
}


/*
xsWidget *xsResource::loadWidget(const char *name)
{
	XS_TRACE("xsResource::loadWidget: %s", name);

	char fullpath[XS_MAX_PATH];
	xsStrCpy(fullpath, "ui/");
	xsStrCatN(fullpath, name, XS_MAX_PATH);

	xsWidget *wid = NULL;
	xsValue value = loadValue(fullpath);
	if (value.type == XS_VALUE_OBJECT
		&& value.data.obj->isTypeOf(XS_CLASS(xsWidget)))
		wid = static_cast<xsWidget *>(value.data.obj);
	else if (value.type != XS_VALUE_NONE)
		xsValueDestroy(&value, XS_FALSE);

	return wid;
}
*/
char *xsResource::loadScript(const char *name, size_t *size)
{
	XS_TRACE("xsResource::loadScript");
	char fullpath[XS_MAX_PATH];

	// load encrypted data
	xsStrCpy(fullpath, "scripts/");
	xsStrCatN(fullpath, name, XS_MAX_PATH);

	char *data = (char *)loadData(fullpath, size);
	if (_loadMode == xsRuntimeContext::MODE_STAGING)
		return data;

	// load key
	xsValue value;
	const char *uuid = _app->getUUID();
	if (uuid == NULL)
		return data; // application is not encryped
	
	int ret = -1;
	size_t keyLen;
	xsS32 *key;

	// load key from res
	xsStrCpy(fullpath, "lk/");
	xsStrCat(fullpath, _app->getUUID());
	if (_loadMode == xsRuntimeContext::MODE_RESOURCE)
		ret = xsBonReadValueFromRes(_res, fullpath, &value);
	else if (_loadMode == xsRuntimeContext::MODE_FILE)
		ret = xsBonReadValueFromFd(_fd, fullpath, &value);
	else
		return NULL;

	// or load from file
	if (ret != XS_EC_OK)
		ret = xsBonReadValueFromFile(xsT("license.key"), _app->getUUID(), &value);
	if (ret != XS_EC_OK)
		return NULL; // failed

	if (value.type != XS_VALUE_BINARY)
	{// not binary
		goto keyFailed;
	}

	// decrypte key
	keyLen = *((size_t *)value.data.ptr);
	key = (xsS32 *)(value.data.s + sizeof(void *));
	if (keyLen == 17)
	{// has a paddingcount, key is encrypted
		char devId[16];
		if (!xsGetDeviceUniqueId(devId, 16))
			goto keyFailed;

//		xsDataDecode(key, 4, (xsS32 *)devId);
	}
	else if (keyLen != 16)
	{// malformed key, not 128bits key
		goto keyFailed;
	}

	// decrypt scripts
	*size -= 1; // last byte is padding length
//	xsDataDecode((xsS32 *)data, *size / sizeof(xsU32), key);
	*size -= data[*size]; // remove padding

	return data;

keyFailed:
	xsValueDestroy(&value, false);
	return NULL;
}

xsAFD *xsResource::createAFD(const xsTChar *src, const xsTChar *path)
{
	xsAFD *afd;

	afd = (xsAFD *)xsCalloc(sizeof(xsAFD));
	if (afd == NULL)
		return NULL;

	if (_loadMode == xsRuntimeContext::MODE_STAGING)
	{
		afd->srcType = XS_AFD_FILENAME;
		// add path prefix
		const xsTChar *base = _context->getBasePath();
		afd->src.filename = (xsTChar *)xsMalloc((xsTcsLen(base) + xsTcsLen(src) + xsTcsLen(path) + 1) * sizeof(xsTChar));
		xsTcsCpy(afd->src.filename, base);
		xsTcsCat(afd->src.filename, path);
		xsTcsCat(afd->src.filename, src);

		return afd;
	}

	char fullpath[XS_MAX_PATH];
	// add path prefix
	size_t len = xsTcsToUtf8(fullpath, XS_MAX_PATH, path, xsTcsLen(path));
	xsTcsToUtf8(fullpath + len, XS_MAX_PATH - len, src, xsTcsLen(src));
	int ret = XS_EC_ERROR;

	if (_loadMode == xsRuntimeContext::MODE_RESOURCE)
	{
		afd->srcType = XS_AFD_RESOURCE;
		afd->src.res = _res;
		ret = xsBonLocateBinaryInRes(_res, fullpath, &afd->offset, &afd->length);
	}
	else if (_loadMode == xsRuntimeContext::MODE_FILE)
	{
		afd->srcType = XS_AFD_FILE;
		afd->src.fd = _fd;
		ret = xsBonLocateBinaryInFd(_fd, fullpath, &afd->offset, &afd->length);
	}
	
	if (ret != XS_EC_OK)
	{
		XS_ERROR("xsResource::createAFD: Locate AFD failed.");
		xsFree(afd);
		return NULL;
	}

	return afd;
}

void xsResource::freeAFD(xsAFD *afd)
{
	if (afd->srcType == XS_AFD_FILENAME)
		xsFree(afd->src.filename);

	xsFree(afd);
}

xsImage *xsResource::loadImage(const xsTChar *src)
{
	// create and return, image use lazy load mechanism
	return createAFD(src, xsT("res/"));
}

void xsResource::freeImage(xsImage *img)
{
	if (img == NULL)
		return;

	if (img->object != NULL)
		xsFreeImageObject(img);

	freeAFD(img);
}

xsMedia *xsResource::loadMedia(const xsTChar *src)
{
	xsMedia *media = createAFD(src, xsT("res/"));
	if (media == NULL)
		return NULL;

#ifdef XS_FEATURE_MEDIA
	if (xsMediaOpen(media) != XS_EC_OK)
	{
		XS_ERROR("xsResource::loadMedia: Open media failed.");
		xsFree(media);
		media = NULL;
	}
#endif

	return media;
}

void xsResource::freeMedia(xsMedia *media)
{
	if (media == NULL)
		return;

#ifdef XS_FEATURE_MEDIA
	if (media->object != NULL)
		xsMediaClose(media);
#endif

	freeAFD(media);
}

void *xsResource::loadDataFromFile(const xsTChar *filename, size_t *size)
{
	xsFile handle = xsOpenFile(filename, XS_OF_READONLY);
	char *buffer = NULL;

	XS_TRACE("xsResource::loadDataFromFile");
	if (!xsFileHandleValid(handle))
		return NULL;

	*size = xsGetFileSize(handle);
	XS_TRACE("xsResource::loadDataFromFile: size:%d", *size);
	if (*size == 0)
		goto final;

	buffer = (char *)xsMalloc(*size);
	if (buffer == NULL)
		goto final;

	if ((*size = xsReadFile(handle, buffer, *size)) != XS_EC_ERROR)
		goto final;

	// on error
	xsFree(buffer);
	buffer = NULL;

final:
	XS_TRACE("xsResource::loadDataFromFile: readed:%d", *size);
	xsCloseFile(handle);
	return buffer;
}

#define CHECK_CONTEXT(context) (context != NULL && ((xsObject *)context)->isTypeOf(XS_CLASS(xsCoreApplication)))

xsImage *xsLoadImage(void *context, const xsTChar *src)
{
	XS_ASSERT(CHECK_CONTEXT(context));

	return ((xsCoreApplication *)context)->getResource()->loadImage(src);
}

void xsFreeImage(void *context, xsImage *img)
{
	XS_ASSERT(CHECK_CONTEXT(context));

	((xsCoreApplication *)context)->getResource()->freeImage(img);
}

xsMedia *xsLoadMedia(void *context, const xsTChar *src)
{
	XS_ASSERT(CHECK_CONTEXT(context));

	return ((xsCoreApplication *)context)->getResource()->loadMedia(src);
}

void xsFreeMedia(void *context, xsMedia *media)
{
	XS_ASSERT(CHECK_CONTEXT(context));

	((xsCoreApplication *)context)->getResource()->freeMedia(media);
}

xsTChar *xsGetBasePath(void *context, xsTChar *buf)
{
	XS_ASSERT(CHECK_CONTEXT(context));

	const xsTChar *base = ((xsCoreApplication *)context)->getContext()->getBasePath();
	if (base == NULL)
		return NULL;

	xsTcsCpy(buf, base);
	return buf;
}
