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

#ifndef _XS_REMOTE_H_
#define _XS_REMOTE_H_

#include <xs/object.h>
#include <xs/uri.h>
#include <xs/value.h>

class xsCoreApplication;
class xsDataSource;

class xsRemote : public xsObject
{
	XS_OBJECT_DECLARE(Remote, XS_CLS_REMOTE)
public:
	xsBool invoke(const char *uri, xsCoreApplication *app, const char *cmdCallback);

protected:
	~xsRemote();

	int processEvent(xsEvent *e);

private:
	xsCoreApplication *app;
	xsDataSource *ds;
	char *cmd;

	void __onData(xsDataEvent *e);
};

#endif /* _XS_REMOTE_H_ */
