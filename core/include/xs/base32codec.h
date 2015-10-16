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
