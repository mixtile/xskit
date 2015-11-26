/*
 * bufferstream.h
 *
 *  Created on: 2015-11-18
 *      Author: lewis
 */

#ifndef BUFFERSTREAM_H_
#define BUFFERSTREAM_H_

#include <xs/stream.h>

class xsBufferStream : public xsStream
{
public:
	xsBufferStream();
	xsBufferStream(size_t size);
	virtual ~xsBufferStream();

	virtual void close();
	void clear();

	// read related method
	virtual int available();
	virtual int read();
	virtual size_t read(void *buf, size_t size);
	virtual size_t skip(size_t bytes);

	// write related method
	virtual void flush();
	virtual void write(const void *buf, size_t size);
	virtual void write(xsU8 byte);

	inline size_t getSize(){return this->size;}
	inline xsU8 *getData(){return this->buffer;}

private:
	xsU8 *buffer;
	size_t pos;
	size_t size;
	size_t capacity;
};

#endif /* BUFFERSTREAM_H_ */
