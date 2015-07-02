#ifndef _XS_STREAM_H_
#define _XS_STREAM_H_

#include <xs/pal.h>
#include <xs/base.h>
/*
typedef struct _xsStream
{
	int type;

} xsStream;
*/
class xsStream : public xsBase
{
public:
	virtual void close() = 0;

	// read related method
	virtual int available() = 0;
	virtual int read() = 0;
	virtual size_t read(void *buf, size_t size) = 0;
	virtual size_t skip(size_t bytes) = 0;

	// write related method
	virtual void flush() = 0;
	virtual void write(const void *buf, size_t size) = 0;
	virtual void write(xsU8 byte) = 0;
};

class xsRandomAccessStream : public xsStream
{
public:
	virtual long setPosition(long pos) = 0;
	virtual long getPosition() = 0;
};
/*
enum _xsStreamType
{
	XS_STREAM_BUFFER,
	XS_STREAM_FILE
};

xsStream *xsCreateFileStream(const xsTChar *filename, xsU32 style);
void xsDestroyStream(xsStream *stream);
int xsStreamWrite(xsStream *stream, xsU8 byte);
int xsStreamWriteBuffer(xsStream *stream, const void *buf, size_t size);
*/
#endif /* _XS_STREAM_H_ */
