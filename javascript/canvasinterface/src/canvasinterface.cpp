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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "xs/pal.h"
#include "xs/canvas.h"
#include "xs/string.h"
#include "duktape.h"
#include "canvasinterface.h"

typedef struct _xsJSEvent
{
	xsU16 keyCode;
	xsU32 which;
}xsJSEvent;

duk_context *g_ctx = NULL;
char keyProcessingFunc[128] = {0};
xsJSEvent event = {};
xsImage *g_img[100] = {0};
int g_imgNum = 0;

//convert a hexadecimal string to an integer.
int htoi(char s[])
{
    int i;
    int n = 0;
    if (s[0] == '0' && (s[1]=='x' || s[1]=='X'))
    {
        i = 2;
    }
    else
    {
        i = 0;
    }
    for (; (s[i] >= '0' && s[i] <= '9') || (s[i] >= 'a' && s[i] <= 'z') || (s[i] >='A' && s[i] <= 'Z');++i)
    {
        if (tolower(s[i]) > '9')
        {
            n = 16 * n + (10 + tolower(s[i]) - 'a');
        }
        else
        {
            n = 16 * n + (tolower(s[i]) - '0');
        }
    }
    return n;
}

void parseFontString(const char *fontString, xsFontType &font)
{
    const char *spacePos = xsStrChr(fontString, ' ');
    const char *currentStart = fontString;
    char divideStr[20] = {0};
    const char *pxPos = NULL;
    while(spacePos != NULL)
    {
    	if(spacePos - currentStart < 20)
    	{
    		xsStrCpyN(divideStr, currentStart, spacePos - currentStart);
    	}

    	pxPos = xsStrStr(divideStr, "px") == NULL ? xsStrStr(divideStr, "PX") : NULL;
    	if(pxPos)
    	{
    		if(pxPos - divideStr < 10)
    		{
    			char px[10] = {0};
    			xsStrCpyN(px, divideStr, pxPos - divideStr);
    			font.size = atoi(px);
    		}
    	}
    	else
    	{

    	}
    }
}

static duk_ret_t native_canvas_dtor(duk_context *ctx)
{
    duk_get_prop_string(ctx, 0, "nativeCanvasDelete");

    bool deleted = duk_to_boolean(ctx, -1);
    duk_pop(ctx);

    if (!deleted) {
        duk_get_prop_string(ctx, 0, "nativeCanvasData");
        delete static_cast<xsCanvas *>(duk_to_pointer(ctx, -1));
        duk_pop(ctx);
    	duk_get_prop_string(ctx, 0, "getContext");
    	duk_get_prop_string(ctx, -1, "nativeContextData");
        delete static_cast<xsCanvasContext *>(duk_to_pointer(ctx, -1));
        duk_pop(ctx);
        // Mark as deleted
        duk_push_boolean(ctx, true);
        duk_put_prop_string(ctx, 0, "nativeCanvasDelete");
    }

    return 0;
}

static 	duk_ret_t native_addEventListener(duk_context *ctx)
{
	const char *keyType = duk_require_string(ctx,0);
	void *listener = duk_require_heapptr(ctx,1);
	xsBool useCapture = duk_require_boolean(ctx,2);

	duk_push_heapptr(ctx, listener);
	duk_get_prop_string(ctx, -1, "name");
	const char* listenerName = duk_to_string(ctx, -1);
	duk_pop(ctx);

	if(strcmp(keyType, "keydown") == 0)
	{
		strcpy(keyProcessingFunc, listenerName);
	}


	return 1;
}

static xsS32 native_timer_func( void *data)
{
	if(!data)
	{
		return false;
	}
	const char *funcName = (const char *)data;
    duk_push_global_object(g_ctx);
    duk_get_prop_string(g_ctx, -1, funcName);
    if (duk_pcall(g_ctx, 0) != 0) {
        printf("Error1: %s\n", duk_safe_to_string(g_ctx, -1));
    }
    duk_pop(g_ctx);

	return true;
}

static xsS32 native_timeOut_func( void *data)
{
	if(!data)
	{
		return false;
	}
	const char *funcName = (const char *)data;
    duk_push_global_object(g_ctx);
    duk_get_prop_string(g_ctx, -1, funcName);
    if (duk_pcall(g_ctx, 0) != 0) {
        printf("Error1: %s\n", duk_safe_to_string(g_ctx, -1));
    }
    duk_pop(g_ctx);

	return false;
}

