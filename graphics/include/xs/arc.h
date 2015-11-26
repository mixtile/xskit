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

#ifndef ARC_H_
#define ARC_H_

#include <xs/object.h>
#include <xs/shape.h>

class xsArc: public xsShape
{
	XS_OBJECT_DECLARE(Arc, XS_CLS_ARC)
public:
	float x;
	float y;
	float radius;
	float startAngle;
	float endAngle;
	float lineWidth;
	xsBool anticlockwise;

	xsColor strokeColor;
	xsColor fillColor;

	virtual void paint(xsGraphics *gc);
	void fill(xsGraphics *gc);
	void stroke(xsGraphics *gc);
};


#endif /* ARC_H_ */
