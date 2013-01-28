/*
 * debug.h
 *
 *  Created on: Nov 9, 2012
 *      Author: tobi
 */

#ifndef ROFL_PIPELINE_DEBUG_H_
#define ROFL_PIPELINE_DEBUG_H_

#ifndef NDEBUG
#define ROFL_PIPELINE_DEBUG
#endif

enum rofl_pipeline_debug_levels {
	UNDEF_DEBUG_LEVEL = -1,	/* Undefined debug level */
	//EMERGENCY,			/* system is unusable */
	//ALERT,					/* action must be taken immediately */
	//CRITICAL,				/* critical conditions */
	ERROR,					/* error conditions */
	WARN,					/* warning conditions */
	//NOTICE,					/* normal but significant condition */
	//INFO					/* informational */
	DBG,					/* debug-level messages */
	MAX_DEBUG_LEVEL			/* DO NOT USE */
	/* do not put anything beyond MAX_DEBUG_LEVEL! */
};

enum rofl_pipeline_debug_class {
	UNDEF_DEBUG_CLASS = -1,			/* Undefined debug level */
	DEFAULT = 0,					/* todo name it correct */
	MAX_DEBUG_CLASS					/* DO NOT USE */
	/* do not put anything beyond MAX_DEBUG_CLASS! */
};

#define ROFL_DBG_DEFAULT	{ DEBUG } /* default for each class */

extern int rofl_pipeline_debug_level[MAX_DEBUG_CLASS];
extern int (*rofl_pipeline_debug_print)(const char *format, ... );

#ifdef ROFL_PIPELINE_DEBUG
#define ROFL_DEBUG_CHECK(cn, level)  \
    ( rofl_pipeline_debug_level[cn] < level )
#define ROFL_DEBUG_PRINT(cn, level, stuff...)  \
    if (ROFL_DEBUG_CHECK(cn, level) && *rofl_pipeline_debug_print != 0) \
        rofl_pipeline_debug_print(stuff)

#define ROFL_WARN(cn, stuff...) \
	ROFL_DEBUG_PRINT(cn, WARN, stuff)
#define ROFL_ERR(cn, stuff...)          \
	ROFL_DEBUG_PRINT(cn, ERROR, stuff)
#define ROFL_DEBUG(cn, stuff...)        \
	ROFL_DEBUG_PRINT(cn, DBG, stuff)

#else
#define ROFL_DEBUG_CHECK(flags)  0
#define ROFL_DEBUG_PRINT(flags, stuff)             /* ROFL_DEBUG_CHECK */
#define ROFL_WARN(stuff)
#define ROFL_ERR(stuff)
#define ROFL_DEBUG(stuff)
#endif

#endif /* ROFL_PIPELINE_DEBUG_H_ */
