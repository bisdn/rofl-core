/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef HAL_UTILS_H
#define HAL_UTILS_H 

/* Extern C for C++ linkage */

#ifdef    __cplusplus
	# define HAL_BEGIN_DECLS extern "C" {
	# define HAL_END_DECLS   }
#else
	# define HAL_BEGIN_DECLS
	# define HAL_END_DECLS
#endif

#endif /* HAL_UTILS_H */
