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

#ifndef _XS_BASE_H_
#define _XS_BASE_H_

#ifdef __cplusplus

#include <xs/pal.h> // which defined XS_MEM_DEBUG

class xsBase
{
public:
#ifndef XS_MEM_DEBUG

	void *operator new(size_t size);
	void *operator new[](size_t size);

#else
	void *operator new(size_t size, const char *file, int line);
	void *operator new[](size_t size, const char *file, int line);
	void operator delete(void *p, const char *file, int line);
	void operator delete[](void *p, const char *file, int line);

#endif /* XS_MEM_DEBUG */

	void operator delete(void *p);
	void operator delete[](void *p);
};

#ifdef XS_MEM_DEBUG

#define XS_NEW_DEBUG				new(__FILE__, __LINE__)
#ifndef XS_BASE_SELF_INCLUDE
#define new							XS_NEW_DEBUG
#endif

#endif

#endif /* __cplusplus */

#endif /* _XS_BASE_H_ */
