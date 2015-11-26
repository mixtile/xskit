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

#ifndef _XS_RECTANGLE_H_
#define _XS_RECTANGLE_H_

#include <xs/object.h>
#include <xs/shape.h>

class xsRectangle : public xsShape
{
	XS_OBJECT_DECLARE(Rectangle, XS_CLS_RECTANGLE)
public:
	float x;
	float y;
	float width;
	float height;
	float lineWidth;
	xsColor strokeColor;
	xsColor fillColor;

	virtual void paint(xsGraphics *gc);
	void fill(xsGraphics *gc);
	void stroke(xsGraphics *gc);
};


#endif /* _XS_RECTANGLE_H_ */
