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

#ifndef _XS_BUFFER_H_
#define _XS_BUFFER_H_

typedef struct _xsBuffer
{
	size_t size;
	size_t len;
	char *head;
	char *tail;
	char data[4];
} xsBuffer;

xsBuffer *xsBufferCreate(size_t size);
size_t xsBufferBlockSize(xsBuffer *buffer);
xsBuffer *xsBufferClear(xsBuffer *buffer);
xsBuffer *xsBufferExtend(xsBuffer *buffer);
int xsBufferProduce(xsBuffer *buffer, size_t size);
int xsBufferConsume(xsBuffer *buffer, size_t size);

#endif /* _XS_BUFFER_H_ */
