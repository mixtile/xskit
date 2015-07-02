#include <xs/filestream.h>
#include <xs/pal.h>
#include <xs/memory.h>

xsFileStream::xsFileStream()
{
	_fd = xsFileInvalidHandle();
	_externalHandle = XS_FALSE;
//	_inputBuffer = NULL;
}

xsFileStream::xsFileStream(xsFile handle)
{
	_fd = handle;
	_externalHandle = XS_TRUE;
}

xsFileStream::~xsFileStream()
{
	close();
}

int xsFileStream::open(const xsTChar *filename, xsU32 style)
{
	// close opened handle if exists
	close();

	xsFile fd = xsOpenFile(filename, style);
	if (!xsFileHandleValid(fd))
		return XS_EC_ERROR;

//	if ((style & XS_OF_READONLY) || (style & XS_OF_READWRITE))
//		_inputBuffer = (char *)xsMalloc(XS_CONFIG_FILE_IOBUFFER_SIZE);

	_fd = fd;
	return XS_EC_OK;
}

void xsFileStream::close()
{
	if (!_externalHandle && xsFileHandleValid(_fd))
	{
		xsCloseFile(_fd);
		_fd = xsFileInvalidHandle();
	}

//	if (_inputBuffer != NULL)
//	{
//		xsFree(_inputBuffer);
//		_inputBuffer = NULL;
//	}
}

int xsFileStream::available()
{
	return 0;
}

int xsFileStream::read()
{
	xsU8 byte;
	if (xsReadFile(_fd, &byte, 1) != sizeof(xsU8))
		return XS_EC_ERROR;

	return byte;
}

size_t xsFileStream::read(void *buf, size_t size)
{
	return xsReadFile(_fd, buf, size);
}

size_t xsFileStream::skip(size_t bytes)
{
	return xsSetFilePointer(_fd, bytes, XS_FILE_CURRENT);
}

void xsFileStream::flush()
{
	xsFlushFile(_fd);
}

void xsFileStream::write(const void *buf, size_t size)
{
	size_t written = xsWriteFile(_fd, buf, size);
	if (written != size)
	{
		XS_ERROR("xsFileStream::write: Write file error. required:%d bytes, written:%d bytes.", size, written);
	}
}

void xsFileStream::write(xsU8 byte)
{
	write(&byte, 1);
}

long xsFileStream::setPosition(long pos)
{
	return xsSetFilePointer(_fd, pos, XS_FILE_BEGIN);
}

long xsFileStream::getPosition()
{
	return xsGetFilePointer(_fd);
}

/*
xsStream *xsCreateFileStream(const xsTChar *filename, xsU32 style)
{
	xsFileStream *stream = (xsFileStream *)xsMalloc(sizeof(xsFileStream));
	if (stream == NULL)
		return NULL;

	stream->type = XS_STREAM_FILE;
	stream->fd = xsOpenFile(filename, style);
	if (!xsFileHandleValid(stream->fd))
	{
		xsFree(stream);
		return NULL;
	}

	return (xsStream *)stream;
}

void xsDestroyStream(xsStream *stream)
{
	if (stream == NULL)
		return;
	
	if (stream->type == XS_STREAM_FILE)
	{
		xsFileStream *fs = (xsFileStream *)stream;
		xsCloseFile(fs->fd);
	}

	xsFree(stream);
}

int xsStreamWrite(xsStream *stream, xsU8 byte)
{
	return xsStreamWriteBuffer(stream, &byte, 1);
}

int xsStreamWriteBuffer(xsStream *stream, const void *buf, size_t size)
{
	if (stream == NULL)
		return XS_EC_NOT_EXISTS;

	if (stream->type == XS_STREAM_FILE)
	{
		xsFileStream *fs = (xsFileStream *)stream;
		return xsWriteFile(fs->fd, buf, size);
	}

	return 0;
}
*/
