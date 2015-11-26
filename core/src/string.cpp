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

#include <xs/pal.h>
#include <xs/memory.h>
#include <xs/string.h>

static const xsU8 g_xsBytesPerUtf8Char[16] = 
{
	1, 1, 1, 1, 1, 1, 1, 1,
	0, 0, 0, 0, 2, 2, 3, 4
};

char *xsStrDup(const char *string)
{
	char *ret;

	if (string == NULL)
		return NULL;

	ret = (char *)xsMalloc(xsStrLen(string) + sizeof(char));
	if (ret == NULL)
		return NULL;
	xsStrCpy(ret, string);
	return ret;
}

char *xsStrDupN(const char *string, size_t max)
{
	char *ret;
	size_t len;
	
	if (string == NULL)
		return NULL;
		
	len = xsStrLen(string);
	if (len > max)
		len = max;

	ret = (char *)xsMalloc(len + sizeof(char));
	if (ret == NULL)
		return NULL;
	xsStrCpyN(ret, string, len);
	ret[len] = 0;

	return ret;
}

xsWChar *xsWcsDup(const xsWChar *string)
{
	xsWChar *ret;

	if (string == NULL)
		return NULL;

	ret = (xsWChar *)xsMalloc((xsWcsLen(string) + 1) * sizeof(xsWChar));
	if (ret == NULL)
		return NULL;
	xsWcsCpy(ret, string);
	return ret;
}

xsWChar *xsWcsDupN(const xsWChar *string, size_t max)
{
	xsWChar *ret;
	size_t len;
	
	if (string == NULL)
		return NULL;
		
	len = xsWcsLen(string);
	if (len > max)
		len = max;

	ret = (xsWChar *)xsMalloc((len + 1) * sizeof(xsWChar));
	if (ret == NULL)
		return NULL;
	xsWcsCpyN(ret, string, len);
	ret[len] = 0;

	return ret;
}

char *xsStrAppend(char *string, char c)
{
	if (string == NULL)
		return NULL;

	size_t count = xsStrLen(string);
	string[count] = c;
	string[count + 1] = 0;
	return string;
}

xsWChar *xsWcsAppend(xsWChar *string, xsWChar c)
{
	if (string == NULL)
		return NULL;

	size_t count = xsWcsLen(string);
	string[count] = c;
	string[count + 1] = 0;
	return string;
}

xsTChar *xsStrToTcsDup(const char *str)
{
	return xsStrToTcsDupN(str, (size_t)-1);
}

xsTChar *xsStrToTcsDupN(const char *str, size_t max)
{
#ifdef XS_UNICODE
	// Need convert
	size_t wcslen, mbslen;
	xsTChar *tstr;

	if (str == NULL)
		return NULL;

	mbslen = xsStrLen(str);
	if (mbslen > max)
		mbslen = max;

	wcslen = mbslen + 1;
	tstr = (xsTChar *)xsMalloc(wcslen * sizeof(xsWChar));
	wcslen = xsMbsToWcs(tstr, str, mbslen);
	tstr[wcslen] = 0;

	return tstr;
#else
	return xsTcsDupN((xsTChar *)str, max);
#endif
}

char *xsTcsToStrDup(const xsTChar *tstr)
{
	return xsTcsToStrDupN(tstr, (size_t)-1);
}

char *xsTcsToStrDupN(const xsTChar *tstr, size_t max)
{
#ifdef XS_UNICODE
	// Need convert
	size_t wcslen, mbslen;
	char *str;

	wcslen = xsTcsLen(tstr);
	if (wcslen > max)
		wcslen = max;
	mbslen = wcslen * 2 + 1;
	str = (char *)xsMalloc(mbslen * sizeof(char));
	mbslen = xsWcsToMbs(str, tstr, wcslen);
	str[mbslen] = 0;

	return str;
#else
	return xsTcsDupN((char *)tstr, max);
#endif
}

char *xsTcsToUtf8Dup(const xsTChar *tstr)
{
#ifdef XS_UNICODE
	size_t wcslen, utf8len;
	char *str;

	wcslen = xsTcsLen(tstr);
	utf8len = wcslen * 3 + 1;
	str = (char *)xsMalloc(utf8len * sizeof(char));
	utf8len = xsWcsToUtf8(str, utf8len, tstr, wcslen);

	return str;
#elif defined(XS_UTF8)
	return xsStrDup(tstr);
#else
#error Unimplemented
#endif

}

/**
 * Concatenate C-string to a xsTChar string.
 * @param tstr destination buffer
 * @param size tstr buffer length in xsTChar
 * @param str string to be concatenated
 * @return concatenate string length. or -1 when size is insufficient
 */
