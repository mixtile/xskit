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
