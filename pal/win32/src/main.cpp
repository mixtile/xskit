#include <windows.h>
#include <string.h>
#include <tchar.h>
#include <gdiplus.h>
#include <locale.h>
#include <stdio.h>
#include "CmdLine.h"
using namespace Gdiplus;

#include <xs/pal.h>

extern "C"
{
	HWND g_hMainWnd;
	HINSTANCE g_hInst;
	void xsPalResetFrameWnd();
}
TCHAR g_szTitle[64];
TCHAR g_szWindowClass[64];
char g_szAppId[MAX_PATH] = {0};
float g_nScreenWidth = 240;
float g_nScreenHeight = 320;

extern Bitmap *g_bmpBase;
extern Graphics *g_gcBase;
HDC g_dcBase;

int g_nFrmWidth;
int g_nFrmHeight;

Font *g_fntSysBtn;
int g_nSysBtnWidth;
int g_nSysBtnHeight;
int g_nTitleHeight;
BOOL g_bSysBtnCaptured = FALSE;

Bitmap *g_bmpFrame;	// window frame's bitmap
HDC g_dcFrame;

ATOM RegisterWndClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL EnableLayeredWnd();
void PaintSysBtn(BOOL bButtonDown);

static int HandleGetAddrByName(UINT message, WPARAM wParam, LPARAM lParam);
static xsU16 TranslateKeyCode(xsU32 code);

int MainEntry(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	MSG msg;
	ULONG_PTR           gdiplusToken;
	GdiplusStartupInput gdiplusStartupInput;
	BOOL bConsoleAttached = FALSE;

	// Set locale
	char *locale = setlocale(LC_ALL, ".936");

	// Startup GDI+
	Status status = GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	if (status != Ok)
		return FALSE;

	// set options by command line
	CCmdLine cmdline;
	if (cmdline.SplitLine(__argc, __argv) > 0)
	{
		if (cmdline.HasSwitch("-width"))
			g_nScreenWidth = atoi(cmdline.GetArgument("-width", 0).c_str());
		if (cmdline.HasSwitch("-height"))
			g_nScreenHeight = atoi(cmdline.GetArgument("-height", 0).c_str());

		if (cmdline.HasSwitch("-d"))
		{
			if (AttachConsole(ATTACH_PARENT_PROCESS))
			{
				bConsoleAttached = TRUE;
				freopen("CONIN$", "r", stdin);
				freopen("CONOUT$", "w", stdout);
				freopen("CONOUT$", "w", stderr);
			}
		}

		if (cmdline.HasSwitch("-appid"))
		{
			xsStrCpyN(g_szAppId, cmdline.GetArgument("-appid", 0).c_str(), sizeof(g_szAppId));
		}
	}

	xsTcsCpy((xsTChar *)g_szTitle, xsT("XSKit"));
	xsTcsCpy((xsTChar *)g_szWindowClass, xsT("XSKitMainWnd"));

	RegisterWndClass(hInstance);

	if (!InitInstance (hInstance, nCmdShow))
	{
		if (bConsoleAttached)
			FreeConsole();
		return FALSE;
	}

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	xsPalGuiUninit();

	if (bConsoleAttached)
		FreeConsole();
	// Shutdown GDI+
	GdiplusShutdown(gdiplusToken);

	return msg.wParam;
}

ATOM RegisterWndClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= NULL;
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= g_szWindowClass;
	wcex.hIconSm		= NULL;

	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;
	DWORD dwStyle;
	xsSysEvent evt = {0};

	evt.type = XS_EVT_LOAD;
	evt.data.app.uri = g_szAppId;
	if (xsSysEventHandler(&evt) == XS_EC_OK)
	{// get app properties
		if (evt.data.app.name != NULL)
			xsTcsCpy((xsTChar *)g_szTitle, evt.data.app.name);
		if (evt.data.app.orient != XS_APP_ORIENT_DEFAULT)
		{
			int tempWidth = g_nScreenWidth;
			// exchange width and height if required
			if ((evt.data.app.orient == XS_APP_ORIENT_LANDSCAPE && g_nScreenWidth < g_nScreenHeight) ||
				(evt.data.app.orient == XS_APP_ORIENT_PORTRAIT && g_nScreenWidth > g_nScreenHeight))
			{
				g_nScreenWidth = g_nScreenHeight;
				g_nScreenHeight = tempWidth;
			}
		}
	}
	else
	{
		return FALSE;
	}

	g_hInst = hInstance;

	hWnd = CreateWindowEx(WS_EX_LAYERED, g_szWindowClass, g_szTitle, WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT, g_nScreenWidth, g_nScreenHeight, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}

	g_hMainWnd = hWnd;

	dwStyle = GetWindowLong(hWnd, GWL_STYLE);
	dwStyle = dwStyle & (~WS_CAPTION);
	SetWindowLong(hWnd, GWL_STYLE, dwStyle);

	xsPalResetFrameWnd();

	xsPalGuiInit();

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	// start application
	evt.type = XS_EVT_START;
	xsSysEventHandler(&evt);

	return TRUE;
}

