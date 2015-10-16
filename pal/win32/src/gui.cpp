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
 
#include <windows.h>
#include <gdiplus.h>
using namespace Gdiplus;

#include <xs/pal.h>

extern int g_nScreenWidth;
extern int g_nScreenHeight;
extern int g_nTitleHeight;

static volatile xsBool g_doubleBufferLocked = XS_FALSE;

#define XS_RGB(c) Color::MakeARGB(c.alpha, c.red, c.green, c.blue)
const float PI = 3.1415926;

//static Font *g_fonts[XS_FONT_SIZE_MAX] = {NULL};
static REAL g_fontSizes[XS_FONT_SIZE_COUNT] = {6, 9, 12, 16, 20, 24, 36};
static const TCHAR *g_fontDefault = L"SimSun";
static Font *g_fontCurrent = NULL;
static xsGraphics g_sysGc = {0};

Bitmap *g_bmpBase = NULL;
Graphics *g_gcBase = NULL;
static Pen *g_penBase = NULL;
static SolidBrush *g_brushBase = NULL;
static SolidBrush *g_brushText = NULL;

int xsPalGuiInit(void)
{
	// System Font
/*	const FontFamily *ff = new FontFamily(L"SimSun");
	if (!ff->IsAvailable())
		ff = FontFamily::GenericSansSerif();

	int i;
	for (i = 0; i < XS_FONT_SIZE_MAX; i++)
	{
		g_fonts[i] = new Font(ff, g_fontSizes[i], FontStyleRegular, UnitPixel);
	}
	
	if (ff != FontFamily::GenericSansSerif())
		delete ff;*/

	// Double buffer
	Status status;
	g_bmpBase = new Bitmap(g_nScreenWidth, g_nScreenHeight);
	g_gcBase = new Graphics(g_bmpBase);
	status = g_gcBase->SetTextRenderingHint(TextRenderingHintClearTypeGridFit);

	// Device
	g_penBase = new Pen(Color::Black, 1);
	g_brushBase = new SolidBrush(Color::Black);
	g_brushText = new SolidBrush(Color::Black);

	return 0;
}

void xsPalGuiUninit(void)
{
	// Destroy device
	delete g_penBase;
	g_penBase = NULL;
	delete g_brushBase;
	g_brushBase = NULL;
	delete g_brushText;
	g_brushText = NULL;

	// Destroy double buffer
	delete g_gcBase;
	g_gcBase = NULL;
	delete g_bmpBase;
	g_bmpBase = NULL;

	// Clear system fonts.
/*	int i;
	for (i = 0; i < XS_FONT_SIZE_MAX; i++)
	{
		delete g_fonts[i];
		g_fonts[i] = NULL;
	}*/
}

void xsLockScreen(void)
{
	g_doubleBufferLocked = XS_TRUE;
}

void xsUnlockScreen(void)
{
	g_doubleBufferLocked = XS_FALSE;

	// TODO: flush updated area only
	xsFlushScreen(0, 0, g_nScreenWidth, g_nScreenHeight);
}

void xsFlushScreen(int left, int top, int right, int bottom)
{
	if (!g_doubleBufferLocked)
		xsUpdateScreen(left, top, right, bottom);
}

xsGraphics *xsGetSystemGc(void)
{
	return &g_sysGc;
}


void *xsLockPixelBuffer(xsGraphics *gc, int pixelDepth)
{
	PixelFormat pf;
	switch (pixelDepth)
	{
	case 15:
		pf = PixelFormat16bppRGB555;
		break;
	case 16:
		pf = PixelFormat16bppRGB565;
		break;
	case 24:
		pf = PixelFormat24bppRGB;
		break;
	case 32:
		pf = PixelFormat32bppARGB;
		break;
	default:
		return NULL;
	}

	if (gc->fb != NULL)
	{
		if (pixelDepth == gc->pixelDepth)
			return ((BitmapData *)(gc->fb))->Scan0;
		
		xsUnlockPixelBuffer(gc);
	}

	gc->fb = new BitmapData;
	gc->pixelDepth = pixelDepth;

	Rect rect;
	rect.X = 0;
	rect.Y = 0;
	rect.Width = g_nScreenWidth;
	rect.Height = g_nScreenHeight;

	int ret = g_bmpBase->LockBits(&rect, ImageLockModeRead | ImageLockModeWrite,
		pf, (BitmapData *)gc->fb);
	if (ret != Ok)
	{
		delete gc->fb;
		gc->fb = NULL;
		return NULL;
	}

	return ((BitmapData *)(gc->fb))->Scan0;
}

