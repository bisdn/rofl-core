/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef LIKELY_H
#define LIKELY_H 1

#ifdef LIKELY_CUSTOM_HDR

#include "likely_custom.h"

#else

	//#if defined(__GCC__) || defined(__INTEL_COMPILER)
	#if defined(__GNUC__) || defined(__INTEL_COMPILER)

		#ifndef likely
			#define likely(x)	__builtin_expect(((x)),1)
		#endif

		#ifndef unlikely
			#define unlikely(x)	__builtin_expect(((x)),0)
		#endif
		
	#else

		#ifndef likely
			#define likely(x) x
		#endif

		#ifndef unlikely
			#define unlikely(x) x
		#endif

	#endif //ifdef GCC or ICC

#endif //LIKELY_CUSTOM_HDR
	
#endif /* LIKELY_H_ */
