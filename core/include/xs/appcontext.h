#ifndef _XS_APPCONTEXT_H_
#define _XS_APPCONTEXT_H_

#include <xs/runtimecontext.h>
#include <xs/launcher.h>
//#include <xs/loader.h>
#include <xs/stream.h>
#include <xs/entry.h>
#include <xs/httpconnection.h>

class xsAppContext : public xsRuntimeContext
{
public:

	static xsAppContext *createInstance(const char *appUri);
	void destroyInstance();

	int start();
	void suspend();
	int resume();
	void exit();
	virtual int processEvent(xsEvent *e);

	inline int getOrient() { return _orient; }
	inline xsTChar *getName() { return _name; }
	inline int getLoaded() { return _loaded; }

	inline int getIndex() const { return _index; }
	inline void setIndex(int index) { _index = index; }

	virtual const char *getId() const { return _id; }
	virtual const int getLoadMode() const { return _loadMode; }
	virtual const xsTChar *getBasePath() const { return _loadPath; }
	virtual xsRes getRes() const { return _res; }
	virtual xsFile getFd() const { return _fd; }

public:
	enum _xsLoadState
	{
		LOAD_READY		= 0,
		LOAD_COMPLETE	= 10000,
		LOAD_FAILED		= -1
	};

protected:
	xsAppContext();
	virtual ~xsAppContext();


private:
	int prepare(const char *appUri);

	int fromStage(const char *id);
	int fromFile(const char *path);
	int fromLocal(const char *path);
	int fromRes(const char *name);
	int fromHttp(const char *url);

	int fromStream(xsRandomAccessStream &stream, const xsTChar *loadPath, int loadMode);

private:
	char *_id;			// application id
	int _loadMode;		// source load mode
	xsTChar *_loadPath;	// source directory/file load from
	int _loaded;		// load progress, 0 - 10000, 10000 indicates complete, -1 indicates failure.
	xsFile _fd;			// application resource file descriptor
	xsRes _res;			// application resource handle
	xsHttpConnection *_conn;	// application download handle, it will destroy self after done.

	char *_uri;			// Application source URI
	int _index;			// index in application list
	int _orient;		// screen default orient
	xsTChar *_name;		// application's name

	/// Entry function pointer define, see entry.h
	xsLibInitFunc _libInit;				// initial of library
	xsLibUninitFunc _libUninit;			// uninitial of library
	xsAppCreatorFunc _appCreator;		// interface object creator of library
	xsAppDestroyerFunc _appDestroyer;	// interface object destroyer of library
	xsAppInvokerFunc _appInvoker;		// interface object destroyer of library

	void *_appObject;

	friend xsAppContext *xsLauncher::load(const char *appUri);
};

#endif /* _XS_APPCONTEXT_H_ */
