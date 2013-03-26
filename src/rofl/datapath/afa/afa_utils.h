/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef AFA_UTILS_H
#define AFA_UTILS_H 

/* Extern C for C++ linkage */

#ifdef    __cplusplus
	# define AFA_BEGIN_DECLS extern "C" {
	# define AFA_END_DECLS   }
#else
	# define AFA_BEGIN_DECLS
	# define AFA_END_DECLS
#endif

#endif /* AFA_UTILS_H */