void xsUnlockPixelBuffer(xsGraphics *gc)
{
	if (gc->fb != NULL)
	{
		g_bmpBase->UnlockBits((BitmapData *)gc->fb);
		delete gc->fb;
		gc->fb = NULL;
	}
}

static void LayoffPixelBuffer(xsGraphics *gc)
{
	if (gc->fb != NULL)
		g_bmpBase->UnlockBits((BitmapData *)gc->fb);
}

static void RelockPixelBuffer(xsGraphics *gc)
{
	if (gc->fb == NULL)
		return;

	PixelFormat pf;
	switch (gc->pixelDepth)
	{
	case 15:
		pf = PixelFormat16bppRGB555;
		break;
	case 16:
		pf = PixelFormat16bppRGB565;
		break;
	case 24:
		pf = PixelFormat24bppRGB;
		break;
	case 32:
		pf = PixelFormat32bppARGB;
		break;
	default:
		return;
	}

	Rect rect;
	rect.X = 0;
	rect.Y = 0;
	rect.Width = g_nScreenWidth;
	rect.Height = g_nScreenHeight;

	g_bmpBase->LockBits(&rect, ImageLockModeRead | ImageLockModeWrite,
		pf, (BitmapData *)gc->fb);
}

void xsGcTranslate(xsGraphics *gc, float xoffset, float yoffset)
{
	XS_ASSERT(gc != NULL);

	gc->xoffset += xoffset;
	gc->yoffset += yoffset;
}

void xsGcRotate(xsGraphics *gc, float angle)
{
	g_gcBase->RotateTransform(angle);
}

void xsGcScale(xsGraphics *gc, float scalewidth, float scaleheight)
{
	g_gcBase->ScaleTransform(scalewidth, scaleheight);
}

void xsGcTransform(xsGraphics *gc, float xx, float yx, float xy, float yy, float x0, float y0)
{
	Matrix matrix(xx, yx, xy, yy, x0, y0);
	g_gcBase->SetTransform(&matrix);
}


xsBool xsSetScreenOrient(int orient)
{
	if (orient != XS_APP_ORIENT_DEFAULT)
	{
		int tempWidth = g_nScreenWidth;
		// exchange width and height if required
		if ((orient == XS_APP_ORIENT_LANDSCAPE && g_nScreenWidth < g_nScreenHeight) ||
			(orient == XS_APP_ORIENT_PORTRAIT && g_nScreenWidth > g_nScreenHeight))
		{
			g_nScreenWidth = g_nScreenHeight;
			g_nScreenHeight = tempWidth;
		
			delete g_bmpBase;
			delete g_gcBase;

			g_bmpBase = new Bitmap(g_nScreenWidth, g_nScreenHeight);
			g_gcBase = new Graphics(g_bmpBase);
			g_gcBase->SetTextRenderingHint(TextRenderingHintClearTypeGridFit);

			xsPalResetFrameWnd();
		}
	}

	return XS_TRUE;
}

xsBool xsGetScreenDimension(int *width, int *height)
{
	if (width == NULL || height == NULL)
		return XS_FALSE;

	*width = g_nScreenWidth;
	*height = g_nScreenHeight;

	return XS_TRUE;
}

xsBool xsGetClientRect(xsRect *rect)
{
	if (rect == NULL)
		return XS_FALSE;

	rect->left = 0;
	rect->top = 0;
	rect->bottom = g_nScreenHeight - 1;
	rect->right = g_nScreenWidth - 1;

	return XS_TRUE;
}

void xsSetClipRect(xsGraphics *gc, xsRect *rect)
{
	Rect rc;
	rc.X = rect->left + gc->xoffset;
	rc.Y = rect->top + gc->yoffset;
	rc.Width = rect->right - rect->left + 1;
	rc.Height = rect->bottom - rect->top + 1;
	g_gcBase->SetClip(rc);
}

