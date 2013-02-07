/*
 * platform_hooks_of12.h
 *
 *  Created on: Feb 7, 2013
 *      Author: tobi
 */

#ifndef PLATFORM_HOOKS_OF12_H_
#define PLATFORM_HOOKS_OF12_H_

#include "../../../openflow/of_switch.h"
#include "../../../../pipeline/common/datapacket.h"


//Packet in
void platform_of12_packet_in(const of_switch_t* sw, uint8_t table_id, datapacket_t* pkt, of_packet_in_reason_t reason);


#endif /* PLATFORM_HOOKS_OF12_H_ */