static 	duk_ret_t native_setTimeout(duk_context *ctx)
{
	const char *method = duk_require_string(ctx,0);
	xsU32 millisec = (xsU32)duk_require_number(ctx,1);
	char *jsMethod = (char *)xsCalloc(128);//函数名长度大于128会有问题
	const char * endPos = strchr(method, '(');
	strncpy(jsMethod, method, endPos - method);
	xsU32 timerID = xsStartTimer(millisec, native_timeOut_func, (void *)jsMethod);
	duk_push_number(ctx, timerID);

	return 1;
}

static 	duk_ret_t native_setInterval(duk_context *ctx)
{
	const char *method = duk_require_string(ctx,0);
	xsU32 millisec = (xsU32)duk_require_number(ctx,1);
	char *jsMethod = (char *)xsCalloc(128);//函数名长度大于128会有问题
	const char * endPos = strchr(method, '(');
	strncpy(jsMethod, method, endPos - method);
	xsU32 timerID = xsStartTimer(millisec, native_timer_func, (void *)jsMethod);
	duk_push_number(ctx, timerID);

	return 1;
}

static 	duk_ret_t native_clearInterval(duk_context *ctx)
{
	xsU32 timerID = (xsU32)duk_require_number(ctx,0);
	xsStopTimer(timerID);

	return 1;
}

static duk_ret_t updrate_param(duk_context *ctx)
{
	duk_push_this(ctx);
    duk_get_prop_string(ctx, -1, "nativeContextData");
    xsCanvasContext *context = static_cast<xsCanvasContext *>(duk_to_pointer(ctx, -1));
    duk_pop(ctx);
    duk_get_prop_string(ctx, -1, "lineWidth");
    float lineWidth = duk_to_number(ctx, -1);
    context ->lineWidth = lineWidth;
    duk_pop(ctx);
    duk_get_prop_string(ctx, -1, "strokeStyle");
    const char *strokeStyle = duk_to_string(ctx, -1);
    char strokeTmp[2] = {0};
    xsStrCpyN(strokeTmp, strokeStyle + 1, 2);
    context ->strokeColor.red = htoi(strokeTmp);
    xsStrCpyN(strokeTmp, strokeStyle + 3, 2);
    context ->strokeColor.green = htoi(strokeTmp);
    xsStrCpyN(strokeTmp, strokeStyle + 5, 2);
    context ->strokeColor.blue = htoi(strokeTmp);
    duk_pop(ctx);
    duk_get_prop_string(ctx, -1, "fillStyle");
    if(duk_is_string(ctx, -1))
    {
//    	printf("string\n");
    }
    else if(duk_is_object(ctx, -1))
    {
    	printf("object\n");
    }
    else if(duk_is_pointer(ctx, -1))
    {
    	printf("pointer\n");
    }
    const char *fillStyle = duk_to_string(ctx, -1);
    char fillTmp[2] = {0};
    xsStrCpyN(fillTmp, fillStyle + 1, 2);
    context ->fillColor.red = htoi(fillTmp);
    xsStrCpyN(fillTmp, fillStyle + 3, 2);
    context ->fillColor.green = htoi(fillTmp);
    xsStrCpyN(fillTmp, fillStyle + 5, 2);
    context ->fillColor.blue = htoi(fillTmp);
    duk_pop(ctx);
//    duk_get_prop_string(ctx, -1, "font");
//    const char *font = duk_to_string(ctx, -1);
//    parseFontString(fontTmp, context ->font);

    return 1;
}

static xsCanvasContext * get_native_context(duk_context *ctx)
{
	duk_push_this(ctx);
    duk_get_prop_string(ctx, -1, "nativeContextData");
    xsCanvasContext *context = static_cast<xsCanvasContext *>(duk_to_pointer(ctx, -1));
    duk_pop(ctx);
    updrate_param(ctx);
    return context;
}

static duk_ret_t native_canvasContext_moveTo(duk_context *ctx)
{
	float x = (float)duk_require_number(ctx,0);
	float y = (float)duk_require_number(ctx,1);

	xsCanvasContext *context = get_native_context(ctx);
	context ->moveTo(x, y);

	return 1;
}

