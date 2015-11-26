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

#ifndef _XS_RUNTIMECONTEXT_H_
#define _XS_RUNTIMECONTEXT_H_

#include <xs/event.h>

class xsRuntimeContext : public xsListener
{
public:
	virtual const char *getId() const = 0;
	virtual const int getLoadMode() const = 0;
	virtual const xsTChar *getBasePath() const = 0;	/// application base path, must short than XS_MAX_PATH
	virtual xsRes getRes() const = 0;	/// system resource handle
	virtual xsFile getFd() const = 0;	/// xskit resource file handle
	virtual int processEvent(xsEvent *e) = 0;

	enum LoadMode
	{
		MODE_UNKNOWN = -1,
		MODE_STAGING = 0,	// load from stage folder
		MODE_FILE,			// load from xskit resource file (.xpk)
		MODE_RESOURCE,		// load from system resource
		MODE_HTTP			// load from URL
	};

};

#endif /* _XS_RUNTIMECONTEXT_H_ */