void xsGetClipRect(xsGraphics *gc, xsRect *rect)
{
	Region reg;
	Rect rc;
	g_gcBase->GetClip(&reg);
	reg.GetBounds(&rc, g_gcBase);

	rect->left = rc.X - gc->xoffset;
	rect->top = rc.Y - gc->yoffset;
	rect->right = rc.X + rc.Width - 1 - gc->xoffset;
	rect->bottom = rc.Y + rc.Height - 1 - gc->yoffset;
}

void xsResetClipRect(xsGraphics *gc)
{
	g_gcBase->ResetClip();
}

void xsSetColor(xsGraphics *gc, xsColor color)
{
	g_penBase->SetColor(XS_RGB(color));
	g_brushBase->SetColor(XS_RGB(color));
}

void xsSetStrokeStyle(xsGraphics *gc, xsStrokeStyle *style)
{}

void xsDrawLine(xsGraphics *gc, float x1, float y1, float x2, float y2)
{
	Status status;
	status = g_gcBase->DrawLine(g_penBase, x1 + gc->xoffset, y1 + gc->yoffset, x2 + gc->xoffset, y2 + gc->yoffset);
}

void xsDrawRectangle(xsGraphics *gc, float x, float y, float width, float height)
{
	g_gcBase->DrawRectangle(g_penBase, x + gc->xoffset, y + gc->yoffset, width - 1, height - 1); // buggy GDI+?
}

void xsFillRectangle(xsGraphics *gc, float x, float y, float width, float height)
{
	g_gcBase->FillRectangle(g_brushBase, x + gc->xoffset, y + gc->yoffset, width, height);
}

void xsFillTriangle(xsGraphics *gc, float x1, float y1, float x2, float y2, float x3, float y3, xsColor c)
{
	Point points[3];

	points[0].X = x1 + gc->xoffset - 1; // buggy GDI+?
	points[0].Y = y1 + gc->yoffset - 1; // buggy GDI+?
	points[1].X = x2 + gc->xoffset;
	points[1].Y = y2 + gc->yoffset;
	points[2].X = x3 + gc->xoffset;
	points[2].Y = y3 + gc->yoffset;

	g_brushBase->SetColor(XS_RGB(c));
	g_gcBase->FillPolygon(g_brushBase, points, 3);
}

void xsDrawPolygon(xsGraphics *gc, xsPoint pt[], xsU32 count)
{
	Point *points = (Point*)malloc(sizeof(Point)*count);
	memset(points, 0, sizeof(Point));
	points[0].X = pt[0].x + gc->xoffset - 1;
	points[0].Y = pt[0].y + gc->yoffset - 1;
	for(int i=1; i<count; i++)
	{
		points[i].X = pt[i].x + gc->xoffset;
		points[i].Y = pt[i].y + gc->yoffset;
	}

	g_gcBase->DrawPolygon(g_penBase, points, count);
}

void xsFillPolygon(xsGraphics *gc, xsPoint pt[], xsU32 count)
{
	Point *points = (Point*)malloc(sizeof(Point)*count);
	memset(points, 0, sizeof(Point));
	points[0].X = pt[0].x + gc->xoffset - 1;
	points[0].Y = pt[0].y + gc->yoffset - 1;
	for(int i=1; i<count; i++)
	{
		points[i].X = pt[i].x + gc->xoffset;
		points[i].Y = pt[i].y + gc->yoffset;
	}
	g_gcBase->FillPolygon(g_brushBase, points, count);
}

void xsDrawCircle(xsGraphics *gc, int x, int y, int r)
{
	g_gcBase->DrawEllipse(g_penBase, (int)(x - r + gc->xoffset - 1), (int)(y - r + gc->yoffset - 1), r * 2 - 1, r * 2 - 1); // buggy GDI+?
}

void xsFillCircle(xsGraphics *gc, int x, int y, int r)
{
	g_gcBase->FillEllipse(g_brushBase, (int)(x - r + gc->xoffset - 1), (int)(y - r + gc->yoffset - 1), r * 2 - 1, r * 2 - 1); // buggy GDI+?
}

void xsDrawArc(xsGraphics *gc, float x, float y, float r, float startAngle, float endAngle)
{
	g_gcBase->DrawPie(g_penBase, x - r/2 + gc->xoffset - 1, y - r/2 + gc->yoffset - 1, 2*r -1, 2*r -1, startAngle/PI*180, (startAngle + endAngle)/PI*180);
}

