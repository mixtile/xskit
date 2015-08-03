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
#include <iostream>

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

static duk_ret_t native_canvas_dtor(duk_context *ctx)
{
    duk_get_prop_string(ctx, 0, "nativeCanvasDelete");

    bool deleted = duk_to_boolean(ctx, -1);
    duk_pop(ctx);

    if (!deleted) {
        duk_get_prop_string(ctx, 0, "nativeCanvasData");
        delete static_cast<xsCanvas *>(duk_to_pointer(ctx, -1));
        duk_pop(ctx);

        // Mark as deleted
        duk_push_boolean(ctx, true);
        duk_put_prop_string(ctx, 0, "nativeCanvasDelete");
    }

    return 0;
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
    const char *fillStyle = duk_to_string(ctx, -1);
    std::string fillTmp = fillStyle;
    context ->fillColor.red = htoi(const_cast<char *>(fillTmp.substr(1, 2).c_str()));
    context ->fillColor.green = htoi(const_cast<char *>(fillTmp.substr(3, 2).c_str()));
    context ->fillColor.blue = htoi(const_cast<char *>(fillTmp.substr(5, 2).c_str()));
//    printf("fillColor = %s\n", fillStyle);
//    printf("green = %s\n", const_cast<char *>(fillTmp.substr(3, 2).c_str()));
//	printf("red = %d, green = %d, blue = %d\n", context ->fillColor.red, context ->fillColor.green, context ->fillColor.blue);
    duk_pop(ctx);

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
	float x = (float)duk_require_number(ctx,1);
	float y = (float)duk_require_number(ctx,2);
	xsU32 width = (xsU32)duk_get_number(ctx,3);

	xsCanvasContext *context = get_native_context(ctx);
	if(width > 0)
    {
		context ->fillText(text, x, y, width);
    }
	else
	{
		context ->fillText(text, x, y);
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
		context ->strokeText(text, x, y, width);
    }
	else
	{
		context ->strokeText(text, x, y);
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
	char colorNum[10] = {0};
	snprintf(colorNum, 8, "#%02x%02x%02x", context ->strokeColor.red, context ->strokeColor.green, context ->strokeColor.blue);
	duk_push_string(ctx, colorNum);
	duk_put_prop_string(ctx, -2, "strokeStyle");
	snprintf(colorNum, 8, "#%02x%02x%02x", context ->fillColor.red, context ->fillColor.green, context ->fillColor.blue);
	duk_push_string(ctx, "#000000");
	duk_put_prop_string(ctx, -2, "fillStyle");

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

// duk_push_c_function(ctx, native_canvasContext_strokeRect, 4);
//	duk_put_prop_string(ctx, -2, "strokeRect");
	duk_put_function_list(ctx, -1, contextmethods);

    return 1;
}

static duk_ret_t native_canvas_ctor(duk_context *ctx)
{
	xsCanvas *canvas = static_cast<xsCanvas *>(xsCanvas::createInstance());
	duk_push_this(ctx);
    duk_push_c_function(ctx, native_canvas_getContext, 1);
//    duk_push_object(ctx);
//    duk_put_function_list(ctx, -1, contextmethods);
//    duk_put_prop_string(ctx, -2, "prototype");
	duk_put_prop_string(ctx, -2, "getContext");
	duk_push_pointer(ctx, canvas);
	duk_put_prop_string(ctx, -2, "nativeCanvasData");
    duk_push_boolean(ctx, false);
    duk_put_prop_string(ctx, -2, "nativeCanvasDelete");
    duk_push_c_function(ctx, native_canvas_dtor, 1);
    duk_set_finalizer(ctx, -2);

    return 1;
}

void duktape_test(void) {
    duk_context *ctx = NULL;

    ctx = duk_create_heap_default();
    if (!ctx) {
        printf("Failed to create a Duktape heap.\n");
        exit(1);
    }

    duk_push_global_object(ctx);
    duk_push_c_function(ctx, native_canvas_ctor, 0);
    duk_put_prop_string(ctx, -2, "Canvas");


    if (duk_peval_file(ctx, "/home/lewis/git/xs-new/prime.js") != 0) {
        printf("Error: %s\n", duk_safe_to_string(ctx, -1));
        goto finished;
    }
    duk_pop(ctx);  /* ignore result */

//    duk_get_prop_string(ctx, -1, "primeTest");
//    if (duk_pcall(ctx, 0) != 0) {
//        printf("Error1: %s\n", duk_safe_to_string(ctx, -1));
//    }
//    duk_pop(ctx);  /* ignore result */

    duk_get_prop_string(ctx, -1, "init");
    if (duk_pcall(ctx, 0) != 0) {
        printf("Error1: %s\n", duk_safe_to_string(ctx, -1));
    }
    duk_pop(ctx);  /* ignore result */

 finished:
    duk_destroy_heap(ctx);

    //exit(0);
}

