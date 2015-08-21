/*
 * canvasinterface.c
 *
 *  Created on: 2015-7-23
 *      Author: lewis
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "duktape.h"
#include "xs/canvas.h"
#include "xs/pal.h"
#include <iostream>

typedef struct _xsJSEvent
{
	xsU16 keyCode;
	xsU32 which;
}xsJSEvent;

duk_context *g_ctx = NULL;
char keyProcessingFunc[128] = {0};
xsJSEvent event = {};

/*将大写字母转换成小写字母*/
int tolower(int c)
{
	if (c >= 'A' && c <= 'Z')
	{
		return c + 'a' - 'A';
	}
	else
	{
		return c;
	}
}

//将十六进制的字符串转换成整数
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

void parseFontString(std::string &fontString, xsFontType &font)
{
    int spacePos = fontString.find(" ", 0);
    std::string font1 = fontString.substr(0, spacePos).c_str();
    int pxPos = font1.find("px", 0);
    if(pxPos)
    {
    	const char *px = font1.substr(0, pxPos).c_str();
    	int size = atoi(px);
    	font.size = size;
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
    std::string strokeTmp = strokeStyle;
    context ->strokeColor.red = htoi(const_cast<char *>(strokeTmp.substr(1, 2).c_str()));
    context ->strokeColor.green = htoi(const_cast<char *>(strokeTmp.substr(3, 2).c_str()));
    context ->strokeColor.blue = htoi(const_cast<char *>(strokeTmp.substr(5, 2).c_str()));
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
    std::string fillTmp = fillStyle;
    context ->fillColor.red = htoi(const_cast<char *>(fillTmp.substr(1, 2).c_str()));
    context ->fillColor.green = htoi(const_cast<char *>(fillTmp.substr(3, 2).c_str()));
    context ->fillColor.blue = htoi(const_cast<char *>(fillTmp.substr(5, 2).c_str()));
    duk_pop(ctx);
    duk_get_prop_string(ctx, -1, "font");
    const char *font = duk_to_string(ctx, -1);
    std::string fontTmp = font;
    parseFontString(fontTmp, context ->font);

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

//static duk_ret_t native_canvasContext_drawImage(duk_context *ctx)
//{
//	float x1 = (float)duk_require_number(ctx,0);
//	float y1 = (float)duk_require_number(ctx,1);
//	float x2 = (float)duk_require_number(ctx,2);
//	float y2 = (float)duk_require_number(ctx,3);
//	float x3 = (float)duk_require_number(ctx,4);
//	float y3 = (float)duk_require_number(ctx,5);
//
//	xsCanvasContext *context = get_native_context(ctx);
//    context ->transform(x1, y1, x2, y2, x3, y3);
//
//    return 1;
//}

static duk_ret_t native_canvasContext_clip(duk_context *ctx)
{

	xsCanvasContext *context = get_native_context(ctx);
    context ->clip();

    return 1;
}

static duk_ret_t native_canvasContext_fillText(duk_context *ctx)
{
	const char *text = duk_require_string(ctx,0);
	int count = duk_require_int(ctx, 1);
	float x = (float)duk_require_number(ctx,2);
	float y = (float)duk_require_number(ctx,3);
	xsU32 width = (xsU32)duk_get_number(ctx,4);

	xsCanvasContext *context = get_native_context(ctx);
	if(width > 0)
    {
		context ->fillText(text, count, x, y, width);
    }
	else
	{
		context ->fillText(text, count, x, y);
	}

    return 1;
}

static duk_ret_t native_canvasContext_strokeText(duk_context *ctx)
{
	const char *text = duk_require_string(ctx,0);
	int count = duk_require_int(ctx, 1);
	float x = (float)duk_require_number(ctx,2);
	float y = (float)duk_require_number(ctx,3);
	xsU32 width = (xsU32)duk_get_number(ctx,4);

	xsCanvasContext *context = get_native_context(ctx);
	if(width > 0)
    {
		context ->strokeText(text, count, x, y, width);
    }
	else
	{
		context ->strokeText(text, count, x, y);
	}

    return 1;
}

static duk_ret_t native_canvasContext_measureText(duk_context *ctx)
{
	const char *text = duk_require_string(ctx,0);

	xsCanvasContext *context = get_native_context(ctx);
	xsTextSize textSize = context ->measureText(text);

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
	snprintf(colorNum, 8, "#%02x%02x%02x", context ->strokeColor.red, context ->strokeColor.green, context ->strokeColor.blue);
	duk_push_string(ctx, colorNum);
	duk_put_prop_string(ctx, -2, "strokeStyle");
	snprintf(colorNum, 8, "#%02x%02x%02x", context ->fillColor.red, context ->fillColor.green, context ->fillColor.blue);
	duk_push_string(ctx, colorNum);
	duk_put_prop_string(ctx, -2, "fillStyle");
	char fontString[128] ={0};
	snprintf(fontString, 128, "%dpx %s", context ->font.size, context ->font.style);
	duk_push_string(ctx, fontString);
	duk_put_prop_string(ctx, -2, "font");

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

static void destroyHeap()
{
	duk_destroy_heap(g_ctx);
	exit(0);
}

const duk_function_list_entry globalmethods[] = {
    { "Canvas",   native_canvas_ctor,  0  },
    { "setTimeout", native_setTimeout, 2},
    { "setInterval",   native_setInterval,  2  },
    { "clearInterval",   native_clearInterval,  1  },
    { "rgb",   native_rgb_creator,  3  },
    { "rgba",   native_rgba_creator,  4  },
    { NULL,  NULL,        0   }
};

void duktape_test(void) {
	g_ctx = duk_create_heap_default();
    if (!g_ctx) {
        printf("Failed to create a Duktape heap.\n");
        exit(1);
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

    if (duk_peval_file(g_ctx, "/home/lewis/git/xs-new/prime.js") != 0) {
        printf("Error: %s\n", duk_safe_to_string(g_ctx, -1));
        destroyHeap();
    }
    duk_pop(g_ctx);  /* ignore result */

//    duk_get_prop_string(g_ctx, -1, "primeTest");
//    if (duk_pcall(g_ctx, 0) != 0) {
//        printf("Error1: %s\n", duk_safe_to_string(g_ctx, -1));
//    }
//    duk_pop(g_ctx);  /* ignore result */

    duk_get_prop_string(g_ctx, -1, "init");
    if (duk_pcall(g_ctx, 0) != 0) {
        printf("Error1: %s\n", duk_safe_to_string(g_ctx, -1));
    }
    duk_pop(g_ctx);  /* ignore result */

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
	case XS_PAD_KEY_ENTER:
		event.keyCode = 13;
		break;
	}

    duk_push_global_object(g_ctx);
    duk_get_prop_string(g_ctx, -1, keyProcessingFunc);
    duk_get_prop_string(g_ctx, -2, "event");
	duk_push_number(g_ctx, event.keyCode);
	duk_put_prop_string(g_ctx, -2, "keyCode");
    if (duk_pcall(g_ctx, 1) != 0) {
        printf("Error1: %s\n", duk_safe_to_string(g_ctx, -1));
    }
    duk_pop(g_ctx);

}
