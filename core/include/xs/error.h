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

#ifndef _XS_ERROR_H_
#define _XS_ERROR_H_

enum
{
	XS_EC_BEGIN_AT				= XS_EC_BEGIN,

	XS_EC_BUFFER_LOW,
	XS_EC_UNKNOWN_FIELD,
	XS_EC_USER_BREAK,

	XS_EC_NOT_READY,

	XS_EC_SCRIPT_BEGIN,
	XS_EC_SCRIPT_END		= XS_EC_SCRIPT_BEGIN + 32,

	XS_EC_ERROR_MAX
};

#endif /* _XS_ERROR_H_ */