void xsPalResetFrameWnd()
{
	// Initial system button
	g_fntSysBtn = new Font(_T("Marlett"), 10);
	RectF rectf;
	RectF layoutRect(0, 0, 800, 800);
	Graphics g(GetDC(g_hMainWnd));
	g.MeasureString(_T("\uF072"), 1, g_fntSysBtn, layoutRect, &rectf);
	g_nSysBtnWidth = (int)rectf.Width;
	g_nSysBtnHeight = (int)rectf.Height;
	g_nTitleHeight = g_nSysBtnHeight + XS_MARGIN * 2;

	g_nFrmWidth = g_nScreenWidth + XS_MARGIN * 2;
	g_nFrmHeight = g_nScreenHeight + g_nTitleHeight + XS_MARGIN;

	SetWindowPos(g_hMainWnd, NULL, 0, 0, g_nFrmWidth, g_nFrmHeight, SWP_NOMOVE | SWP_NOREPOSITION);
	EnableLayeredWnd();
}

BOOL EnableLayeredWnd()
{
	HDC dcWnd = GetDC(g_hMainWnd);

	g_bmpFrame = new Bitmap(g_nFrmWidth, g_nFrmHeight);
	g_dcFrame = CreateCompatibleDC(dcWnd);

	//Bitmap bitmap(g_szSkinFile);
	HBITMAP bmp;
	g_bmpFrame->GetHBITMAP(Color(0, 0, 0), &bmp);
	SelectObject(g_dcFrame, bmp);
	Graphics g(g_dcFrame);
	SolidBrush brh(Color(192, 0, 0, 0));
	g.FillRectangle(&brh, 0, 0, g_nFrmWidth, g_nFrmHeight);

	PaintSysBtn(FALSE);

	return TRUE;
}

int xsUpdateScreen(int left, int top, int right, int bottom)
{
	// Update screen
	Graphics gc(g_dcFrame);
	gc.DrawImage(g_bmpBase, left + XS_MARGIN, top + g_nTitleHeight, left, top, right - left + 1, bottom - top + 1, UnitPixel);

	// Update layered window
	HDC dcWnd = GetDC(g_hMainWnd);

	POINT ptSrc;
	ptSrc.x = ptSrc.y = 0;

	RECT rect;
	GetWindowRect(g_hMainWnd, &rect);
	POINT pt;
	pt.x = rect.left;
	pt.y = rect.top;

	SIZE size;
	size.cx = g_nFrmWidth;
	size.cy = g_nFrmHeight;

	BLENDFUNCTION bf;
	bf.BlendOp             = AC_SRC_OVER;						// Only works with a 32bpp bitmap
	bf.BlendFlags          = 0;									// Always 0
	bf.SourceConstantAlpha = 255;								// Set to 255 for per-pixel alpha values
	bf.AlphaFormat         = AC_SRC_ALPHA;						// Only works when the bitmap contains an alpha channel

	return UpdateLayeredWindow(g_hMainWnd, dcWnd, &pt, &size, g_dcFrame, &ptSrc, 0, &bf, ULW_ALPHA);
}

BOOL InSysBtn(int x, int y)
{
	if (y < XS_MARGIN || y > XS_MARGIN + g_nSysBtnHeight
		|| x > g_nFrmWidth - XS_MARGIN || x < g_nFrmWidth - g_nSysBtnWidth)
		return FALSE;

	return TRUE;
}

