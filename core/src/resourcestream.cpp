#include <xs/resourcestream.h>
#include <xs/pal.h>
#include <xs/memory.h>

xsResourceStream::xsResourceStream()
{
	_rd = xsResInvalidHandle();
	_pointer = (size_t)-1;
	_externalHandle = XS_FALSE;
//	_inputBuffer = NULL;
}

xsResourceStream::xsResourceStream(xsRes handle)
{
	_rd = handle;
	_pointer = 0;
	_externalHandle = XS_TRUE;
}

xsResourceStream::~xsResourceStream()
{
	close();
}

int xsResourceStream::open(const xsTChar *resname)
{
	// close opened handle if exists
	close();

	xsRes rd = xsOpenRes(resname);
	if (!xsResHandleValid(rd))
		return XS_EC_ERROR;

	_rd = rd;
	_pointer = 0;
	return XS_EC_OK;
}

void xsResourceStream::close()
{
	if (!_externalHandle && xsResHandleValid(_rd))
	{
		xsCloseRes(_rd);
		_rd = xsResInvalidHandle();
	}

//	if (_inputBuffer != NULL)
//	{
//		xsFree(_inputBuffer);
//		_inputBuffer = NULL;
//	}
}

int xsResourceStream::available()
{
	return 0;
}

int xsResourceStream::read()
{
	xsU8 byte;

	if (xsReadRes(_rd, &byte, _pointer, 1) != sizeof(xsU8))
		return XS_EC_ERROR;
	_pointer++;

	return byte;
}

size_t xsResourceStream::read(void *buf, size_t size)
{
	size_t read = xsReadRes(_rd, buf, _pointer, size);
	if (read > 0)
		_pointer += read;
	return read;
}

size_t xsResourceStream::skip(size_t bytes)
{
	_pointer += bytes;
	return _pointer;
}

void xsResourceStream::flush()
{
}

void xsResourceStream::write(const void *buf, size_t size)
{
}

void xsResourceStream::write(xsU8 byte)
{
}

long xsResourceStream::setPosition(long pos)
{
	_pointer = pos;
	return _pointer;
}

long xsResourceStream::getPosition()
{
	return _pointer;
}
