/*
 * Copyright (C) 2015 Focalcrest, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdlib.h>
#include <xs/pal.h>
#include <xs/bon.h>
#include <xs/error.h>
#include <xs/string.h>
#include <xs/memory.h>
#include <xs/container.h>
#include <xs/filestream.h>
#include <xs/bufferstream.h>
#include <xs/resourcestream.h>
#include <xs/utils.h>
#include <xs/value.h>
#ifndef XS_NO_COMPLEX_DATA_TYPE
#include <xs/array.h>
#include <xs/object.h>
#endif

#if 0
#define XS_TRACE_DEBUG	XS_TRACE
#else
#define XS_TRACE_DEBUG	XS_TRACE_NONE
#endif

const int FLAG_USE_UTF8 = 0x02;
const int FLAG_USE_NATIVE = 0x04;
const int FLAG_INDEXED = 0x10;
const int FLAG_HASHED = 0x20;
const int MAX_SALT_RETRY = 10;

static int flag = 0;
static const char *charset = "utf8";

typedef struct _xsParseNode
{
	xsValue container;
	xsValue lastKey;
	int fields;
} xsParseNode;

typedef struct _xsParseData
{
	xsParseNode *head;

	xsStack stack;
} xsParseData;

static int BonCheckEndian(xsStream &stream, xsBool *bigendian);

static int ReadLength(xsStream &stream, xsBool bigendian, size_t *length);
static int ReadInt32(xsStream &stream, size_t bytes, xsBool bigendian, xsBool sign, xsS32 *value);
static int ReadInt64(xsStream &stream, size_t bytes, xsBool sign, void **value);
static int ValueHandler(xsValue *value, void *userdata);

static void WriteInt16(xsStream &stream, xsS16 n);
static void WriteInt32(xsStream &stream, xsS32 n);
static void WriteInt64(xsStream &stream, void *ptr);
#ifndef XS_NO_COMPLEX_DATA_TYPE
static void WriteObject(xsStream &stream, xsValue *value);
static void WriteArray(xsStream &stream, xsValue *value);
#endif


/**
 * extract data from BON to value
 * @param bon
 * @param length
 * @param value
 * @return
 */
int xsBonUnpack(xsStream &stream, xsBool bigendian, xsValue *value)
{
	xsParseData userdata;
	int ret;

	XS_ASSERT(value != NULL);
	XS_TRACE("[BON]xsBonUnpack");
	
	if (bigendian != xsIsBigEndian())
	{
		XS_ERROR("BON file wrong endian."); // TODO Handle different endian
		return XS_EC_ERROR;
	}

	xsMemSet(&userdata, 0 , sizeof(userdata));
	userdata.stack = xsStackCreate(32);

	ret = xsBonParser(stream, bigendian, ValueHandler, &userdata);
	*value = userdata.head->container;
	xsFree(userdata.head);
	XS_TRACE("[BON]xsBonUnpack: value.type:%d", value->type);

	while (!xsStackEmpty(userdata.stack))
	{
		xsParseNode *node = (xsParseNode *)xsStackPop(userdata.stack);
		xsValueDestroy(&node->container, XS_FALSE);
		xsValueDestroy(&node->lastKey, XS_FALSE);
	}
	xsStackDestroy(userdata.stack);

	return ret;
}

int xsBonPack(xsStream &stream, xsValue *value)
{
	if (value == NULL)
		return XS_EC_NOT_EXISTS;

	if (value->type == XS_VALUE_INT32 || value->type == XS_VALUE_UINT32)
	{
		int n = value->data.n;

		if (value->type == XS_VALUE_UINT32 && (xsU32)n > 0x7FFFFFFFU)
		{
			stream.write(XS_BON_INT | XS_BON_UINT32);
			WriteInt32(stream, n);
		}
		else if (n >= -32 && n <= 127)
		{// Combo uint/mint
			stream.write((xsU8)n);
		}
		else if (n >= -128 && n <= 127)
		{// Int8
			stream.write(XS_BON_INT | XS_BON_INT8);
			stream.write((xsU8)n);
		}
		else if (n > 127 && n <= 255)
		{// UInt8
			stream.write(XS_BON_UINT | XS_BON_UINT8);
			stream.write((xsU8)n);
		}
		else if (n >= -32768 && n <= 32767)
		{// Int16
			stream.write(XS_BON_INT | XS_BON_INT16);
			WriteInt16(stream, (xsS16)n);		
		}
		else if (n > 32767 && n <= 65535)
		{// UInt16
			stream.write(XS_BON_UINT | XS_BON_UINT16);
			WriteInt16(stream, (xsS16)n);
		}
		else if (n >= (-2147483647 - 1) && n <= 2147483647)
		{// Int32
			stream.write(XS_BON_INT | XS_BON_INT32);
			WriteInt32(stream, n);
		}
		else
		{
			stream.write(XS_BON_UNDEFINE);
		}
	}
	else if (value->type == XS_VALUE_TEXT)
	{
#ifdef XS_UNICODE
		stream.write(XS_BON_STRING | XS_BON_TEXT_UCS2);
#else
		if((flag & FLAG_USE_UTF8) == FLAG_USE_UTF8)
		{
			stream.write(XS_BON_STRING | XS_BON_TEXT_UTF8);
		}
		else
		{
			stream.write(XS_BON_STRING | XS_BON_TEXT_NCS); // Native charset
		}
#endif
		if (value->data.t == NULL)
			stream.write(xsT("\0"), sizeof(xsTChar));
		else
		{
			int size = (xsTcsLen(value->data.t) + 1) * sizeof(xsTChar);
			xsValue val;
			val.type = XS_VALUE_UINT32;
			val.data.n = size;
			xsBonPack(stream, &val);
			stream.write(value->data.t, (xsTcsLen(value->data.t) + 1) * sizeof(xsTChar));
		}
	}
	else if (value->type == XS_VALUE_STRING)
	{
		stream.write(XS_BON_STRING | XS_BON_STRING_ANSI);
		if (value->data.s == NULL)
			stream.write("\0", 1);
		else
		{
			int size =xsStrLen(value->data.s) + 1;
			xsValue val;
			val.type = XS_VALUE_UINT32;
			val.data.n = size;
			xsBonPack(stream, &val);
			stream.write(value->data.s, xsStrLen(value->data.s) + 1);
		}
	}
	else if (value->type == XS_VALUE_BOOL)
	{
		if (value->data.n)
			stream.write(XS_BON_BOOL | XS_BON_TRUE);
		else
			stream.write(XS_BON_BOOL | XS_BON_FALSE);
	}
#ifndef XS_NO_COMPLEX_DATA_TYPE
	else if (value->type == XS_VALUE_OBJECT)
	{
		WriteObject(stream, value);
	}
	else if (value->type == XS_VALUE_ARRAY)
	{
		WriteArray(stream, value);
	}
#endif
	else if (value->type == XS_VALUE_NULL)
	{
		stream.write(XS_BON_NULL);
	}
	else if (value->type == XS_VALUE_DOUBLE)
	{
		stream.write(XS_BON_DOUBLE);
		WriteInt64(stream, value->data.ptr);
	}
	else if (value->type == XS_VALUE_FLOAT)
	{
		stream.write(XS_BON_FLOAT);
		WriteInt32(stream, value->data.n);
	}
	else if (value->type == XS_VALUE_INT64)
	{
		stream.write(XS_BON_FLOAT);
		WriteInt64(stream, value->data.ptr);
	}
	else
	{
		stream.write(XS_BON_UNDEFINE);
	}

	return 0;
}

