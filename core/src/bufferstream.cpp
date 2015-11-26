/*
 * bufferstream.cpp
 *
 *  Created on: 2015-11-18
 *      Author: lewis
 */

#include <xs/bufferstream.h>
#include <xs/pal.h>
#include <xs/memory.h>

const size_t  XS_BUFFER_SIZE = 4096;

xsBufferStream::xsBufferStream()
{
	buffer = (xsU8 *)xsCalloc(XS_BUFFER_SIZE * sizeof(xsU8));
	if(buffer == NULL)
	{
		printf("xsCalloc failed!\n");
		return;
	}
	capacity = XS_BUFFER_SIZE;
	pos = 0;
	size = 0;
}

xsBufferStream::xsBufferStream(size_t size)
{
	buffer = (xsU8 *)xsCalloc(size * sizeof(xsU8));
	if(buffer == NULL)
	{
		printf("xsCalloc failed!\n");
		return;
	}
	capacity = size;
	pos = 0;
	this->size = 0;
}

xsBufferStream::~xsBufferStream()
{
	close();
}

void xsBufferStream::close()
{
	if(buffer != NULL)
	{
		xsFree(buffer);
		buffer = NULL;
		capacity = 0;
		size = 0;
	}
}

int xsBufferStream::available()
{
	if(buffer != NULL)
		return 1;
	else
		return 0;
}

int xsBufferStream::read()
{
	if(buffer == NULL || pos >= capacity)
		return XS_EC_ERROR;

	pos++;
	return *(buffer + pos -1);
}

size_t xsBufferStream::read(void *buf, size_t size)
{
	if(buffer == NULL || buf == NULL)
		return 0;

	if(pos + size > capacity)
	{
		xsMemCpy(buf, (buffer + pos), capacity - pos);
		return capacity - pos;
		pos = capacity;
	}

	xsMemCpy(buf, (buffer + pos), size);
	pos += size;
	return size;
}

size_t xsBufferStream::skip(size_t bytes)
{
	if(buffer == NULL || pos + bytes >capacity)
		return 0;

	pos += bytes;
	return bytes;
}

void xsBufferStream::flush()
{}

void xsBufferStream::write(const void *buff, size_t size)
{
	if(buff == NULL || buffer == NULL)
	{
		printf("NULL  pointer!");
		return;
	}

	if(this->size + size > capacity)
	{
		while(capacity < this->size + size)
		{
			capacity *= 2;
		}

		xsU8 *oldBuffer = buffer;
		buffer = (xsU8 *)xsCalloc(capacity * sizeof(xsU8));
		if(buffer == NULL)
		{
			printf("xsCalloc failed!\n");
			return;
		}
		xsMemCpy(buffer, oldBuffer, this->size);

		xsFree(oldBuffer);
		oldBuffer = NULL;
	}

	for(size_t i = 0; i < size; i++)
	{
		*(buffer + this->size++) = *((xsU8 *)buff + i);
	}
}

void xsBufferStream::write(xsU8 byte)
{
	if(buffer == NULL)
		return;

	if(size >= capacity)
	{
		xsU8 *oldBuffer = buffer;
		buffer = (xsU8 *)xsCalloc(2 * capacity * sizeof(xsU8));
		if(buffer == NULL)
		{
			printf("xsCalloc failed!\n");
			return;
		}
		xsMemCpy(buffer, oldBuffer, capacity);

		capacity *= 2;
		xsFree(oldBuffer);
		oldBuffer = NULL;
	}

	*(buffer + size) = byte;
	size++;
}

void xsBufferStream::clear()
{
	if(buffer == NULL)
		return;

	xsMemSet(buffer, 0, capacity);
	pos = 0;
	size = 0;
}
