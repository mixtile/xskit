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

#ifndef _XS_SYSEVENT_H_
#define _XS_SYSEVENT_H_

#include <xs/systypes.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _xsSysEvent
{
	int type;

	short screen; /// Reserved
	short window; /// Reserved

	union
	{
		struct
		{
			int param;
			void *ptr;
		} common;

		struct
		{
			int x, y;
			xsU16 button;
		} mouse;

		struct
		{
			xsU16 keyCode;
		} key;

		struct
		{
			char *uri;
			void *instance;
			int loaded;			// load progress, 0 - 10000, 10000 indicates complete, -1 indicates failure.

			int orient;
			const xsTChar *name;
			const xsTChar *basePath;
		} app;

		struct
		{
			int tag;
			xsSockAddr addr;
			xsBool success;
		} resolve;

		struct
		{
			int sock;
			int status;		// error code
		} socket;

	} data;
} xsSysEvent;

enum
{
	/// System boot and shutdown
	XS_EVT_STARTUP		= 0,
	XS_EVT_SHUTDOWN		= 1,
	/// Program install/uninstall/list
	XS_EVT_APP_INSTALL		= 2,	/// Install application to launcher's registry. app.uri: App URI
	XS_EVT_APP_UNINSTALL	= 3,	/// Uninstall application from launcher's registry. common.ptr: package id
	XS_EVT_APP_QUERY		= 4,	/// Query installed application. common.param[in]: zero-based app index; common.ptr[out]: pointer to xsAppInfo
	XS_EVT_APP_COUNT		= 5,	/// Count installed applications. common.param[out]: application's count
	/// Application enter to exit
	XS_EVT_LOAD			= 6,	/// Load App context. app.uri[in]: App URI; app.instance[out]: Instance of app context; app.loaded[out] loading progress
	XS_EVT_START		= 7,	/// app.instance: App Object
	XS_EVT_SUSPEND		= 8,	/// app.uri: NULL (current app)
	XS_EVT_RESUME		= 9,	/// app.uri: NULL (last app)
	XS_EVT_EXIT			= 10,	/// app.uri: App URI
	/// Mouse event
	XS_EVT_MOUSE_DOWN	= 11,
	XS_EVT_MOUSE_UP		= 12,
	XS_EVT_MOUSE_MOVE	= 13,
	/// Keypad event
	XS_EVT_KEY_DOWN		= 16,
	XS_EVT_KEY_UP		= 17,
	XS_EVT_KEY_HOLD		= 18,
	/// Resolve event
	XS_EVT_RESOLVE		= 25,
	/// Socket event
	XS_EVT_SOCK_CONNECT	= 26,
	XS_EVT_SOCK_READ	= 27,
	XS_EVT_SOCK_WRITE	= 28,
	XS_EVT_SOCK_CLOSE	= 29,

	XS_EVT_SYS_END		= 64
};

enum
{
	XS_APP_ORIENT_DEFAULT		= 0,
	XS_APP_ORIENT_LANDSCAPE		= 1,
	XS_APP_ORIENT_PORTRAIT		= 2
};

enum
{
	XS_MOUSE_KEY_LEFT	= 1,
	XS_MOUSE_KEY_RIGHT	= 2,
	XS_MOUSE_KEY_MIDDLE	= 4
};

enum _xsPadKeyCode
{
	XS_PAD_KEY_UNDEFINED	= 0xFF,

	XS_PAD_KEY_0			= 0x30,
	XS_PAD_KEY_1,
	XS_PAD_KEY_2,
	XS_PAD_KEY_3,
	XS_PAD_KEY_4,
	XS_PAD_KEY_5,
	XS_PAD_KEY_6,
	XS_PAD_KEY_7,
	XS_PAD_KEY_8,
	XS_PAD_KEY_9,
	
	XS_PAD_KEY_STAR			= 0x2A,
	XS_PAD_KEY_POUND		= 0x23,
	
	XS_PAD_KEY_LSK			= 0x80,
	XS_PAD_KEY_RSK,
	XS_PAD_KEY_CSK,
	XS_PAD_KEY_SELECT,
	XS_PAD_KEY_ENTER,
	XS_PAD_KEY_UP_ARROW,
	XS_PAD_KEY_DOWN_ARROW,
	XS_PAD_KEY_LEFT_ARROW,
	XS_PAD_KEY_RIGHT_ARROW
};

/**
 * System event handler.
 * @param event
 */
int xsSysEventHandler(xsSysEvent *event);

typedef int (*xsCallbackFunc)(void *param);

#ifdef __cplusplus
}
#endif

#endif /* _XS_SYSEVENT_H_ */