static duk_ret_t native_canvasContext_lineTo(duk_context *ctx)
{
	float x = (float)duk_require_number(ctx,0);
	float y = (float)duk_require_number(ctx,1);

	xsCanvasContext *context = get_native_context(ctx);
	context ->lineTo(x, y);

	return 1;
}

static duk_ret_t native_canvasContext_stroke(duk_context *ctx)
{
	xsCanvasContext *context = get_native_context(ctx);
	context ->stroke();

	return 1;
}

static duk_ret_t native_canvasContext_fill(duk_context *ctx)
{
	xsCanvasContext *context = get_native_context(ctx);
	context ->fill();

	return 1;
}

static duk_ret_t native_canvasContext_rect(duk_context *ctx)
{
	float x = (float)duk_require_number(ctx,0);
	float y = (float)duk_require_number(ctx,1);
	float width = (float)duk_require_number(ctx,2);
	float height = (float)duk_require_number(ctx,3);

	xsCanvasContext *context = get_native_context(ctx);
    context ->rect(x, y, width, height);

    return 1;
}

static duk_ret_t native_canvasContext_strokeRect(duk_context *ctx)
{
	float x = (float)duk_require_number(ctx,0);
	float y = (float)duk_require_number(ctx,1);
	float width = (float)duk_require_number(ctx,2);
	float height = (float)duk_require_number(ctx,3);

	xsCanvasContext *context = get_native_context(ctx);
    context ->strokeRect(x, y, width, height);
    duk_push_sprintf(ctx, "OK!");

	return 1;
}

static duk_ret_t native_canvasContext_fillRect(duk_context *ctx)
{
	float x = (float)duk_require_number(ctx,0);
	float y = (float)duk_require_number(ctx,1);
	float width = (float)duk_require_number(ctx,2);
	float height = (float)duk_require_number(ctx,3);

	xsCanvasContext *context = get_native_context(ctx);
    context ->fillRect(x, y, width, height);
    duk_push_sprintf(ctx, "OK!");

	return 1;
}

static duk_ret_t native_canvasContext_clearRect(duk_context *ctx)
{
	float x = (float)duk_require_number(ctx,0);
	float y = (float)duk_require_number(ctx,1);
	float width = (float)duk_require_number(ctx,2);
	float height = (float)duk_require_number(ctx,3);

	xsCanvasContext *context = get_native_context(ctx);
    context ->clearRect(x, y, width, height);

    return 1;
}

static duk_ret_t native_canvasContext_arc(duk_context *ctx)
{
	float x = (float)duk_require_number(ctx,0);
	float y = (float)duk_require_number(ctx,1);
	float radius = (float)duk_require_number(ctx,2);
	float startAngle = (float)duk_require_number(ctx,3);
	float endAngle = (float)duk_require_number(ctx,4);
	xsBool anticlockwise = duk_require_boolean(ctx,5);

	xsCanvasContext *context = get_native_context(ctx);
    context ->arc(x, y, radius, startAngle, endAngle, anticlockwise);

    return 1;
}

static duk_ret_t native_canvasContext_arcTo(duk_context *ctx)
{
	float x1 = (float)duk_require_number(ctx,0);
	float y1 = (float)duk_require_number(ctx,1);
	float x2 = (float)duk_require_number(ctx,2);
	float y2 = (float)duk_require_number(ctx,3);
	float radius = (float)duk_require_number(ctx,4);

	xsCanvasContext *context = get_native_context(ctx);
    context ->arcTo(x1, y1, x2, y2, radius);

    return 1;
}

static duk_ret_t native_canvasContext_beginPath(duk_context *ctx)
{
	xsCanvasContext *context = get_native_context(ctx);
    context ->beginPath();

    return 1;
}

static duk_ret_t native_canvasContext_closePath(duk_context *ctx)
{
	xsCanvasContext *context = get_native_context(ctx);
    context ->closePath();

    return 1;
}

static duk_ret_t native_canvasContext_bezierCurveTo(duk_context *ctx)
{
	float x1 = (float)duk_require_number(ctx,0);
	float y1 = (float)duk_require_number(ctx,1);
	float x2 = (float)duk_require_number(ctx,2);
	float y2 = (float)duk_require_number(ctx,3);
	float x3 = (float)duk_require_number(ctx,4);
	float y3 = (float)duk_require_number(ctx,5);

	xsCanvasContext *context = get_native_context(ctx);
    context ->bezierCurveTo(x1, y1, x2, y2, x3, y3);

    return 1;
}

