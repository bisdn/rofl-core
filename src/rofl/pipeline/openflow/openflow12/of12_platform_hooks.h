/*
 * Prototypes of the functions that depend on OF 1.2 and
 * wich platform must support and provide an implementation.
 * 
 */
#ifndef OF12_PLATFORM_HOOKS_H_
#define OF12_PLATFORM_HOOKS_H_

//C++ extern C
ROFL_PIPELINE_BEGIN_DECLS

unsigned int of12_platform_process_packet_in();

ROFL_PIPELINE_END_DECLS

#endif //OF12_PLATFORM_HOOKS_H_