void xsFillArc(xsGraphics *gc, float x, float y, float r, float startAngle, float endAngle)
{
	g_gcBase->FillPie(g_brushBase, x - r/2 + gc->xoffset - 1, y - r/2 + gc->yoffset - 1, 2*r -1, 2*r -1, startAngle/PI*180, (startAngle + endAngle)/PI*180);
}

void xsDrawCubicBezierCurve(xsGraphics *gc, float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4)
{
	g_gcBase->DrawBezier(g_penBase, x1 + gc->xoffset -1, y1 + gc->yoffset -1, x2 + gc->xoffset -1, y2 + gc->yoffset -1, x3 + gc->xoffset -1, y3 + gc->yoffset -1, x4 + gc->xoffset -1, y4 + gc->yoffset -1);
}

void xsFillCubicBezierCurve(xsGraphics *gc, float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4)
{
	GraphicsPath path;
	path.StartFigure();
	path.AddBezier(x1 + gc->xoffset -1, y1 + gc->yoffset -1, x2 + gc->xoffset -1, y2 + gc->yoffset -1, x3 + gc->xoffset -1, y3 + gc->yoffset -1, x4 + gc->xoffset -1, y4 + gc->yoffset -1);
	path.CloseFigure();
	g_gcBase->FillPath(g_brushBase, &path);
}

void xsDrawQuadraticBezierCurve(xsGraphics *gc, float x1, float y1, float x2, float y2, float x3, float y3)
{}

void xsFillQuadraticBezierCurve(xsGraphics *gc, float x1, float y1, float x2, float y2, float x3, float y3)
{}

static REAL GetRealSize(xsFontType *font)
{
	REAL size;

	if (font->size < 0) 
	{// preset font size
		int idx = -1 - font->size;
		if (idx >= 0 && idx < XS_FONT_SIZE_COUNT)
			size = g_fontSizes[idx];
		else
			size = XS_FONT_MEDIUM;
	}
	else
	{
		size = font->size;
	}

	return size;
}

void xsSetFont(xsGraphics *gc, xsFontType *font)
{
/*	if (font->size > 0 && font->size < XS_FONT_SIZE_MAX)
		g_fontCurrent = font->size - 1;
	else
		g_fontCurrent = 0;*/
	REAL size = GetRealSize(font);

	if (g_fontCurrent != NULL)
		delete g_fontCurrent;

	// font->style's value is same as Font's defination
	g_fontCurrent = new Font(g_fontDefault, size, font->style, UnitPixel);
	// g_brushText->SetColor(XS_RGB(font->color));
}

int xsGetFontHeight(xsFontType *font)
{
	if (font != NULL)
	{
		REAL size = GetRealSize(font);
		Font f(g_fontDefault, size, FontStyleRegular, UnitPixel);
		return (int)f.GetHeight(g_gcBase);
	}

	return (int)g_fontCurrent->GetHeight(g_gcBase);
}

void xsDrawText(xsGraphics *gc, const xsTChar *text, int count, float x, float y)
{
	LayoffPixelBuffer(gc);
	g_gcBase->DrawString((const WCHAR *)text, count, g_fontCurrent,
		PointF((REAL)x + gc->xoffset, (REAL)y + gc->yoffset), g_brushText);
	RelockPixelBuffer(gc);
}

void xsDrawBorderText(xsGraphics *gc, const xsTChar *text, int count, float x, float y, float width, xsColor tc, xsColor bc, xsBool is_bordered)
{
	GraphicsPath path;
	FontFamily fm(g_fontDefault);
	path.StartFigure();
	path.AddString((const WCHAR *)text,count,&fm, g_fontCurrent->GetStyle(), g_fontCurrent->GetSize(),PointF((REAL)x + gc->xoffset, (REAL)y + gc->yoffset),NULL);
	path.CloseFigure();
	g_brushBase->SetColor(XS_RGB(tc));
	g_gcBase->FillPath(g_brushBase, &path);
	if(is_bordered)
	{
		g_penBase->SetColor(XS_RGB(bc));
		g_gcBase->DrawPath(g_penBase, &path);
	}	
}