static duk_ret_t native_canvasContext_quadraticCurveTo(duk_context *ctx)
{
	float x1 = (float)duk_require_number(ctx,0);
	float y1 = (float)duk_require_number(ctx,1);
	float x2 = (float)duk_require_number(ctx,2);
	float y2 = (float)duk_require_number(ctx,3);

	xsCanvasContext *context = get_native_context(ctx);
    context ->quadraticCurveTo(x1, y1, x2, y2);

    return 1;
}

static duk_ret_t native_canvasContext_translate(duk_context *ctx)
{
	float x = (float)duk_require_number(ctx,0);
	float y = (float)duk_require_number(ctx,1);


	xsCanvasContext *context = get_native_context(ctx);
    context ->translate(x, y);

    return 1;
}

static duk_ret_t native_canvasContext_scale(duk_context *ctx)
{
	float scalewidth = (float)duk_require_number(ctx,0);
	float scaleheight = (float)duk_require_number(ctx,1);

	xsCanvasContext *context = get_native_context(ctx);
    context ->scale(scalewidth, scaleheight);

    return 1;
}

static duk_ret_t native_canvasContext_rotate(duk_context *ctx)
{
	float angle= (float)duk_require_number(ctx,0);

	xsCanvasContext *context = get_native_context(ctx);
    context ->rotate(angle);

    return 1;
}

static duk_ret_t native_canvasContext_transform(duk_context *ctx)
{
	float x1 = (float)duk_require_number(ctx,0);
	float y1 = (float)duk_require_number(ctx,1);
	float x2 = (float)duk_require_number(ctx,2);
	float y2 = (float)duk_require_number(ctx,3);
	float x3 = (float)duk_require_number(ctx,4);
	float y3 = (float)duk_require_number(ctx,5);

	xsCanvasContext *context = get_native_context(ctx);
    context ->transform(x1, y1, x2, y2, x3, y3);

    return 1;
}

static duk_ret_t native_canvasContext_drawImage(duk_context *ctx)
{
	float sx = (float)duk_require_number(ctx,1);
	float sy = (float)duk_require_number(ctx,2);
	float swidth = (float)duk_get_number(ctx,3);
	float sheight = (float)duk_get_number(ctx,4);
	float x = (float)duk_get_number(ctx,5);
	float y = (float)duk_get_number(ctx,6);
	float width = (float)duk_get_number(ctx,7);
	float height = (float)duk_get_number(ctx,8);

	/*获取object对象的方法一*/
//	void *imgFunc = duk_require_heapptr(ctx, 0);
//	duk_push_heapptr(ctx, imgFunc);
//	duk_get_prop_string(ctx, -1, "src");
//    const char *src = duk_to_string(ctx, -1);
//    duk_pop(ctx);
//    duk_get_prop_string(ctx, -1, "imageData");
//    xsImage *image = (xsImage *)duk_to_pointer(ctx, -1);
//    image ->src.filename = const_cast<char *>(src);
//    duk_pop(ctx);

	/*获取object对象的方法二*/
	duk_get_prop_string(ctx, 0, "src");
	const char *src = duk_to_string(ctx, -1);
	duk_get_prop_string(ctx, 0, "imageData");
	xsImage *image = (xsImage *)duk_to_pointer(ctx, -1);
	image ->src.filename = xsUtf8ToTcsDup(const_cast<char *>(src));
	duk_pop_2(ctx);

	xsCanvasContext *context = get_native_context(ctx);
	duk_push_number(ctx, swidth);
	if(duk_is_nan(ctx, -1))
	{
		context ->drawImage(image, sx, sy);
	}
	else
	{
		duk_push_number(ctx, x);
		if(duk_is_nan(ctx, -1))
		{
			duk_push_number(ctx, sheight);
			if(!duk_is_nan(ctx, -1))
			{
				context ->drawImage(image, sx, sy, swidth, sheight);
			}
			duk_pop(ctx);
		}
		else
		{
			duk_push_number(ctx, y);
			duk_push_number(ctx, width);
			duk_push_number(ctx, height);
			if(!duk_is_nan(ctx, -1) && !duk_is_nan(ctx, -2) && !duk_is_nan(ctx, -3))
			{
				context ->drawImage(image, sx, sy, swidth, sheight, x, y, width, height);
			}
			duk_pop_3(ctx);
		}
		duk_pop(ctx);
	}
	duk_pop(ctx);

    return 1;
}

