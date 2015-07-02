#include <xs/utils.h>
#include <xs/object.h>
#include <xs/memory.h>
#include <xs/array.h>
#include <xs/md5.h>

xsColor XS_COLOR_NONE			= {0, 255, 255, 255};
xsColor XS_COLOR_BLACK			= {255, 0, 0, 0};
xsColor XS_COLOR_GAINSBORO		= {255, 220, 220, 220};
xsColor XS_COLOR_WHITE			= {255, 255, 255, 255};
xsColor XS_COLOR_ORANGE			= {255, 255, 165, 0};
xsColor XS_COLOR_DODGERBLUE		= {255, 30, 144, 255};

xsColor XS_COLOR_BLANK			= {255, 255, 255, 255};
xsColor XS_COLOR_BUTTONFACE		= {220, 240, 240, 240};//{255, 252, 187, 1};
xsColor XS_COLOR_BORDER			= {255, 192, 192, 192};//{255, 255, 250, 205};
xsColor XS_COLOR_DARKBORDER		= {255, 0, 0, 0};
xsColor XS_COLOR_FOCUSBORDER	= {255, 0, 0, 255};
xsColor XS_COLOR_SELECTED		= {220, 192, 192, 192};//{255, 30, 144, 255};

void xsRectIntersect(xsRect *rcDest, const xsRect *rc1, const xsRect *rc2)
{
	rcDest->left = XS_MAX(rc1->left, rc2->left);
	rcDest->top = XS_MAX(rc1->top, rc2->top);
	rcDest->right = XS_MIN(rc1->right, rc2->right);
	rcDest->bottom = XS_MIN(rc1->bottom, rc2->bottom);

	if (rcDest->right < rcDest->left)
		rcDest->right = rcDest->left;

	if (rcDest->bottom < rcDest->top)
		rcDest->bottom = rcDest->top;
}

xsS32 xsColorToArgb(xsColor color)
{
	xsS32 Argb;

	Argb = color.blue;
	Argb |= color.green << 8;
	Argb |= color.red << 16;
	Argb |= color.alpha << 24;

	return Argb;
}

xsColor xsArgbToColor(xsS32 Argb)
{
	xsColor color;

	color.alpha = Argb >> 24;
	color.red = (Argb >> 16) & 0xFF;
	color.green = (Argb >> 8) & 0xFF;
	color.blue = Argb & 0xFF;

	return color;
}

void xsFillColor(xsColor *color, xsU8 alpha, xsU8 red, xsU8 green, xsU8 blue)
{
	XS_ASSERT(color != NULL);

	color->alpha = alpha;
	color->red = red;
	color->green = green;
	color->blue = blue;
}

xsColor xsGetLightColor(xsColor color, xsU8 percents)
{
	xsColor clr = color;

	clr.blue += (((int)(0xFF - color.blue) * percents) / 100) & 0xFF;
	clr.green += (((int)(0xFF - color.green) * percents) / 100) & 0xFF;
	clr.red += (((int)(0xFF - color.red) * percents) / 100) & 0xFF;

	// Avoid overflow
	clr.blue = clr.blue < color.blue ? 0xFF : clr.blue;
	clr.green = clr.green < color.green ? 0xFF : clr.green;
	clr.red = clr.red < color.red ? 0xFF : clr.red;

	return clr;
}

xsColor xsGetDarkColor(xsColor color, xsU8 percents)
{
	color.blue = ((int)color.blue * (100 - percents)) / 100;
	color.green = ((int)color.green * (100 - percents)) / 100;
	color.red = ((int)color.red * (100 - percents)) / 100;

	return color;
}

xsColor xsGetGreyColor(xsColor color)
{
	int clr = color.blue + color.green + color.red;

	clr = clr / 3;
	color.blue = (xsU8)clr;
	color.green = (xsU8)clr;
	color.red = (xsU8)clr;

	return color;
}

xsColor xsGetInvertColor(xsColor color)
{
	color.blue = 255 - color.blue;
	color.green = 255 - color.green;
	color.red = 255 - color.red;

	return color;
}

xsBool xsGetDeviceUniqueId(char *idBuf, size_t size)
{
	if (idBuf == NULL || size == 0)
		return XS_FALSE;

	size_t infoSize = 0;
	char *info = xsGetDeviceInfo(&infoSize);
	if (info == NULL)
		return XS_FALSE;
	
	// hash
	MD5_CTX mdContext;

	MD5Init(&mdContext);
	MD5Update(&mdContext, (unsigned char *)info, infoSize);
	MD5Final(&mdContext);
	xsFree(info);

	// copy to idBuf
	if (size > sizeof(mdContext.buf))
		size = sizeof(mdContext.buf);
	xsMemCpy(idBuf, mdContext.buf, size);
	return XS_TRUE;
}