/**
 * seek a value by path
 * @param bon
 * @param length
 * @param value
 * @return
 */
int BonSeekValue(xsRandomAccessStream &stream, xsBool bigendian, const char *path)
{
	const char *name;
	const char *split;
	xsU32 hash = 0, salt = 0;
	int fieldsCount = 0, index = -1, ret = XS_EC_ERROR;
	xsBool hit = XS_FALSE;
	int lastContainer = 0;

	XS_TRACE("[BON]BonSeekValue");

	// Prepare name
	name = path;
	if (name == NULL)
		return XS_EC_OK;

	if (*name == '/')
		name++;
	if (*name == '\0')
		return XS_EC_OK;

	int type;
	unsigned char subtype;
	while ((type = stream.read()) > 0)
	{
		if ((xsU8)(type & ~XS_BON_OBJECT_MASK) == XS_BON_OBJECT ||
				(xsU8)(type & ~XS_BON_OBJCB_MASK) == XS_BON_OBJECT_COMBO)
		{// Object
			lastContainer = XS_BON_OBJECT;
			// Read properties count
			fieldsCount = 0;
			if ((xsU8)(type & ~XS_BON_OBJECT_MASK) == XS_BON_OBJECT)
			{// normal object
				subtype = type & XS_BON_OBJECT_MASK;
				subtype = 2 << subtype; // properties count in bytes

				// read properties count
				ret = ReadInt32(stream, subtype, bigendian, XS_FALSE, (xsS32 *)&fieldsCount);
			}
			else
			{// combo-object
				fieldsCount = (xsU8)(type & ~XS_BON_OBJECT_COMBO);
			}
		}
		else if ((xsU8)(type & ~XS_BON_ARRAY_MASK) == XS_BON_ARRAY ||
			(xsU8)(type & ~XS_BON_ARRCB_MASK) == XS_BON_ARRAY_COMBO)
		{// Array
			lastContainer = XS_BON_ARRAY;
			// Read properties count
			fieldsCount = 0;
			if ((xsU8)(type & ~XS_BON_ARRAY_MASK) == XS_BON_ARRAY)
			{// normal array
				subtype = type & XS_BON_ARRAY_MASK;
				subtype = 2 << subtype;

				ret = ReadInt32(stream, subtype, bigendian, XS_FALSE, (xsS32 *)&fieldsCount);
			}
			else
			{// Array combo
				fieldsCount = (xsU8)(type & ~XS_BON_ARRAY_COMBO);
			}
		}
		else if ((xsU8)(type & ~XS_BON_INDEX_MASK) == XS_BON_INDEX)
		{
			subtype = type & XS_BON_INDEX_MASK;
			if (subtype == XS_BON_INDEX_SALT)
			{// salt
				ret = ReadInt32(stream, 4, bigendian, XS_FALSE, (xsS32 *)&salt);
			}
			else
			{// index
				size_t size, offsetSize;
				int i;
				xsU32 propId = 0;

				// Field name hash
				split = xsStrChr(name, '/');
				if (split == NULL)
				{
					if (lastContainer == XS_BON_OBJECT)
						hash = xsBonHashName(name, (int)salt);
					else if (lastContainer == XS_BON_ARRAY)
						index = xsStrToInt(name);
					hit = XS_TRUE;
				}
				else
				{
					if (lastContainer == XS_BON_OBJECT)
						hash = xsBonHashNameN(name, split - name, salt);
					else if (lastContainer == XS_BON_ARRAY)
						index = xsStrToInt(name);
					name = split + 1;
				}
				
				// Find offset
				offsetSize = 1 << subtype; // one index in bytes

				if (lastContainer == XS_BON_OBJECT)
				{
					size = fieldsCount * (4 + offsetSize); // 4bytes hash + nbytes index
					long pos = stream.getPosition() + size;

					for (i = 0; i < fieldsCount; i++)
					{
						// read property id's hash
						ret = ReadInt32(stream, 4, bigendian, XS_FALSE, (xsS32 *)&propId);
						if (ret != XS_EC_OK)
							return ret;

						if (propId == hash)
						{// Jump to the property
							size_t offset = 0;
							// read offset
							ret = ReadInt32(stream, offsetSize, bigendian, XS_FALSE, (xsS32 *)&offset);
							if (ret != XS_EC_OK)
								return ret;

							// skip to property
							stream.setPosition(pos + offset);
							break;
						}
						stream.skip(offsetSize);
					}

					if (i == fieldsCount)
					{// field missed
						return XS_EC_NOT_EXISTS;
					}
				}
				else if (lastContainer == XS_BON_ARRAY)
				{
					if (index < 0 || index > fieldsCount)
						return XS_EC_NOT_EXISTS;

					size = fieldsCount * offsetSize;
					long pos = stream.getPosition() + size;
					size_t offset = 0;
					// read offset
					stream.skip(offsetSize * index);
					ret = ReadInt32(stream, offsetSize, bigendian, XS_FALSE, (xsS32 *)&offset);
					if (ret != XS_EC_OK)
						return ret;
					stream.setPosition(pos + offset);
				}
				else
				{
					return XS_EC_UNKNOWN_FIELD;
				}
			}
		}
		else if ((xsU8)(type & ~XS_BON_PROTOTYPE_MASK) == XS_BON_PROTOTYPE)
		{// prototype, skip
			subtype = type & XS_BON_PROTOTYPE_MASK;
			if (subtype == XS_BON_PROTOTYPE_HASH)
			{// skip hash
				stream.skip(sizeof(xsU32));
			}
			else
			{
				size_t strlen;
				ret = ReadLength(stream, bigendian, &strlen);
				if (ret == XS_EC_OK)
					stream.skip(strlen);
				else
					return ret;
			}
		}
		else
		{// Oops
			ret = XS_EC_UNKNOWN_FIELD;
			break;
		}

		if (hit)
		{
			return ret;
		}
	}

	return ret;
}

