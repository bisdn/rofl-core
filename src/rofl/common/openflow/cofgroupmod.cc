/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cofgroupmod.h"

using namespace rofl;


cofgroupmod::cofgroupmod(uint8_t ofp_version) :
		ofp_version(ofp_version),
		buckets(ofp_version)
{
	switch (ofp_version) {
	case openflow::OFP_VERSION_UNKNOWN: {

	} break;
	case openflow12::OFP_VERSION: {
		group_mod_area.resize(sizeof(struct openflow12::ofp_group_mod));
	} break;
	case openflow13::OFP_VERSION: {
		group_mod_area.resize(sizeof(struct openflow13::ofp_group_mod));
	} break;
	default: {
		throw eBadVersion();
	};
	}

	grp_mod = group_mod_area.somem();

	reset();
}


cofgroupmod::~cofgroupmod()
{

}


cofgroupmod&
cofgroupmod::operator= (const cofgroupmod& ge)
{
	if (this == &ge)
		return *this;

	this->ofp_version 		= ge.ofp_version;
	this->buckets 			= ge.buckets;
	this->group_mod_area 	= ge.group_mod_area;

	this->grp_mod = this->group_mod_area.somem();

	return *this;
}


void
cofgroupmod::reset()
{
	group_mod_area.clear();
	grp_mod = group_mod_area.somem();

	switch (ofp_version) {
	case openflow::OFP_VERSION_UNKNOWN: {

	} break;
	case openflow12::OFP_VERSION: {
		of12_grp_mod->command 	= htobe16(openflow12::OFPGC_ADD);	// default: add flow-mod entry
		of12_grp_mod->type 		= openflow12::OFPGT_ALL;
		of12_grp_mod->group_id 	= htobe32(0);
	} break;
	case openflow13::OFP_VERSION: {
		of13_grp_mod->command 	= htobe16(openflow13::OFPGC_ADD);	// default: add flow-mod entry
		of13_grp_mod->type 		= openflow13::OFPGT_ALL;
		of13_grp_mod->group_id 	= htobe32(0);
	} break;
	}
}


uint16_t
cofgroupmod::get_command() const
{
	switch (ofp_version) {
	case openflow12::OFP_VERSION: return be16toh(of12_grp_mod->command);
	case openflow13::OFP_VERSION: return be16toh(of13_grp_mod->command);
	default: throw eBadVersion();
	}
}


void
cofgroupmod::set_command(uint16_t command)
{
	switch (ofp_version) {
	case openflow12::OFP_VERSION: of12_grp_mod->command = htobe16(command); break;
	case openflow13::OFP_VERSION: of13_grp_mod->command = htobe16(command); break;
	default: throw eBadVersion();
	}
}


uint8_t
cofgroupmod::get_type() const
{
	switch (ofp_version) {
	case openflow12::OFP_VERSION: return of12_grp_mod->type;
	case openflow13::OFP_VERSION: return of13_grp_mod->type;
	default: throw eBadVersion();
	}
}


void
cofgroupmod::set_type(uint8_t type)
{
	switch (ofp_version) {
	case openflow12::OFP_VERSION: of12_grp_mod->type = type; break;
	case openflow13::OFP_VERSION: of13_grp_mod->type = type; break;
	default: throw eBadVersion();
	}
}


uint32_t
cofgroupmod::get_group_id() const
{
	switch (ofp_version) {
	case openflow12::OFP_VERSION: return be32toh(of12_grp_mod->group_id);
	case openflow13::OFP_VERSION: return be32toh(of13_grp_mod->group_id);
	default: throw eBadVersion();
	}
}


void
cofgroupmod::set_group_id(uint32_t group_id)
{
	switch (ofp_version) {
	case openflow12::OFP_VERSION: of12_grp_mod->group_id = htobe32(group_id); break;
	case openflow13::OFP_VERSION: of13_grp_mod->group_id = htobe32(group_id); break;
	default: throw eBadVersion();
	}
}


size_t
cofgroupmod::pack()
{
	size_t bclen = buckets.length(); // length required for packing buckets in binary array of "struct ofp_bucket"

	if ((sizeof(struct openflow12::ofp_group_mod) + bclen) > group_mod_area.memlen()) // not enough space? => resize memory area for group_mod
	{
		group_mod_area.resize(sizeof(struct openflow12::ofp_group_mod) + bclen);
		grp_mod = group_mod_area.somem();
	}

	buckets.pack((uint8_t*)of12_grp_mod->buckets, bclen); // pack our bucket list into the memory area group_mod->buckets

	return (sizeof(struct openflow12::ofp_group_mod) + bclen); // return size of struct openflow12::ofp_group_mod including appended buckets
}







void
cofgroupmod::test()
{
	cofgroupmod ge(OFP12_VERSION);

	ge.set_command((uint16_t)openflow12::OFPGC_ADD);
	ge.set_group_id(32);
	ge.set_type(openflow12::OFPGT_ALL);

	ge.get_buckets().append_bucket(cofbucket(openflow12::OFP_VERSION, /*weight=*/0x0800, /*watch-port=*/8, /*watch-group=*/1));
	ge.get_buckets().back().get_actions().append_action_output(6);

	ge.get_buckets().append_bucket(cofbucket(openflow12::OFP_VERSION, /*weight=*/0x0800, /*watch-port=*/8, /*watch-group=*/1));
	ge.get_buckets().back().get_actions().append_action_pop_vlan();
	ge.get_buckets().back().get_actions().append_action_push_vlan(600);
	ge.get_buckets().back().get_actions().append_action_dec_nw_ttl();
	ge.get_buckets().back().get_actions().append_action_set_queue(3);
	ge.get_buckets().back().get_actions().append_action_copy_ttl_out();
	ge.get_buckets().back().get_actions().append_action_copy_ttl_in();
	ge.get_buckets().back().get_actions().append_action_output(16);

	std::cerr << "XXXXXX => " << ge << std::endl;
}
