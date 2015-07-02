#include <xs/stream.h>
#include <xs/pal.h>
#include <xs/memory.h>
/*
typedef struct _xsFileStream
{
	int type;
	xsFile fd;
} xsFileStream;

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