/**
 * Load BON from file
 */
int xsBonLoadFromFile(const xsTChar *filename, xsValue *value)
{
	xsFileStream stream;
	if (stream.open(filename, XS_OF_READONLY) != XS_EC_OK)
		return XS_EC_ERROR;

	int ret = XS_EC_ERROR;
	xsBool bigendian;
	if (BonCheckEndian(stream, &bigendian) == XS_EC_OK)
		ret = xsBonUnpack(stream, bigendian, value);
	stream.close();
	XS_TRACE("[BON]xsBONLoadFromFile: loaded value.type:%d", value->type);
	return ret;
}

/**
 * Save BON to file
 */
int xsBonSaveToFile(const xsTChar *filename, xsValue *value, xsBool indexed)
{
	xsFileStream stream;
	stream.open(filename, XS_OF_WRITEONLY | XS_OF_CREATE);
	
	if (xsIsBigEndian())
		stream.write("BON\0", 4);
	else
		stream.write("bon\0", 4);

	setPackFlag(indexed);

	xsBonPack(stream, value);

	stream.close();

	return XS_EC_OK;
}

static int BonCheckEndian(xsStream &stream, xsBool *bigendian)
{
	char header[4];

	size_t read = stream.read(header, 4);
	if (read < 4)
		return XS_EC_ERROR;

	if (xsStrCmpN((const char *)header, "bon", 4) == 0)
	{
		*bigendian = XS_FALSE;
	}
	else if (xsStrCmpN((const char *)header, "BON", 4) == 0)
	{
		*bigendian = XS_TRUE;
	}
	else
	{
		XS_ERROR("BON file malformed. Load failed."); // malformed file
		return XS_EC_ERROR;
	}

	return XS_EC_OK;
}

xsBool xsIsBigEndian(void)
{
	short n = 0x0100;
	return *((char*)(&n));
}

void setPackFlag(xsBool indexed)
{
	if(indexed)
	{
		flag |= FLAG_INDEXED;
	}

	if(xsStrCmp(charset, "utf8") == 0)
	{
		flag |= FLAG_USE_UTF8;
	}
	else if(xsStrCmp(charset, "native") == 0)
	{
		flag |= FLAG_USE_NATIVE;
	}
}

/**
* This function is based on OpenSSL lh_strhash. Don't forget say thanks to
* Eric Young (eay@cryptsoft.com)
* @param key
* @param salt
* @return
*/
xsU32 xsBonHashName(const char *name, int salt)
{
	return xsBonHashNameN(name, (size_t)-1, salt);
}

xsU32 xsBonHashNameN(const char *name, size_t len, int salt)
{
	const xsS8 *c = (const xsS8 *)name;
	xsU32 ret = 0;
	xsS32 n;
	xsU32 v;
	xsS32 r;
	int i = 0;

	if ((c == NULL) || (*c == '\0'))
		return(ret);

	n = 0x100;
	while (*c && (size_t)i < len)
	{
		v = n | (*c);
		n += 0x100;
		r = (int)((v >> 2) ^ v) & 0x0F;
		ret = (ret << r) | (ret >> (32 - r));
		ret &= 0xFFFFFFFFL;
		ret ^= v * v;
		c++;
		i++;
	}

	return ((ret >> 16) ^ ret);
}

int xsBonReadValueFromStream(xsRandomAccessStream &stream, const char *path, xsValue *value)
{
	int ret = XS_EC_ERROR;

	value->type = XS_VALUE_NONE;
	XS_TRACE("[BON]xsBonReadValueFromStream");

	xsBool bigendian;
	long pos = stream.getPosition();
	stream.setPosition(0);
	if (BonCheckEndian(stream, &bigendian) == XS_EC_OK)
	{
		if (BonSeekValue(stream, bigendian, path)  == XS_EC_OK)
		{
			ret = xsBonUnpack(stream, bigendian, value);
		}
	}
	stream.setPosition(pos);

	return ret;
}

/**
 * Open a file and read value specified by path
 * @return XS_EC_OK if read successfully. Other value indicates failure.
 */
int xsBonReadValueFromFile(const xsTChar *filename, const char *path, xsValue *value)
{
	xsFileStream stream;
	if (stream.open(filename, XS_OF_READONLY) != XS_EC_OK)
		return XS_EC_ERROR;

	int ret = xsBonReadValueFromStream(stream, path, value);

	XS_TRACE("[BON]xsBonReadValueFromFile: loaded value.type:%d", value->type);
	return ret;
}

/**
 * Open a file and read value specified by path
 * @return XS_EC_OK if read successfully. Other value indicates failure.
 */
int xsBonReadValueFromFd(xsFile handle, const char *path, xsValue *value)
{
	xsFileStream stream(handle);

	int ret = xsBonReadValueFromStream(stream, path, value);

	XS_TRACE("[BON]xsBonReadValueFromFile: loaded value.type:%d", value->type);
	return ret;
}

/**
 * Read value from resource.
 * @param handle
 * @param path
 * @param value
 * @return
 */
int xsBonReadValueFromRes(xsRes handle, const char *path, xsValue *value)
{
	xsResourceStream stream(handle);

	int ret = xsBonReadValueFromStream(stream, path, value);

	XS_TRACE("[BON]xsBonReadValueFromRes: loaded value.type:%d", value->type);
	return ret;
}

