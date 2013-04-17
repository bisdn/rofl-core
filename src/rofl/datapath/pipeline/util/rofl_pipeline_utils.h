/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef ROFL_PIPELINE_UTILS_H
#define ROFL_PIPELINE_UTILS_H 

/**
* @author Marc Sune<marc.sune (at) bisdn.de>, Tobias Jungel<tobias.jungel (at) bisdn.de>  
*/

/* Extern C for C++ linkage */

#ifdef __cplusplus
	# define ROFL_PIPELINE_BEGIN_DECLS extern "C" {
	# define ROFL_PIPELINE_END_DECLS   }
#else
	# define ROFL_PIPELINE_BEGIN_DECLS
	# define ROFL_PIPELINE_END_DECLS
#endif
#endif /* ROFL_PIPELINE_UTILS_H */
