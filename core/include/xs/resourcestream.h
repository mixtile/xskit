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

#ifndef _XS_RESOURCESTREAM_H_
#define _XS_RESOURCESTREAM_H_

#include <xs/stream.h>
class xsResourceStream : public xsRandomAccessStream
{
public:
	xsResourceStream();
	xsResourceStream(xsRes handle);
	virtual ~xsResourceStream();

	int open(const xsTChar *resname);
	virtual void close();

	// read related method
	virtual int available();
	virtual int read();
	virtual size_t read(void *buf, size_t size);
	virtual size_t skip(size_t bytes);

	// random accesss
	virtual long setPosition(long pos);
	virtual long getPosition();

	inline xsRes getRes() { return _rd; }

private:
	// write related method disabled
	virtual void flush();
	virtual void write(const void *buf, size_t size);
	virtual void write(xsU8 byte);



private:
	xsRes _rd;
	xsBool _externalHandle;
	size_t _pointer;
// TODO: implement input buffer.	char *_inputBuffer;
};

#endif /* _XS_RESOURCESTREAM_H_ */
