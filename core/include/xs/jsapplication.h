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

#ifndef JSAPPLICATION_H_
#define JSAPPLICATION_H_

#include <xs/coreapplication.h>

class xsJSApplication : public xsCoreApplication
{
public:
	xsJSApplication(void);
	~xsJSApplication(void);

	int start();
	void suspend();
	int resume();
	void exit();

	int processEvent(xsEvent *e);

private:
	int width, height;
	xsU32 timer;
	int x, y;
	int rateX, rateY;
	xsImage *img;

	static int _onTimeout(void *userdata);
	int onTimeout();

};

#endif /* JSAPPLICATION_H_ */
