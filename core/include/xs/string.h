#ifndef _XS_STRING_H_
#define _XS_STRING_H_

#include <xs/pal.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef XS_UNICODE

#define xsTcsDup	xsWcsDup
#define xsTcsDupN	xsWcsDupN
#define xsTcsAppend xsWcsAppend

#else

#define xsTcsDup	xsStrDup
#define xsTcsDupN	xsStrDupN
#define xsTcsAppend xsStrAppend

#endif

#ifdef DOXYGEN
/** Duplicates a string.
 * @param
 * @return
 */
xsTChar *xsTcsDup(const xsTChar *string);
xsTChar *xsTcsDupN(const xsTChar *string);
#endif

/**
 * Duplicates a string.
 * @param
 * @return
 */
char *xsStrDup(const char *string);
char *xsStrDupN(const char *string, size_t max);

/**
 * Duplicates a string.
 * @param
 * @return
 */
xsWChar *xsWcsDup(const xsWChar *string);
xsWChar *xsWcsDupN(const xsWChar *string, size_t max);

/**
 * Append a character to C string
 */
char *xsStrAppend(char *string, char c);

/**
 * Append a character to wide-char string
 */
xsWChar *xsWcsAppend(xsWChar *string, xsWChar c);

xsTChar *xsStrToTcsDup(const char *str);
xsTChar *xsStrToTcsDupN(const char *str, size_t max);
char *xsTcsToStrDup(const xsTChar *tstr);
char *xsTcsToStrDupN(const xsTChar *tstr, size_t max);
char *xsTcsToUtf8Dup(const xsTChar *tstr);

size_t xsTcsCatStrS(xsTChar *tstr, size_t size, const char *str);

size_t xsUtf8Len(const char *string);
size_t xsUtf8ToWcs(xsWChar *dest, size_t sizeInWChars, const char *src, size_t count);
size_t xsWcsToUtf8(char *dest, size_t sizeInBytes, const xsWChar *src, size_t count);
size_t xsTcsToUtf8(char *dest, size_t sizeInBytes, const xsTChar *src, size_t count);
xsTChar *xsUtf8ToTcsDup(const char *str);

#ifdef __cplusplus
}
#endif

#endif /* _XS_STRING_H_ */
