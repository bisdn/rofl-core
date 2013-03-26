#ifndef __ROFL_H__
#define __ROFL_H__

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * Common ROFL header. 
 *
 * @author: andreas, msune
 *
 */

#include <stdlib.h>

/*
* Common(!) return codes
*/
enum rofl_result {
	ROFL_SUCCESS	= EXIT_SUCCESS,
	ROFL_FAILURE	= EXIT_FAILURE,

	//Add more here... if they are common!
};
typedef enum rofl_result rofl_result_t;

#ifdef __cplusplus
	# define ROFL_BEGIN_DECLS extern "C" {
	# define ROFL_END_DECLS   }
#else
	# define ROFL_BEGIN_DECLS
	# define ROFL_END_DECLS
#endif
//TODO: add other stuff here...

#endif //ROFL
