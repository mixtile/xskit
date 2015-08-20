
static void InitConsole();
static void UninitConsole();
static void OutputConsole(const char *text);

static void ShowEVKitBanner();
static void OutputTestInfo(const char *msg, const char *filename, int line);
static void DoTest();

typedef struct _xsEVKitApp
{
	xsTChar *name;
	int orient;
} xsEVKitApp;

typedef struct _xsEVKitConsole
{
	float rowHeight;
	float rowCount;
	xsTChar **rows;
	int cursor;
	int top;
} xsEVKitConsole;

typedef int (*xsTestInitializeFunc)();
typedef int (*xsTestCleanupFunc)();
typedef void (*xsTestEntryFunc)();

typedef struct _xsTestUnit
{
	const char *name;
	
	xsTestInitializeFunc initial;
	xsTestCleanupFunc cleanup;
	xsTestEntryFunc entry;
} xsTestUnit;

#define XS_TEST_UNIT_NULL	{NULL, NULL, NULL, NULL}
#define XS_TEST(value)	\
	{\
		if (!(value))\
		{\
			g_xsUnitTestState = XS_FALSE;\
			OutputTestInfo(#value, __FILE__, __LINE__);\
			return;\
		}\
		else\
		{\
			g_xsUnitTestState = XS_TRUE;\
		}\
	}

static xsBool g_xsUnitTestState = XS_TRUE;
#define XS_MAX_TEST_INFO_LEN	512
static char g_testInfo[XS_MAX_TEST_INFO_LEN] = {0};

static xsEVKitApp g_xsTheApp = {xsT("EVKit App"), XS_APP_ORIENT_DEFAULT};
static xsEVKitConsole g_xsConsole = {0};
static int g_testCursor = 0;

///////////////////////////////////////////////////////////////////////////////
// Test unit collection
///////////////////////////////////////////////////////////////////////////////

#if XS_EVKIT_STORAGE
	#include "storage.c"
#endif
#if XS_EVKIT_MEMORY
	#include "memory.c"
#endif
static xsTestUnit g_testList[] =
{
#if XS_EVKIT_STORAGE
	{"CreateFile", NULL, NULL, TestCreateFile},
#endif
#if XS_EVKIT_MEMORY
	{"MemCpy", NULL, NULL, TestMemCpy},
#endif
	XS_TEST_UNIT_NULL
};

int xsSysEventHandler(xsSysEvent *event)
{
	switch (event->type)
	{
	case XS_EVT_KEY_UP:
	case XS_EVT_MOUSE_UP:
		DoTest();
		break;
	case XS_EVT_LOAD:
		xsTrace("XS_EVT_LOAD");
		event->data.app.instance = &g_xsTheApp;
		event->data.app.orient = g_xsTheApp.orient;
		event->data.app.name = g_xsTheApp.name;
		break;
	case XS_EVT_START:
		xsTrace("XS_EVT_START");

		InitConsole();
		ShowEVKitBanner();
		break;
	case XS_EVT_SUSPEND:
		xsTrace("XS_EVT_EXIT");
		break;
	case XS_EVT_RESUME:
		xsTrace("XS_EVT_RESUME");
		break;
	case XS_EVT_EXIT:
		xsTrace("XS_EVT_EXIT");
		UninitConsole();
		break;
	}
	
	return XS_EC_OK;
}

static void InitConsole()
{
#if XS_EVKIT_GRAPHICS
	int width, height;
	xsGraphics *gc = NULL;
	xsFontType font;

	gc = xsGetSystemGc();
	xsGetScreenDimension(&width, &height);

	font.size = XS_FONT_SMALL;
	g_xsConsole.rowHeight = xsGetFontHeight(gc, &font);
	g_xsConsole.cursor = 0;
	g_xsConsole.top = 0;

	if (g_xsConsole.rowHeight > 0)
	{
		g_xsConsole.rowCount = height / g_xsConsole.rowHeight;
		g_xsConsole.rows = (xsTChar **)xsCallocNative(sizeof(xsTChar *) * g_xsConsole.rowCount);
	}
#endif
}

static void UninitConsole()
{
	int i;
	if (g_xsConsole.rowCount != 0 && g_xsConsole.rows != NULL)
	{
		for (i = 0; i < g_xsConsole.rowCount; i++)
		{
			xsFreeNative(g_xsConsole.rows[i]);
		}

		xsFreeNative(g_xsConsole.rows);

		g_xsConsole.rowCount = 0;
		g_xsConsole.rows = NULL;
	}	
}

static void OutputConsole(const char *text)
{
#if XS_EVKIT_GRAPHICS
	xsFontType font;
	xsTChar *line;
	size_t len;
	int i, cursor;
	xsGraphics *gc = NULL;
	int width, height;
	xsColor c = {0};

	// replace current row if exists
	line = g_xsConsole.rows[g_xsConsole.cursor];
	if (line != NULL)
		xsFreeNative(line);

	// store text to row
	len = xsStrLen(text);

	line = (xsTChar *)xsMallocNative(sizeof(xsTChar) * (len + 1)); // xsTChar with '\0' end
#ifdef XS_UNICODE
	xsMbsToWcs(line, text, len);
#else
	xsTcsCpy(line, text);
#endif
	line[len] = 0;
	g_xsConsole.rows[g_xsConsole.cursor] = line;

	// paint to screen
	gc = xsGetSystemGc();
	xsGetScreenDimension(&width, &height);

	xsLockScreen();

	// clear screen
	c.alpha = 255;
	c.blue = c.green = c.red = 0;
	xsSetColor(gc, c);
	xsFillRectangle(gc, 0, 0 , width, height);

	// draw rows
	c.alpha = 255;
	c.red = c.green = c.blue = 220;
	font.size = XS_FONT_SMALL;
	xsSetColor(gc, c);
	xsSetFont(gc, &font);
		
	cursor = g_xsConsole.top;
	for (i = 0; i < g_xsConsole.rowCount; i++)
	{
		line = g_xsConsole.rows[cursor];
		if (line == NULL)
			break;

		xsDrawText(gc, line, 0, g_xsConsole.rowHeight * i);

		cursor++;
		if (cursor >= g_xsConsole.rowCount)
			cursor = 0;
	}
	xsUnlockScreen();
	xsFlushScreen(0, 0, width - 1, height - 1);
	// move cursor to next row
	g_xsConsole.cursor++;

	if (g_xsConsole.cursor >= g_xsConsole.rowCount)
		g_xsConsole.cursor = 0;

	if (g_xsConsole.cursor == g_xsConsole.top)
		g_xsConsole.top++;

	if (g_xsConsole.top >= g_xsConsole.rowCount)
		g_xsConsole.top = 0;
#endif

	// output to trace
	xsTrace(text);
}

static void OutputTestInfo(const char *msg, const char *filename, int line)
{
	xsSnprintf(g_testInfo, sizeof(g_testInfo), "%s %s:%d", msg, filename, line);
	OutputConsole(g_testInfo);
}

static void ShowEVKitBanner()
{
#if XS_EVKIT_GRAPHICS
	int width, height;
	float fw, fh, fw2, fh2, offsetY;
	xsColor c = {0};
	xsFontType font = {0};
	const xsTChar *title, *notice;

	xsGraphics *gc = NULL;
	
	gc = xsGetSystemGc();
	xsGetScreenDimension(&width, &height);
	
	xsLockScreen();

	// draw black background
	c.alpha = 255;
	c.blue = 0;
	c.green = 0;
	c.red = 0;
	xsSetColor(gc, c);
	xsFillRectangle(gc, 0, 0 , width, height);

	c.alpha = 255;
	c.red = c.green = c.blue = 255;
	xsSetColor(gc, c);
	font.size = XS_FONT_LARGE;
	xsSetFont(gc, &font);

	// draw title centralized
	fw = fh = 0;
	title = xsT("EVKIT Start");
	xsMeasureText(gc, title, &fw, &fh);
	offsetY = (height - fw) / 2;
	xsDrawText(gc, title, (width - fw) / 2, offsetY);
	offsetY += fh;

	// draw notice
	c.alpha = 255;
	c.red = c.green = c.blue = 220;
	xsSetColor(gc, c);
	font.size = XS_FONT_SMALL;
	xsSetFont(gc, &font);

	fw2 = fh2 = 0;
	notice = xsT("press any key");
	xsMeasureText(gc, notice, &fw2, &fh2);
	xsDrawText(gc, notice, (width - fw2) / 2, offsetY);
	offsetY += fh2;

	notice = xsT("or");
	xsMeasureText(gc, notice, &fw2, &fh2);
	xsDrawText(gc, notice, (width - fw2) / 2, offsetY);
	offsetY += fh2;

	notice = xsT("touch screen");
	xsMeasureText(gc, notice, &fw2, &fh2);
	xsDrawText(gc, notice, (width - fw2) / 2, offsetY);
	offsetY += fh2;

	notice = xsT("to continue.");
	xsMeasureText(gc, notice, &fw2, &fh2);
	xsDrawText(gc, notice, (width - fw2) / 2, offsetY);
	offsetY += fh2;

	xsUnlockScreen();
	
	xsFlushScreen(0, 0, width - 1, height - 1);
#endif

	xsTrace("EVKIT Start");
	xsTrace("touch screen or press any key to continue.");
}

static void DoTest()
{
	xsTestUnit *unit;
	int ret = XS_EC_OK;

	if (g_testList[g_testCursor].name == NULL)
	{// test finished.
		g_testCursor = 0;
		OutputConsole("--------------------");
		OutputConsole("Test finished.");
		OutputConsole("--------------------");
		return;
	}

	unit = &g_testList[g_testCursor];
	OutputConsole(unit->name);

	if (unit->initial != NULL)
		ret = unit->initial();

	if (ret == XS_EC_OK)
	{
		if (unit->entry != NULL)
		{
			unit->entry();
			if (g_xsUnitTestState)
				OutputConsole("Success.");
			else
				OutputConsole("Failed.");
		}
		if (unit->cleanup != NULL)
			ret = unit->cleanup();

		if (ret != XS_EC_OK)
			OutputConsole("Unit clear failed.");
	}

	g_testCursor++;
}
