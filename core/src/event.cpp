#include <xs/event.h>
#include <xs/memory.h>

// Post事件队列
static xsQueue g_xsEventQueue = NULL;
typedef struct _xsEventQueueItem
{
	xsListener *callback;
	xsEvent evt;
} xsEventQueueItem;

xsEventSource::xsEventSource()
{
	_listeners = xsArrayListCreate(2);
}

xsEventSource::~xsEventSource()
{
	// destroy listener list
	if (_listeners != NULL)
		xsArrayListDestroy(_listeners);
}

void xsEventSource::addListener(xsListener *callback)
{
	if (_listeners == NULL)
	{
		_listeners = xsArrayListCreate(2);
	}

	xsArrayListAdd(_listeners, callback);
}

void xsEventSource::removeListener(xsListener *callback)
{
	int index;
	if (_listeners == NULL)
		return;

	index = xsArrayListIndex(_listeners, callback);
	if (index >= 0)
		xsArrayListRemove(_listeners, index);
}

void xsEventSource::notifyListener(xsEvent *e, xsBool usePost)
{
	xsListener *callback;
	xsIterator iter = NULL;

	for (;;)
	{
		callback = (xsListener *)xsArrayListIterate(_listeners, &iter);
		if (callback == NULL || iter == NULL)
			break;

		if (usePost)
			xsEvent::post(callback, e);
		else
			xsEvent::send(callback, e);
	}
}

static int EventNotify(void *userdata)
{
	if (g_xsEventQueue != NULL)
	{
		xsEventQueueItem *eqi;
		
		while ((eqi = (xsEventQueueItem *)xsQueuePop(g_xsEventQueue)) != NULL)
		{
			eqi->callback->processEvent(&eqi->evt);
			xsFree(eqi);
		}
	}

	return 0;
}

void xsEvent::post(xsListener *callback, xsEvent *evt)
{
	xsEventQueueItem *eqi;
	XS_ASSERT(callback != NULL);

	eqi = (xsEventQueueItem *)xsMalloc(sizeof(xsEventQueueItem));
	if (eqi == NULL)
		return;

	eqi->callback = callback;
	eqi->evt = *evt;

	xsQueuePush(g_xsEventQueue, eqi);

	// Start a timer
	if (xsQueueSize(g_xsEventQueue) == 1)
		xsStartTimer(10, EventNotify, NULL);
}

//////////////////////////////////////////////////////////////////////////////
// Event module functions
//////////////////////////////////////////////////////////////////////////////
int xsEvtInit(void)
{
	XS_TRACE("[CORE]xsEvtInit");

	// 初始化Post消息队列
	XS_ASSERT(g_xsEventQueue == NULL);
	g_xsEventQueue = xsQueueCreate(16);
	if (g_xsEventQueue == NULL)
		return XS_EC_NOMEM;

	return XS_EC_OK;
}

void xsEvtUninit(void)
{
	// clear event queue
	if (g_xsEventQueue != NULL)
	{
		xsEventQueueItem *eqi;
		
		while ((eqi = (xsEventQueueItem *)xsQueuePop(g_xsEventQueue)) != NULL)
		{
			xsFree(eqi);
		}
		xsQueueDestroy(g_xsEventQueue);
		g_xsEventQueue = NULL;
	}
}
