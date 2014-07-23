/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "logging.h"

enum rofl_pipeline_debug_levels rofl_pipeline_debug_level[PIPELINE_MAX_DEBUG_CLASS] = ROFL_PIPELINE_DBG_DEFAULT;
int (*rofl_pipeline_debug_print)(FILE *stream, const char *format, ...) = &fprintf; // todo move to the driver side

//API to set the logging function
void rofl_pipeline_set_logging_function(int (*logging_func)(FILE *stream, const char *format, ...)){
	rofl_pipeline_debug_print = logging_func;	
}

void rofl_pipeline_set_logging_level(/*cn,*/ enum rofl_pipeline_debug_levels level){
	rofl_pipeline_debug_level[PIPELINE_DEFAULT] = level;
}
