#include <xs/pal.h>
#include <xs/appcontext.h>
#include <xs/string.h>
#include <xs/path.h>
#include <xs/base32codec.h>
#include <xs/memory.h>
#include <xs/bon.h>
#include <xs/filestream.h>
#include <xs/resourcestream.h>
#include <xs/uri.h>
#include <xs/bon.h>
#include <xs/entry.h>
#include <xs/httpconnection.h>

xsAppContext::xsAppContext()
{
	_appObject = NULL;
	_loadMode = MODE_UNKNOWN;
	_fd = xsFileInvalidHandle();
	_res = xsResInvalidHandle();
}

xsAppContext::~xsAppContext()
{
	// destroy application before free library
	if (_appObject != NULL)
		_appDestroyer(_appObject);

	if (_libUninit != NULL)
		_libUninit();

	if (_name != NULL)
		xsFree(_name);

	if (_uri != NULL)
		xsFree(_uri);

	if (_id != NULL)
		xsFree(_id);

	if (_loadPath != NULL)
		xsFree(_loadPath);

	if (_res != NULL)
		xsCloseRes(_res);

	if (xsFileHandleValid(_fd))
		xsCloseFile(_fd);

	if (xsResHandleValid(_res))
		xsCloseRes(_res);
}

int xsAppContext::start()
{
	xsEvent e;
	e.type = XS_EVT_START;
	return _appInvoker(_appObject, &e);
}

void xsAppContext::suspend()
{
	xsEvent e;
	e.type = XS_EVT_SUSPEND;
	_appInvoker(_appObject, &e);
}

int xsAppContext::resume()
{
	xsEvent e;
	e.type = XS_EVT_RESUME;
	return _appInvoker(_appObject, &e);
}

void xsAppContext::exit()
{
	xsEvent e;
	e.type = XS_EVT_EXIT;
	_appInvoker(_appObject, &e);
}

int xsAppContext::processEvent(xsEvent *e)
{
	switch (e->type)
	{
	case XS_EVT_HTTP_PROGRESS:
		_loaded = ((xsCommonEvent *)e)->param;
		if (((xsCommonEvent *)e)->param == LOAD_COMPLETE)
		{// download complete, reload from cache
			prepare(_uri);
		}

		return XS_EC_OK;
	}

	if (_appInvoker != NULL)
		return _appInvoker(_appObject, e);

	return XS_EC_OK;
}

xsAppContext *xsAppContext::createInstance(const char *appUri)
{
	XS_TRACE("Create application's context");
	xsAppContext *context = new xsAppContext();
	if (context->prepare(appUri) == XS_EC_OK)
		return context;

	// failed, return NULL
	context->destroyInstance();
	return NULL;
}

void xsAppContext::destroyInstance()
{
	XS_TRACE("Destroy application context");
	delete this;
}

int xsAppContext::fromStage(const char *id)
{
	XS_TRACE("Load '%s' from stage", id);

	xsTChar base[XS_MAX_PATH];
	// get stage base path
	if (xsGetStagePath(base) == NULL)
		return XS_EC_ERROR;

	// assamble staging dir path
	size_t len = xsStrLen(id);
	size_t offset = xsTcsLen(base);
	if (len >= XS_MAX_PATH - offset)
	{// id too long
		XS_ERROR("Application id '%s' is too long.", id);
		return XS_EC_ERROR;
	}
#ifdef XS_UNICODE
	len = xsMbsToWcs(base + offset, id, len);
	if (len == -1)
		return XS_EC_ERROR;
	base[offset + len] = 0;
#else
	xsStrCpy(base + offset, id);
#endif
	xsTcsAppend(base, XS_PATH_DELIMITER);

	// load app manifest
	xsTChar filename[XS_MAX_PATH];
	xsTcsCpy(filename, base);
	xsTcsCat(filename, xsT("build"));
	xsTcsAppend(filename, XS_PATH_DELIMITER);
	xsTcsCat(filename, xsT("app.bon"));
	xsFileStream stream;
	if (stream.open(filename, XS_OF_READONLY) != XS_EC_OK)
	{
		XS_ERROR("Open app.bon failed.");
		return XS_EC_ERROR;
	}

	return fromStream(stream, base, MODE_STAGING);
}

int xsAppContext::fromFile(const char *path)
{
	xsFileStream stream;
	xsTChar *filename;

	if (path == NULL || *path == 0)
	{
		XS_ERROR("Empty XPK path.");
		return XS_EC_ERROR;
	}

#ifdef XS_UNICODE
	xsWChar base[XS_MAX_PATH];
	xsMbsToWcs(base, path, XS_MAX_PATH);
	
	filename = base;
#else
	filename = path;
#endif
	
	if (stream.open(filename, XS_OF_READONLY) != XS_EC_OK)
	{
		XS_ERROR("Open app.bon failed.");
		return XS_EC_ERROR;
	}

	_fd = stream.getFd(XS_TRUE);
	return fromStream(stream, filename, MODE_FILE);
}

int xsAppContext::fromLocal(const char *path)
{
	xsTChar filename[XS_MAX_PATH];
	// get local base path
	if (xsGetLocalDataPath(filename) == NULL)
		return XS_EC_ERROR;

	// assamble
	size_t len = xsTcsCatStrS(filename, XS_MAX_PATH - 1, path);
	if (len == -1)
	{
		XS_ERROR("Application path '%s' is too long.", path);
		return XS_EC_ERROR;
	}

	xsFileStream stream;
	if (stream.open(filename, XS_OF_READONLY) != XS_EC_OK)
	{
		XS_ERROR("Open %s failed.", path);
		return XS_EC_ERROR;
	}

	_fd = stream.getFd(XS_TRUE);
	return fromStream(stream, filename, MODE_FILE);
}

