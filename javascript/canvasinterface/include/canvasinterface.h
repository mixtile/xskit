/*
 * canvasinterface.h
 *
 *  Created on: 2015-7-23
 *      Author: lewis
 */

#ifndef CANVASINTERFACE_H_
#define CANVASINTERFACE_H_

#include "duktape.h"

/**
 * init canvas for javascript and load javascript file
 * @param scriptURL URL of javascript source file
 */
extern void invokeJavascript(const char *scriptURL);

/**
 * handle key event for javascript
 * @param e event data
 */
extern void xsArrowKeysHandler(xsEvent *e);

#endif /* CANVASINTERFACE_H_ */
