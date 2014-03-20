/*
 * cofasyncconfig.cc
 *
 *  Created on: 18.03.2014
 *      Author: andreas
 */

#include "rofl/common/openflow/cofasyncconfig.h"

using namespace rofl::openflow;


cofasync_config::cofasync_config(
		uint8_t ofp_version) :
				ofp_version(ofp_version)
{}



cofasync_config::~cofasync_config()
{}



cofasync_config::cofasync_config(
		cofasync_config const& async_config)
{
	*this = async_config;
}



cofasync_config&
cofasync_config::operator= (
		cofasync_config const& async_config)
{
	if (this == &async_config)
		return *this;

	clear();

	ofp_version = async_config.ofp_version;

	for (std::map<uint8_t, uint32_t>::const_iterator
			it = async_config.packet_in_mask.begin(); it != async_config.packet_in_mask.end(); ++it) {
		packet_in_mask[it->first] = it->second;
	}
	for (std::map<uint8_t, uint32_t>::const_iterator
			it = async_config.port_status_mask.begin(); it != async_config.port_status_mask.end(); ++it) {
		port_status_mask[it->first] = it->second;
	}
	for (std::map<uint8_t, uint32_t>::const_iterator
			it = async_config.flow_removed_mask.begin(); it != async_config.flow_removed_mask.end(); ++it) {
		flow_removed_mask[it->first] = it->second;
	}

	return *this;
}



void
cofasync_config::clear()
{
	packet_in_mask.clear();
	port_status_mask.clear();
	flow_removed_mask.clear();
}



size_t
cofasync_config::length() const
{
	switch (ofp_version) {
	case rofl::openflow13::OFP_VERSION: {
		return (sizeof(struct async_config_t));
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofasync_config::pack(uint8_t* buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();

	switch (ofp_version) {
	case rofl::openflow13::OFP_VERSION: {

		struct async_config_t *async = (struct async_config_t*)buf;

		async->packet_in_mask[0] 	= htobe32(packet_in_mask[0]);
		async->packet_in_mask[1] 	= htobe32(packet_in_mask[1]);
		async->port_status_mask[0] 	= htobe32(port_status_mask[0]);
		async->port_status_mask[1] 	= htobe32(port_status_mask[1]);
		async->flow_removed_mask[0] = htobe32(flow_removed_mask[0]);
		async->flow_removed_mask[1] = htobe32(flow_removed_mask[1]);

	} break;
	default:
		throw eBadVersion();
	}
}



void
cofasync_config::unpack(uint8_t* buf, size_t buflen)
{
	if (buflen < length())
		throw eInval();

	clear();

	switch (ofp_version) {
	case rofl::openflow13::OFP_VERSION: {

		struct async_config_t *async = (struct async_config_t*)buf;

		packet_in_mask[0] 		= be32toh(async->packet_in_mask[0]);
		packet_in_mask[1] 		= be32toh(async->packet_in_mask[1]);
		port_status_mask[0] 	= be32toh(async->port_status_mask[0]);
		port_status_mask[1] 	= be32toh(async->port_status_mask[1]);
		flow_removed_mask[0] 	= be32toh(async->flow_removed_mask[0]);
		flow_removed_mask[1] 	= be32toh(async->flow_removed_mask[1]);

	} break;
	default:
		throw eBadVersion();
	}
}



uint32_t const&
cofasync_config::get_packet_in_mask_master() const
{
	switch (ofp_version) {
	case rofl::openflow13::OFP_VERSION: {
		return packet_in_mask.at(0);
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofasync_config::set_packet_in_mask_master(uint32_t packet_in_mask_master)
{
	set_packet_in_mask_master() = packet_in_mask_master;
}



uint32_t&
cofasync_config::set_packet_in_mask_master()
{
	switch (ofp_version) {
	case rofl::openflow13::OFP_VERSION: {
		return packet_in_mask[0];
	} break;
	default:
		throw eBadVersion();
	}
}



uint32_t const&
cofasync_config::get_packet_in_mask_slave() const
{
	switch (ofp_version) {
	case rofl::openflow13::OFP_VERSION: {
		return packet_in_mask.at(1);
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofasync_config::set_packet_in_mask_slave(uint32_t packet_in_mask_slave)
{
	set_packet_in_mask_slave() = packet_in_mask_slave;
}



uint32_t&
cofasync_config::set_packet_in_mask_slave()
{
	switch (ofp_version) {
	case rofl::openflow13::OFP_VERSION: {
		return packet_in_mask[1];
	} break;
	default:
		throw eBadVersion();
	}
}



uint32_t const&
cofasync_config::get_port_status_mask_master() const
{
	switch (ofp_version) {
	case rofl::openflow13::OFP_VERSION: {
		return port_status_mask.at(0);
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofasync_config::set_port_status_mask_master(uint32_t port_status_mask_master)
{
	set_port_status_mask_master() = port_status_mask_master;
}



uint32_t&
cofasync_config::set_port_status_mask_master()
{
	switch (ofp_version) {
	case rofl::openflow13::OFP_VERSION: {
		return port_status_mask[0];
	} break;
	default:
		throw eBadVersion();
	}
}



uint32_t const&
cofasync_config::get_port_status_mask_slave() const
{
	switch (ofp_version) {
	case rofl::openflow13::OFP_VERSION: {
		return port_status_mask.at(1);
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofasync_config::set_port_status_mask_slave(uint32_t port_status_mask_slave)
{
	set_port_status_mask_slave() = port_status_mask_slave;
}



uint32_t&
cofasync_config::set_port_status_mask_slave()
{
	switch (ofp_version) {
	case rofl::openflow13::OFP_VERSION: {
		return port_status_mask[1];
	} break;
	default:
		throw eBadVersion();
	}
}



uint32_t const&
cofasync_config::get_flow_removed_mask_master() const
{
	switch (ofp_version) {
	case rofl::openflow13::OFP_VERSION: {
		return flow_removed_mask.at(0);
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofasync_config::set_flow_removed_mask_master(uint32_t flow_removed_mask_master)
{
	set_flow_removed_mask_master() = flow_removed_mask_master;
}



uint32_t&
cofasync_config::set_flow_removed_mask_master()
{
	switch (ofp_version) {
	case rofl::openflow13::OFP_VERSION: {
		return flow_removed_mask[0];
	} break;
	default:
		throw eBadVersion();
	}
}



uint32_t const&
cofasync_config::get_flow_removed_mask_slave() const
{
	switch (ofp_version) {
	case rofl::openflow13::OFP_VERSION: {
		return flow_removed_mask.at(1);
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofasync_config::set_flow_removed_mask_slave(uint32_t flow_removed_mask_slave)
{
	set_flow_removed_mask_slave() = flow_removed_mask_slave;
}



uint32_t&
cofasync_config::set_flow_removed_mask_slave()
{
	switch (ofp_version) {
	case rofl::openflow13::OFP_VERSION: {
		return flow_removed_mask[1];
	} break;
	default:
		throw eBadVersion();
	}
}






