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

#include <xs/pal.h>
#include <xs/canvas.h>
#include <xs/memory.h>
#include <xs/coreclassid.h>
#include <xs/string.h>
#include <xs/resource.h>

xsCanvas::xsCanvas()
{
	context = NULL;
	setFlag(0);
	xsGetScreenDimension(&width, &height);
}

xsCanvas::~xsCanvas()
{
	if (context != NULL)
		context->destroyInstance();
}

int xsCanvas::getWidth(void)
{
	return width;
}
int xsCanvas::getHeight(void)
{
	return height;
}

void xsCanvas::getPreferredSize(int proposedWidth, xsMetrics *m)
{
	//m->width = width;
	//m->height = height;
}

void xsCanvas::paint(xsGraphics *gc)
{
	if (context == NULL)
		return;

	//context->paint(gc);
}

xsCanvasContext *xsCanvas::getContext()
{
	if (context != NULL)
		return context;

	context = static_cast<xsCanvasContext *>(xsCanvasContext::createInstance());
	return context;
}