void xsMeasureText(xsGraphics *gc, const xsTChar *text, int count, xsFontType *font, float *width, float *height)
{
	RectF rect;
	PointF p(0, 0);

	if (font != NULL)
	{
		REAL size = GetRealSize(font);
		Font f(g_fontDefault, size, FontStyleRegular, UnitPixel);
		g_gcBase->MeasureString((const WCHAR*)text, count, &f,
			p, &rect);
	}
	else
	{
		g_gcBase->MeasureString((const WCHAR*)text, count, g_fontCurrent,
			p, &rect);
	}
    *width = (int)rect.Width;
    *height = (int)rect.Height;
}

static void LoadImageFromRes(xsImage *img)
{
	Bitmap *bmp = NULL;
	img->object = NULL;
	HGLOBAL hBuffer  = ::GlobalAlloc(GMEM_MOVEABLE, img->length);
	if (hBuffer)
	{
		void *pBuffer = ::GlobalLock(hBuffer);
		if (pBuffer)
		{
			size_t read = xsReadRes(img->src.res, pBuffer, img->offset, img->length);
			if (read == img->length)
			{
				IStream* pStream = NULL;
				if (::CreateStreamOnHGlobal(hBuffer, FALSE, &pStream) == S_OK)
				{
					bmp = Bitmap::FromStream(pStream);
					pStream->Release();
					if (bmp)
					{ 
						if (bmp->GetLastStatus() == Gdiplus::Ok)
							img->object = bmp;
						else
							delete bmp;
					}
				}
			}
			::GlobalUnlock(hBuffer);
		}

		if (img->object == NULL)
		{
			XS_ERROR("[PAL]Load image from resource failed.");
			::GlobalFree(hBuffer);
		}
	}
}

static void ImageInstant(xsImage *img)
{
	if (img->object != NULL)
	{
		return;
	}
	else if (img->srcType == XS_AFD_FILENAME)
	{
		img->object = new Bitmap((const WCHAR *)img->src.filename);
	}
/*	else if (img->srcType == XS_IMAGE_MEMORY)
	{
		//
		XS_ERROR("Unimplement memory image");
		XS_ASSERT(0);
	}*/
	else if (img->srcType == XS_AFD_RESOURCE)
	{
		LoadImageFromRes(img);
	}
	else
	{
		XS_ERROR("Unimplement memory image");
		XS_ASSERT(0);
	}

	Status status = ((Bitmap *)img->object)->GetLastStatus();
	if (status != Ok)
	{
		XS_ERROR("[PAL]Load image failed.");
	}
}

int xsGetImageType(xsImage *img)
{
	Bitmap *bmp;
	GUID guid;

	ImageInstant(img);
	if (img->object == NULL)
		return XS_EC_ERROR;

	if (img->fileType != XS_AFD_FILETYPE_UNKNOWN)
		return img->fileType;

	bmp = (Bitmap *)img->object;
	if (bmp != NULL)
	{
		bmp->GetRawFormat(&guid);
		if (guid == ImageFormatJPEG)
			img->fileType = XS_IMGTYPE_JPEG;
		else if (guid == ImageFormatGIF)
			img->fileType = XS_IMGTYPE_GIF;
		else if (guid == ImageFormatPNG)
			img->fileType = XS_IMGTYPE_PNG;
		else if (guid == ImageFormatBMP)
			img->fileType = XS_IMGTYPE_BMP;
		else
			img->fileType = XS_IMGTYPE_OTHER;
	}

	return img->fileType;
}

void xsFreeImageObject(xsImage *img)
{
	if (img->object != NULL)
	{
		delete (Bitmap *)img->object;
		img->object = NULL;
	}
}

int xsGetImageDimension(xsImage *img, float *width, float *height)
{
	XS_ASSERT(img != NULL);

	ImageInstant(img);
	if (img->object == NULL)
		return XS_EC_ERROR;

	*width = ((Bitmap *)img->object)->GetWidth();
	*height = ((Bitmap *)img->object)->GetHeight();

	return XS_EC_OK;
}

