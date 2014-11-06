#include "c_logger.h"
#include <string.h>
#include <assert.h>

enum rofl_debug_levels rofl_debug_level[MAX_DEBUG_CLASS] = ROFL_DEFAULT_LEVELS;
int (*rofl_debug_print)(FILE *stream, const char *format, ...) = &fprintf; 

//API to set the logging function
void rofl_set_logging_function(int (*logging_func)(FILE *stream, const char *format, ...)){
	rofl_debug_print = logging_func;	
}

void rofl_set_logging_level(enum rofl_debug_class c, enum rofl_debug_levels level){
	if (ALL == c) {
		memset(rofl_debug_level, level, sizeof(rofl_debug_level));
	} else {
		assert(MAX_DEBUG_CLASS < c);
		assert(UNDEF_DEBUG_CLASS >= c);
		rofl_debug_level[c] = level;
	}
}
