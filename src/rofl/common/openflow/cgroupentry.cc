/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cgroupentry.h"

using namespace rofl;


cgroupentry::cgroupentry(uint8_t ofp_version) :
		ofp_version(ofp_version),
		group_mod(NULL),
		group_mod_area(sizeof(struct openflow12::ofp_group_mod) + 128/*space for actions, will be extended in method pack() if necessary*/),
		buckets(ofp_version)
{
	reset();
}


cgroupentry::~cgroupentry()
{

}


cgroupentry&
cgroupentry::operator= (const cgroupentry& ge)
{
	if (this == &ge)
		return *this;

	this->ofp_version 		= ge.ofp_version;
	this->buckets 			= ge.buckets;
	this->group_mod_area 	= ge.group_mod_area;

	this->group_mod = (struct openflow12::ofp_group_mod*)this->group_mod_area.somem();

	return *this;
}


void
cgroupentry::reset()
{
	bzero(group_mod_area.somem(), group_mod_area.memlen());
	group_mod = (struct openflow12::ofp_group_mod*)group_mod_area.somem();

	switch (ofp_version) {
	case openflow12::OFP_VERSION: {
		group_mod->command = htobe16(openflow12::OFPGC_ADD);			// default: add flow-mod entry
		group_mod->type = openflow12::OFPGT_ALL;
	} break;
	case openflow13::OFP_VERSION: {
		group_mod->command = htobe16(openflow13::OFPGC_ADD);			// default: add flow-mod entry
		group_mod->type = openflow13::OFPGT_ALL;
	} break;
	}

	group_mod->group_id = htobe32(0);
}


uint16_t
cgroupentry::get_command() const
{
	return be16toh(group_mod->command);
}


void
cgroupentry::set_command(uint16_t command)
{
	group_mod->command = htobe16(command);
}


uint8_t
cgroupentry::get_type() const
{
	return group_mod->type;
}


void
cgroupentry::set_type(uint8_t type)
{
	group_mod->type = type;
}


uint32_t
cgroupentry::get_group_id() const
{
	return be32toh(group_mod->group_id);
}


void
cgroupentry::set_group_id(uint32_t group_id)
{
	group_mod->group_id = htobe32(group_id);
}


size_t
cgroupentry::pack()
{
	size_t bclen = buckets.length(); // length required for packing buckets in binary array of "struct ofp_bucket"

	if ((sizeof(struct openflow12::ofp_group_mod) + bclen) > group_mod_area.memlen()) // not enough space? => resize memory area for group_mod
	{
		group_mod_area.resize(sizeof(struct openflow12::ofp_group_mod) + bclen);
		group_mod = (struct openflow12::ofp_group_mod*)group_mod_area.somem();
	}

	buckets.pack((uint8_t*)group_mod->buckets, bclen); // pack our bucket list into the memory area group_mod->buckets

	return (sizeof(struct openflow12::ofp_group_mod) + bclen); // return size of struct openflow12::ofp_group_mod including appended buckets
}







void
cgroupentry::test()
{
	cgroupentry ge(OFP12_VERSION);

	ge.set_command((uint16_t)openflow12::OFPGC_ADD);
	ge.set_group_id(32);
	ge.set_type(openflow12::OFPGT_ALL);

	ge.buckets[0].watch_group = 1;
	ge.buckets[0].watch_port = 8;
	ge.buckets[0].weight = 0x0800;
	ge.buckets[0].actions.append_action_output(6);
	ge.buckets[1].actions.append_action_pop_vlan();
	ge.buckets[1].actions.append_action_push_vlan(600);
	ge.buckets[1].actions.append_action_dec_nw_ttl();
	ge.buckets[1].actions.append_action_set_queue(3);
	ge.buckets[1].actions.append_action_copy_ttl_out();
	ge.buckets[1].actions.append_action_copy_ttl_in();
	ge.buckets[1].actions.append_action_output(16);

	std::cerr << "XXXXXX => " << ge << std::endl;
}
