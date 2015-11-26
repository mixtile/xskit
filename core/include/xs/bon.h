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

#ifndef _XS_BON_H_
#define _XS_BON_H_

#include <xs/pal.h>
#include <xs/stream.h>
#include <xs/value.h>

#ifdef __cplusplus
extern "C" {
#endif

enum
{
	XS_BON_NULL				= 0xA0,
	XS_BON_UNDEFINE			= 0xA1,
	
	XS_BON_BOOL				= 0xA2,	// 0xA2 - 0xA3
	XS_BON_BOOL_MASK		= 0x01,
	XS_BON_FALSE			= 0x00,
	XS_BON_TRUE				= 0x01,
	
	XS_BON_STRING			= 0xA4,	// 0xA4 - 0xA7
	XS_BON_STRING_MASK		= 0x03,	
	XS_BON_STRING_ANSI		= 0x00,	// ANSI string
	XS_BON_TEXT_NCS			= 0x01,	// Native CharSet
	XS_BON_TEXT_UCS2		= 0x02, // UCS-2
	XS_BON_TEXT_UTF8		= 0x03,	// UTF-8
	
	XS_BON_INT				= 0xA8, // 0xA8 - 0xAB
	XS_BON_INT_MASK			= 0x03,
	XS_BON_INT8				= 0x00,	// 8-bits
	XS_BON_INT16			= 0x01,	// 16-bits
	XS_BON_INT32			= 0x02, // 32-bits
	XS_BON_INT64			= 0x03, // 64-bits
	
	XS_BON_UINT				= 0xAC, // 0xAC - 0xAF
	XS_BON_UINT_MASK		= 0x03,
	XS_BON_UINT8			= 0x00,	// 8-bits
	XS_BON_UINT16			= 0x01,	// 16-bits
	XS_BON_UINT32			= 0x02, // 32-bits
	XS_BON_UINT64			= 0x03, // 64-bits

	XS_BON_FLOAT			= 0xB0,	// 32bits float
	XS_BON_DOUBLE			= 0xB1,	// 64bits float

	// --- RESERVE 0xB2 0xB3 ---
	
	XS_BON_BINARY			= 0xB4,	// 0xB4 - 0xB5
	XS_BON_BINARY_MASK		= 0x01,
	XS_BON_BINARY16			= 0x00,	// < 65536 bytes
	XS_BON_BINARY32			= 0x01,	// < UINT32 max

	XS_BON_ARRAY			= 0xB6,	// 0xB6 - 0xB7
	XS_BON_ARRAY_MASK		= 0x01,
	XS_BON_ARRAY16			= 0x00,	// < 65536
	XS_BON_ARRAY32			= 0x01,	// < UINT32 max

	XS_BON_OBJECT			= 0xB8,	// 0xB8 - 0xB9
	XS_BON_OBJECT_MASK		= 0x01,
	XS_BON_OBJECT16			= 0x00,	// < 65536
	XS_BON_OBJECT32			= 0x01,	// < UINT32 max

	XS_BON_PROTOTYPE		= 0xBA,	// 0xBA - 0xBB
	XS_BON_PROTOTYPE_MASK	= 0x01,
	XS_BON_PROTOTYPE_HASH	= 0x00,	// 32bits Index
	XS_BON_PROTOTYPE_STRING	= 0x01,	// name
	
	XS_BON_INDEX			= 0xBC,	// 0xBC - 0xBF
	XS_BON_INDEX_MASK		= 0x03,	//
	XS_BON_INDEX8			= 0x00,	// < 256 bytes
	XS_BON_INDEX16			= 0x01,	// < 65536
	XS_BON_INDEX32			= 0x02,	// < UINT32 max
	XS_BON_INDEX_SALT		= 0x03,	// 4bytes salt
	
	// Combo value is combine type and value or value length
	XS_BON_UINT_COMBO		= 0x7F,	// 0x00 - 0x7F
	XS_BON_MINT_COMBO		= 0xE0,	// 0xE0(-32) - 0xFF

	XS_BON_ARRAY_COMBO		= 0xC0,	// 0xA0 - 0xBF
	XS_BON_ARRCB_MASK		= 0x1F,	// 0xA0 - 0xBF

	XS_BON_OBJECT_COMBO		= 0x80,	// 0x80 - 0x9F
	XS_BON_OBJCB_MASK		= 0x1F
};

typedef int (*xsBonHandler)(xsValue *value, void *userdata);

xsBool xsIsBigEndian(void);
xsU32 xsBonHashName(const char *key, int salt);
xsU32 xsBonHashNameN(const char *name, size_t len, int salt);

int xsBonParser(xsStream &stream, xsBool bigendian, xsBonHandler handler,
		void *userdata);
int xsBonUnpack(xsStream &stream, xsBool bigendian, xsValue *value);

int xsBonLoadFromFile(const xsTChar *filename, xsValue *value);
int xsBonSaveToFile(const xsTChar *filename, xsValue *value);

int xsBonReadValueFromStream(xsRandomAccessStream &stream, const char *path, xsValue *value);
int xsBonReadValueFromFile(const xsTChar *filename, const char *path, xsValue *value);
int xsBonReadValueFromFd(xsFile handle, const char *path, xsValue *value);
int xsBonReadValueFromRes(xsRes handle, const char *path, xsValue *value);

int xsBonCountItemInStream(xsRandomAccessStream &stream, const char *path);
int xsBonCountItemInFile(const xsTChar *filename, const char *path);
int xsBonCountItemInFd(xsFile handle, const char *path);
int xsBonCountItemInRes(xsRes handle, const char *path);

int xsBonLocateBinaryInStream(xsRandomAccessStream &stream, const char *path, size_t *offset, size_t *size);
int xsBonLocateBinaryInFile(const xsTChar *filename, const char *path, size_t *offset, size_t *size);
int xsBonLocateBinaryInFd(xsFile fd, const char *path, size_t *offset, size_t *size);
int xsBonLocateBinaryInRes(xsRes handle, const char *path, size_t *offset, size_t *size);

#ifdef __cplusplus
}
#endif

#endif /* _XS_BON_H_ */