static duk_ret_t native_canvasContext_clip(duk_context *ctx)
{

	xsCanvasContext *context = get_native_context(ctx);
    context ->clip();

    return 1;
}

static duk_ret_t native_canvasContext_fillText(duk_context *ctx)
{
	const char *text = duk_require_string(ctx,0);
	float x = (float)duk_require_number(ctx,1);
	float y = (float)duk_require_number(ctx,2);
	xsU32 width = (xsU32)duk_get_number(ctx,3);

	xsCanvasContext *context = get_native_context(ctx);
	if(width > 0)
    {
		context ->fillText(xsUtf8ToTcsDup(text), x, y, width);
    }
	else
	{
		context ->fillText(xsUtf8ToTcsDup(text), x, y);
	}

    return 1;
}

static duk_ret_t native_canvasContext_strokeText(duk_context *ctx)
{
	const char *text = duk_require_string(ctx,0);
	float x = (float)duk_require_number(ctx,1);
	float y = (float)duk_require_number(ctx,2);
	xsU32 width = (xsU32)duk_get_number(ctx,3);

	xsCanvasContext *context = get_native_context(ctx);
	if(width > 0)
    {
		context ->strokeText(xsUtf8ToTcsDup(text),  x, y, width);
    }
	else
	{
		context ->strokeText(xsUtf8ToTcsDup(text), x, y);
	}

    return 1;
}

static duk_ret_t native_canvasContext_measureText(duk_context *ctx)
{
	const char *text = duk_require_string(ctx,0);

	xsCanvasContext *context = get_native_context(ctx);
	xsTextSize textSize = context ->measureText(xsUtf8ToTcsDup(text));

	duk_push_this(ctx);
	duk_push_number(ctx, textSize.width);
	duk_put_prop_string(ctx, -2, "width");
	duk_push_number(ctx, textSize.height);
	duk_put_prop_string(ctx, -2, "height");

    return 1;
}

static duk_ret_t native_canvasContext_save(duk_context *ctx)
{

	xsCanvasContext *context = get_native_context(ctx);
    context ->save();

    return 1;
}

static duk_ret_t native_canvasContext_restore(duk_context *ctx)
{

	xsCanvasContext *context = get_native_context(ctx);
    context ->restore();
    duk_push_this(ctx);
	duk_push_number(ctx, context ->lineWidth);
	duk_put_prop_string(ctx, -2, "lineWidth");
	char colorNum[8] = {0};
	xsSnprintf(colorNum, 8, "#%02x%02x%02x", context ->strokeColor.red, context ->strokeColor.green, context ->strokeColor.blue);
	duk_push_string(ctx, colorNum);
	duk_put_prop_string(ctx, -2, "strokeStyle");
	xsSnprintf(colorNum, 8, "#%02x%02x%02x", context ->fillColor.red, context ->fillColor.green, context ->fillColor.blue);
	duk_push_string(ctx, colorNum);
	duk_put_prop_string(ctx, -2, "fillStyle");
//	char fontString[128] ={0};
//	xsSnprintf(fontString, 128, "%fpx %d", context ->font.size, context ->font.style);
//	duk_push_string(ctx, fontString);
//	duk_put_prop_string(ctx, -2, "font");
//	printf("%s    %s\n", fontString, colorNum);
    return 1;
}

const duk_function_list_entry contextmethods[] = {
    { "strokeRect",   native_canvasContext_strokeRect,  4   },
    { "fillRect",   native_canvasContext_fillRect,  4   },
    { "moveTo",   native_canvasContext_moveTo,  2   },
    { "lineTo",   native_canvasContext_lineTo,  2  },
    { "stroke",   native_canvasContext_stroke,  0   },
    { "fill",   native_canvasContext_fill,  0   },

    { "rect",   native_canvasContext_rect,  4   },
    { "clearRect",   native_canvasContext_clearRect,  4   },
    { "arc",   native_canvasContext_arc,  6   },
    { "arcTo",   native_canvasContext_arcTo,  5  },
    { "beginPath",   native_canvasContext_beginPath,  0   },
    { "closePath",   native_canvasContext_closePath,  0   },

    { "bezierCurveTo",   native_canvasContext_bezierCurveTo,  6   },
    { "quadraticCurveTo",   native_canvasContext_quadraticCurveTo,  4   },
    { "translate",   native_canvasContext_translate,  2   },
    { "scale",   native_canvasContext_scale,  2  },
    { "rotate",   native_canvasContext_rotate,  1   },
    { "transform",   native_canvasContext_transform,  6   },
    { "drawImage",   native_canvasContext_drawImage,  9  },

    { "clip",   native_canvasContext_clip,  0   },
    { "fillText",   native_canvasContext_fillText,  4   },
    { "strokeText",   native_canvasContext_strokeText,  4   },
    { "measureText",   native_canvasContext_measureText,  1  },
    { "save",   native_canvasContext_save,  0   },
    { "restore",   native_canvasContext_restore,  0   },
    { NULL,  NULL,        0   }
};

