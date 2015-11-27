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

#ifndef CANVASINTERFACE_H_
#define CANVASINTERFACE_H_

#include "duktape.h"

enum
{
	LOAD_JS_FILE = 0,
	LOAD_JS_STRING = 1,
};
/**
 * init canvas for javascript and load javascript file
 * @param scriptURL URL of javascript source file
 */
extern void invokeJavascript(const char *scriptURL, int type);

/**
 * handle key event for javascript
 * @param e event data
 */
extern void xsArrowKeysHandler(xsEvent *e);

#endif /* CANVASINTERFACE_H_ */