int xsBonCountItemInStream(xsRandomAccessStream &stream, const char *path)
{
	int ret = XS_EC_ERROR;
	int fieldsCount = 0;

	XS_TRACE("xsBonCountItemInStream");

	xsBool bigendian;
	long pos = stream.getPosition();
	stream.setPosition(0);
	if (BonCheckEndian(stream, &bigendian) == XS_EC_OK)
	{
		if (BonSeekValue(stream, bigendian, path)  == XS_EC_OK)
		{
			int type = stream.read();
			unsigned char subtype;

			if ((xsU8)(type & ~XS_BON_OBJECT_MASK) == XS_BON_OBJECT ||
				(xsU8)(type & ~XS_BON_OBJCB_MASK) == XS_BON_OBJECT_COMBO)
			{
				if ((xsU8)(type & ~XS_BON_OBJECT_MASK) == XS_BON_OBJECT)
				{
					subtype = type & XS_BON_OBJECT_MASK;
					subtype = 2 << subtype;

					ret = ReadInt32(stream, subtype, bigendian, XS_FALSE, (xsS32 *)&fieldsCount);
				}
				else
				{// Object combo
					fieldsCount = (xsU8)(type & ~XS_BON_OBJECT_COMBO);
					ret = XS_EC_OK;
				}
			}
			else if ((xsU8)(type & ~XS_BON_ARRAY_MASK) == XS_BON_ARRAY ||
				(xsU8)(type & ~XS_BON_ARRCB_MASK) == XS_BON_ARRAY_COMBO)
			{
				if ((xsU8)(type & ~XS_BON_ARRAY_MASK) == XS_BON_ARRAY)
				{
					subtype = type & XS_BON_ARRAY_MASK;
					subtype = 2 << subtype;

					ret = ReadInt32(stream, subtype, bigendian, XS_FALSE, (xsS32 *)&fieldsCount);
				}
				else
				{// Array combo
					fieldsCount = (xsU8)(type & ~XS_BON_ARRAY_COMBO);					
					ret = XS_EC_OK;
				}
			}
		}
	}
	stream.setPosition(pos);

	if (ret == XS_EC_OK)
		return fieldsCount;
	else
		return ret;
}

int xsBonCountItemInFile(const xsTChar *filename, const char *path)
{
	xsFileStream stream;
	if (stream.open(filename, XS_OF_READONLY) != XS_EC_OK)
		return XS_EC_ERROR;

	return xsBonCountItemInStream(stream, path);
}

int xsBonCountItemInFd(xsFile handle, const char *path)
{
	xsFileStream stream(handle);

	return xsBonCountItemInStream(stream, path);
}

int xsBonCountItemInRes(xsRes handle, const char *path)
{
	xsResourceStream stream(handle);

	return xsBonCountItemInStream(stream, path);
}

/**
 * Locate specified binary value in resource, return offset and size.
 * @param handle resource handle
 * @param path value's path
 * @param offset beginning offset in bytes
 * @param size value's length.
 * @return XS_EC_OK if locate successfully. otherwise return XS_EC_ERROR.
 */

int xsBonLocateBinaryInRes(xsRes handle, const char *path, size_t *offset, size_t *size)
{
	xsResourceStream stream(handle);

	return xsBonLocateBinaryInStream(stream, path, offset, size);
}

int xsBonLocateBinaryInFile(const xsTChar *filename, const char *path, size_t *offset, size_t *size)
{
	xsFileStream stream;
	if (stream.open(filename, XS_OF_READONLY) != XS_EC_OK)
		return XS_EC_ERROR;

	return xsBonLocateBinaryInStream(stream, path, offset, size);
}

int xsBonLocateBinaryInFd(xsFile fd, const char *path, size_t *offset, size_t *size)
{
	xsFileStream stream(fd);

	return xsBonLocateBinaryInStream(stream, path, offset, size);
}

int xsBonLocateBinaryInStream(xsRandomAccessStream &stream, const char *path, size_t *offset, size_t *size)
{
	int ret = XS_EC_ERROR;
	xsBool bigendian;
	
	long pos = stream.getPosition();
	stream.setPosition(0);
	if (BonCheckEndian(stream, &bigendian) == XS_EC_OK)
	{
		if (BonSeekValue(stream, bigendian, path)  == XS_EC_OK)
		{
			// get binary value's length and skip to data's beginning
			int type = stream.read();
			if ((xsU8)(type & ~XS_BON_BINARY_MASK) == XS_BON_BINARY)
			{
				int subtype = type & XS_BON_BINARY_MASK;
				subtype = 2 << subtype; // size bytes

				ret = ReadInt32(stream, subtype, bigendian, XS_FALSE, (xsS32 *)size);
				*offset = stream.getPosition();
			}
		}
	}
	stream.setPosition(pos);
	stream.close();
	return ret;
}

/**
 * parser BON data in buffer
 * @param buffer
 * @param length [IN|OUT]
 * @param handler
 * @param userdata
 * @return
 */