void xsDrawImage(xsGraphics *gc, xsImage *img, float x, float y, float width, float height)
{
	XS_ASSERT(img != NULL);

	ImageInstant(img);
	if (img->object == NULL)
	{
		XS_ERROR("xsDrawImage failed.");
		return;
	}

	if(0.0 != width && 0.0 != height)
	{
		g_gcBase->DrawImage((Bitmap *)img->object, x + gc->xoffset, y + gc->yoffset, width, height);
	}
	else
	{
		g_gcBase->DrawImage((Bitmap *)img->object, x + gc->xoffset, y + gc->yoffset);
	}
}

#define XS_INPUT_CTRL_ID	1020
static HWND g_hWndInput = NULL;
static WNDPROC g_xsEditWndProc = NULL;

static xsTChar *g_xsInputBuffer = NULL;
static size_t g_xsBufferLen = 0;
static int g_xsInputType = 0;
static xsCallbackFunc g_xsInputCallback = NULL;
static void *g_xsInputUserdata = NULL;

static void UpdateInput()
{
	int ret = GetWindowText(g_hWndInput, (LPWSTR)g_xsInputBuffer, g_xsBufferLen);
	if (ret == 0)
		g_xsInputBuffer[0] = 0;

	if (g_xsInputCallback != NULL)
		g_xsInputCallback(g_xsInputUserdata);
}

static void InputDone(void)
{
	if (g_xsInputBuffer == NULL || g_xsBufferLen == 0)
		return;

	UpdateInput();

	g_xsInputBuffer = NULL;
	g_xsBufferLen = 0;
	g_xsInputCallback = NULL;
	g_xsInputUserdata = NULL;
	g_xsInputType = 0;
}

LRESULT CALLBACK InputWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CHAR:
	case WM_PAINT:
		{
			LRESULT ret = g_xsEditWndProc(hWnd, uMsg, wParam, lParam);
			
			UpdateInput();
			return ret;
		}
		break;
	case WM_SETFOCUS:
		break;
	case WM_SHOWWINDOW: // store text and notice
		if (!wParam)
			InputDone();
		break;
	}

	return g_xsEditWndProc(hWnd, uMsg, wParam, lParam);
}

void xsInputCtrlShow(int x, int y, int width, int height,
	const xsFontType *font, xsTChar *buffer, size_t len,
	int type, xsCallbackFunc cbUpdate, void *userdata)
{
	g_xsInputBuffer = buffer;
	g_xsBufferLen = len;
	g_xsInputType = type;
	g_xsInputCallback = cbUpdate;
	g_xsInputUserdata = userdata;

	if (g_xsInputBuffer == NULL || g_xsBufferLen == 0)
		return;

	if (g_hWndInput == NULL)
	{// Create input control
		DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_NOHIDESEL
			| ES_AUTOVSCROLL | ES_MULTILINE | ES_WANTRETURN;  

		g_hWndInput = CreateWindow(TEXT("edit"), NULL, dwStyle, x + XS_MARGIN, y + g_nTitleHeight, width, height,
			g_hMainWnd, (HMENU)XS_INPUT_CTRL_ID, g_hInst, NULL);          
		
		// over ride wndproc
		g_xsEditWndProc = (WNDPROC)GetWindowLong(g_hWndInput, GWL_WNDPROC);
		SetWindowLong(g_hWndInput, GWL_WNDPROC, (LONG)InputWndProc);
	}
	else
	{
		MoveWindow(g_hWndInput, x + XS_MARGIN, y + g_nTitleHeight, width, height, FALSE);
	}

	SetWindowText(g_hWndInput, (LPCWSTR)g_xsInputBuffer);
	//SendMessage(g_hWndInput, EM_SETSEL, 0, -1);
	ShowWindow(g_hWndInput, SW_SHOW);
	SetFocus(g_hWndInput);
}

void xsInputCtrlHide(void)
{
	if (g_hWndInput != NULL)
		ShowWindow(g_hWndInput, SW_HIDE);
}

xsBool xsGetCaretPos(int *x, int *y)
{
	POINT pt = {0};

	if (GetCaretPos(&pt))
	{
		*x = pt.x;
		*y = pt.y;
		return XS_TRUE;
	}

	return XS_FALSE;	
}

void xsInputDialog(const xsTChar *prompt, xsTChar *buffer, size_t len, int type, xsCallbackFunc cb, void *userdata)
{
	XS_ERROR("Unimplement xsInputDialog");
}
