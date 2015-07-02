#ifndef _XS_BASE32_H_
#define _XS_BASE32_H_

#include <xs/pal.h>

#ifdef __cplusplus
extern "C" {
#endif

int xsBase32Encode(char *dest, size_t destLen, const xsU8 *src, size_t srcLen);
int xsBase32Decode(xsU8 *dest, size_t destLen, const char *src, size_t srcLen);

#ifdef __cplusplus
}
#endif

#endif /* _XS_BASE32_H_ */
