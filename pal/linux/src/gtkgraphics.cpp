#include <gdk/gdk.h>
#include <xs/pal.h>

extern int g_nScreenWidth;
extern int g_nScreenHeight;

static xsGraphics g_sysGc = {0};
static GdkWindow *g_mainWindow = NULL;
static float g_fontSizes[XS_FONT_SIZE_COUNT] = {6, 9, 12, 16, 20, 24, 36};

int PalGiInit(GdkWindow *window)
{
	g_mainWindow = window;

	// prepare double buffer
	g_sysGc.surface = gdk_window_create_similar_surface(window,
            CAIRO_CONTENT_COLOR, g_nScreenWidth, g_nScreenHeight);
	g_sysGc.device = cairo_create((cairo_surface_t *) g_sysGc.surface);
	return 0;
}

void PalGiUninit(void)
{
	cairo_surface_destroy((cairo_surface_t *) g_sysGc.surface);
	cairo_destroy((cairo_t *) g_sysGc.device);

	g_sysGc.surface = NULL;
	g_sysGc.device = NULL;
}

///////////////////////////////////////////////////////////////////////////////
// Screen Operation Functions
///////////////////////////////////////////////////////////////////////////////
static xsU16 g_screenLock = 0;

xsBool xsSetScreenOrient(int orient)
{
	return XS_FALSE;
}

void xsLockScreen(void)
{
	g_screenLock++;
}

void xsUnlockScreen(void)
{
	g_screenLock--;
}

void xsFlushScreen(int left, int top, int right, int bottom)
{
	// check screen lock
	if (g_screenLock > 0)
		return;

	GdkRectangle rect;
	rect.x = left;
	rect.y = top;
	rect.width = right - left + 1;
	rect.height = bottom - top + 1;
	gdk_window_invalidate_rect(g_mainWindow, &rect, TRUE);
}

xsGraphics *xsGetSystemGc(void)
{
	return &g_sysGc;
}

void *xsLockPixelBuffer(xsGraphics *gc, int pixelDepth)
{
	return NULL;
}

void xsUnlockPixelBuffer(xsGraphics *gc)
{
}

void xsGcTranslate(xsGraphics *gc, float xoffset, float yoffset)
{
	cairo_translate(XS_CAIRO(gc), xoffset, yoffset);
}

void xsGcRotate(xsGraphics *gc, float angle)
{
	cairo_rotate(XS_CAIRO(gc), angle);
}

void xsGcScale(xsGraphics *gc, float scalewidth, float scaleheight)
{
	cairo_scale(XS_CAIRO(gc), scalewidth, scaleheight);
}

void xsGcTransform(xsGraphics *gc, float xx, float yx, float xy, float yy, float x0, float y0)
{
	cairo_matrix_t matrix;
	matrix.xx = xx;
	matrix.yx = yx;
	matrix.xy = xy;
	matrix.yy = yy;
	matrix.x0 = x0;
	matrix.y0 = y0;
	cairo_transform(XS_CAIRO(gc), &matrix);
}

xsBool xsGetScreenDimension(int *width, int *height)
{
	*width = g_nScreenWidth;
	*height = g_nScreenHeight;

	return XS_TRUE;
}

xsBool xsGetClientRect(xsRect *rect)
{
	return XS_FALSE;
}

void xsGetClipRect(xsGraphics *gc, xsRect *rect)
{
	double x1, y1, x2, y2;
	cairo_clip_extents(XS_CAIRO(gc), &x1, &y1, &x2, &y2);

	rect->left = x1;
	rect->top = y1;
	rect->right = x2;
	rect->bottom = y2;
}

void xsSetClipRect(xsGraphics *gc, xsRect *rect)
{
	cairo_rectangle(XS_CAIRO(gc), rect->left, rect->top,
			XS_RECT_WIDTH(rect), XS_RECT_HEIGHT(rect));
	cairo_clip(XS_CAIRO(gc));
}

void xsResetClipRect(xsGraphics *gc)
{
	cairo_reset_clip(XS_CAIRO(gc));
}

