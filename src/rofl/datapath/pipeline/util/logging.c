#include "logging.h"

enum rofl_pipeline_debug_levels rofl_pipeline_debug_level[MAX_DEBUG_CLASS] = ROFL_PIPELINE_DBG_DEFAULT;
int (*rofl_pipeline_debug_print)(FILE *stream, const char *format, ...) = &fprintf; // todo move to the driver side

//API to set the logging function
void rofl_pipeline_set_logging_function(int (*logging_func)(FILE *stream, const char *format, ...)){
	rofl_pipeline_debug_print = logging_func;	
} 
