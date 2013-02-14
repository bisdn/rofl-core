#ifndef PLATFORM_OF12_HOOKS
#define PLATFORM_OF12_HOOKS

#include <stdint.h>

#include "../util/rofl_pipeline_utils.h"
#include "../openflow/openflow12/of12_switch.h"

/*
*
* Prototypes of the functions that platform must support and provide an implementation.
*
*/

//C++ extern C
ROFL_PIPELINE_BEGIN_DECLS

/*
* Hooks for configuration
*/
//TODO: evaluate if they must return errors
void platform_post_init_of12_switch(of12_switch_t* sw);
void platform_pre_destroy_of12_switch(of12_switch_t* sw);

ROFL_PIPELINE_END_DECLS

#endif //PLATFORM_OF12_HOOKS