static duk_ret_t native_canvas_getContext(duk_context *ctx)
{
	const char *data = duk_require_string(ctx, 0);
	if(strcmp(data, "2d") != 0)
		return 0;
	duk_push_this(ctx);
    duk_get_prop_string(ctx, -1, "nativeCanvasData");
    xsCanvas *canvas = static_cast<xsCanvas *>(duk_to_pointer(ctx, -1));
    duk_pop(ctx);
    duk_push_pointer(ctx, canvas ->getContext());
	duk_put_prop_string(ctx, -2, "nativeContextData");
	duk_push_number(ctx, canvas ->getContext() ->lineWidth);
	duk_put_prop_string(ctx, -2, "lineWidth");
	duk_push_string(ctx, "#000000");
	duk_put_prop_string(ctx, -2, "strokeStyle");
	duk_push_string(ctx, "#000000");
	duk_put_prop_string(ctx, -2, "fillStyle");
	duk_push_string(ctx, "10px sans-serif");
	duk_put_prop_string(ctx, -2, "font");

// duk_push_c_function(ctx, native_canvasContext_strokeRect, 4);
//	duk_put_prop_string(ctx, -2, "strokeRect");
	duk_put_function_list(ctx, -1, contextmethods);

    return 1;
}

static duk_ret_t native_canvas_ctor(duk_context *ctx)
{
	xsCanvas *canvas = static_cast<xsCanvas *>(xsCanvas::createInstance());
	duk_push_this(ctx);
    duk_push_int(ctx, canvas ->getWidth());
    duk_put_prop_string(ctx, -2, "width");
    duk_push_int(ctx, canvas ->getHeight());
    duk_put_prop_string(ctx, -2, "height");
    duk_push_c_function(ctx, native_canvas_getContext, 1);
	duk_put_prop_string(ctx, -2, "getContext");
	duk_push_pointer(ctx, canvas);
	duk_put_prop_string(ctx, -2, "nativeCanvasData");
    duk_push_boolean(ctx, false);
    duk_put_prop_string(ctx, -2, "nativeCanvasDelete");
    duk_push_c_function(ctx, native_canvas_dtor, 1);
    duk_set_finalizer(ctx, -2);

    return 1;
}

static duk_ret_t native_rgb_creator(duk_context *ctx)
{
	xsU8 red = (xsU8)duk_require_uint(ctx,0);
	xsU8 green = (xsU8)duk_require_uint(ctx,1);
	xsU8 blue = (xsU8)duk_require_uint(ctx,2);

	xsColor rgb = {255, red, green, blue};
	duk_push_pointer(ctx, (void *)&rgb);

	return 1;
}

static duk_ret_t native_rgba_creator(duk_context *ctx)
{
	xsU8 red = (xsU8)duk_require_number(ctx,0);
	xsU8 green = (xsU8)duk_require_number(ctx,1);
	xsU8 blue = (xsU8)duk_require_number(ctx,2);
	xsU8 alpha =  (xsU8)(duk_require_number(ctx,3) * 255);

	xsColor rgb = {alpha, red, green, blue};
	duk_push_pointer(ctx, (void *)&rgb);

	return 1;
}

static duk_ret_t native_image_creator(duk_context *ctx)
{
	const char *src = duk_require_string(ctx, 0);
	xsImage *img = (xsImage*)xsCalloc(sizeof(xsImage));
	xsImageParam *param = (xsImageParam*)xsCalloc(sizeof(xsImageParam));
	img ->fileType = XS_IMGTYPE_UNKNOWN;
	img ->srcType = XS_AFD_FILENAME;
	img ->srcparam = (void *)param;

	g_img[g_imgNum] = img;
	g_imgNum++;
	duk_push_this(ctx);
	duk_push_pointer(ctx, (void *)img);
	duk_put_prop_string(ctx, -2, "imageData");
	duk_push_string(ctx, src);
    duk_put_prop_string(ctx, -2, "src");

    return 1;
}

