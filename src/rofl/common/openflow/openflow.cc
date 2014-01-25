/*
 * openflow.cc
 *
 *  Created on: 25.01.2014
 *      Author: andreas
 */

#ifdef __cplusplus
extern "C" {
#endif
#include <inttypes.h>
#ifdef __cplusplus
}
#endif

#include "rofl/common/openflow/openflow.h"
#include "rofl/common/croflexception.h"

using namespace rofl::openflow;


uint32_t
get_ofp_no_buffer(uint8_t ofp_version) {

	switch (ofp_version) {
	case rofl::openflow10::OFP_VERSION: return rofl::openflow10::OFP_NO_BUFFER; break;
	case rofl::openflow12::OFP_VERSION: return rofl::openflow12::OFP_NO_BUFFER; break;
	case rofl::openflow13::OFP_VERSION: return rofl::openflow13::OFP_NO_BUFFER; break;
	default: throw rofl::eBadVersion();
	}
}


uint32_t
get_ofp_flood_port(uint8_t ofp_version) {

	switch (ofp_version) {
	case rofl::openflow10::OFP_VERSION: return rofl::openflow10::OFPP_FLOOD; break;
	case rofl::openflow12::OFP_VERSION: return rofl::openflow12::OFPP_FLOOD; break;
	case rofl::openflow13::OFP_VERSION: return rofl::openflow13::OFPP_FLOOD; break;
	default: throw rofl::eBadVersion();
	}
}


uint32_t
get_ofp_controller_port(uint8_t ofp_version) {

	switch (ofp_version) {
	case rofl::openflow10::OFP_VERSION: return rofl::openflow10::OFPP_CONTROLLER; break;
	case rofl::openflow12::OFP_VERSION: return rofl::openflow12::OFPP_CONTROLLER; break;
	case rofl::openflow13::OFP_VERSION: return rofl::openflow13::OFPP_CONTROLLER; break;
	default: throw rofl::eBadVersion();
	}
}