void xsSetColor(xsGraphics *gc, xsColor color)
{
	cairo_set_source_rgba(XS_CAIRO(gc), color.red / 255, color.green / 255,
				color.blue / 255, color.alpha / 255);
}

void xsSetStrokeStyle(xsGraphics *gc, xsStrokeStyle *style)
{
	cairo_set_line_width(XS_CAIRO(gc), style->width);

	cairo_line_cap_t cap;
	switch (style->cap)
	{
	default:
	case XS_STROKE_CAP_BUTT:
		cap = CAIRO_LINE_CAP_BUTT;
		break;
	case XS_STROKE_CAP_ROUND:
		cap = CAIRO_LINE_CAP_ROUND;
		break;
	case XS_STROKE_CAP_SQUARE:
		cap = CAIRO_LINE_CAP_SQUARE;
		break;
	}
	cairo_set_line_cap(XS_CAIRO(gc), cap);
}

void xsDrawLine(xsGraphics *gc, float x1, float y1, float x2, float y2)
{
	cairo_move_to(XS_CAIRO(gc), x1, y1);
	cairo_line_to(XS_CAIRO(gc), x2, y2);
	cairo_stroke(XS_CAIRO(gc));
}

void xsDrawRectangle(xsGraphics *gc, float x, float y,
		float width, float height)
{
	cairo_rectangle(XS_CAIRO(gc), x, y, width, height);
	cairo_stroke(XS_CAIRO(gc));
}

void xsFillRectangle(xsGraphics *gc, float x, float y, float width,
		float height)
{
	cairo_rectangle(XS_CAIRO(gc), x, y, width, height);
	cairo_fill(XS_CAIRO(gc));
}

void xsDrawPolygon(xsGraphics *gc, xsPoint pt[], xsU32 count)
{
	cairo_move_to(XS_CAIRO(gc), pt[0].x ,pt[0].y);
	for(xsU32 i = 1; i < count; i++)
	{
		cairo_line_to(XS_CAIRO(gc), pt[i].x, pt[i].y);
	}
	cairo_stroke(XS_CAIRO(gc));
}
void xsFillPolygon(xsGraphics *gc, xsPoint pt[], xsU32 count)
{
	cairo_move_to(XS_CAIRO(gc), pt[0].x ,pt[0].y);
	for(xsU32 i = 1; i < count; i++)
	{
		cairo_line_to(XS_CAIRO(gc), pt[i].x, pt[i].y);
	}
	cairo_fill(XS_CAIRO(gc));
}

void xsDrawArc(xsGraphics *gc, float x, float y, float r, float startAngle, float endAngle)
{
	cairo_arc(XS_CAIRO(gc), x, y, r, startAngle, endAngle);
	cairo_stroke(XS_CAIRO(gc));
}

void xsFillArc(xsGraphics *gc, float x, float y, float r, float startAngle, float endAngle)
{
	cairo_arc(XS_CAIRO(gc), x, y, r, startAngle, endAngle);
	cairo_fill(XS_CAIRO(gc));
}

void xsDrawCubicBezierCurve(xsGraphics *gc, float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4)
{
	cairo_move_to(XS_CAIRO(gc), 100, 120);
	cairo_curve_to(XS_CAIRO(gc), x2, y2, x3, y3, x4, y4);
	cairo_stroke(XS_CAIRO(gc));
}

void xsFillCubicBezierCurve(xsGraphics *gc, float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4)
{
	cairo_move_to(XS_CAIRO(gc), 100, 120);
	cairo_curve_to(XS_CAIRO(gc), x2, y2, x3, y3, x4, y4);
	cairo_fill(XS_CAIRO(gc));
}

void xsDrawQuadraticBezierCurve(xsGraphics *gc, float x1, float y1, float x2, float y2, float x3, float y3)
{}

void xsFillQuadraticBezierCurve(xsGraphics *gc, float x1, float y1, float x2, float y2, float x3, float y3)
{}

void xsDrawCircle(xsGraphics *gc, int x, int y, int r)
{
	cairo_arc(XS_CAIRO(gc), x, y, r, 0, 360);
	cairo_stroke(XS_CAIRO(gc));
}