int xsBonParser(xsStream &stream, xsBool bigendian, xsBonHandler handler, void *userdata)
{
	xsValue value;
	int fieldsCount = 0;
	xsBool expectArrayIndex = XS_FALSE;
	int ret = 0;

	XS_TRACE_DEBUG("[BON]xsBONParser");
	int type;
	unsigned char subtype;
	while ((type = stream.read()) >= 0)
	{
		XS_TRACE_DEBUG("[BON]xsBONParser: type:%02X", type);

		value.type = XS_VALUE_NONE;
		value.data.ptr = 0;
		ret = XS_EC_OK;

		if ((xsU8)type == XS_BON_NULL)
		{
			value.type = XS_VALUE_NULL;
		}
		else if ((xsU8)type == XS_BON_UNDEFINE)
		{
			value.type = XS_VALUE_UNDEFINE;
		}
		else if ((xsU8)(type & ~XS_BON_BOOL_MASK) == XS_BON_BOOL)
		{
			value.type = XS_VALUE_BOOL;
			value.data.n = type & XS_BON_BOOL_MASK;
		}
		else if ((type | XS_BON_UINT_COMBO) == XS_BON_UINT_COMBO ||
			(xsU8)(type & XS_BON_MINT_COMBO) == XS_BON_MINT_COMBO)
		{
			value.type = XS_VALUE_INT32;
			value.data.n = (signed char)(type & 0xFF);
		}
		else if ((xsU8)(type & ~XS_BON_INT_MASK) == XS_BON_INT ||
			(xsU8)(type & ~XS_BON_UINT_MASK) == XS_BON_UINT)
		{
			xsBool sign;

			subtype = type & XS_BON_INT_MASK;
			subtype = 1 << subtype; // bytes per number

			if ((xsU8)(type & ~XS_BON_INT_MASK) == XS_BON_INT)
				sign = XS_TRUE;
			else
				sign = XS_FALSE;
		
			if (subtype <= 4)
			{// Int32
				value.type = XS_VALUE_INT32;
				ret = ReadInt32(stream, subtype, bigendian, sign, &value.data.n);
			}
			else
			{// Int64
				value.type = XS_VALUE_INT64;
				ret = ReadInt64(stream, subtype, sign, &value.data.ptr);
			}
		}
		else if ((xsU8)(type & ~XS_BON_STRING_MASK) == XS_BON_STRING)
		{
			subtype = type & XS_BON_STRING_MASK;
			size_t strlen;
			ret = ReadLength(stream, bigendian, &strlen);
			if (ret == XS_EC_OK)
			{
				char *str = (char *)xsMalloc(strlen);
				size_t size = stream.read(str, strlen);
				if (size == strlen)
				{
					if (subtype == XS_BON_TEXT_UCS2)
						value.type = XS_VALUE_TEXT;
					else if (subtype == XS_BON_STRING_ANSI)
						value.type = XS_VALUE_STRING;
					else
						value.type = XS_VALUE_TEXT;
#ifdef XS_UNICODE
					if (subtype == XS_BON_TEXT_UTF8)
					{// convert UTF-8 to UCS-2
						xsTChar *tstr = xsUtf8ToTcsDup(str);
						xsFree(str);
						str = (char *)tstr;
					}
#endif
					value.data.ptr = str;
				}
				else
				{// malformed
					xsFree(str);
					ret = XS_EC_ERROR;
				}
			}
		}
		else if ((xsU8)(type & ~XS_BON_BINARY_MASK) == XS_BON_BINARY)
		{
			size_t size = 0;

			value.type = XS_VALUE_BINARY;
			subtype = type & XS_BON_BINARY_MASK;
			subtype = 2 << subtype; // size bytes

			ret = ReadInt32(stream, subtype, bigendian, XS_FALSE, (xsS32 *)&size);
			if (ret == XS_EC_OK)
			{
				value.data.ptr = xsMalloc(size + sizeof(size));
				*((size_t *)value.data.ptr) = size;
				if (value.data.ptr != NULL)
				{
					size_t read = stream.read((char *)value.data.ptr + sizeof(size), size);
					if (read != size)
						ret = XS_EC_ERROR;
				}
				else
				{
					ret = XS_EC_NOMEM;
				}
			}
		}
		else if ((xsU8)(type & ~XS_BON_INDEX_MASK) == XS_BON_INDEX)
		{
			subtype = type & XS_BON_INDEX_MASK;
			if (subtype == XS_BON_INDEX_SALT)
			{
				value.type = XS_VALUE_SALT;
				ret = ReadInt32(stream, 4, bigendian, XS_FALSE, &value.data.n);
			}
			else
			{
				size_t size = 0;

				value.type = XS_VALUE_INDEX;
				if (expectArrayIndex)
					size = fieldsCount * (1 << subtype); // nbytes index
				else
					size = fieldsCount * (4 + (1 << subtype)); // 4bytes hash + nbytes index
				fieldsCount = 0; // clear
				expectArrayIndex = XS_FALSE;

				stream.skip(size); // simply skip index
			}
		}
		else if ((xsU8)(type & ~XS_BON_ARRAY_MASK) == XS_BON_ARRAY ||
			(xsU8)(type & ~XS_BON_ARRCB_MASK) == XS_BON_ARRAY_COMBO)
		{
			fieldsCount = 0;
			if ((xsU8)(type & ~XS_BON_ARRAY_MASK) == XS_BON_ARRAY)
			{
				subtype = type & XS_BON_ARRAY_MASK;
				subtype = 2 << subtype;

				ret = ReadInt32(stream, subtype, bigendian, XS_FALSE, (xsS32 *)&fieldsCount);
			}
			else
			{// Array combo
				fieldsCount = (xsU8)(type & ~XS_BON_ARRAY_COMBO);
			}

			value.type = XS_VALUE_ARRAY;
			expectArrayIndex = XS_TRUE;
			value.data.n = fieldsCount;
		}
		else if ((xsU8)(type & ~XS_BON_OBJECT_MASK) == XS_BON_OBJECT ||
			(xsU8)(type & ~XS_BON_OBJCB_MASK) == XS_BON_OBJECT_COMBO)
		{
			fieldsCount = 0;
			if ((xsU8)(type & ~XS_BON_OBJECT_MASK) == XS_BON_OBJECT)
			{
				subtype = type & XS_BON_OBJECT_MASK;
				subtype = 2 << subtype;

				ret = ReadInt32(stream, subtype, bigendian, XS_FALSE, (xsS32 *)&fieldsCount);
			}
			else
			{// Object combo
				fieldsCount = (xsU8)(type & ~XS_BON_OBJECT_COMBO);
			}

			value.type = XS_VALUE_OBJECT;
			expectArrayIndex = XS_FALSE;
			value.data.n = fieldsCount;
		}
		else if ((xsU8)(type & ~XS_BON_PROTOTYPE_MASK) == XS_BON_PROTOTYPE)
		{
			subtype = type & XS_BON_PROTOTYPE_MASK;
			if (subtype == XS_BON_PROTOTYPE_HASH)
			{
				value.type = XS_VALUE_PROTOTYPE_HASH;
				ret = ReadInt32(stream, 4, bigendian, XS_FALSE, (xsS32 *)&value.data.n);
			}
			else
			{
				size_t strlen;
				ret = ReadLength(stream, bigendian, &strlen);
				if (ret == XS_EC_OK)
				{
					char *str = (char *)xsMalloc(strlen);
					size_t size = stream.read(str, strlen);
					if (size == strlen)
					{
						value.type = XS_VALUE_PROTOTYPE_STRING;
#ifdef XS_UNICODE
						if (subtype == XS_BON_TEXT_UTF8)
						{// convert UTF-8 to UCS-2
							xsTChar *tstr = xsUtf8ToTcsDup(str);
							xsFree(str);
							str = (char *)tstr;
						}
#endif
						value.data.ptr = str;
					}
					else
					{
						ret = XS_EC_ERROR;
					}
				}
			}
		}
		else
		{
			ret = XS_EC_UNKNOWN_FIELD;
		}

		XS_TRACE_DEBUG("[BON]xsBONParser: code:%d", ret);
		XS_TRACE_DEBUG("[BON]xsBONParser: value.type:%d, value:%08X", value.type, value.data.n);
		if (ret == XS_EC_OK)
		{
			int ind;

			if (value.type == XS_VALUE_INDEX)
				continue;

			ind = handler(&value, userdata);

			if (ind == XS_EC_ERROR)
			{
				return XS_EC_USER_BREAK;
			}
			else if (ind > 0)
			{
				break;
			}
		}
		else
		{// failed
			XS_ERROR("[BON]xsBONParser: failed. code:%d", ret);

			return ret;
		}
	}
	
	return 0;
}