static void destroyHeap()
{
	for(int i = 0; i < g_imgNum; i++)
	{
		if(NULL != g_img[i])
		{
			if(NULL != g_img[i] ->srcparam)
			{
				delete (xsImageParam *)(g_img[i] ->srcparam);
			}

			delete g_img[i];
		}
	}

	duk_destroy_heap(g_ctx);
}

const duk_function_list_entry globalmethods[] = {
    { "Canvas",   native_canvas_ctor,  0  },
    { "setTimeout", native_setTimeout, 2},
    { "setInterval",   native_setInterval,  2  },
    { "clearInterval",   native_clearInterval,  1  },
    { "rgb",   native_rgb_creator,  3  },
    { "rgba",   native_rgba_creator,  4  },
    { "Image",   native_image_creator,  1  },
    { NULL,  NULL,        0   }
};

void invokeJavascript(const char *scriptURL, int type)
{
	g_ctx = duk_create_heap_default();
    if (!g_ctx)
    {
        printf("Failed to create a Duktape heap.\n");
        return;
    }

	duk_push_global_object(g_ctx);
	duk_put_function_list(g_ctx, -1, globalmethods);
	duk_push_object(g_ctx);
	duk_push_c_function(g_ctx, native_addEventListener, 3);
	duk_put_prop_string(g_ctx, -2, "addEventListener");
	duk_put_prop_string(g_ctx, -2, "document");
	duk_push_object(g_ctx);
	duk_push_number(g_ctx, event.keyCode);
	duk_put_prop_string(g_ctx, -2, "keyCode");
	duk_put_prop_string(g_ctx, -2, "event");

	if(type == LOAD_JS_FILE)
    {
		if (duk_peval_file(g_ctx, scriptURL) != 0)
		{
			printf("Error: %s\n", duk_safe_to_string(g_ctx, -1));
			destroyHeap();
			goto end;
		}
    }
	else if(type == LOAD_JS_STRING)
	{
		if (duk_peval_string(g_ctx, scriptURL) != 0)
		{
			printf("Error: %s\n", duk_safe_to_string(g_ctx, -1));
			destroyHeap();
			goto end;
		}
	}
    duk_pop(g_ctx);  /* ignore result */

/*    duk_get_prop_string(g_ctx, -1, "primeTest");
    if (duk_pcall(g_ctx, 0) != 0)
     {
        printf("Error1: %s\n", duk_safe_to_string(g_ctx, -1));
    }
    duk_pop(g_ctx);  /* ignore result */

    duk_get_prop_string(g_ctx, -1, "init");
    if (duk_pcall(g_ctx, 0) != 0)
    {
        printf("Error1: %s\n", duk_safe_to_string(g_ctx, -1));
    }
	duk_pop(g_ctx);  /* ignore result */
	return;
end:
	printf("Fail to invoke javascript file.\n");
	return;

}

void xsArrowKeysHandler(xsEvent *e)
{
	switch(e ->sys ->data.key.keyCode)
	{
	case XS_PAD_KEY_LEFT_ARROW:
		event.keyCode = 37;
		break;
	case XS_PAD_KEY_UP_ARROW:
		event.keyCode = 38;
		break;
	case XS_PAD_KEY_RIGHT_ARROW:
		event.keyCode = 39;
		break;
	case XS_PAD_KEY_DOWN_ARROW:
		event.keyCode = 40;
		break;
	case XS_PAD_KEY_SELECT:
	case XS_PAD_KEY_ENTER:
		event.keyCode = 13;
		break;
	}

    duk_push_global_object(g_ctx);
    duk_get_prop_string(g_ctx, -1, keyProcessingFunc);
    duk_get_prop_string(g_ctx, -2, "event");
	duk_push_number(g_ctx, event.keyCode);
	duk_put_prop_string(g_ctx, -2, "keyCode");
    if (duk_pcall(g_ctx, 1) != 0)
    {
        printf("Error1: %s\n", duk_safe_to_string(g_ctx, -1));
    }
    duk_pop(g_ctx);

}
