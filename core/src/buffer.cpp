#include <xs/pal.h>
#include <xs/buffer.h>
#include <xs/memory.h>

xsBuffer *xsBufferCreate(size_t size)
{
	xsBuffer *buffer = (xsBuffer *)xsMalloc(size + sizeof(xsBuffer) - sizeof(buffer->data));
	if (buffer == NULL)
		return NULL;

	buffer->size = size;
	buffer->len = 0;
	buffer->head = buffer->data;
	buffer->tail = buffer->data;

	return buffer;
}

size_t xsBufferBlockSize(xsBuffer *buffer)
{
	if (buffer->len == buffer->size)
		return 0;

	if (buffer->tail >= buffer->data + buffer->size)
		buffer->tail = buffer->data; // cyclic

	if (buffer->tail >= buffer->head)
		return buffer->size - (buffer->tail - buffer->head);
	else
		return buffer->head - buffer->tail;
}

xsBuffer *xsBufferClear(xsBuffer *buffer)
{
	buffer->len = 0;
	buffer->head = buffer->tail = buffer->data;
	return buffer;
}

xsBuffer *xsBufferExtend(xsBuffer *buffer)
{
	xsBuffer *bufnew;
	size_t sizenew;
	size_t head, tail;
	if (buffer == NULL)
		return NULL;

	head = buffer->head - buffer->data;
	tail = buffer->tail - buffer->data;
	sizenew = (buffer->size * 3) / 2;
	bufnew = (xsBuffer *)xsReAlloc(buffer, sizenew + sizeof(xsBuffer) - sizeof(buffer->data));
	if (bufnew == NULL)
		return buffer;

	bufnew->head = bufnew->data + head;
	bufnew->tail = bufnew->data + tail;

	// cyclic buffer, need enlarge space
	if (head > 0 && head > tail)
		xsMemMove(buffer->head + bufnew->size - sizenew, buffer->head, buffer->len - tail);
	
	bufnew->size = sizenew;
	return bufnew;
}

int xsBufferProduce(xsBuffer *buffer, size_t size)
{
	if (size + buffer->len > buffer->size)
		return XS_EC_OVERFLOW;

	buffer->tail += size;
	buffer->len += size;

	if (buffer->len < buffer->size && buffer->tail >= buffer->data + buffer->size)
		buffer->tail = buffer->tail - buffer->size; // cyclic

	return XS_EC_OK;
}

int xsBufferConsume(xsBuffer *buffer, size_t size)
{
	if (size > buffer->len)
		return XS_EC_OVERFLOW;

	buffer->head += size;
	buffer->len -= size;

	if (buffer->len > 0 && buffer->head >= buffer->data + buffer->size)
		buffer->head = buffer->head - buffer->size; // cyclic
	if (buffer->len == 0)
		buffer->head = buffer->tail = buffer->data;

	return XS_EC_OK;
}