static int ReadLength(xsStream &stream, xsBool bigendian, size_t *length)
{
	int type = stream.read();
	if (type < 0)
		return XS_EC_ERROR;

	if ((type | XS_BON_UINT_COMBO) == XS_BON_UINT_COMBO)
	{
		*length = type;
	}
	else if ((xsU8)(type & ~XS_BON_INT_MASK) == XS_BON_INT ||
			(xsU8)(type & ~XS_BON_UINT_MASK) == XS_BON_UINT)
	{
		xsBool sign;
		int subtype = type & XS_BON_INT_MASK;
		subtype = 1 << subtype; // bytes per number

		if ((xsU8) (type & ~XS_BON_INT_MASK) == XS_BON_INT)
			sign = XS_TRUE;
		else
			sign = XS_FALSE;

		if (subtype <= 4)
		{// support up to Int32
			if (ReadInt32(stream, subtype, bigendian, sign, (xsS32 *)length) != XS_EC_OK)
				return XS_EC_ERROR;
		}
		else
		{
			return XS_EC_NOT_SUPPORT;
		}
	}

	return XS_EC_OK;
}

static int ReadInt32(xsStream &stream, size_t bytes, xsBool bigendian, xsBool sign, xsS32 *value)
{
	int val;

	size_t read = stream.read(&val, bytes);
	if (read != bytes)
		return XS_EC_ERROR;

	*value = 0;
	char *data = (char *)&val;
	if (bigendian)
	{
		if (bytes == 1)
		{
			*value |= (*data++) & 0xFF;
		}
		else if (bytes == 2)
		{
			*value |= (*(data++) << 8) & 0xFF00;
			*value |= (*data++) & 0xFF;
		}
		else // 4bytes
		{
			*value |= (*(data++) << 24) & 0xFF000000;
			*value |= (*(data++) << 16) & 0xFF0000;
			*value |= (*(data++) << 8) & 0xFF00;
			*value |= (*data++) & 0xFF;
		}
	}
	else
	{
		if (bytes == 1)
		{
			*value |= (*data++) & 0xFF;
		}
		else if (bytes == 2)
		{
			*value |= (*data++) & 0xFF;
			*value |= (*(data++) << 8) & 0xFF00;
		}
		else // 4bytes
		{
			*value |= (*data++) & 0xFF;
			*value |= (*(data++) << 8) & 0xFF00;
			*value |= (*(data++) << 16) & 0xFF0000;
			*value |= (*(data++) << 24) & 0xFF000000;
		}
	}

	if (sign)
	{
		if (bytes == 1 && (*value & 0x80))
			*value |= 0xFFFFFF00;
		else if (bytes == 2 && (*value & 0x8000))
			*value |= 0xFFFF0000;
	}

	return XS_EC_OK;
}

static int ReadInt64(xsStream &stream, size_t bytes, xsBool sign, void **value)
{
	*value = xsMalloc(bytes);
	if (*value == NULL)
		return XS_EC_NOMEM;

	size_t read = stream.read(*value, bytes);
	if (read != bytes)
	{
		xsFree(*value);
		return XS_EC_ERROR;
	}
	
	return XS_EC_OK;
}

