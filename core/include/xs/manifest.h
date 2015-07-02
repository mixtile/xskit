#ifndef _XS_MANIFEST_H_
#define _XS_MANIFEST_H_

#include <xs/object.h>
#include <xs/resource.h>

class xsManifest : public xsObject
{
	XS_OBJECT_DECLARE(Manifest, XS_CLS_MANIFEST)
public:
	xsBool setProperty(int propId, xsValue *value);

	inline int getOrient() { return _orient; }
	inline xsTChar *getName() { return _name; }
	inline const char *getUUID() { return _uuid; };
	inline const char *getPreload() { return _preload; };
	inline const char *getStartup() { return _startup; };

protected:
	~xsManifest();

private:
	int _orient;
	xsTChar *_name;		// application's name
	char *_uuid;		// application's UUID
	char *_preload;		// preload library module
	char *_startup;		// startup script
};

#endif /* _XS_MANIFEST_H_ */
