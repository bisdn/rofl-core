/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef CVASTRING_H
#define CVASTRING_H 1

#include <string>

#ifdef __cplusplus
extern "C" {
#endif
#include <stdio.h>
#include <stdarg.h>
#include <strings.h>
#include <stdlib.h>
//#include <pthread.h>
#ifdef __cplusplus
}
#endif

class cvastring {
public:
	/** constructor
	 */
	cvastring(size_t _len = 8192) :
		len(_len) { };
	/** destructor
	 */
	~cvastring() { };
	/** call operator
	 */
	const char* operator() (const char* fmt, ...)
	{
		data.clear();
#if 0
		char *tmp = 0;
		if ((tmp = (char*)calloc(1, len)) == 0)
		{
			return 0;
		}
		va_list ap;
		va_start(ap, fmt);
		vsnprintf((char*)tmp, len-1, fmt, ap);
		va_end(ap);
		data.assign((char*)tmp, len);
		free(tmp);
#endif
		return data.c_str();
	};

protected:

	// string containing
	std::string data;
	// memory area length
	size_t len;
};

#endif
