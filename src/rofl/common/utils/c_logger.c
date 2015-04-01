#include "c_logger.h"
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdarg.h>

inline int sample_logging_func(const char *format, ...) {
	  va_list args;
	  va_start (args, format);
	  register int rv = vfprintf (stderr, format, args);
	  va_end (args);
	  return rv;
}

enum rofl_debug_levels rofl_debug_level[MAX_DEBUG_CLASS] = ROFL_DEFAULT_LEVELS;
int (*rofl_debug_print)(const char *format, ...) = &sample_logging_func;

//API to set the logging function
void rofl_set_logging_function(int (*logging_func)(const char *format, ...)){
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
