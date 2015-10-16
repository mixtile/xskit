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

#ifndef _XS_EVENT_H_
#define _XS_EVENT_H_

#include <xs/pal.h>
#include <xs/base.h>

#ifndef __cplusplus
// for c interface only
typedef struct xsEvent
{
	int type;
	xsSysEvent *sys;
} xsEvent;

#else

#include <xs/container.h>
#include <xs/datatypes.h>

struct xsEvent;

class xsListener : public xsBase
{
public:
	virtual int processEvent(xsEvent *e) = 0;
};

class xsCallbackListener : public xsListener
{
public:
	xsCallbackListener() {};
	xsCallbackListener(xsCallbackFunc cb) { _cb = cb; }

	inline void setCallback(xsCallbackFunc cb) { _cb = cb; }

	virtual inline int processEvent(xsEvent *e) { return _cb(e); }

private:
	xsCallbackFunc _cb;
};

typedef xsListener *(*xsListenerCreateFunc)();

class xsEventSource
{
public:
	xsEventSource();
	~xsEventSource();

	void addListener(xsListener *callback);
	void removeListener(xsListener *callback);
	void notifyListener(xsEvent *e, xsBool usePost);

private:
	xsArrayList _listeners;
};

/**
 * Base event
 */
struct xsEvent
{
	int type;
	xsSysEvent *sys;

	static inline int send(xsListener *callback, xsEvent *evt) { XS_ASSERT(callback != NULL); return callback->processEvent(evt); }
	static void post(xsListener *callback, xsEvent *evt);
};

/**
 * Common event
 */
struct xsCommonEvent : public xsEvent
{
	int param;
	void *ptr;
};

/*
 * GUI Events
 */
struct xsMouseEvent : public xsEvent
{
	int x, y;
	int button;
};

struct xsKeyEvent : public xsEvent
{
	int keyCode;
};

struct xsFocusEvent : public xsEvent
{
};

struct xsPopupEvent : public xsEvent
{
	xsWidget *invoker;
	xsU8 justify;
	xsBool stretch;
};

struct xsSelectEvent : public xsEvent
{
	xsWidget *selected;
};

struct xsMultiSelectEvent : public xsEvent
{
	xsArray *items;
};

enum _xsJustifyType
{
	XS_JUSTIFY_NONE			= 0,
	XS_JUSTIFY_COLUMN,
	XS_JUSTIFY_LINE,
	XS_JUSTIFY_CENTER
};

struct xsValueEvent : public xsEvent
{
	xsValue *value;
	int focusId;	// optional, for multi-value widget
};

struct xsGetElementEvent : public xsEvent
{
	const char *id;
	xsObject *element;
};

/*
 * Data
 */
struct xsDataEvent : public xsEvent
{
	xsDataSource *ds;	// DataSource
};

/*
 * Connection event
 */
struct xsConnEvent : public xsEvent
{
	xsAny result;	// xsConnResult
};

/*
 * Http connection event
 */
struct xsHttpConnEvent : public xsEvent
{
	xsAny conn;	// HttpConn

	short status;
	xsAny headers; // ArrayList
	char *content;
	size_t len;
};

enum _xsEventType
{
	XS_EVT_BEGIN			= XS_EVT_SYS_END,

	// Mouse
	XS_EVT_MOUSE_LEAVE,
	// Focus management
	XS_EVT_FOCUS_GAINED,
	XS_EVT_FOCUS_LOST,
	// Form
	XS_EVT_FORM_LOAD,			// xsCommonEvent
	XS_EVT_FORM_SHOW,			// xsCommonEvent
	XS_EVT_FORM_BACK,			// xsCommonEvent
	// Popup
	XS_EVT_POPUP,
	XS_EVT_POPUP_CLOSE,
	XS_EVT_POPUP_END,
	// Animation
	XS_EVT_ANIMATE_START,
	XS_EVT_ANIMATE_STOP,
	// Selection
	XS_EVT_SELECT,
	XS_EVT_GET_SELECTED,
	XS_EVT_GET_SELECTED_ITEMS,	// xsMultiSelectEvent
	XS_EVT_GET_SELECT_STATUS,	// xsMultiSelectEvent

//	XS_EVT_GET_VALUE,
//	XS_EVT_SET_VALUE,
	XS_EVT_GET_SELECTED_VALUE,
	XS_EVT_CLEAR_VALUE,			// xsCommonEvent

	XS_EVT_GET_WIDGET,
	// Data
	//XS_EVT_DATA_BOUND,			// xsDataEvent
	XS_EVT_DATA_UNFINISHED,		// xsCommonEvent
	XS_EVT_DATA_FINISHED,		// xsCommonEvent
	XS_EVT_DATA_UPDATE,			// xsDataEvent, data source updated notice
	XS_EVT_DATA_FLUSH,			// xsCommonEvent, flush shadow data to data-source
	XS_EVT_DATA_SAVE,			// xsCommonEvent, save widget's bound data source
	// Network
	XS_EVT_CONN_ESTABLISHED,
	XS_EVT_CONN_RECEIVED,
	XS_EVT_CONN_SENT,
	XS_EVT_CONN_CLOSED,

	XS_EVT_HTTP_RESPONSE,		// xsHttpConnEvent
	XS_EVT_HTTP_PROGRESS,		// xsCommonEvent.param, 0 ~ 10000, 100x download percents

	XS_EVT_END
};

typedef struct _xsEventQueue
{
	int count;
	int head;
	int tail;
	xsArrayList queue;
} xsEventQueue;

#endif /* __cplusplus */

#endif /* _XS_EVENT_H_ */