void xsFillCircle(xsGraphics *gc, int x, int y, int r)
{
	cairo_arc(XS_CAIRO(gc), x, y, r, 0, 360);
	cairo_fill(XS_CAIRO(gc));
}

static float GetFontRealSize(xsFontType *font)
{
	float size;

	if (font->size < 0)
	{// preset font size
		int idx = -1 - font->size;
		if (idx >= 0 && idx < XS_FONT_SIZE_COUNT)
			size = g_fontSizes[idx];
		else
			size = g_fontSizes[-1 - XS_FONT_MEDIUM];
	}
	else
	{// font->size is real size
		size = font->size;
	}

	return size;
}

void xsSetFont(xsGraphics *gc, xsFontType *font)
{
	// TODO: set font face
	cairo_select_font_face(XS_CAIRO(gc), "Sans", CAIRO_FONT_SLANT_NORMAL,
			CAIRO_FONT_WEIGHT_BOLD);

	cairo_set_font_size(XS_CAIRO(gc), GetFontRealSize(font));
}

float xsGetFontHeight(xsGraphics *gc, xsFontType *font)
{
	cairo_font_extents_t extents;

	cairo_font_extents(XS_CAIRO(gc), &extents);

	return extents.height;
}

void xsDrawText(xsGraphics *gc, const xsTChar *text, int count, float x, float y)
{
	cairo_move_to(XS_CAIRO(gc), x, y);
	cairo_show_text(XS_CAIRO(gc), text);
}

void xsDrawBorderText(xsGraphics *gc, const xsTChar *text, int count,  float x, float y,  float width, xsColor tc, xsColor bc, xsBool is_bordered)
{
	cairo_move_to(XS_CAIRO(gc), x, y);
	cairo_text_path (XS_CAIRO(gc), text);
	xsSetColor(gc, tc);
	cairo_fill_preserve (XS_CAIRO(gc));
	if(is_bordered)
	{
		xsSetColor(gc, bc);
	}
	else
	{
		xsSetColor(gc, {255, 255, 255, 255});
	}

	cairo_set_line_width (XS_CAIRO(gc), 1);
	cairo_stroke (XS_CAIRO(gc));
}

void xsMeasureText(xsGraphics *gc, const xsTChar *text, int count,
		xsFontType *font, float *width, float *height)
{
	cairo_text_extents_t extents;

	cairo_text_extents(XS_CAIRO(gc), text, &extents);

	*width = extents.width;
	*height = extents.height;
}

xsImage *xsLoadImage(int loadType, xsUIntPtr source)
{
	xsImage *img;

	if (source == 0)
		return NULL;

	img = (xsImage *)xsCallocNative(sizeof(xsImage));
	if (img == NULL)
		return NULL;

	img->srcType = loadType;
	switch (loadType)
	{
	case XS_AFD_FILENAME:
		img->src.filename = (xsTChar *)xsMallocNative(xsTcsLen((xsTChar *)source) + sizeof(xsTChar));
		if (img->src.filename == NULL)
		{// allocate memory failed, roll back
			xsFreeNative(img);
			return NULL;
		}
		xsTcsCpy(img->src.filename, (xsTChar *)source);
		break;
	case XS_AFD_RESOURCE:
	case XS_AFD_FILE:
	case XS_AFD_MEMORY:
		img->src.ptr = (void *)source;
		break;
	default: // unknown source type
		xsFreeNative(img);
		return NULL;
	}

	return img;
}

int xsGetImageType(xsImage *img)
{
	if (img == NULL)
		return XS_AFD_UNKNOWN;
	return img->srcType;
}

static void ImageLoaderSizeCallback(GdkPixbufLoader *loader, int width,
		int height, gpointer *user_data)
{
	xsImage *img = (xsImage *) user_data;
	if (img == NULL)
		return;

	if((xsImageParam *)img->srcparam)
	{
		((xsImageParam *)img->srcparam)->width = width;
		((xsImageParam *)img->srcparam)->height = height;
	}
}

static void ImageLoaderLoadedCallback(GdkPixbufLoader *loader, gpointer *user_data)
{
	xsImage *img = (xsImage *) user_data;
	if (img == NULL)
		return;
	if((xsImageParam *)img->srcparam)
	{
		((xsImageParam *)img->srcparam)->loaded = XS_TRUE;
	}
}