int xsAppContext::fromRes(const char *name)
{
	xsTChar *resname;

#ifdef XS_UNICODE
	xsWChar tstr[XS_MAX_PATH];
	xsMbsToWcs(tstr, name, XS_MAX_PATH);

	resname = tstr;
#else
	resname = name;
#endif

	xsResourceStream stream;
	if (stream.open(resname) != XS_EC_OK)
	{
		XS_ERROR("Open %s failed.", name);
		return XS_EC_ERROR;
	}

	_res = stream.getRes();
	return fromStream(stream, NULL, MODE_RESOURCE);
}

int xsAppContext::fromStream(xsRandomAccessStream &stream, const xsTChar *loadPath, int loadMode)
{
	xsValue orient, name, aid, preload;
	
	XS_TRACE("Read application's manifest");
	if (loadMode == MODE_STAGING)
	{// app.bon
		if (xsBonReadValueFromStream(stream, "orient", &orient) != XS_EC_OK)
			return XS_EC_ERROR;
		if (xsBonReadValueFromStream(stream, "name", &name) != XS_EC_OK)
		{
			xsValueDestroy(&orient, XS_FALSE);
			return XS_EC_ERROR;
		}
		if (xsBonReadValueFromStream(stream, "id", &aid) != XS_EC_OK)
		{
			xsValueDestroy(&orient, XS_FALSE);
			xsValueDestroy(&name, XS_FALSE);
			return XS_EC_ERROR;
		}
		xsBonReadValueFromStream(stream, "preload", &preload);
	}
	else
	{// app object of BON
		if (xsBonReadValueFromStream(stream, "app/orient", &orient) != XS_EC_OK)
			return XS_EC_ERROR;
		if (xsBonReadValueFromStream(stream, "app/name", &name) != XS_EC_OK)
		{
			xsValueDestroy(&orient, XS_FALSE);
			return XS_EC_ERROR;
		}
		if (xsBonReadValueFromStream(stream, "app/id", &aid) != XS_EC_OK)
		{
			xsValueDestroy(&orient, XS_FALSE);
			xsValueDestroy(&name, XS_FALSE);
			return XS_EC_ERROR;
		}
		xsBonReadValueFromStream(stream, "app/preload", &preload);
	}

	_loadMode = loadMode;
	_orient = orient.data.n;
	_name = name.data.t;
	_id = aid.data.s;

	if (loadPath != NULL)
		_loadPath = xsTcsDup(loadPath);

	_loaded = LOAD_COMPLETE;

	return XS_EC_OK;
}

int xsAppContext::fromHttp(const char *url)
{
	int ret;

	// load app from cache
	xsTChar filename[XS_MAX_PATH];
	if (xsGetUserDataPath(filename) == NULL)
		return XS_EC_ERROR;

	// build cache file name
	char name[XS_MAX_PATH];
	xsBase32Encode(name, XS_MAX_PATH, (const xsU8 *)url, xsStrLen(url));

	size_t len = xsTcsLen(filename);
	xsTcsCatStrS(filename, XS_MAX_PATH - len, name);

	// try to open cache file
	xsFileStream stream;
	if (stream.open(filename, XS_OF_READONLY) == XS_EC_OK)
	{
		ret = fromStream(stream, NULL, MODE_FILE);
		if (ret == XS_EC_OK)
		{// return when success
			_fd = stream.getFd(XS_TRUE);
			return XS_EC_OK;
		}

		// close file and try download
		stream.close();
	}

	// not download yet, download from internet
	_conn = xsHttpConnection::createDownload(_uri, filename);
	if (_conn == NULL)
		return XS_EC_ERROR;

	_conn->setListener(this);
	ret = _conn->connect();
	if (ret < XS_EC_OK && ret != XS_EC_IN_PROGRESS)
		return ret;

	return XS_EC_OK;
}

int xsAppContext::prepare(const char *appUri)
{
	int ret = XS_EC_ERROR;

	// store app's URI if never stored
	if (_uri == NULL)
		_uri = xsStrDup(appUri);

	xsUri *uri = xsUriParse(appUri);
	if (uri == NULL)
		ret = fromStage(appUri); // staging mode. appUri is application id
	else if (uri->schema == XS_URI_FILE)
		ret = fromFile(uri->path); // file mode. appUri is .XPK file's full path
	else if (uri->schema == XS_URI_LOCAL)
		ret = fromLocal(uri->path); // file mode. appUri is .XPK file's relative path
	else if (uri->schema == XS_URI_RES)
		ret = fromRes(uri->path); // resource mode. appUri.path is application's id
	else if (uri->schema == XS_URI_HTTP)
		ret = fromHttp(appUri);
	xsUriDestroy(uri);

	if (ret < XS_EC_OK)
	{
		XS_ERROR("Load application failed.");
		return ret;
	}

	if (_loaded != LOAD_COMPLETE)
		return XS_EC_OK;

	context->_appCreator = xsCreateApplication;
	context->_appDestroyer = xsDestroyApplication;
	context->_appInvoker = xsInvokeApplication;

	// create application object
	XS_TRACE("Create application object");
	_appObject = _appCreator();
	if (_appObject == NULL)
		XS_WARN("Application create NULL object. Try to continue.");

	// send load event
	XS_TRACE("Send XS_EVT_LOAD to application");
	xsEvent e;
	e.type = XS_EVT_LOAD;
	xsSysEvent se;
	se.data.app.instance = this;
	se.data.app.basePath = getBasePath();
	e.sys = &se;
	if (_appInvoker(_appObject, &e) != XS_EC_OK)
	{
		XS_ERROR("Application load resource failed.");
		_appDestroyer(_appObject);
		return XS_EC_ERROR;
	}

	XS_TRACE("Prepare application context successful");
	return XS_EC_OK;
}

