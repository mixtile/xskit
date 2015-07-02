#ifndef _XS_FILESTREAM_H_
#define _XS_FILESTREAM_H_

#include <xs/stream.h>

class xsFileStream : public xsRandomAccessStream
{
public:
	xsFileStream();
	xsFileStream(xsFile handle);
	virtual ~xsFileStream();

	int open(const xsTChar *filename, xsU32 style);
	virtual void close();

	// read related method
	virtual int available();
	virtual int read();
	virtual size_t read(void *buf, size_t size);
	virtual size_t skip(size_t bytes);

	// write related method
	virtual void flush();
	virtual void write(const void *buf, size_t size);
	virtual void write(xsU8 byte);

	// random accesss
	virtual long setPosition(long pos);
	virtual long getPosition();

	inline xsFile getFd(xsBool keepOpen = XS_FALSE)
	{
		_externalHandle = keepOpen;
		return _fd;
	}

private:
	xsFile _fd;
	xsBool _externalHandle;
// TODO: implement input buffer.	char *_inputBuffer;
};

#endif /* _XS_FILESTREAM_H_ */