static int ImageInstantFromFile(GdkPixbufLoader *loader, GError **err,
		xsImage *img)
{
	xsFile imgFile = xsOpenFile(img->src.filename, XS_OF_READONLY);
	if (!xsFileHandleValid(imgFile))
		return XS_EC_NOT_EXISTS;

	xsU8 buffer[8192];
	size_t read;
	gboolean ret;

	while ((read = xsReadFile(imgFile, buffer, sizeof(buffer))) > 0)
	{
		ret = gdk_pixbuf_loader_write(loader, buffer, read, err);
		if (ret == FALSE)
			break; // interrupt loading
	}

	xsCloseFile(imgFile);

	return XS_EC_OK;
}

static int ImageInstant(xsImage *img)
{
	GdkPixbufLoader *loader = gdk_pixbuf_loader_new();
	GError *err = NULL;
	gboolean loadret;
	int ret = XS_EC_ERROR;

	if(NULL == img)
	{
		return ret;
	}

	g_signal_connect(G_OBJECT(loader), "size-prepared",
			G_CALLBACK(ImageLoaderSizeCallback), img);

	g_signal_connect(G_OBJECT(loader), "area-prepared",
			G_CALLBACK(ImageLoaderLoadedCallback), img);

	switch (img->srcType)
	{
	case XS_AFD_FILENAME:
		ret = ImageInstantFromFile(loader, &err, img);
		break;
	case XS_AFD_RESOURCE:
	case XS_AFD_FILE:
	default:
		ret = XS_EC_NOT_SUPPORT;
		break;
	case XS_AFD_MEMORY:
		loadret = gdk_pixbuf_loader_write(loader,
				(const unsigned char *)img->src.ptr,
				img->length, &err);
		if (loadret != FALSE)
			ret = XS_EC_OK;
		break;
	}

	if (ret != XS_EC_OK && err != NULL)
	{
		XS_ERROR("error close %s",  err->message);
		g_error_free(err);
	}

	err = NULL;
	if (gdk_pixbuf_loader_close(loader, &err) == FALSE)
	{
		XS_ERROR("error close %s",  err->message);
		g_error_free(err);
	}

	// keep pixbuf
	img->object = gdk_pixbuf_loader_get_pixbuf(loader);
	//g_object_unref(G_OBJECT(loader));

	return ret;
}

int xsGetImageDimension(xsImage *img, float *width, float *height)
{
	if(NULL == img || (NULL != img && NULL == img->srcparam))
	{
		return XS_EC_ERROR;
	}

	if (((xsImageParam *)img->srcparam)->loaded < 0)
		return XS_EC_ERROR;

	if (((xsImageParam *)img->srcparam)->loaded == 0)
	{
		int ret = ImageInstant(img);
		if (ret != XS_EC_OK)
			return ret;
	}

	*width = ((xsImageParam *)img->srcparam)->width;
	*height = ((xsImageParam *)img->srcparam)->height;
	return XS_EC_OK;
}

void xsFreeImage(xsImage *img)
{
	if (img == NULL)
		return;

	if (img->object != NULL)
		g_object_unref(img->object);

	xsFreeNative(img);
}

void xsDrawImage(xsGraphics *gc, xsImage *img, float x, float y, float width, float height)
{
	if (img == NULL)
		return;

	if(img->object == NULL ||img ->srcparam == NULL)
		return;

	if(((xsImageParam *)img->srcparam)->loaded != 1)
		return;

	if(0 != width && 0 != height)
	{
		img->object = (void *)gdk_pixbuf_scale_simple((GdkPixbuf *)img->object, width, height, GDK_INTERP_BILINEAR);
	}

	gdk_cairo_set_source_pixbuf(XS_CAIRO(gc),
			(GdkPixbuf *)(img->object), x, y);
	cairo_paint(XS_CAIRO(gc));
}

/*为了编译通过，复制win32下的pal.c的部分函数*/
void xsFreeImageObject(xsImage *img)
{}

char *xsGetDeviceInfo(size_t *size)
{}