size_t xsTcsCatStrS(xsTChar *tstr, size_t size, const char *str)
{
	// test id length
	size_t len = xsStrLen(str);
	size_t offset = xsTcsLen(tstr);
	if (len >= size - offset)
		return (size_t)-1; // too long

	// assamble
#ifdef XS_UNICODE
	len = xsMbsToWcs(tstr + offset, str, len);
	if (len == -1)
		return NULL;
	tstr[offset + len] = 0;
#else
	xsStrCpy(tstr + offset, str);
#endif

	return offset + len;
}

size_t xsUtf8Len(const char *string)
{
	size_t len = 0;
	int strlen;
	int n;
	const xsU8 *utf8 = (const xsU8 *)string;

	if (utf8 == NULL || *utf8 == '\0')
		return 0;

	strlen = xsStrLen(string);

	while (*utf8 != '\0' && strlen > 0)
	{
		n = g_xsBytesPerUtf8Char[*utf8 >> 4];
		if (n == 0)
			return len;

		if (n > strlen)
			return len;

		utf8 += n;
		strlen -= n;

		len++;
	}

	return len;
}

size_t xsWcsToUtf8(char *dest, size_t sizeInBytes, const xsWChar *src, size_t count)
{
	unsigned short ucs2;
	size_t converted = 0;

	while (*src != 0 && sizeInBytes > converted)
	{
		ucs2 = *src;
		if (ucs2 < 0x80)
		{
			dest[0] = (char)(ucs2 & 0xFF);
			
			converted += 1;
			dest += 1;
		}
		else if (ucs2 >= 0x80  && ucs2 < 0x800)
		{
			dest[0] = (char)((ucs2 >> 6)   | 0xC0);
			dest[1] = (char)((ucs2 & 0x3F) | 0x80);

			converted += 2;
			dest += 2;
		}
		else if (ucs2 >= 0x800 && ucs2 < 0xFFFF)
		{
			dest[0] = (char)(((ucs2 >> 12)       ) | 0xE0);
			dest[1] = (char)(((ucs2 >> 6 ) & 0x3F) | 0x80);
			dest[2] = (char)(((ucs2      ) & 0x3F) | 0x80);

			converted += 3;
			dest += 3;
		}
		src++;
	}

	if (sizeInBytes > 0)
		*dest = 0;

    return converted;
}

size_t xsTcsToUtf8(char *dest, size_t sizeInBytes, const xsTChar *src, size_t count)
{
#ifdef XS_UNICODE
	return xsWcsToUtf8(dest, sizeInBytes, src, count);
#else
	size_t size = sizeInBytes < count ? sizeInBytes : count;
	xsStrCpyN(dest, src, size);
	dest[size] = '\0';
	return size;
#endif
}

size_t xsUtf8ToWcs(xsWChar *dest, size_t sizeInWChars, const char *src, size_t count)
{
    xsU8 n;
	const xsU8 *in = (const xsU8 *)src;
	xsWChar *out = dest;
	size_t len = 0;
	size_t outLen = sizeInWChars;

	while (outLen > 0 && count > 0 && *in != '\0')
	{
		n = g_xsBytesPerUtf8Char[*in >> 4];
		if (n == 1)
		{
			*out = *in;
			in++;
		}
		else if (n == 2)
		{
			if (in[1])
				*out = ((xsU16) (*in & 0x1F) << 6) | (xsU16) (in[1] ^ 0x80);
			else
				break;
			in += 2;
		}
		else if (n == 3)
		{
			if (in[1] && in[2])
				*out = ((xsU16) (*in & 0x0F) << 12)
				| ((xsU16) (in[1] ^ 0x80) << 6)
				| (xsU16) (in[2] ^ 0x80);
			else
				break;
			in += 3;
		}
		else
		{
			break;
		}

		count--;
		outLen--;
		out++;
		len++;
	}

	dest[sizeInWChars - 1] = 0;
    return len;
}

xsTChar *xsUtf8ToTcsDup(const char *str)
{
#ifdef XS_UNICODE
	size_t utf8len, wcslen;
	xsTChar *tstr;

	utf8len = xsUtf8Len(str);
	wcslen = utf8len + 1;
	tstr = (xsTChar *)xsMalloc(wcslen * sizeof(xsWChar));
	wcslen = xsUtf8ToWcs(tstr, wcslen, str, utf8len);

	return tstr;
#elif defined(XS_UTF8)
	return xsStrDup(str);
#else
#error Unimplemented
#endif

}
