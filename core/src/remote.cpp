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
#include <xs/remote.h>
#include <xs/object.h>
//#include <xs/datasource.h>
//#include <xs/script.h>
#include <xs/string.h>
#include <xs/memory.h>

xsRemote::~xsRemote()
{
	if (cmd != NULL)
		xsFree(cmd);

//	if (ds != NULL)
//		ds->destroyInstance();
}

int xsRemote::processEvent(xsEvent *e)
{
	switch (e->type)
	{
	case XS_EVT_DATA_UPDATE:
		__onData((xsDataEvent *)e);
		break;
	}

	return 0;
}

xsBool xsRemote::invoke(const char *uri, xsCoreApplication *app, const char *cmdCallback)
{
	this->app = app;
	this->cmd = xsStrDup(cmdCallback);

//	ds = xsDataSource::alloc(uri, this);
	if (ds == NULL)
		return XS_FALSE;

	return XS_TRUE;
}

void xsRemote::__onData(xsDataEvent *e)
{
//	xsValue args[1];
	
//	args[0] = ((xsDataEvent *)e)->ds->get(NULL);

//	xsScriptCall(app, cmd, 1, args, NULL);
}