void PaintSysBtn(BOOL bButtonDown)
{
	int x = g_nFrmWidth - XS_MARGIN - g_nSysBtnWidth;
	int y = XS_MARGIN;

	Color clrText;

	if (bButtonDown)
		clrText = Color(255, 255, 255);
	else
		clrText = Color(220, 220, 220);

	Graphics g(g_dcFrame);
	//SolidBrush brh(Color(192, 0, 0, 0));
	//g.FillRectangle(&brh, x, y, g_nSysBtnWidth, g_nSysBtnHeight);
	
	SolidBrush brhText(clrText);
	g.DrawString(_T("\uF072"), 1, g_fntSysBtn, PointF((REAL)x, (REAL)y), &brhText);

	// Map to screen based coodinary, then update
	x -= XS_MARGIN;
	y -= g_nTitleHeight;
	xsUpdateScreen(x, y, x + g_nSysBtnWidth - 1, y + g_nSysBtnHeight - 1);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	xsSysEvent evt;
	POINT pt = {0};
	static BOOL bIsKeyDown = FALSE;
//	HDC hdc;
//	PAINTSTRUCT ps;

	if (message >= WM_XS_RESOLVE_MSG && message <= WM_XS_RESOLVE_MSG_END)
		return xsPalHandleGetAddrByName(message, wParam, lParam);

	switch (message)
	{
/*	case WM_PAINT:
		//hdc = BeginPaint(hWnd, &ps);
		//			hdcBase = g_gcBase->GetHDC();
		//			BitBlt(hdc, 0, 0, g_nScreenWidth, g_nScreenHeight,
		//					hdcBase, 0, 0, SRCCOPY);
		//			g_gcBase->ReleaseHDC(hdcBase);
		{
	//		Graphics gc(hdc);
		//	gc.DrawImage(g_bmpBase, 0, 0);
		}
		//EndPaint(hWnd, &ps);
		break;*/
	case WM_LBUTTONDOWN:
		SetCapture(g_hMainWnd);
		pt.x = LOWORD(lParam);
		pt.y = HIWORD(lParam);
		if (InSysBtn(pt.x, pt.y))
		{
			PaintSysBtn(TRUE);			
			g_bSysBtnCaptured = TRUE;
			break;
		}
		evt.type = XS_EVT_MOUSE_DOWN;
		evt.data.mouse.x = pt.x - XS_MARGIN;
		evt.data.mouse.y = pt.y - g_nTitleHeight;
		xsSysEventHandler(&evt);
		break;
	case WM_LBUTTONUP:
		ReleaseCapture();
		pt.x = LOWORD(lParam);
		pt.y = HIWORD(lParam);
		if (InSysBtn(pt.x, pt.y) || g_bSysBtnCaptured)
		{
			PaintSysBtn(FALSE);
			g_bSysBtnCaptured = FALSE;

			if (InSysBtn(pt.x, pt.y))
				PostMessage(g_hMainWnd, WM_CLOSE, 0, 0);
			break;
		}
		evt.type = XS_EVT_MOUSE_UP;
		evt.data.mouse.x = pt.x - XS_MARGIN;
		evt.data.mouse.y = pt.y - g_nTitleHeight;
		xsSysEventHandler(&evt);
		break;
	case WM_MOUSEMOVE:
		evt.type = XS_EVT_MOUSE_MOVE;
		pt.x = LOWORD(lParam);
		pt.y = HIWORD(lParam);
		if (g_bSysBtnCaptured)
		{
			static BOOL bIn = FALSE;
			
			if (bIn != InSysBtn(pt.x, pt.y))
			{
				bIn = !bIn;
				PaintSysBtn(bIn);
			}
			break;
		}
		evt.data.mouse.x = pt.x - XS_MARGIN;
		evt.data.mouse.y = pt.y - g_nTitleHeight;
		evt.data.mouse.button = wParam;
		xsSysEventHandler(&evt);
		break;
	case WM_KEYDOWN:
		bIsKeyDown = TRUE;
		evt.type = XS_EVT_KEY_DOWN;
		evt.data.key.keyCode = TranslateKeyCode((xsU32)wParam);
		xsSysEventHandler(&evt);
		break;

	case WM_KEYUP:
		if (bIsKeyDown)
		{
			evt.type = XS_EVT_KEY_UP;
			evt.data.key.keyCode = TranslateKeyCode((xsU32)wParam);
			xsSysEventHandler(&evt);
		}
		bIsKeyDown = FALSE;
		break;
	case WM_NCHITTEST:
        pt.x = lParam & 0x0000FFFF;
        pt.y = (lParam & 0xFFFF0000) >> 16;
        ScreenToClient(hWnd, &pt);

        if (pt.y >= 0 && pt.y < g_nTitleHeight && !InSysBtn(pt.x, pt.y))
			return HTCAPTION;

		return DefWindowProc(hWnd, message, wParam, lParam);
	case WM_XS_SOCKET_MSG:
		xsPalHandleSocketEvent(message, wParam, lParam);
		break;
	case WM_CLOSE:
		evt.type = XS_EVT_EXIT;
		xsSysEventHandler(&evt);
		DestroyWindow(hWnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

static xsU16 TranslateKeyCode(xsU32 code)
{
    xsU16 key = code;

	switch (code)
	{
	case VK_F4:
		return XS_PAD_KEY_LSK;
	case VK_F5:
		return XS_PAD_KEY_CSK;
	case VK_F6:
		return XS_PAD_KEY_RSK;
	case VK_NUMPAD0:
		return XS_PAD_KEY_0;
	case VK_NUMPAD1:
		return XS_PAD_KEY_1;
	case VK_NUMPAD2:
		return XS_PAD_KEY_2;
	case VK_NUMPAD3:
		return XS_PAD_KEY_3;
	case VK_NUMPAD4:
		return XS_PAD_KEY_4;
	case VK_NUMPAD5:
		return XS_PAD_KEY_5;
	case VK_NUMPAD6:
		return XS_PAD_KEY_6;
	case VK_NUMPAD7:
		return XS_PAD_KEY_7;
	case VK_NUMPAD8:
		return XS_PAD_KEY_8;
	case VK_NUMPAD9:
		return XS_PAD_KEY_9;
	case VK_MULTIPLY:
		return XS_PAD_KEY_STAR;
	case VK_LEFT:
		return XS_PAD_KEY_LEFT_ARROW;
	case VK_RIGHT:
		return XS_PAD_KEY_RIGHT_ARROW;
	case VK_UP:
		return XS_PAD_KEY_UP_ARROW;
	case VK_DOWN:
		return XS_PAD_KEY_DOWN_ARROW;
	case VK_RETURN:
		return XS_PAD_KEY_SELECT;
	}

	return key;
}
