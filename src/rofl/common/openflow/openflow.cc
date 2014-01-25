/*
 * openflow.cc
 *
 *  Created on: 25.01.2014
 *      Author: andreas
 */

#include "rofl/common/openflow/openflow.h"

using namespace rofl::openflow;

uint32_t
base::get_ofp_no_buffer(uint8_t ofp_version) {

	switch (ofp_version) {
	case rofl::openflow10::OFP_VERSION: return rofl::openflow10::OFP_NO_BUFFER; break;
	case rofl::openflow12::OFP_VERSION: return rofl::openflow12::OFP_NO_BUFFER; break;
	case rofl::openflow13::OFP_VERSION: return rofl::openflow13::OFP_NO_BUFFER; break;
	default: return 0;
	}
}


uint32_t
base::get_ofpp_flood_port(uint8_t ofp_version) {

	switch (ofp_version) {
	case rofl::openflow10::OFP_VERSION: return rofl::openflow10::OFPP_FLOOD; break;
	case rofl::openflow12::OFP_VERSION: return rofl::openflow12::OFPP_FLOOD; break;
	case rofl::openflow13::OFP_VERSION: return rofl::openflow13::OFPP_FLOOD; break;
	default: return 0;
	}
}


uint32_t
base::get_ofpp_controller_port(uint8_t ofp_version) {

	switch (ofp_version) {
	case rofl::openflow10::OFP_VERSION: return rofl::openflow10::OFPP_CONTROLLER; break;
	case rofl::openflow12::OFP_VERSION: return rofl::openflow12::OFPP_CONTROLLER; break;
	case rofl::openflow13::OFP_VERSION: return rofl::openflow13::OFPP_CONTROLLER; break;
	default: return 0;
	}
}



uint8_t
base::get_ofptt_all(uint8_t ofp_version) {

	switch (ofp_version) {
	case rofl::openflow10::OFP_VERSION: return rofl::openflow10::OFPTT_ALL; break;
	case rofl::openflow12::OFP_VERSION: return rofl::openflow12::OFPTT_ALL; break;
	case rofl::openflow13::OFP_VERSION: return rofl::openflow13::OFPTT_ALL; break;
	default: return 0;
	}
}


