/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef ROFL_PIPELINE_LOG_H_
#define ROFL_PIPELINE_LOG_H_

#include <stdlib.h>
#include <stdio.h>

/**
* @author Tobias Jungel<tobias.jungel (at) bisdn.de>, Marc Sune<marc.sune (at) bisdn.de>  
*/

//If DEBUG is not defined, set pipeline to NOT log
#ifdef DEBUG
	#define ROFL_PIPELINE_LOGGING_ENABLED
#endif

//Define debug levesl
enum rofl_pipeline_debug_levels {
	UNDEF_DEBUG_LEVEL = -1,		/* Undefined debug level */
	//EMERGENCY,			/* system is unusable */
	//ALERT,			/* action must be taken immediately */
	//CRITICAL,			/* critical conditions */
	ERROR,				/* error conditions */
	WARN,				/* warning conditions */
	//NOTICE,			/* normal but significant condition */
	INFO,				/* informational */
	DBG,				/* debug-level messages */
	MAX_DEBUG_LEVEL			/* DO NOT USE */

	/* do not put anything beyond MAX_DEBUG_LEVEL! */
};

enum rofl_pipeline_debug_class {
	UNDEF_DEBUG_CLASS = -1,		/* Undefined debug level */
	DEFAULT = 0,			/* todo name it correct */
	MAX_DEBUG_CLASS			/* DO NOT USE */

	/* do not put anything beyond MAX_DEBUG_CLASS! */
};

#define ROFL_PIPELINE_DBG_DEFAULT { DBG } /* default for each class */

extern enum rofl_pipeline_debug_levels rofl_pipeline_debug_level[MAX_DEBUG_CLASS];
extern int (*rofl_pipeline_debug_print)(FILE *stream, const char *format, ...);

//API to manage logging (Capturing)
void rofl_pipeline_set_logging_function(int (*logging_func)(FILE *stream, const char *format, ...));

#ifdef ROFL_PIPELINE_LOGGING_ENABLED

	#define ROFL_PIPELINE_DEBUG_CHECK(cn, level)  \
	    !( rofl_pipeline_debug_level[cn] <= level )
	#define ROFL_PIPELINE_DEBUG_PRINT(fd, cn, level, stuff, ...)  \
	    do{\
		    if (ROFL_PIPELINE_DEBUG_CHECK(cn, level) && *rofl_pipeline_debug_print != NULL){ \
			rofl_pipeline_debug_print(fd,stuff, ##__VA_ARGS__);\
		    }\
	    }while(0)

	#define ROFL_PIPELINE_WARN(stuff,...) \
		ROFL_PIPELINE_DEBUG_PRINT(stderr, DEFAULT, WARN, stuff, ##__VA_ARGS__)

	#define ROFL_PIPELINE_ERR(stuff, ...)          \
		ROFL_PIPELINE_DEBUG_PRINT(stderr, DEFAULT, ERROR, stuff, ##__VA_ARGS__)

	#define ROFL_PIPELINE_INFO(stuff,...) \
		ROFL_PIPELINE_DEBUG_PRINT(stderr, DEFAULT, INFO, stuff, ##__VA_ARGS__)

	#define ROFL_PIPELINE_DEBUG(stuff, ...)        \
		ROFL_PIPELINE_DEBUG_PRINT(stderr, DEFAULT, DBG, stuff, ##__VA_ARGS__)

#else
	//No logging
	#define ROFL_PIPELINE_DEBUG_CHECK(stuff, ...) do{}while(0)
	#define ROFL_PIPELINE_DEBUG_PRINT(stuff, ...) do{}while(0)             /* ROFL_PIPELINE_DEBUG_CHECK */
	#define ROFL_PIPELINE_WARN(stuff, ...) do{}while(0)
	#define ROFL_PIPELINE_ERR(stuff, ...) do{}while(0)
	#define ROFL_PIPELINE_INFO(stuff,...) do{}while(0)
	#define ROFL_PIPELINE_DEBUG(stuff, ...) do{}while(0)

#endif //ROFL_PIPELINE_NO_LOGGING


#endif /* ROFL_PIPELINE_LOG_H_ */