static int ValueHandler(xsValue *value, void *userdata)
{
	xsParseData *data = (xsParseData *)userdata;
	XS_ASSERT(value != NULL && userdata != NULL);
	XS_TRACE_DEBUG("[BON]ValueHandler");

	if (data->head != NULL && data->head->fields == 0 &&
		value->type != XS_VALUE_PROTOTYPE_HASH &&
		value->type != XS_VALUE_PROTOTYPE_STRING)
	{
		return 1; // Object full
	}

	switch (value->type)
	{
	case XS_VALUE_NULL:
	case XS_VALUE_UNDEFINE:
	case XS_VALUE_BOOL:
	case XS_VALUE_INT32:
	case XS_VALUE_INT64:
	case XS_VALUE_FLOAT:
	case XS_VALUE_DOUBLE:
	case XS_VALUE_STRING:
	case XS_VALUE_TEXT:
	case XS_VALUE_BINARY:
		if (data->head == NULL)
		{
			data->head = (xsParseNode *)xsCalloc(sizeof(xsParseNode));
			if (data->head == NULL)
				return XS_EC_ERROR;
			data->head->container = *value;
			return 1; // complete
		}
		else if (data->head->container.type == XS_VALUE_OBJECT && data->head->lastKey.type == XS_VALUE_NONE)
		{// it's a key
			XS_TRACE_DEBUG("[BON]ValueHandler: key");
			if (value->type != XS_VALUE_INT32 && value->type != XS_VALUE_STRING)
				return XS_EC_ERROR; // TODO: Handle OTHER key type
			data->head->lastKey = *value;
		}
		else
		{// Value
			XS_TRACE_DEBUG("[BON]ValueHandler: value");
#ifndef XS_NO_COMPLEX_DATA_TYPE
			if (data->head->container.type == XS_VALUE_OBJECT)
			{// Add as object's field
				xsBool set = XS_FALSE;
				if (data->head->container.data.ptr == NULL)
				{
					data->head->container.data.ptr = xsObject::createInstance();
					data->head->container.data.obj->allocFields(data->head->fields);
				}

//				if (value->type == XS_VALUE_OBJECT)
//					value->data.obj->setId((xsU32)data->head->key.data.n);
				if (data->head->lastKey.type == XS_VALUE_INT32)
					set = data->head->container.data.obj->setProperty(data->head->lastKey.data.n, value);
				else if (data->head->lastKey.type == XS_VALUE_STRING)
					set = data->head->container.data.obj->setProperty(data->head->lastKey.data.s, value);
				if (!set)
				{
					if (data->head->lastKey.type == XS_VALUE_INT32)
						XS_ERROR("[BON]Object has no such property: ID:%08X", data->head->lastKey.data.n);
					else if (data->head->lastKey.type == XS_VALUE_STRING)
						XS_ERROR("[BON]Object has no such property: %s", data->head->lastKey.data.s);
					else
						XS_ERROR("[BON]Unknown object's property type.");
					xsValueDestroy(value, XS_FALSE);
				}
			}
			else // container->type == XS_VALUE_ARRAY
			{// Add as array's element
				if (data->head->container.data.ptr == NULL)
					data->head->container.data.ptr = xsArrayCreate(data->head->fields);
				xsArrayAdd(data->head->container.data.array, value);
			}
#endif
			// Completion, clear last key and decrease fields count
			data->head->lastKey.type = XS_VALUE_NONE;
			data->head->fields--;
		}
		break;
#ifndef XS_NO_COMPLEX_DATA_TYPE
	case XS_VALUE_OBJECT:
	case XS_VALUE_ARRAY:
		if (data->head != NULL)
		{
			xsStackPush(data->stack, data->head);
			XS_TRACE_DEBUG("[BON]ValueHandler: Push stack: key:%08X, container:%08X", data->head->lastKey.data.n, data->head->container.data.ptr);
		}
		XS_TRACE_DEBUG("[BON]ValueHandler: New container");
		data->head = (xsParseNode *)xsCalloc(sizeof(xsParseNode));
		if (data->head == NULL)
			return XS_EC_ERROR;
		data->head->container = *value;
		data->head->fields = data->head->container.data.n;
		data->head->container.data.ptr = NULL; // Clear fields count
		if (value->type == XS_VALUE_OBJECT && data->head->fields == 0)
			return XS_EC_OK; // Avoid empty object pop before set prototype
		break;

	//case XS_VALUE_PROTOTYPE_HASH:
	//	data->head->container.data.ptr = xsObjectCreate(value->data.n);
	//	if (data->head->container.data.ptr == NULL)
	//		return XS_EC_ERROR;
	//	break;

	case XS_VALUE_PROTOTYPE_STRING:
		data->head->container.data.ptr = xsObjectCreate(value->data.s);
		if (data->head->container.data.ptr == NULL)
			return XS_EC_ERROR;
		break;

	case XS_VALUE_SALT:
		if (data->head->container.data.ptr == NULL)
		{
			data->head->container.data.ptr = xsObject::createInstance();
			data->head->container.data.obj->allocFields(data->head->fields);
		}
//XXX		XS_OBJ_SETTER(data->head->container->value.ptr, XS_PROP_OBJECT_SALT, value);
		break;
#endif
	case XS_VALUE_INDEX:
		break;

	case XS_VALUE_NONE:
	default:
		break;
	}

	// Pop full object
	while (data->head != NULL && data->head->fields == 0)
	{// fields already filled
		if (!xsStackEmpty(data->stack))
		{// Add to container
			xsBool set = XS_FALSE;
			xsParseNode *node = (xsParseNode *)xsStackPop(data->stack);
			XS_TRACE_DEBUG("[BON]ValueHandler: Pop stack: key:%08X, container:%08X", node->lastKey.data.n, node->container.data.ptr);

#ifndef XS_NO_COMPLEX_DATA_TYPE
			if (node->container.type == XS_VALUE_OBJECT)
			{// Add as object's field
				if (node->container.data.ptr == NULL)
				{// None-classed object
					node->container.data.ptr = xsObject::createInstance();
					node->container.data.obj->allocFields(node->fields);
				}
				
				//if (data->head->container.type == XS_VALUE_OBJECT)
				//	data->head->container.data.obj->setId((xsU32)node->key.data.n);
				if (node->lastKey.type == XS_VALUE_INT32)
					set = node->container.data.obj->setProperty(node->lastKey.data.n, &data->head->container);
				else if (node->lastKey.type == XS_VALUE_STRING)
					set = node->container.data.obj->setProperty(node->lastKey.data.s, &data->head->container);
				if (!set)
				{
					if (node->lastKey.type == XS_VALUE_INT32)
						XS_ERROR("[BON]Object has no such property: ID:%08X", node->lastKey.data.n);
					else if (node->lastKey.type == XS_VALUE_STRING)
						XS_ERROR("[BON]Object has no such property: %s", node->lastKey.data.s);
					else
						XS_ERROR("[BON]Unknown object's property type.");
					xsValueDestroy(&data->head->container, XS_FALSE);
				}
			}
			else
			{// Add as array's element
				if (node->container.data.ptr == NULL)
					node->container.data.ptr = xsArrayCreate(node->fields);
				xsArrayAdd(node->container.data.array, &data->head->container);
			}
			// Free head's values
			xsValueDestroy(&data->head->lastKey, XS_FALSE);
			xsFree(data->head);
#endif
			// Completion
			data->head = node;
			data->head->lastKey.type = XS_VALUE_NONE;
			data->head->fields--;
		}
		else
		{// stack empty
			break;
		}
	}

	return XS_EC_OK;
}

static void WriteInt16(xsStream &stream, xsS16 n)
{
	stream.write(&n, sizeof(xsS16));
}

static void WriteInt32(xsStream &stream, xsS32 n)
{
	stream.write(&n, sizeof(xsS32));
}

static void WriteInt64(xsStream &stream, void *ptr)
{
	stream.write(ptr, 8); // 64bits is 8bytes
}

