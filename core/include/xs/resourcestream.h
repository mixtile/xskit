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
