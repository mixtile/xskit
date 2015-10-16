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

#ifndef _XS_UTILS_H_
#define _XS_UTILS_H_

#include <xs/pal.h>

#ifdef __cplusplus
extern "C" {
#endif

#define XS_MAX(m, n)		((m) > (n) ? (m) : (n))
#define XS_MIN(m, n)		((m) < (n) ? (m) : (n))
#define XS_ABS(v)			((v) < 0 ? (0 - (v)) : (v))

void xsRectIntersect(xsRect *rcDest, const xsRect *rc1, const xsRect *rc2);

xsS32 xsColorToArgb(xsColor color);
xsColor xsArgbToColor(xsS32 Argb);
void xsFillColor(xsColor *color, xsU8 alpha, xsU8 red, xsU8 green, xsU8 blue);
xsColor xsGetLightColor(xsColor color, xsU8 percents);
xsColor xsGetDarkColor(xsColor color, xsU8 percents);
xsColor xsGetInvertColor(xsColor color);
xsColor xsGetGreyColor(xsColor color);

void xsDataEncode(xsS32 *v, xsS32 n, xsS32 const k[4]);
void xsDataDecode(xsS32 *v, xsS32 n, xsS32 const k[4]);

xsBool xsGetDeviceUniqueId(char *buf, size_t size);

extern xsColor XS_COLOR_NONE;
extern xsColor XS_COLOR_BLACK;
extern xsColor XS_COLOR_WHITE;
extern xsColor XS_COLOR_ORANGE;
extern xsColor XS_COLOR_DODGERBLUE;
extern xsColor XS_COLOR_GAINSBORO;

extern xsColor XS_COLOR_BLANK;
extern xsColor XS_COLOR_BUTTONFACE;
extern xsColor XS_COLOR_BORDER;
extern xsColor XS_COLOR_DARKBORDER;
extern xsColor XS_COLOR_FOCUSBORDER;
extern xsColor XS_COLOR_SELECTED;

#ifdef __cplusplus
}
#endif

#endif /* _XS_UTILS_H_ */