#ifndef XS_NO_COMPLEX_DATA_TYPE
static void WriteObject(xsStream &stream, xsValue *value)
{
	xsIterator iter = NULL;
	xsObject *obj = value->data.obj;
	xsU32 key;
	xsValue *val;
	xsValue tmpVal = {XS_VALUE_NONE, 0};
	int size = 0;
	xsBool ret;
	xsU8 type;
	char name[20] = {0};

	if (obj == NULL)
	{
		stream.write(XS_BON_NULL);
		return;
	}

	// TODO: write native properties
	// Write ZERO field header
	if (obj->getProperties() == NULL)
	{
		stream.write(XS_BON_OBJECT_COMBO | (xsU8)size);
		return;
	}

	// TODO: write index
	// Write headers
	type = XS_BON_OBJECT;
	size = xsHashMapSize(obj->getProperties());
	if(obj->getProperty("id", &tmpVal))
	{
		if(tmpVal.data.s != NULL)
		{
			size += 1;
		}
	}

	if (size < 32)
	{
		stream.write(XS_BON_OBJECT_COMBO | (xsU8)size);
	}
	else if (size < 65536)
	{
		stream.write(type | XS_BON_OBJECT16);
		WriteInt16(stream, (xsS16)size);
	}
	else
	{
		stream.write(type | XS_BON_OBJECT32);
		WriteInt32(stream, size);
	}

	if(obj->getProperty("prototype", &tmpVal))
	{
		if(tmpVal.type == XS_VALUE_STRING)
		{
			stream.write(XS_BON_PROTOTYPE | XS_BON_PROTOTYPE_STRING);
			stream.write(xsStrLen(tmpVal.data.s) + 1);
			stream.write(tmpVal.data.s, xsStrLen(tmpVal.data.s) + 1);
		}
		else if(tmpVal.type == XS_VALUE_UINT32)
		{
			stream.write(XS_BON_PROTOTYPE | XS_BON_PROTOTYPE_HASH);
			WriteInt32(stream, tmpVal.data.n);
		}
	}

	if((flag & FLAG_INDEXED) != FLAG_INDEXED)
	{
		if(obj->getProperty("id", &tmpVal))
		{
			if(NULL != tmpVal.data.s)
			{
				if((flag & FLAG_HASHED) == FLAG_HASHED)
				{
					stream.write(XS_BON_INT | XS_BON_UINT32);
					WriteInt32(stream, obj->getPropertyId("id"));
				}
				else
				{
					stream.write(XS_BON_STRING | XS_BON_STRING_ANSI);
					stream.write(xsStrLen("id") + 1);
					stream.write("id", xsStrLen("id") + 1);
				}
				xsBonPack(stream, &tmpVal);
			}
		}

		// write dynamic properties
		for (;;)
		{
			ret = xsHashMapIterateEntry(obj->getProperties(), &iter, (int *)&key, name, (void **)&val);
			if (!ret || iter == NULL || val == NULL)
				break;

			// write key
			if((flag & FLAG_HASHED) == FLAG_HASHED)
			{
				stream.write(XS_BON_UINT | XS_BON_UINT32);
				WriteInt32(stream, key);
			}
			else
			{
				stream.write(XS_BON_STRING | XS_BON_STRING_ANSI);
				stream.write(xsStrLen(name) + 1);
				stream.write(name, xsStrLen(name) + 1);
			}
			printf("name = %s\n", name);
			// write value
			xsBonPack(stream, val);
		}
	}
	else
	{
		int salt = 0, retry = 0, index = 0;
		bool collisionFlag = false;
		long *offsets = (long *)xsCalloc(size * sizeof(long));
		int *hashSet= (int *)xsCalloc(size * sizeof(int));
		if(offsets == NULL || hashSet == NULL)
		{
			printf("calloc failed!");
			return;
		}
		xsBufferStream buffer;
RESTART:
		xsMemSet(offsets, 0, size * sizeof(long));
		xsMemSet(offsets, 0, size * sizeof(int));
		if(obj->getProperty("id", &tmpVal))
		{
			if(NULL != tmpVal.data.s)
			{
				if((flag & FLAG_HASHED) == FLAG_HASHED)
				{
					buffer.write(XS_BON_INT | XS_BON_UINT32);
					WriteInt32(buffer, obj->getPropertyId("id"));
				}
				else
				{
					buffer.write(XS_BON_STRING | XS_BON_STRING_ANSI);
					buffer.write(xsStrLen("id") + 1);
					buffer.write("id", xsStrLen("id") + 1);
				}
				offsets[index] = buffer.getSize();
				index++;
				xsBonPack(buffer, &tmpVal);
			}
		}

		for (;;)
		{
			ret = xsHashMapIterateEntry(obj->getProperties(), &iter, (int *)&key, name, (void **)&val);
			if (!ret || iter == NULL || val == NULL)
				break;

			for(int i = 0; i < size; i++)
			{
				if(hashSet[i] == key)
				{
					printf("Object fields hash collision: [%s]\n", name);
					salt = rand();
					iter = NULL;
					index = 1;
					for(int i = 0; i < size; i++)
					{
						offsets[i] = 0;
						hashSet[i] = 0;
					}
					retry++;
					collisionFlag = true;
					if(retry > MAX_SALT_RETRY)
					{
						printf("Cannot find valid salt!");
						abort();
					}
					break;
				}
			}
			if(collisionFlag)
			{
				collisionFlag = false;
				buffer.clear();
				goto RESTART;
			}
			// write key
			if((flag & FLAG_HASHED) == FLAG_HASHED)
			{
				buffer.write(XS_BON_UINT | XS_BON_UINT32);
				WriteInt32(buffer, key);
			}
			else
			{
				buffer.write(XS_BON_STRING | XS_BON_STRING_ANSI);
				buffer.write(xsStrLen(name) + 1);
				buffer.write(name, xsStrLen(name) + 1);
			}
			offsets[index] = buffer.getSize();
			index++;
			// write value
			xsBonPack(buffer, val);
		}
		if(salt != 0)
		{
			stream.write(XS_BON_INDEX | XS_BON_INDEX_SALT);
			WriteInt32(stream, salt);
		}

		int bufferSize = buffer.getSize();
		int type = XS_BON_INDEX;
		if(bufferSize <= 255)
		{
			type |= XS_BON_INDEX8;
		}
		else if(bufferSize <= 65535)
		{
			type |= XS_BON_INDEX16;
		}
		else
		{
			type |= XS_BON_INDEX32;
		}
		stream.write(type);

		WriteInt32(stream, xsBonHashName("id", salt));
		if(type == (XS_BON_INDEX | XS_BON_INDEX8))
		{
			stream.write(offsets[0]);
		}
		else if(type == (XS_BON_INDEX | XS_BON_INDEX16))
		{
			WriteInt16(stream, offsets[0]);
		}
		else
		{
			WriteInt32(stream, offsets[0]);
		}

		iter = NULL;
		index = 1;
		for (;;)
		{
			ret = xsHashMapIterateEntry(obj->getProperties(), &iter, (int *)&key, name, (void **)&val);
			if (!ret || iter == NULL || val == NULL)
				break;

			WriteInt32(stream, xsBonHashName(name, salt));
			if(type == (XS_BON_INDEX | XS_BON_INDEX8))
			{
				stream.write(offsets[index]);
			}
			else if(type == (XS_BON_INDEX | XS_BON_INDEX16))
			{
				WriteInt16(stream, offsets[index]);
			}
			else
			{
				WriteInt32(stream, offsets[index]);
			}
			index++;
		}

		xsFree(offsets);
		xsFree(hashSet);
		stream.write(buffer.getData(), buffer.getSize());
		buffer.close();
	}
}

static void WriteArray(xsStream &stream, xsValue *value)
{
	xsArray *ary = value->data.array;
	xsIterator iter = NULL;
	xsValue *val;
	int size = 0;
	xsU8 type;

	if (ary == NULL)
	{
		stream.write(XS_BON_NULL);
		return;
	}

	size = xsArraySize(ary);
	// TODO: write index
	// Write headers
	type = XS_BON_ARRAY;
	if (size < 32)
	{
		stream.write(XS_BON_ARRAY_COMBO | (xsU8)size);
	}
	else if (size < 65536)
	{
		stream.write(type | XS_BON_ARRAY16);
		WriteInt16(stream, (xsS16)size);
	}
	else
	{
		stream.write(type | XS_BON_ARRAY32);
		WriteInt32(stream, size);
	}

	// write elements
	for (;;)
	{
		val = xsArrayIterate(ary, &iter);
		if (iter == NULL || val == NULL)
			break;

		// write value
		xsBonPack(stream, val);
	}
}
#endif
