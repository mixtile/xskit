#include <xs/base32codec.h>

static const char base32[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";

static const char base32Table[] =
{
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, 26, 27, 28, 29, 30, 31, -1, -1, -1, -1, -1, -1, -1, -1,
	-1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
	15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,
	-1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
	15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};

/**
 * destLen must >= srcLen * 8 / 5 + [1] + 1(zero end)
 */
int xsBase32Encode(char *dest, size_t destLen, const xsU8 *src, size_t srcLen)
{
	xsU8 value;
	xsU8 offset = 0;

	const xsU8* srcEnd = src + srcLen;
	char* destEnd = dest + destLen;

	while (src != srcEnd && dest != destEnd)
	{
		if (offset <= 3)
		{// has 5bits
			value = (*src >> (8 - (offset + 5))) & 0x1F;
			offset = (offset + 5) % 8;
			if (offset == 0)
				src++;
		}
		else
		{// piece
			value = (*src & (0xFF >> offset));
			offset = (offset + 5) % 8;
			value <<= offset;
			if (src + 1 != srcEnd)
				value |= *(src + 1) >> (8 - offset);
			src++;
		}

		*dest++ = base32[value];
	}

	if (dest == destEnd)
		return -1;

	*dest = '\0';
	return 0;
}

/**
 * destLen must >= srcLen * 5 / 8 + [1]
 */
int xsBase32Decode(xsU8 *dest, size_t destLen, const char *src, size_t srcLen)
{
	xsU8 offset = 0;
	xsS8 value;

	const char* srcEnd = src + srcLen;
	xsU8* destEnd = dest + destLen;

	while (src != srcEnd && dest != destEnd)
	{
		if ((value = base32Table[*src++]) == -1)
			continue; // skip other char

		if (offset <= 3)
		{
			offset = (offset + 5) % 8;
			if (offset == 0)
				*dest++ |= value;
			else
				*dest |= value << (8 - offset);
		}
		else
		{
			offset = (offset + 5) % 8;
			*dest++ |= (value >> offset);
			*dest |= value << (8 - offset);
		}
	}

	if (src != srcEnd && dest == destEnd)
		return -1;

	return 0;
}
