/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "c_logger.h"

enum rofl_debug_levels rofl_debug_level[MAX_DEBUG_CLASS] = ROFL_DEFAULT_LEVELS;
int (*rofl_debug_print)(FILE *stream, const char *format, ...) = &fprintf; 

//API to set the logging function
void rofl_set_logging_function(int (*logging_func)(FILE *stream, const char *format, ...)){
	rofl_debug_print = logging_func;	
}

void rofl_set_logging_level(/*cn,*/ enum rofl_debug_levels level){
	rofl_debug_level[DEFAULT] = level;
}
