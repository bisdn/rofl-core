/*
 * rofl_pipeline_debug.c
 *
 *  Created on: Nov 12, 2012
 *      Author: tobi
 */

#include "rofl_pipeline_debug.h"

#include <stdio.h>

int rofl_pipeline_debug_level[MAX_DEBUG_CLASS] = ROFL_DBG_DEFAULT;
int (*rofl_pipeline_debug_print)(const char *format, ... ) = &printf; // todo move to the driver side
